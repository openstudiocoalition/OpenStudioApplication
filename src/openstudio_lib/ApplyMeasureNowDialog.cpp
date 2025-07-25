/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
#include "MainWindow.hpp"
#include "OSItem.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include "../openstudio_app/OpenStudioApp.hpp"
#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/PathHelpers.hpp>
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

#define LOADING_ARG_TEXT "<FONT COLOR = BLACK>Loading Arguments..."
#define FAILED_ARG_TEXT "<FONT COLOR = RED>Failed to Show Arguments<FONT COLOR = BLACK> <br> <br>Reason(s): <br> <br>"

#define ACCEPT_CHANGES "Accept Changes"
#define APPLY_MEASURE "Apply Measure"

namespace openstudio {

ApplyMeasureNowDialog::ApplyMeasureNowDialog(QWidget* parent)
  : OSDialog(false, parent),
    m_editController(nullptr),
    m_mainPaneStackedWidget(nullptr),
    m_rightPaneStackedWidget(nullptr),
    m_argumentsLoadingTextEdit(nullptr),
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

  m_argumentsLoadingTextEdit = new QTextEdit(LOADING_ARG_TEXT);
  m_argumentsLoadingTextEdit->setReadOnly(true);

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
  m_argumentsLoadingPageIdx = m_rightPaneStackedWidget->addWidget(m_argumentsLoadingTextEdit);
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
  std::unique_lock lock(m_displayMutex, std::try_to_lock);
  if (!lock.owns_lock()) {
    return;
  }

  this->okButton()->setText(APPLY_MEASURE);
  this->okButton()->show();
  this->okButton()->setEnabled(false);

