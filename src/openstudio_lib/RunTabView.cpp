/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "RunTabView.hpp"

#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include <openstudio/OpenStudio.hxx>

#include <openstudio/model/FileOperations.hpp>
#include <openstudio/model/DaylightingControl.hpp>
#include <openstudio/model/DaylightingControl_Impl.hpp>
#include <openstudio/model/GlareSensor.hpp>
#include <openstudio/model/GlareSensor_Impl.hpp>
#include <openstudio/model/IlluminanceMap.hpp>
#include <openstudio/model/IlluminanceMap_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/UtilityBill.hpp>
#include <openstudio/model/UtilityBill_Impl.hpp>

//#include "../runmanager/lib/JobStatusWidget.hpp"
//#include "../runmanager/lib/RubyJobUtils.hpp"
//#include "../runmanager/lib/RunManager.hpp"

#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

// Include this to find E+ etc
#include <openstudio/utilities/core/ApplicationPathHelpers.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/sql/SqlFile.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include "../shared_gui_components/WorkflowTools.hpp"

#include <openstudio/energyplus/ForwardTranslator.hpp>

#include "../model_editor/Application.hpp"
#include "../model_editor/Utilities.hpp"
#include "MainWindow.hpp"

#include <QButtonGroup>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyleOption>
#include <QSysInfo>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCheckBox>

namespace openstudio {

RunTabView::RunTabView(const model::Model& model, QWidget* parent)
  : MainTabView("Run Simulation", MainTabView::MAIN_TAB, parent), m_runView(new RunView()) {
  addTabWidget(m_runView);
}

RunView::RunView() : QWidget(), m_runSocket(nullptr) {
  auto mainLayout = new QGridLayout();
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(5);
  setLayout(mainLayout);

  m_playButton = new QToolButton();
  m_playButton->setText("Run");
  m_playButton->setCheckable(true);
  m_playButton->setChecked(false);
  QIcon playbuttonicon(QPixmap(":/images/run_simulation_button.png"));
  playbuttonicon.addPixmap(QPixmap(":/images/run_simulation_button.png"), QIcon::Normal, QIcon::Off);
  playbuttonicon.addPixmap(QPixmap(":/images/cancel_simulation_button.png"), QIcon::Normal, QIcon::On);
  m_playButton->setStyleSheet("QToolButton { background:transparent; font: bold; }");
  m_playButton->setIconSize(QSize(35, 35));
  m_playButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_playButton->setIcon(playbuttonicon);
  m_playButton->setLayoutDirection(Qt::RightToLeft);
  m_playButton->setStyleSheet("QAbstractButton:!hover { border: none; }");

  mainLayout->addWidget(m_playButton, 0, 0);
  connect(m_playButton, &QToolButton::clicked, this, &RunView::playButtonClicked);

  // Progress bar area
  m_progressBar = new QProgressBar();
  m_progressBar->setMaximum(State::complete);

  auto progressbarlayout = new QVBoxLayout();
  progressbarlayout->addWidget(m_progressBar);
  mainLayout->addLayout(progressbarlayout, 0, 1);

  auto* mainWindow = OSAppBase::instance()->currentDocument()->mainWindow();
  bool verboseOutput = mainWindow->verboseOutput();
  m_verboseOutputBox = new QCheckBox();
  m_verboseOutputBox->setText("Verbose Output");
  m_verboseOutputBox->setChecked(verboseOutput);
  connect(m_verboseOutputBox, &QCheckBox::clicked, mainWindow, &MainWindow::toggleVerboseOutput);
  mainLayout->addWidget(m_verboseOutputBox, 0, 2);

  m_openSimDirButton = new QPushButton();
  m_openSimDirButton->setText("Show Simulation");
  m_openSimDirButton->setFlat(true);
  m_openSimDirButton->setObjectName("StandardGrayButton");
  connect(m_openSimDirButton, &QPushButton::clicked, this, &RunView::onOpenSimDirClicked);
  mainLayout->addWidget(m_openSimDirButton, 0, 3);

  m_textInfo = new QTextEdit();
  m_textInfo->setReadOnly(true);
  mainLayout->addWidget(m_textInfo, 1, 0, 1, 4);

  m_runProcess = new QProcess(this);
  connect(m_runProcess, &QProcess::finished, this, &RunView::onRunProcessFinished);
  connect(m_runProcess, &QProcess::errorOccurred, this, &RunView::onRunProcessErrored);
  connect(m_runProcess, &QProcess::readyReadStandardError, this, &RunView::readyReadStandardError);
  connect(m_runProcess, &QProcess::readyReadStandardOutput, this, &RunView::readyReadStandardOutput);

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  auto energyPlusExePath = getEnergyPlusExecutable();
  if (!energyPlusExePath.empty()) {
    env.insert("ENERGYPLUS_EXE_PATH", toQString(energyPlusExePath));
  }

  auto radianceDirectory = getRadianceDirectory();
  if (!radianceDirectory.empty()) {
    env.insert("OS_RAYPATH", toQString(radianceDirectory));
  }

  auto perlExecutablePath = getPerlExecutable();
  if (!perlExecutablePath.empty()) {
    env.insert("PERL_EXE_PATH", toQString(perlExecutablePath));
  }

  m_runProcess->setProcessEnvironment(env);

  m_runTcpServer = new QTcpServer();
  m_runTcpServer->listen();
  connect(m_runTcpServer, &QTcpServer::newConnection, this, &RunView::onNewConnection);
}

void RunView::onOpenSimDirClicked() {
  std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
  path runDir = getCompanionFolder(toPath(osdocument->savePath())) / toPath("run");
  QString path = QDir::toNativeSeparators(toQString(runDir));
  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
    QMessageBox::critical(this, "Unable to open simulation", "Please save the OpenStudio Model to view the simulation.");
  }
}

