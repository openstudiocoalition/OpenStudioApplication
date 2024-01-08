/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

RunTabView::RunTabView(const model::Model& /*model*/, QWidget* parent)
  : MainTabView("Run Simulation", MainTabView::MAIN_TAB, parent), m_runView(new RunView()) {
  addTabWidget(m_runView);
}

RunView::RunView() : m_runSocket(nullptr) {
  auto* mainLayout = new QGridLayout();
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(5);
  setLayout(mainLayout);

  m_playButton = new QToolButton();
  m_playButton->setText("Run");
  m_playButton->setCheckable(true);
  m_playButton->setChecked(false);
  QIcon playbuttonicon(QPixmap(":/images/run_simulation_button.png"));
  playbuttonicon.addPixmap(QPixmap(":/images/run_simulation_button.png"), QIcon::Normal, QIcon::Off);
  playbuttonicon.addPixmap(QPixmap(":/images/run_cancel.png"), QIcon::Normal, QIcon::On);
  m_playButton->setStyleSheet("QToolButton { background:transparent; font: bold; }");
  m_playButton->setIconSize(QSize(35, 35));
  m_playButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_playButton->setIcon(playbuttonicon);
  m_playButton->setLayoutDirection(Qt::RightToLeft);
  m_playButton->setStyleSheet("QAbstractButton:!hover { border: none; }");

  mainLayout->addWidget(m_playButton, 0, 0);
  connect(m_playButton, &QToolButton::clicked, this, &RunView::playButtonClicked);

  // Progress bar area
  m_progressBar = new ProgressBarWithError();
  m_progressBar->setMaximum(State::complete);

  auto* progressbarlayout = new QVBoxLayout();
  progressbarlayout->addWidget(m_progressBar);
  mainLayout->addLayout(progressbarlayout, 0, 1);

  auto* mainWindow = OSAppBase::instance()->currentDocument()->mainWindow();
  const bool verboseOutput = mainWindow->verboseOutput();
  m_verboseOutputBox = new QCheckBox();
  m_verboseOutputBox->setText("Verbose");
  m_verboseOutputBox->setChecked(verboseOutput);
  connect(m_verboseOutputBox, &QCheckBox::clicked, mainWindow, &MainWindow::toggleVerboseOutput);
  mainLayout->addWidget(m_verboseOutputBox, 0, 2);

  const bool useClassicCLI = mainWindow->useClassicCLI();
  m_useClassicCLIBox = new QCheckBox();
  m_useClassicCLIBox->setText("Classic CLI");
  m_useClassicCLIBox->setChecked(useClassicCLI);
  connect(m_useClassicCLIBox, &QCheckBox::clicked, mainWindow, &MainWindow::toggleUseClassicCLI);
  mainLayout->addWidget(m_useClassicCLIBox, 0, 3);

  m_openSimDirButton = new QPushButton();
  m_openSimDirButton->setText("Show Simulation");
  m_openSimDirButton->setFlat(true);
  m_openSimDirButton->setObjectName("StandardGrayButton");
  connect(m_openSimDirButton, &QPushButton::clicked, this, &RunView::onOpenSimDirClicked);
  mainLayout->addWidget(m_openSimDirButton, 0, 4);

  m_textInfo = new QTextEdit();
  m_textInfo->setReadOnly(true);
  mainLayout->addWidget(m_textInfo, 1, 0, 1, 4);
  resetFont();

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
  const auto runDir = getCompanionFolder(toPath(OSAppBase::instance()->currentDocument()->savePath())) / toPath("run");
  const QString qpath = QDir::toNativeSeparators(toQString(runDir));
  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(qpath))) {
    QMessageBox::critical(this, "Unable to open simulation", "Please save the OpenStudio Model to view the simulation.");
  }
}