  m_rightPaneStackedWidget->setCurrentIndex(m_argumentsLoadingPageIdx);

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
    m_rightPaneStackedWidget->setCurrentIndex(m_argumentsOkPageIdx);

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
  if (OSAppBase::instance()->currentDocument()->mainWindow()->useClassicCLI()) {
    arguments << "classic";
  }
  arguments << "run" << "-m" << "-w" << toQString(*tempWorkflowJSONPath);
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
  boost::system::error_code ec;
  if (boost::filesystem::exists(*m_reloadPath, ec)) {
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

DataPointJobHeaderView::DataPointJobHeaderView()
  : OSHeader(new HeaderToggleButton()),
    m_name(nullptr),
    m_lastRunTime(nullptr),
    m_status(nullptr),
    m_na(nullptr),
    m_warnings(nullptr),
    m_errors(nullptr) {
  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(15, 5, 5, 5);
  mainHLayout->setSpacing(5);
  mainHLayout->setAlignment(Qt::AlignLeft);
  setLayout(mainHLayout);

  mainHLayout->addWidget(toggleButton);

  m_name = new QLabel();

  mainHLayout->addWidget(m_name);
  mainHLayout->addStretch();

  m_lastRunTime = new QLabel();
  m_lastRunTime->setFixedWidth(150);
  mainHLayout->addWidget(m_lastRunTime);

  m_status = new QLabel();
  m_status->setFixedWidth(75);
  mainHLayout->addWidget(m_status);

  m_na = new QLabel();
  m_na->setFixedWidth(50);
  mainHLayout->addWidget(m_na);

  m_warnings = new QLabel();
  m_warnings->setFixedWidth(75);
  mainHLayout->addWidget(m_warnings);

  m_errors = new QLabel();
  m_errors->setFixedWidth(75);
  mainHLayout->addWidget(m_errors);
}

void DataPointJobHeaderView::setName(const std::string& name) {
  m_name->setText(toQString(name));
}

void DataPointJobHeaderView::setLastRunTime(const boost::optional<openstudio::DateTime>& lastRunTime) {
  if (lastRunTime) {
    std::string s = lastRunTime->toString();
    m_lastRunTime->setText(toQString(s));
  } else {
    m_lastRunTime->setText("Not Started");
  }
}

void DataPointJobHeaderView::setStatus(const std::string& status, bool isCanceled) {
  if (!isCanceled) {
    m_status->setText(toQString(status));
  } else {
    m_status->setText("Canceled");
  }
}

void DataPointJobHeaderView::setNA(bool na) {
  QString text;
  QString naStyle;
  if (na) {
    text = "   NA";
    naStyle = "QLabel { color : #C47B06; }";
  }
  m_na->setText(text);
  m_na->setStyleSheet(naStyle);
}

void DataPointJobHeaderView::setNumWarnings(unsigned numWarnings) {
  QString warningsStyle;
  if (numWarnings > 0) {
    warningsStyle = "QLabel { color : #C47B06; }";
  }
  m_warnings->setText(QString::number(numWarnings) + QString(numWarnings == 1 ? " Warning" : " Warnings"));
  m_warnings->setStyleSheet(warningsStyle);
}

void DataPointJobHeaderView::setNumErrors(unsigned numErrors) {
  QString errorsStyle;
  if (numErrors > 0) {
    errorsStyle = "QLabel { color : red; }";
  }
  m_errors->setText(QString::number(numErrors) + QString(numErrors == 1 ? " Error" : " Errors"));
  m_errors->setStyleSheet(errorsStyle);
}

DataPointJobContentView::DataPointJobContentView() : QWidget(), m_textEdit(nullptr) {
  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(15, 5, 5, 5);
  mainHLayout->setSpacing(0);
  mainHLayout->setAlignment(Qt::AlignLeft);
  setLayout(mainHLayout);

  m_textEdit = new QLabel();
  m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_textEdit->setWordWrap(true);
  m_textEdit->setOpenExternalLinks(true);

  mainHLayout->addWidget(m_textEdit);
}

void DataPointJobContentView::clear() {
  m_textEdit->setText("");
}

QString DataPointJobContentView::formatMessageForHTML(const std::string& t_message) {
  QString str = QString::fromStdString(t_message);
  str.replace("\n", "<br>");
  return str;
}

void DataPointJobContentView::addInitialConditionMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:blue\">Initial Condition</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addFinalConditionMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:blue\">Final Condition</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addInfoMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:green\">Info</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addWarningMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:#C47B06\">Warning</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addErrorMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += QString("<b style=\"color:red\">Error</b>: ") + formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

void DataPointJobContentView::addStdErrorMessage(const std::string& message) {
  QString html = m_textEdit->text();
  html += formatMessageForHTML(message) + QString("<br></br>");
  m_textEdit->setText(html);
}

DataPointJobItemView::DataPointJobItemView() : OSCollapsibleView(true), m_dataPointJobHeaderView(nullptr), m_dataPointJobContentView(nullptr) {
  setStyleSheet("openstudio--pat--DataPointJobItemView { background: #C3C3C3; margin-left:10px; }");

  m_dataPointJobHeaderView = new DataPointJobHeaderView();
  setHeader(m_dataPointJobHeaderView);

  m_dataPointJobContentView = new DataPointJobContentView();
  setContent(m_dataPointJobContentView);
}

void DataPointJobItemView::paintEvent(QPaintEvent* e) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DataPointJobItemView::update(const BCLMeasure& bclMeasure, const boost::optional<WorkflowJSON>& outWorkflowJSON, bool canceled) {
  OS_ASSERT(m_dataPointJobHeaderView);

  m_dataPointJobHeaderView->setName(bclMeasure.className());

  m_dataPointJobHeaderView->m_na->setText("");
  m_dataPointJobHeaderView->m_warnings->setText("");
  m_dataPointJobHeaderView->m_errors->setText("");

  OS_ASSERT(m_dataPointJobContentView);
  m_dataPointJobContentView->clear();

  if (!outWorkflowJSON) {
    // unknown error
    return;
  }

  if (!outWorkflowJSON->completedStatus() || outWorkflowJSON->completedStatus().get() != "Success") {
    // error
  }

  boost::optional<DateTime> completedAt = outWorkflowJSON->completedAt();
  if (completedAt) {
    m_dataPointJobHeaderView->setLastRunTime(*completedAt);
  }

  boost::optional<std::string> completedStatus = outWorkflowJSON->completedStatus();
  if (completedStatus) {
    m_dataPointJobHeaderView->setStatus(*completedStatus, canceled);
  } else {
    m_dataPointJobHeaderView->setStatus("Unknown", canceled);
  }

  for (const auto& step : outWorkflowJSON->workflowSteps()) {

    boost::optional<WorkflowStepResult> result = step.result();
    if (!result) {
      continue;
    }

    boost::optional<std::string> initialCondition = result->stepInitialCondition();
    if (initialCondition) {
      m_dataPointJobContentView->addInitialConditionMessage(*initialCondition);
    }

    boost::optional<std::string> finalCondition = result->stepFinalCondition();
    if (finalCondition) {
      m_dataPointJobContentView->addFinalConditionMessage(*finalCondition);
    }

    std::vector<std::string> errors = result->stepErrors();
    m_dataPointJobHeaderView->setNumErrors(errors.size());
    for (const std::string& errorMessage : errors) {
      m_dataPointJobContentView->addErrorMessage(errorMessage);
    }

    std::vector<std::string> warnings = result->stepWarnings();
    m_dataPointJobHeaderView->setNumWarnings(warnings.size());
    for (const std::string& warningMessage : warnings) {
      m_dataPointJobContentView->addWarningMessage(warningMessage);
    }

    std::vector<std::string> infos = result->stepInfo();
    // m_dataPointJobHeaderView->setNumInfos(infos.size());
    for (const std::string& info : infos) {
      m_dataPointJobContentView->addInfoMessage(info);
    }

    // there should only be on step so this is ok
    boost::optional<StepResult> stepResult = result->stepResult();
    if (stepResult && stepResult->value() == StepResult::NA) {
      m_dataPointJobHeaderView->setNA(true);
    } else {
      m_dataPointJobHeaderView->setNA(false);
    }
  }
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