void RunView::onRunProcessErrored(QProcess::ProcessError error) {
  m_textInfo->setTextColor(Qt::red);
  m_textInfo->setFontPointSize(18);
  QString text = tr("onRunProcessErrored: Simulation failed to run, QProcess::ProcessError: ") + QString::number(error);
  m_textInfo->append(text);
}

void RunView::onRunProcessFinished(int exitCode, QProcess::ExitStatus status) {
  if (status == QProcess::NormalExit) {
    LOG(Debug, "run finished, exit code = " << exitCode);
  }

  if (exitCode != 0 || status == QProcess::CrashExit) {
    m_textInfo->setTextColor(Qt::red);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(tr("Simulation failed to run, with exit code ") + QString::number(exitCode));

    //m_textInfo->setTextColor(Qt::black);
    //m_textInfo->setFontPointSize(15);
    //m_textInfo->append("Stderr:");
    //m_textInfo->setFontPointSize(12);
    //QString errorString = QString(m_runProcess->readAllStandardError());
    //m_textInfo->append(errorString);

    //m_textInfo->setFontPointSize(15);
    //m_textInfo->append("Stdout:");
    //m_textInfo->setFontPointSize(12);
    //QString outString = QString(m_runProcess->readAllStandardOutput());
    //m_textInfo->append(outString);
  }

  m_playButton->setChecked(false);
  m_state = State::stopped;
  m_progressBar->setMaximum(State::complete);
  m_progressBar->setValue(State::complete);

  std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
  osdocument->save();
  osdocument->enableTabsAfterRun();
  m_openSimDirButton->setEnabled(true);

  if (m_runSocket) {
    delete m_runSocket;
  }
  m_runSocket = nullptr;
}

void RunView::playButtonClicked(bool t_checked) {
  LOG(Debug, "playButtonClicked " << t_checked);

  std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();

  if (t_checked) {
    // run

    if (osdocument->modified()) {
      osdocument->save();
      // save dialog was canceled
      if (osdocument->modified()) {
        m_playButton->setChecked(false);
        return;
      }
    }

    // Use OpenStudioApplicationPathHelpers to find the CLI
    QString openstudioExePath = toQString(openstudio::getOpenStudioCoreCLI());

    // run in save dir
    //auto basePath = getCompanionFolder( toPath(osdocument->savePath()) );

    // run in temp dir
    auto basePath = toPath(osdocument->modelTempDir()) / toPath("resources");

    auto workflowPath = basePath / "workflow.osw";
    auto stdoutPath = basePath / "stdout";
    auto stderrPath = basePath / "stderr";

    OS_ASSERT(exists(workflowPath));

    auto workflowJSONPath = QString::fromStdString(workflowPath.string());

    unsigned port = m_runTcpServer->serverPort();
    m_hasSocketConnexion = (port != 0);
    // NOTE: temp test, uncomment to see fallback to stdout
    // m_hasSocketConnexion = false;

    QStringList arguments;
    LOG(Debug, "Checkbox is checked? " << std::boolalpha << m_verboseOutputBox->isChecked());
    if (m_verboseOutputBox->isChecked()) {
      arguments << "--verbose";
    } else {
      // If not verbose, we save the stdout/stderr to a file, like historical
      // Actually, we don't, we just read it
      // m_runProcess->setStandardOutputFile(toQString(stdoutPath));
      // m_runProcess->setStandardErrorFile(toQString(stderrPath));
    }

    if (m_hasSocketConnexion) {
      arguments << "run"
                << "-s" << QString::number(port) << "-w" << workflowJSONPath;
    } else {
      arguments << "run"
                << "--show-stdout"
                // << "--style-stdout"
                // << "--add-timings"
                << "-w" << workflowJSONPath;
    }
    LOG(Debug, "openstudioExePath='" << toString(openstudioExePath) << "'");
    LOG(Debug, "run arguments = " << arguments.join(";").toStdString());

    osdocument->disableTabsDuringRun();
    m_openSimDirButton->setEnabled(false);

    if (exists(stdoutPath)) {
      remove(stdoutPath);
    }
    if (exists(stderrPath)) {
      remove(stderrPath);
    }
    m_state = State::stopped;
    m_textInfo->clear();

    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(State::complete);
    m_progressBar->setValue(0);

    if (!m_hasSocketConnexion) {
      m_textInfo->setTextColor(Qt::red);
      m_textInfo->setFontPointSize(15);
      m_textInfo->append("Could not open socket connection to OpenStudio CLI.");
      m_textInfo->setFontPointSize(12);
      m_textInfo->append("Falling back to stdout/stderr parsing, live updates might be slower.");
    }

    m_runProcess->start(openstudioExePath, arguments);
  } else {
    // stop running
    LOG(Debug, "Kill Simulation");
    m_textInfo->setTextColor(Qt::red);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append("Aborted");
    m_runProcess->blockSignals(true);
    m_runProcess->kill();
    m_runProcess->blockSignals(false);
  }
}

