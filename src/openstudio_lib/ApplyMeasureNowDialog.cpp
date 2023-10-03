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

#include "ApplyMeasureNowDialog.hpp"

#include "../shared_gui_components/BusyWidget.hpp"
#include "../shared_gui_components/EditController.hpp"
#include "../shared_gui_components/EditView.hpp"
#include "../shared_gui_components/LocalLibraryController.hpp"
#include "../shared_gui_components/LocalLibraryView.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/OSViewSwitcher.hpp"
#include "../shared_gui_components/TextEditDialog.hpp"
#include "../shared_gui_components/WorkflowController.hpp"

#include "MainRightColumnController.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItem.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include "../openstudio_app/OpenStudioApp.hpp"
#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/core/RubyException.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep.hpp>
#include <openstudio/utilities/filetypes/WorkflowStepResult.hpp>
#include <openstudio/utilities/time/DateTime.hpp>

#include <QBoxLayout>
#include <QCloseEvent>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QSharedPointer>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QStandardPaths>
#include <QDialog>

#include <fstream>

#define FAILED_ARG_TEXT "<FONT COLOR = RED>Failed to Show Arguments<FONT COLOR = BLACK> <br> <br>Reason(s): <br> <br>"

#define ACCEPT_CHANGES "Accept Changes"
#define APPLY_MEASURE "Apply Measure"