void RunView::onRunProcessErrored(QProcess::ProcessError error) {
  m_textInfo->setTextColor(Qt::darkRed);
  m_textInfo->setFontPointSize(18);
  const QString text = tr("onRunProcessErrored: Simulation failed to run, QProcess::ProcessError: ") + QString::number(error);
  m_textInfo->append(text);
  resetFont();
}

void RunView::onRunProcessFinished(int exitCode, QProcess::ExitStatus status) {
  if (status == QProcess::NormalExit) {
    LOG(Debug, "run finished, exit code = " << exitCode);
  }

  if (exitCode != 0 || status == QProcess::CrashExit) {
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontPointSize(18);
    m_textInfo->append(tr("Simulation failed to run, with exit code ") + QString::number(exitCode));
    resetFont();

    m_progressBar->setError(true);

    //m_textInfo->setTextColor(Qt::black);
    //m_textInfo->setFontPointSize(15);
    //m_textInfo->append("Stderr:");
    //m_textInfo->setFontPointSize(12);
    //QString errorString = QString(m_runProcess->readAllStandardError());
    //m_textInfo->append(errorString);
    //resetFont();

    //m_textInfo->setFontPointSize(15);
    //m_textInfo->append("Stdout:");
    //m_textInfo->setFontPointSize(12);
    //QString outString = QString(m_runProcess->readAllStandardOutput());
    //m_textInfo->append(outString);
    //resetFont();
  }

  m_playButton->setChecked(false);
  m_state = State::stopped;

  m_progressBar->setMaximum(State::complete);
  m_progressBar->setValue(State::complete);

  const std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
  osdocument->save();
  osdocument->enableTabsAfterRun();
  m_openSimDirButton->setEnabled(true);

  delete m_runSocket;
  m_runSocket = nullptr;
}