void RunView::onNewConnection() {
  m_runSocket = m_runTcpServer->nextPendingConnection();
  connect(m_runSocket, &QTcpSocket::readyRead, this, &RunView::onRunDataReady);
}

void RunView::onRunDataReady() {
  auto appendErrorText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::red);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(text);
  };

  auto appendNormalText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
  };

  auto appendH1Text = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(text);
  };

  auto appendH2Text = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(15);
    m_textInfo->append(text);
  };

  QString data = m_runSocket->readAll();
  QStringList lines = data.split("\n");

  for (const auto& line : lines) {
    //std::cout << data.toStdString() << std::endl;

    QString trimmedLine = line.trimmed();

    // DLM: coordinate with openstudio-workflow-gem\lib\openstudio\workflow\adapters\output\socket.rb
    if (trimmedLine.isEmpty()) {
      continue;
    } else if (QString::compare(trimmedLine, "Starting state initialization", Qt::CaseInsensitive) == 0) {
      appendH1Text("Initializing workflow.");
      m_state = State::initialization;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Started", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Returned from state initialization", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state os_measures", Qt::CaseInsensitive) == 0) {
      appendH1Text("Processing OpenStudio Measures.");
      m_state = State::os_measures;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state os_measures", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state translator", Qt::CaseInsensitive) == 0) {
      appendH1Text("Translating the OpenStudio Model to EnergyPlus.");
      m_state = State::translator;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state translator", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state ep_measures", Qt::CaseInsensitive) == 0) {
      appendH1Text("Processing EnergyPlus Measures.");
      m_state = State::ep_measures;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state ep_measures", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state preprocess", Qt::CaseInsensitive) == 0) {
      // ignore this state
      m_state = State::preprocess;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state preprocess", Qt::CaseInsensitive) == 0) {
      // ignore this state
    } else if (QString::compare(trimmedLine, "Starting state simulation", Qt::CaseInsensitive) == 0) {
      appendH1Text("Starting Simulation.");
      m_state = State::simulation;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state simulation", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state reporting_measures", Qt::CaseInsensitive) == 0) {
      appendH1Text("Processing Reporting Measures.");
      m_state = State::reporting_measures;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state reporting_measures", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Starting state postprocess", Qt::CaseInsensitive) == 0) {
      appendH1Text("Gathering Reports.");
      m_state = State::postprocess;
      m_progressBar->setValue(m_state);
    } else if (QString::compare(trimmedLine, "Returned from state postprocess", Qt::CaseInsensitive) == 0) {
      // no-op
    } else if (QString::compare(trimmedLine, "Failure", Qt::CaseInsensitive) == 0) {
      appendErrorText("Failed.");
    } else if (QString::compare(trimmedLine, "Complete", Qt::CaseInsensitive) == 0) {
      appendH1Text("Completed.");
    } else if (trimmedLine.startsWith("Applying", Qt::CaseInsensitive)) {
      appendH2Text(line);
    } else if (trimmedLine.startsWith("Applied", Qt::CaseInsensitive)) {
      // no-op
    } else {
      appendNormalText(line);
    }
  }
}