namespace openstudio {

ApplyMeasureNowDialog::ApplyMeasureNowDialog(QWidget* parent)
  : OSDialog(false, parent),
    m_editController(nullptr),
    m_mainPaneStackedWidget(nullptr),
    m_rightPaneStackedWidget(nullptr),
    m_argumentsFailedTextEdit(nullptr),
    m_jobItemView(nullptr),
    m_timer(nullptr),
    m_showAdvancedOutput(nullptr),
    m_advancedOutput(QString()),
    m_workingDir(openstudio::path()),
    m_workingFilesDir(openstudio::path()),
    m_advancedOutputDialog(nullptr) {
  setWindowTitle("Apply Measure Now");
  setWindowModality(Qt::ApplicationModal);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setSizeGripEnabled(true);
  createWidgets();

  OSAppBase* app = OSAppBase::instance();
  connect(this, &ApplyMeasureNowDialog::reloadFile, app, &OSAppBase::reloadFile, Qt::QueuedConnection);

  m_advancedOutputDialog = new TextEditDialog("Advanced Output");

  //m_workingDir = toPath("E:/test/ApplyMeasureNow");
  m_workingDir = openstudio::toPath(app->currentDocument()->modelTempDir()) / openstudio::toPath("ApplyMeasureNow");

  // save the model's workflow JSON
  m_modelWorkflowJSON = app->currentModel()->workflowJSON();

  // set a temporary workflow JSON
  m_tempWorkflowJSON = WorkflowJSON();

  // use the temp model as the seed
  m_tempWorkflowJSON.setSeedFile(app->measureManager().tempModelPath());

  // copy the weather file reference
  boost::optional<openstudio::path> weatherFile = m_modelWorkflowJSON.weatherFile();
  if (weatherFile) {
    m_tempWorkflowJSON.setWeatherFile(*weatherFile);
  }

  // The openstudio-workflow gem will prepend the "generated_files" directory to the workflowJSON.filePaths, so match that
  // cf: https://github.com/NREL/OpenStudio-workflow-gem/blob/e569f910be364d33c3ddb1a655570c85f1b24bfa/lib/openstudio/workflow/jobs/run_initialization.rb#L99
  // in requestReload, this directory gets copied over to the first filePath of m_modelWorkflowJSON, which is typically the files/ directory
  m_workingFilesDir = m_workingDir / openstudio::toPath("generated_files");
  // add the WorkingFiles directory as files files path, measures writing output files should be written here
  m_tempWorkflowJSON.addFilePath(m_workingFilesDir);

  // add file paths from current workflow so we can find weather file
  for (const auto& absoluteFilePath : m_modelWorkflowJSON.absoluteFilePaths()) {
    m_tempWorkflowJSON.addFilePath(absoluteFilePath);
  }

  m_tempWorkflowJSON.saveAs(m_workingDir / toPath("temp.osw"));

  // temporarily swap app's workflow for this one
  app->currentModel()->setWorkflowJSON(m_tempWorkflowJSON);
}

ApplyMeasureNowDialog::~ApplyMeasureNowDialog() {
  // DLM: would be nice if this was not needed..
  // restore app state, the app's library controller was swapped out in createWidgets
  openstudio::OSAppBase* app = OSAppBase::instance();
  if (app) {
    app->measureManager().setLibraryController(app->currentDocument()->mainRightColumnController()->measureLibraryController());

    // restore the model's workflow JSON
    // DLM: this is not sufficient if the OSM is being reloaded
    app->currentModel()->setWorkflowJSON(m_modelWorkflowJSON);
  }

  delete m_advancedOutputDialog;
}

QSize ApplyMeasureNowDialog::sizeHint() const {
  return QSize(770, 560);
}

void ApplyMeasureNowDialog::createWidgets() {
  QWidget* widget = nullptr;
  QBoxLayout* layout = nullptr;
  QLabel* label = nullptr;

  openstudio::OSAppBase* app = OSAppBase::instance();

  // PAGE STACKED WIDGET

  m_mainPaneStackedWidget = new QStackedWidget();
  upperLayout()->addWidget(m_mainPaneStackedWidget);

  // INPUT

  m_argumentsFailedTextEdit = new QTextEdit(FAILED_ARG_TEXT);
  m_argumentsFailedTextEdit->setReadOnly(true);

  m_editController = QSharedPointer<EditController>(new EditController(true));
  bool onlyShowModelMeasures = true;
  m_localLibraryController = QSharedPointer<LocalLibraryController>(new LocalLibraryController(app, onlyShowModelMeasures));
  m_localLibraryController->localLibraryView->setStyleSheet("QStackedWidget { border-top: 0px; }");
  m_localLibraryController->localLibraryView->addBCLMeasureButton->setVisible(false);

  // DLM: this is changing application state, needs to be undone in the destructor
  app->measureManager().setLibraryController(m_localLibraryController);
  app->currentDocument()->disable();
  app->measureManager().updateMeasuresLists();
  app->currentDocument()->enable();

  m_rightPaneStackedWidget = new QStackedWidget();
  m_argumentsFailedPageIdx = m_rightPaneStackedWidget->addWidget(m_argumentsFailedTextEdit);

  auto* viewSwitcher = new OSViewSwitcher();
  viewSwitcher->setView(m_editController->editView);
  m_argumentsOkPageIdx = m_rightPaneStackedWidget->addWidget(viewSwitcher);

  layout = new QHBoxLayout();
  layout->addWidget(m_localLibraryController->localLibraryView);
  layout->addWidget(m_rightPaneStackedWidget);

  widget = new QWidget();
  widget->setLayout(layout);
  m_inputPageIdx = m_mainPaneStackedWidget->addWidget(widget);

  // RUNNING

  label = new QLabel("Running Measure");
  label->setObjectName("H2");

  auto* busyWidget = new BusyWidget();

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, busyWidget, &BusyWidget::rotate);

  layout = new QVBoxLayout();
  layout->addStretch();
  layout->addWidget(label, 0, Qt::AlignCenter);
  layout->addWidget(busyWidget, 0, Qt::AlignCenter);
  layout->addStretch();

  widget = new QWidget();
  widget->setLayout(layout);
  m_runningPageIdx = m_mainPaneStackedWidget->addWidget(widget);

  // OUTPUT

  label = new QLabel("Measure Output");
  label->setObjectName("H1");

  m_jobItemView = new DataPointJobItemView();