void RunView::playButtonClicked(bool t_checked) {
  LOG(Debug, "playButtonClicked " << t_checked);

  const std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();

  if (t_checked) {
    // run

    if (osdocument->modified()) {
      osdocument->save();
      // save dialog was canceled
      // cppcheck-suppress identicalInnerCondition
      if (osdocument->modified()) {
        m_playButton->setChecked(false);
        return;
      }
    }

    // Use OpenStudioApplicationPathHelpers to find the CLI
    const QString openstudioExePath = toQString(openstudio::getOpenStudioCoreCLI());

    // run in save dir
    //auto basePath = getCompanionFolder( toPath(osdocument->savePath()) );

    // run in temp dir
    m_basePath = toPath(osdocument->modelTempDir()) / toPath("resources");

    auto workflowPath = m_basePath / "workflow.osw";
    auto stdoutPath = m_basePath / "stdout";
    auto stderrPath = m_basePath / "stderr";

    OS_ASSERT(exists(workflowPath));

    auto workflowJSONPath = QString::fromStdString(workflowPath.string());

    QStringList arguments;
    LOG(Debug, "Classic CLI Checkbox is checked? " << std::boolalpha << m_useClassicCLIBox->isChecked());
    if (m_useClassicCLIBox->isChecked()) {
      arguments << "classic";
    }
    LOG(Debug, "Verbose Checkbox is checked? " << std::boolalpha << m_verboseOutputBox->isChecked());
    if (m_verboseOutputBox->isChecked()) {
      arguments << "--verbose";
    } else if (!m_useClassicCLIBox->isChecked()) {
      // https://github.com/NREL/OpenStudio/issues/5069
      arguments << "--verbose";
    }

    arguments << "run";
    // C++ CLI doesn't have working socket connection yet: https://github.com/NREL/OpenStudio/issues/5073
    m_hasSocketConnection = false;
    if (m_useClassicCLIBox->isChecked()) {
      const unsigned port = m_runTcpServer->serverPort();
      m_hasSocketConnection = (port != 0);
      if (m_hasSocketConnection) {
        arguments << "-s" << QString::number(port);
      } else {
        arguments << "--show-stdout";
      }
      arguments << "-w" << workflowJSONPath;
    } else {
      arguments << "run"
                << "--show-stdout"
                << "--style-stdout"
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
    // touch
    openstudio::filesystem::ofstream{stdoutPath};  // NOLINT(bugprone-unused-raii)
    openstudio::filesystem::ofstream{stderrPath};  // NOLINT(bugprone-unused-raii)

    m_state = State::stopped;
    m_textInfo->clear();

    m_progressBar->setError(false);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(State::complete);
    m_progressBar->setValue(0);

    if (m_useClassicCLIBox->isChecked() && !m_hasSocketConnection) {
      m_textInfo->setTextColor(Qt::darkRed);
      m_textInfo->setFontPointSize(15);
      m_textInfo->append("Could not open socket connection to OpenStudio Classic CLI.");
      m_textInfo->setFontPointSize(12);
      m_textInfo->append("Falling back to stdout/stderr parsing, live updates might be slower.");
      resetFont();
    }

    m_runProcess->start(openstudioExePath, arguments);
  } else {
    // stop running
    LOG(Debug, "Kill Simulation");
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontPointSize(15);
    m_textInfo->append("Aborted");
    resetFont();
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
  const QString data = m_runSocket->readAll();
  const QStringList lines = data.split("\n");

  // Write to stdout (pipe to file, for later viewing)
  auto stdoutPath = m_basePath / "stdout";
  openstudio::filesystem::ofstream stdout_file(stdoutPath, std::ios_base::app);
  if (!stdout_file) {
    LOG(Debug, "Could not open " << stdoutPath << " for appending.");
  } else {
    stdout_file << openstudio::toString(data);
  }

  for (const auto& line : lines) {
    processLine(line, true);
  }
}

void RunView::readyReadStandardOutput() {

  const QString data = m_runProcess->readAllStandardOutput();
  const QStringList lines = data.split("\n");

  // Write to stdout (pipe to file, for later viewing)
  auto stdoutPath = m_basePath / "stdout";
  openstudio::filesystem::ofstream stdout_file(stdoutPath, std::ios_base::app);
  if (!stdout_file) {
    LOG(Debug, "Could not open " << stdoutPath << " for appending.");
  } else {
    stdout_file << openstudio::toString(data);
  }

  for (const auto& line : lines) {
    processLine(line, false);
  }
}

void RunView::processLine(const QString& line, bool fromSocket) {

  if ((m_hasSocketConnection && !fromSocket)) {
    return;
  }

  auto appendNormalText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontWeight(QFont::Normal);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  auto appendWarnText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::red);
    m_textInfo->setFontWeight(QFont::Normal);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  auto appendErrorText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontWeight(QFont::Bold);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  auto appendStateChange = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontWeight(QFont::Bold);
    m_textInfo->setFontPointSize(15);
    m_textInfo->append(text);
    resetFont();
  };

  auto appendResult = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::black);
    m_textInfo->setFontWeight(QFont::DemiBold);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  auto appendErrorResult = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontWeight(QFont::DemiBold);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  QString trimmedLine = line.trimmed();

  // see if we should ignore the line
  if (trimmedLine.isEmpty()) {
    return;
  } else if (trimmedLine.contains("Treating schema as a regular XSD")) {
    return;
  } else if (trimmedLine.contains(QChar(27))) {
    // escape character
    return;
  } else if (trimmedLine.contains(QChar(9472))) {
    // line character
    return;
  }

  // check for state transitions
  int test = 0;
  switch (m_state) {
    case State::stopped:
      test = trimmedLine.indexOf("Starting state initialization", 0, Qt::CaseInsensitive);
      if (trimmedLine.contains("Starting state initialization", Qt::CaseInsensitive)) {
        appendStateChange("Initializing workflow.");
        m_state = State::initialization;
        m_progressBar->setValue(m_state);
        return;
      } else if (trimmedLine.contains("Setting Log Level to Debug", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("OSRunner is deprecated, use OpenStudio::Measure::OSRunner instead", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("command is deprecated and will be removed in a future release", Qt::CaseInsensitive)) {
        appendWarnText("The classic command is deprecated and will be removed in a future release.");
        return;
      }

      break;
    case State::initialization:
      if (QString::compare(trimmedLine, "Started", Qt::CaseInsensitive) == 0) {
        // no-op
        return;
      } else if (trimmedLine.contains("Reading in OSM model", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("Returned from state initialization", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (QString::compare(trimmedLine, "Starting state os_measures", Qt::CaseInsensitive) == 0
                 || trimmedLine.contains("Starting State OpenStudioMeasures", Qt::CaseInsensitive)) {
        appendStateChange("Processing OpenStudio Measures.");
        m_state = State::os_measures;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::os_measures:
      if (QString::compare(trimmedLine, "Returned from state os_measures", Qt::CaseInsensitive) == 0
          || trimmedLine.contains("Returned from State OpenStudioMeasure", Qt::CaseInsensitive)
          || trimmedLine.contains("Translator - selected", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("Starting state translator", Qt::CaseInsensitive)) {
        appendStateChange("Translating the OpenStudio Model to EnergyPlus.");
        m_state = State::translator;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::translator:
      if (trimmedLine.contains("Returned from state translator", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (QString::compare(trimmedLine, "Starting state ep_measures", Qt::CaseInsensitive) == 0
                 || trimmedLine.contains("Starting state EnergyPlusMeasures", Qt::CaseInsensitive)) {
        appendStateChange("Processing EnergyPlus Measures.");
        m_state = State::ep_measures;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::ep_measures:
      if (QString::compare(trimmedLine, "Returned from state ep_measures", Qt::CaseInsensitive) == 0
          || trimmedLine.contains("Returned from State EnergyPlusMeasures", Qt::CaseInsensitive)
          || trimmedLine.contains("PreProcess - selected", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("Starting state preprocess", Qt::CaseInsensitive)) {
        appendStateChange("Adding Simulation Output Requests.");
        m_state = State::preprocess;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::preprocess:
      if (trimmedLine.contains("Returned from state preprocess", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("Starting state simulation", Qt::CaseInsensitive)
                 || trimmedLine.contains("Starting State EnergyPlus", Qt::CaseInsensitive)) {
        appendStateChange("Starting EnergyPlus Simulation.");
        m_state = State::simulation;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::simulation:
      if (trimmedLine.contains("Returned from state simulation", Qt::CaseInsensitive)
          || trimmedLine.contains("Returned from state EnergyPlus", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (QString::compare(trimmedLine, "Starting state reporting_measures", Qt::CaseInsensitive) == 0
                 || trimmedLine.contains("Starting State ReportingMeasures", Qt::CaseInsensitive)) {
        appendStateChange("Processing Reporting Measures.");
        m_state = State::reporting_measures;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::reporting_measures:
      if (QString::compare(trimmedLine, "Returned from state reporting_measures", Qt::CaseInsensitive) == 0
          || trimmedLine.contains("Returned from state ReportingMeasures", Qt::CaseInsensitive)) {
        // no-op
        return;
      } else if (trimmedLine.contains("Starting state postprocess", Qt::CaseInsensitive)) {
        appendStateChange("Gathering Reports.");
        m_state = State::postprocess;
        m_progressBar->setValue(m_state);
        return;
      }
      break;
    case State::postprocess:
      if (trimmedLine.contains("Returned from state postprocess", Qt::CaseInsensitive)) {
        // no-op
        return;
      }
      break;
    default:
      break;
  }

  // detect log level
  int logLevel = -1;
  if ((trimmedLine.contains("DEBUG")) || (trimmedLine.contains("] <-2>")) || (trimmedLine.contains("] <Debug>"))) {
    logLevel = -2;
  } else if ((trimmedLine.contains("INFO")) || (trimmedLine.contains("] <-1>")) || (trimmedLine.contains("] <Info>"))) {
    logLevel = -1;
  } else if ((trimmedLine.contains("WARN")) || (trimmedLine.contains("] <0>")) || (trimmedLine.contains("] <Warn>"))) {
    logLevel = 0;
  } else if ((trimmedLine.contains("ERROR")) || (trimmedLine.contains("] <1>")) || (trimmedLine.contains("] <Error>"))) {
    logLevel = 1;
  } else if ((trimmedLine.contains("FATAL")) || (trimmedLine.contains("] <1>")) || (trimmedLine.contains("] <Fatal>"))) {
    logLevel = 1;
  }

  // remove the log level and channel if present
  static const QRegularExpression channelAndLevel("^\\[.*?\\]\\s*<.*?>\\s*");
  trimmedLine.remove(channelAndLevel);

  static const QRegularExpression foundMeasureRegex("^Found.*?at primaryScriptPath");

  //D, [2024-01-03T23:08:15.526127 #41668] DEBUG -- :
  //I, [2024 - 01 - 03T23:08:15.527811 #41668] INFO -- :

  // check for measure output
  bool isMeasure = false;
  if (m_state == State::os_measures || m_state == State::ep_measures || m_state == State::reporting_measures) {
    isMeasure = true;

    if (QString::compare(trimmedLine, "Failure", Qt::CaseInsensitive) == 0) {
      appendErrorResult("Failed.");
      return;
    } else if (QString::compare(trimmedLine, "Complete", Qt::CaseInsensitive) == 0) {
      appendResult("Completed.");
      return;
    } else if (trimmedLine.startsWith("Applying", Qt::CaseInsensitive)) {
      appendResult(trimmedLine);
      return;
    } else if (trimmedLine.startsWith("Applied", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("Running step", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("Find model's weather file and update LastEpwFilePath", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("Search for weather file", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("Creating run directory for measure in", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (foundMeasureRegex.match(trimmedLine).hasMatch()) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("Measure Script Path", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("getOpenStudioModule", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("completeAndNormalize", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.startsWith("measure->name()", Qt::CaseInsensitive)) {
      // no-op
      return;
    } else if (trimmedLine.contains("Step Result", Qt::CaseInsensitive)) {
      appendResult(trimmedLine);
      return;
    } else if (trimmedLine.contains("Class Name", Qt::CaseInsensitive)) {
      appendResult(trimmedLine);
      return;
    }
  }

  // don't print debug statements unless in verbose or if running a measure
  if (logLevel < -1 && !m_verboseOutputBox->isChecked() && !isMeasure) {
    return;
  }

  // print the line
  if (logLevel > 0) {
    appendErrorText(trimmedLine);
  } else if (logLevel == 0) {
    appendWarnText(trimmedLine);
  } else {
    appendNormalText(trimmedLine);
  }
}

void RunView::readyReadStandardError() {
  auto appendErrorText = [&](const QString& text) {
    m_textInfo->setTextColor(Qt::darkRed);
    m_textInfo->setFontPointSize(12);
    m_textInfo->append(text);
    resetFont();
  };

  const QString data = m_runProcess->readAllStandardError();
  const QStringList lines = data.split("\n");

  // Write to stderr (pipe to file, for later viewing)
  auto stderrPath = m_basePath / "stderr";
  openstudio::filesystem::ofstream stderr_file(stderrPath, std::ios_base::app);
  if (!stderr_file) {
    LOG(Debug, "Could not open " << stderrPath << " for appending.");
  } else {
    stderr_file << openstudio::toString(data);
  }

  for (const auto& line : lines) {

    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty()) {
      continue;
    } else {
      appendErrorText("stderr: " + trimmedLine);
    }
  }
}

void RunView::resetFont() {
  m_textInfo->setTextColor(Qt::black);
  m_textInfo->setFontWeight(QFont::Normal);
  m_textInfo->setFontPointSize(12);
}

}  // namespace openstudio