void RunView::readyReadStandardOutput() {

  auto appendErrorText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::red);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(text);
  };

  auto appendNormalText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
  };

  auto appendH1Text = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(text);
  };

  auto appendH2Text = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontPointSize(15);
    m_textInfo->append(text);
  };

  QString data = m_runProcess->readAllStandardOutput();
  QStringList lines = data.split("\n");

  for (const auto& line : lines) {
    //std::cout << data.toStdString() << std::endl;

    QString trimmedLine = line.trimmed();

    bool b = trimmedLine.contains("DEBUG");

    // DLM: coordinate with openstudio-workflow-gem\lib\openstudio\workflow\adapters\output\socket.rb
    if (trimmedLine.isEmpty()) {
      continue;
    } else if ((trimmedLine.contains("DEBUG")) || (trimmedLine.contains("] <-2>"))) {
      m_textInfo->setFontPointSize(10);
      m_textInfo->setTextColor(Qt::lightGray);
      m_textInfo->append(line);
    } else if ((trimmedLine.contains("INFO")) || (trimmedLine.contains("] <-1>"))) {
      m_textInfo->setFontPointSize(10);
      m_textInfo->setTextColor(Qt::gray);
      m_textInfo->append(line);
    } else if ((trimmedLine.contains("WARN")) || (trimmedLine.contains("] <0>"))) {
      m_textInfo->setFontPointSize(12);
      m_textInfo->setTextColor(Qt::darkYellow);
      m_textInfo->append(line);
    } else if ((trimmedLine.contains("ERROR")) || (trimmedLine.contains("] <1>"))) {
      m_textInfo->setFontPointSize(12);
      m_textInfo->setTextColor(Qt::darkRed);
      m_textInfo->append(line);
    } else if ((trimmedLine.contains("FATAL")) || (trimmedLine.contains("] <1>"))) {
      m_textInfo->setFontPointSize(14);
      m_textInfo->setTextColor(Qt::red);
      m_textInfo->append(line);

    } else if (!m_hasSocketConnexion) {
      // For socket fall back. Avoid doing all these compare if we know we don't need to
      if (QString::compare(trimmedLine, "Starting state initialization", Qt::CaseInsensitive) == 0) {
        appendH1Text("Initializing workflow.");
        m_state = State::initialization;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Started", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Returned from state initialization", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state os_measures", Qt::CaseInsensitive) == 0) {
        appendH1Text("Processing OpenStudio Measures.");
        m_state = State::os_measures;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state os_measures", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state translator", Qt::CaseInsensitive) == 0) {
        appendH1Text("Translating the OpenStudio Model to EnergyPlus.");
        m_state = State::translator;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state translator", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state ep_measures", Qt::CaseInsensitive) == 0) {
        appendH1Text("Processing EnergyPlus Measures.");
        m_state = State::ep_measures;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state ep_measures", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state preprocess", Qt::CaseInsensitive) == 0) {
        // ignore this state
        m_state = State::preprocess;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state preprocess", Qt::CaseInsensitive) == 0) {
        // ignore this state
      } else if (QString::compare(trimmedLine, "Starting state simulation", Qt::CaseInsensitive) == 0) {
        appendH1Text("Starting Simulation.");
        m_state = State::simulation;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state simulation", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state reporting_measures", Qt::CaseInsensitive) == 0) {
        appendH1Text("Processing Reporting Measures.");
        m_state = State::reporting_measures;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state reporting_measures", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Starting state postprocess", Qt::CaseInsensitive) == 0) {
        appendH1Text("Gathering Reports.");
        m_state = State::postprocess;
        m_progressBar->setValue(m_state);
      } else if (QString::compare(trimmedLine, "Returned from state postprocess", Qt::CaseInsensitive) == 0) {
        // no-op
      } else if (QString::compare(trimmedLine, "Failure", Qt::CaseInsensitive) == 0) {
        appendErrorText("Failed.");
      } else if (QString::compare(trimmedLine, "Complete", Qt::CaseInsensitive) == 0) {
        appendH1Text("Completed.");
      } else if (trimmedLine.startsWith("Applying", Qt::CaseInsensitive)) {
        appendH2Text(line);
      } else if (trimmedLine.startsWith("Applied", Qt::CaseInsensitive)) {
        // no-op
      } else {
        appendNormalText(line);
      }
    } else {  // m_hasSocketConnexion: we know it's stdout and not important socket info, so we put that in gray
      m_textInfo->setFontPointSize(10);
      m_textInfo->setTextColor(Qt::gray);
      m_textInfo->append(line);
    }
  }
}

void RunView::readyReadStandardError() {
  auto appendErrorText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(text);
  };

  QString data = m_runProcess->readAllStandardError();
  QStringList lines = data.split("\n");

  for (const auto& line : lines) {

    QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty()) {
      continue;
    } else {
      appendErrorText("stderr: " + line);
    }
  }
}

}  // namespace openstudio