  m_jobPath = new QLabel();
  m_jobPath->setTextInteractionFlags(Qt::TextSelectableByMouse);
#if !(_DEBUG || (__GNUC__ && !NDEBUG))
  m_jobPath->hide();
#endif

  layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(m_jobPath);
  layout->addWidget(m_jobItemView, 0, Qt::AlignTop);

  layout->addStretch();

  m_showAdvancedOutput = new QPushButton("Advanced Output");
  connect(m_showAdvancedOutput, &QPushButton::clicked, this, &ApplyMeasureNowDialog::showAdvancedOutput);

  //layout->addStretch();

  auto* hLayout = new QHBoxLayout();
  hLayout->addWidget(m_showAdvancedOutput);
  hLayout->addStretch();
  layout->addLayout(hLayout);

  widget = new QWidget();
  widget->setLayout(layout);

  auto* scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(widget);

  m_outputPageIdx = m_mainPaneStackedWidget->addWidget(scrollArea);

  // SET CURRENT INDEXES

  m_rightPaneStackedWidget->setCurrentIndex(m_argumentsOkPageIdx);

  m_mainPaneStackedWidget->setCurrentIndex(m_inputPageIdx);

  // BUTTONS

  this->okButton()->setText(APPLY_MEASURE);
  this->okButton()->setEnabled(false);

  this->backButton()->show();
  this->backButton()->setEnabled(false);

  // OS SETTINGS

#ifdef Q_OS_DARWIN
  setWindowFlags(Qt::FramelessWindowHint);
#elif defined(Q_OS_WIN)
  setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
#endif
}

void ApplyMeasureNowDialog::resizeEvent(QResizeEvent* event) {
  // Use the QDialog one so it can be resized (OSDialog prevents resizing)
  QDialog::resizeEvent(event);
}

void ApplyMeasureNowDialog::displayMeasure() {
  this->okButton()->setText(APPLY_MEASURE);
  this->okButton()->show();
  this->okButton()->setEnabled(false);

  m_rightPaneStackedWidget->setCurrentIndex(m_argumentsOkPageIdx);

  m_bclMeasure.reset();
  m_currentMeasureStepItem.clear();
  m_model.reset();
  m_reloadPath.reset();

  openstudio::OSAppBase* app = OSAppBase::instance();

  QPointer<LibraryItem> selectedItem = m_localLibraryController->selectedItem();

  if (!selectedItem) {
    return;
  }

  UUID id = selectedItem->uuid();

  try {
    // Get the selected measure

    m_bclMeasure = app->measureManager().getMeasure(id);
    OS_ASSERT(m_bclMeasure);
    OS_ASSERT(m_bclMeasure->measureType() == MeasureType::ModelMeasure);

    m_tempWorkflowJSON.resetMeasurePaths();
    m_tempWorkflowJSON.addMeasurePath(m_bclMeasure->directory().parent_path());

    // Since we set the measure_paths, we only neeed to reference the name of the directory (=last level directory name)
    // eg: /path/to/measure_folder => measure_folder
    MeasureStep step(toString(getLastLevelDirectoryName(m_bclMeasure->directory())));
    std::vector<WorkflowStep> steps;
    steps.push_back(step);
    m_tempWorkflowJSON.setWorkflowSteps(steps);

    m_tempWorkflowJSON.save();

    m_currentMeasureStepItem = QSharedPointer<measuretab::MeasureStepItem>(new measuretab::MeasureStepItem(MeasureType::ModelMeasure, step, app));

    connect(m_currentMeasureStepItem.data(), &measuretab::MeasureStepItem::argumentsChanged, this, &ApplyMeasureNowDialog::disableOkButton);

    m_currentMeasureStepItem->arguments();
    bool hasIncompleteArguments = m_currentMeasureStepItem->hasIncompleteArguments();
    disableOkButton(hasIncompleteArguments);

    //m_currentMeasureStepItem->setName(m_bclMeasure->name().c_str());
    m_currentMeasureStepItem->setName(m_bclMeasure->displayName().c_str());
    //m_currentMeasureStepItem->setDisplayName(m_bclMeasure->displayName().c_str());
    m_currentMeasureStepItem->setDescription(m_bclMeasure->description().c_str());

    m_editController->setMeasureStepItem(m_currentMeasureStepItem.data(), app);

  } catch (const std::exception& e) {
    QString errorMessage("Failed to display measure: \n\n");
    errorMessage += QString::fromStdString(e.what());
    errorMessage.prepend(FAILED_ARG_TEXT);
    m_argumentsFailedTextEdit->setText(errorMessage);
    m_rightPaneStackedWidget->setCurrentIndex(m_argumentsFailedPageIdx);
    return;
  }
}

void ApplyMeasureNowDialog::runMeasure() {
  m_mainPaneStackedWidget->setCurrentIndex(m_runningPageIdx);
  m_timer->start(50);
  this->okButton()->hide();
  this->backButton()->hide();

  // TODO: Unused Variable app
  // openstudio::OSAppBase * app = OSAppBase::instance();

  removeWorkingDir();

  createWorkingDir();

  m_tempWorkflowJSON.save();

  // DLM: should be able to assert this
  bool hasIncompleteArguments = m_currentMeasureStepItem->hasIncompleteArguments();
  OS_ASSERT(!hasIncompleteArguments);

  m_runProcess = new QProcess(this);
  connect(m_runProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
          &ApplyMeasureNowDialog::displayResults);

  // Use OpenStudioApplicationPathHelpers to find the CLI
  QString openstudioExePath = toQString(openstudio::getOpenStudioCoreCLI());

  boost::optional<openstudio::path> tempWorkflowJSONPath = m_tempWorkflowJSON.oswPath();
  OS_ASSERT(tempWorkflowJSONPath);

  QStringList arguments;
  arguments << "run"
            << "-m"
            << "-w" << toQString(*tempWorkflowJSONPath);
  LOG(Debug, "openstudioExePath='" << toString(openstudioExePath) << "'");
  LOG(Debug, "run arguments" << arguments.join(";").toStdString());

  m_runProcess->start(openstudioExePath, arguments);
}

void ApplyMeasureNowDialog::displayResults() {
  QString qstdout;
  QString qstderr;
  if (m_runProcess) {
    qstdout.append(m_runProcess->readAllStandardOutput());
    qstderr.append(m_runProcess->readAllStandardError());
  }

  delete m_runProcess;
  m_runProcess = nullptr;

  m_reloadPath = m_workingDir / toPath("run/in.osm");
  openstudio::path outWorkflowJSONPath = m_workingDir / toPath("out.osw");

  m_mainPaneStackedWidget->setCurrentIndex(m_outputPageIdx);
  m_timer->stop();

  this->okButton()->setText(ACCEPT_CHANGES);
  this->okButton()->show();
  if (boost::filesystem::exists(*m_reloadPath)) {
    this->okButton()->setEnabled(true);
  } else {
    this->okButton()->setEnabled(false);
  }
  this->backButton()->show();
  this->backButton()->setEnabled(true);
  this->cancelButton()->setEnabled(true);

  boost::optional<WorkflowJSON> outWorkflowJSON = WorkflowJSON::load(outWorkflowJSONPath);

  m_jobItemView->update(*m_bclMeasure, outWorkflowJSON, false);
  m_jobItemView->setExpanded(true);

  if (!outWorkflowJSON || !outWorkflowJSON->completedStatus() || outWorkflowJSON->completedStatus().get() != "Success") {
    this->okButton()->setDisabled(true);
  }

  m_advancedOutput.clear();

  try {

    m_advancedOutput = "";

    m_advancedOutput += "<b>Standard Output:</b>\n";
    m_advancedOutput += qstdout;
    m_advancedOutput += QString("\n");

    m_advancedOutput += "<b>Standard Error:</b>\n";
    m_advancedOutput += qstderr;
    m_advancedOutput += QString("\n");

    openstudio::path logPath = m_workingDir / toPath("run/run.log");
    m_advancedOutput += "<b>run.log:</b>\n";
    QFile file(toQString(logPath));
    if (file.open(QFile::ReadOnly)) {
      QTextStream docIn(&file);
      m_advancedOutput += docIn.readAll();
      file.close();
    }

    m_advancedOutput += QString("\n");

    m_advancedOutput.replace("\n", "<br>");

  } catch (std::exception&) {
  }
}

void ApplyMeasureNowDialog::removeWorkingDir() {
  bool test = removeDirectory(m_workingDir);
  OS_ASSERT(test);
}

void ApplyMeasureNowDialog::createWorkingDir() {
  // DLM: makeParentFolder only makes the parent folder, add '.' to force creation of the parent
  bool test = makeParentFolder(m_workingDir / toPath("."), path(), true);
  OS_ASSERT(test);
  test = makeParentFolder(m_workingFilesDir / toPath("."), path(), true);
  OS_ASSERT(test);
}

/***** SLOTS *****/

void ApplyMeasureNowDialog::on_cancelButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_inputPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // TODO: fix
    //    if(m_job){
    //      m_job->requestStop();
    //      this->cancelButton()->setDisabled(true);
    //      this->okButton()->setDisabled(true);
    //      return;
    //    }
    m_mainPaneStackedWidget->setCurrentIndex(m_inputPageIdx);
    m_timer->stop();
    this->okButton()->show();
    this->backButton()->show();
    return;
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    m_mainPaneStackedWidget->setCurrentIndex(m_inputPageIdx);
  }

  // DLM: m_job->requestStop() might still be working, don't try to delete this here
  //removeWorkingDir();

  OSDialog::on_cancelButton(checked);
}

void ApplyMeasureNowDialog::on_backButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_inputPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    this->okButton()->setEnabled(true);
    this->okButton()->setText(APPLY_MEASURE);
    this->backButton()->setEnabled(false);
    m_mainPaneStackedWidget->setCurrentIndex(m_inputPageIdx);
  }
}

void ApplyMeasureNowDialog::on_okButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_inputPageIdx) {
    runMeasure();
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // N/A
    OS_ASSERT(false);
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    // reload the model
    requestReload();
  }
}

void ApplyMeasureNowDialog::requestReload() {
  // copy any files created in m_workingFilesDir
  std::vector<path> filePaths = m_modelWorkflowJSON.absoluteFilePaths();
  if (!filePaths.empty()) {
    copyDirectory(m_workingFilesDir, filePaths[0]);
  }

  // todo: do this in memory without reloading from disk
  OS_ASSERT(m_reloadPath);
  QString fileToLoad = toQString(*m_reloadPath);
  emit reloadFile(fileToLoad, true, true);

  // close the dialog
  close();
}

void ApplyMeasureNowDialog::closeEvent(QCloseEvent* event) {
  //DLM: don't do this here in case we are going to load the model
  //removeWorkingDir();

  // DLM: do not allow closing window while running
  if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    event->ignore();
    return;
  }

  event->accept();
}

void ApplyMeasureNowDialog::disableOkButton(bool disable) {
  this->okButton()->setDisabled(disable);
}

void ApplyMeasureNowDialog::showAdvancedOutput() {
  if (m_advancedOutput.isEmpty()) {
    QMessageBox::information(this, QString("Advanced Output"), QString("No advanced output."));
  } else {
    m_advancedOutputDialog->setText(m_advancedOutput);
    m_advancedOutputDialog->setSizeHint(QSize(this->geometry().width(), this->geometry().height()));
    m_advancedOutputDialog->exec();
    m_advancedOutputDialog->raise();
  }
}

}  // namespace openstudio
