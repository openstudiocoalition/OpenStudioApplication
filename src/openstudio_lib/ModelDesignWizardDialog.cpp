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

#include "ModelDesignWizardDialog.hpp"

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
#include <QGridLayout>
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
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLocale>

#include <array>
#include <fstream>
#include <string_view>

#define FAILED_ARG_TEXT "<FONT COLOR = RED>Failed to Show Arguments<FONT COLOR = BLACK> <br> <br>Reason(s): <br> <br>"

#define ACCEPT_CHANGES openstudio::ModelDesignWizardDialog::tr("Accept Changes")
#define GENERATE_MODEL openstudio::ModelDesignWizardDialog::tr("Generate Model")
#define NEXT_PAGE openstudio::ModelDesignWizardDialog::tr("Next Page")

namespace openstudio {

ModelDesignWizardDialog::ModelDesignWizardDialog(QWidget* parent)
  : OSDialog(false, parent),
    m_mainPaneStackedWidget(nullptr),
    m_rightPaneStackedWidget(nullptr),
    m_argumentsFailedTextEdit(nullptr),
    m_jobItemView(nullptr),
    m_timer(nullptr),
    m_showAdvancedOutput(nullptr),
    m_advancedOutputDialog(nullptr) {
  setWindowTitle("Apply Measure Now");
  setWindowModality(Qt::ApplicationModal);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setSizeGripEnabled(true);

  // Load support JSON (has to be before createWidgets)
  QFile file(":/library/ModelDesignWizard.json");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    auto supportJson = QJsonDocument::fromJson(QString(file.readAll()).toUtf8());
    m_supportJsonObject = supportJson.object();
    file.close();
  } else {
    LOG(LogLevel::Error, "Failed to open embedded ModelDesignWizard.json");
  }

  // Set the Locale to C, so that "1234.56" is accepted, but not "1234,56", no matter the user's system locale
  const QLocale lo(QLocale::C);

  m_ratioValidator = new QDoubleValidator(0.0, 1.0, 4);
  m_ratioValidator->setLocale(lo);

  m_positiveDoubleValidator = new QDoubleValidator();
  m_positiveDoubleValidator->setBottom(0.0);
  m_positiveDoubleValidator->setLocale(lo);

  createWidgets();

  OSAppBase* app = OSAppBase::instance();
  connect(this, &ModelDesignWizardDialog::reloadFile, app, &OSAppBase::reloadFile, Qt::QueuedConnection);

  m_advancedOutputDialog = new TextEditDialog("Advanced Output");

  //m_workingDir = toPath("E:/test/ApplyMeasureNow");
  m_workingDir = openstudio::toPath(app->currentDocument()->modelTempDir()) / openstudio::toPath("ModelDesignWizard");

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

  std::vector<WorkflowStep> steps;

  auto create_bar_ = app->currentDocument()->createBarFromSpaceTypeRatiosMeasure();
  if (create_bar_) {
    try {
      std::pair<bool, std::string> result = OSAppBase::instance()->measureManager().updateMeasure(*create_bar_);
      if (result.first) {
        // have to reload in case measure manager updated
        create_bar_ = BCLMeasure::load(create_bar_->directory());
        OS_ASSERT(create_bar_);

        MeasureStep srmStep(result.second);
        // DLM: moved to WorkflowStepResult
        //srmStep.setMeasureId(srm->uid());
        //srmStep.setVersionId(srm->versionId());
        //std::vector<std::string> tags = srm->tags();
        //if (!tags.empty()){
        //  srmStep.setTaxonomy(tags[0]);
        //}
        srmStep.setName(create_bar_->displayName());
        srmStep.setDescription(create_bar_->description());
        srmStep.setModelerDescription(create_bar_->modelerDescription());
        steps.push_back(srmStep);
      }
    } catch (const std::exception&) {
      QMessageBox::warning(parent, "Failed to Compute Arguments", "Could not compute arguments for OpenStudio Results Measure.");
      return;
    }
  }
  m_tempWorkflowJSON.setWorkflowSteps(steps);

  m_tempWorkflowJSON.saveAs(m_workingDir / toPath("temp.osw"));

  // temporarily swap app's workflow for this one
  app->currentModel()->setWorkflowJSON(m_tempWorkflowJSON);
}

ModelDesignWizardDialog::~ModelDesignWizardDialog() {
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

QSize ModelDesignWizardDialog::sizeHint() const {
  return QSize(770, 560);
}

QWidget* ModelDesignWizardDialog::createTemplateSelectionPage() {
  auto* widget = new QWidget();
  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  widget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();
  {
    int col = 0;

    {
      auto* standardTypeLabel = new QLabel("Standard Template Type:");
      standardTypeLabel->setObjectName("H2");
      mainGridLayout->addWidget(standardTypeLabel, row, col++, 1, 1);
    }
    {
      auto* targetStandardLabel = new QLabel("Target Standard:");
      targetStandardLabel->setObjectName("H2");
      mainGridLayout->addWidget(targetStandardLabel, row, col++, 1, 1);
    }
    {
      auto* primaryBuildingTypeLabel = new QLabel("Primary Building Template Type:");
      primaryBuildingTypeLabel->setObjectName("H2");
      mainGridLayout->addWidget(primaryBuildingTypeLabel, row, col++, 1, 1);
    }
  }

  ++row;
  {
    int col = 0;
    {
      m_standardTypeComboBox = new QComboBox();
      qDebug() << "m_supportJsonObject.keys()=" << m_supportJsonObject.keys();
      for (const QString& standardType : m_supportJsonObject.keys()) {
        qDebug() << "Adding standardType=" << standardType;

        m_standardTypeComboBox->addItem(standardType);
      }
      m_standardTypeComboBox->setCurrentIndex(0);
      mainGridLayout->addWidget(m_standardTypeComboBox, row, col++, 1, 1);
      const bool isConnected = connect(m_standardTypeComboBox, &QComboBox::currentTextChanged, this, &ModelDesignWizardDialog::onStandardTypeChanged);
      OS_ASSERT(isConnected);
    }

    {
      m_targetStandardComboBox = new QComboBox();
      mainGridLayout->addWidget(m_targetStandardComboBox, row, col++, 1, 1);
      const bool isConnected =
        connect(m_targetStandardComboBox, &QComboBox::currentTextChanged, this, &ModelDesignWizardDialog::onTargetStandardChanged);
      OS_ASSERT(isConnected);
    }

    {
      m_primaryBuildingTypeComboBox = new QComboBox();
      mainGridLayout->addWidget(m_primaryBuildingTypeComboBox, row, col++, 1, 1);
      const bool isConnected =
        connect(m_primaryBuildingTypeComboBox, &QComboBox::currentTextChanged, this, &ModelDesignWizardDialog::onPrimaryBuildingTypeChanged);
      OS_ASSERT(isConnected);
    }
  }

  m_standardTypeComboBox->setCurrentText("DOE");
  mainGridLayout->setRowStretch(mainGridLayout->rowCount(), 100);

  return widget;
}

void ModelDesignWizardDialog::onStandardTypeChanged(const QString& /*text*/) {
  populateTargetStandards();
  populatePrimaryBuildingTypes();
}

void ModelDesignWizardDialog::populateStandardTypes() {}

void ModelDesignWizardDialog::onTargetStandardChanged(const QString& /*text*/) {
  disableOkButton(m_targetStandardComboBox->currentText().isEmpty() || m_primaryBuildingTypeComboBox->currentText().isEmpty());
}

void ModelDesignWizardDialog::populateTargetStandards() {
  m_targetStandardComboBox->blockSignals(true);

  m_targetStandardComboBox->clear();

  m_targetStandardComboBox->addItem("");

  const QString selectedStandardType = m_standardTypeComboBox->currentText();

  for (const auto& temp : m_supportJsonObject[selectedStandardType].toObject()["templates"].toArray()) {
    m_targetStandardComboBox->addItem(temp.toString());
  }

  m_targetStandardComboBox->setCurrentIndex(0);

  m_targetStandardComboBox->blockSignals(false);
}

void ModelDesignWizardDialog::onPrimaryBuildingTypeChanged(const QString& /*text*/) {
  const bool disabled = m_targetStandardComboBox->currentText().isEmpty() || m_primaryBuildingTypeComboBox->currentText().isEmpty();
  disableOkButton(disabled);
  if (!disabled) {
    populateSpaceTypeRatiosPage();
  }
}

void ModelDesignWizardDialog::populateBuildingTypeComboBox(QComboBox* comboBox) {
  comboBox->blockSignals(true);

  comboBox->clear();

  comboBox->addItem("");

  const QString selectedStandardType = m_standardTypeComboBox->currentText();

  for (const auto& temp : m_supportJsonObject[selectedStandardType].toObject()["building_types"].toArray()) {
    comboBox->addItem(temp.toString());
  }

  comboBox->setCurrentIndex(0);

  comboBox->blockSignals(false);
}

void ModelDesignWizardDialog::populatePrimaryBuildingTypes() {
  populateBuildingTypeComboBox(m_primaryBuildingTypeComboBox);
}

void ModelDesignWizardDialog::populateSpaceTypeComboBox(QComboBox* comboBox, const QString& buildingType) {

  comboBox->blockSignals(true);

  comboBox->clear();

  comboBox->addItem("");

  const QString selectedStandardType = m_standardTypeComboBox->currentText();
  const QString selectedStandard = m_targetStandardComboBox->currentText();

  for (const QString& temp :
       m_supportJsonObject[selectedStandardType].toObject()["space_types"].toObject()[selectedStandard].toObject()[buildingType].toObject().keys()) {
    comboBox->addItem(temp);
  }

  comboBox->setCurrentIndex(0);

  comboBox->blockSignals(false);
}

void ModelDesignWizardDialog::populateSpaceTypeRatiosPage() {

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  m_spaceTypeRatiosPageWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();
  {
    int col = 0;

    {
      auto* totalBuildingFloorAreaLabel = new QLabel("Total Building Floor Area:");
      totalBuildingFloorAreaLabel->setObjectName("H2");
      mainGridLayout->addWidget(totalBuildingFloorAreaLabel, row, col++, 1, 1);
    }
    {
      auto* totalBuildingFloorAreaEdit = new QLineEdit();
      totalBuildingFloorAreaEdit->setValidator(m_positiveDoubleValidator);
      mainGridLayout->addWidget(totalBuildingFloorAreaEdit, row, col++, 1, 1);
      totalBuildingFloorAreaEdit->setText(QString::number(10000.0));
    }
  }

  ++row;
  {
    int col = 0;
    {
      auto* buildingTypelabel = new QLabel("Building Type:");
      buildingTypelabel->setObjectName("H2");
      mainGridLayout->addWidget(buildingTypelabel, row, col++, 1, 1);
    }
    {
      auto* spaceTypelabel = new QLabel("Space Type:");
      spaceTypelabel->setObjectName("H2");
      mainGridLayout->addWidget(spaceTypelabel, row, col++, 1, 1);
    }
    {
      auto* ratioLabel = new QLabel("Ratio:");
      ratioLabel->setObjectName("H2");
      mainGridLayout->addWidget(ratioLabel, row, col++, 1, 1);
    }
  }

  const QString selectedStandardType = m_standardTypeComboBox->currentText();
  const QString selectedStandard = m_targetStandardComboBox->currentText();
  const QString selectedPrimaryBuildingType = m_primaryBuildingTypeComboBox->currentText();

  const QJsonObject defaultSpaceTypeRatios = m_supportJsonObject[selectedStandardType]
                                               .toObject()["space_types"]
                                               .toObject()[selectedStandard]
                                               .toObject()[selectedPrimaryBuildingType]
                                               .toObject();
  for (QJsonObject::const_iterator it = defaultSpaceTypeRatios.constBegin(); it != defaultSpaceTypeRatios.constEnd(); ++it) {
    ++row;
    {
      int col = 0;

      auto* buildingTypeComboBox = new QComboBox();
      mainGridLayout->addWidget(buildingTypeComboBox, row, col++, 1, 1);
      populateBuildingTypeComboBox(buildingTypeComboBox);
      buildingTypeComboBox->setCurrentText(selectedPrimaryBuildingType);

      auto* spaceTypeComboBox = new QComboBox();
      mainGridLayout->addWidget(spaceTypeComboBox, row, col++, 1, 1);
      populateSpaceTypeComboBox(spaceTypeComboBox, selectedPrimaryBuildingType);
      spaceTypeComboBox->setCurrentText(it.key());

      auto* spaceTypeRatioEdit = new QLineEdit();
      spaceTypeRatioEdit->setValidator(m_ratioValidator);
      mainGridLayout->addWidget(spaceTypeRatioEdit, row, col++, 1, 1);

      spaceTypeRatioEdit->setText(QString::number(it.value().toObject()["ratio"].toDouble()));

      const bool isConnected = connect(buildingTypeComboBox, &QComboBox::currentTextChanged,
                                       [this, &spaceTypeComboBox](const QString& text) { populateSpaceTypeComboBox(spaceTypeComboBox, text); });
      OS_ASSERT(isConnected);
    }
  }

  mainGridLayout->setRowStretch(mainGridLayout->rowCount(), 100);

  // TODO: add a way to add / delete rows, so one could pick from another building type for eg
}

QWidget* ModelDesignWizardDialog::createSpaceTypeRatiosPage() {
  m_spaceTypeRatiosPageWidget = new QWidget();

  return m_spaceTypeRatiosPageWidget;
}

QWidget* ModelDesignWizardDialog::createRunningPage() {
  auto* widget = new QWidget();

  auto* label = new QLabel("Running Measure");
  label->setObjectName("H2");

  auto* busyWidget = new BusyWidget();

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, busyWidget, &BusyWidget::rotate);

  auto* layout = new QVBoxLayout();
  layout->addStretch();
  layout->addWidget(label, 0, Qt::AlignCenter);
  layout->addWidget(busyWidget, 0, Qt::AlignCenter);
  layout->addStretch();

  widget->setLayout(layout);
  return widget;
}

QWidget* ModelDesignWizardDialog::createOutputPage() {
  auto* widget = new QWidget();

  auto* label = new QLabel("Measure Output");
  label->setObjectName("H1");

  m_jobItemView = new DataPointJobItemView();

  m_jobPath = new QLabel();
  m_jobPath->setTextInteractionFlags(Qt::TextSelectableByMouse);
#if !(_DEBUG || (__GNUC__ && !NDEBUG))
  m_jobPath->hide();
#endif

  auto* layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(m_jobPath);
  layout->addWidget(m_jobItemView, 0, Qt::AlignTop);

  layout->addStretch();

  m_showAdvancedOutput = new QPushButton("Advanced Output");
  connect(m_showAdvancedOutput, &QPushButton::clicked, this, &ModelDesignWizardDialog::showAdvancedOutput);

  //layout->addStretch();

  auto* hLayout = new QHBoxLayout();
  hLayout->addWidget(m_showAdvancedOutput);
  hLayout->addStretch();
  layout->addLayout(hLayout);

  widget->setLayout(layout);

  auto* scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(widget);

  return scrollArea;
}

void ModelDesignWizardDialog::createWidgets() {
  openstudio::OSAppBase* app = OSAppBase::instance();

  // PAGE STACKED WIDGET

  m_mainPaneStackedWidget = new QStackedWidget();
  upperLayout()->addWidget(m_mainPaneStackedWidget);

  // Selection of the template
  m_templateSelectionPageIdx = m_mainPaneStackedWidget->addWidget(createTemplateSelectionPage());

  m_spaceTypeRatiosPageIdx = m_mainPaneStackedWidget->addWidget(createSpaceTypeRatiosPage());

  // RUNNING
  m_runningPageIdx = m_mainPaneStackedWidget->addWidget(createRunningPage());

  // OUTPUT

  m_outputPageIdx = m_mainPaneStackedWidget->addWidget(createOutputPage());

  // SET CURRENT INDEXES
  m_mainPaneStackedWidget->setCurrentIndex(m_templateSelectionPageIdx);

  // BUTTONS

  this->okButton()->setText(NEXT_PAGE);
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

void ModelDesignWizardDialog::resizeEvent(QResizeEvent* event) {
  // Use the QDialog one so it can be resized (OSDialog prevents resizing)
  QDialog::resizeEvent(event);
}

void ModelDesignWizardDialog::runMeasure() {
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
  // bool hasIncompleteArguments = m_currentMeasureStepItem->hasIncompleteArguments();
  // OS_ASSERT(!hasIncompleteArguments);

  m_runProcess = new QProcess(this);
  connect(m_runProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
          &ModelDesignWizardDialog::displayResults);

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

void ModelDesignWizardDialog::displayResults() {
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

void ModelDesignWizardDialog::removeWorkingDir() {
  bool test = removeDirectory(m_workingDir);
  OS_ASSERT(test);
}

void ModelDesignWizardDialog::createWorkingDir() {
  // DLM: makeParentFolder only makes the parent folder, add '.' to force creation of the parent
  bool test = makeParentFolder(m_workingDir / toPath("."), path(), true);
  OS_ASSERT(test);
  test = makeParentFolder(m_workingFilesDir / toPath("."), path(), true);
  OS_ASSERT(test);
}

/***** SLOTS *****/

void ModelDesignWizardDialog::on_cancelButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_templateSelectionPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_spaceTypeRatiosPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // TODO: fix
    //    if(m_job){
    //      m_job->requestStop();
    //      this->cancelButton()->setDisabled(true);
    //      this->okButton()->setDisabled(true);
    //      return;
    //    }
    m_mainPaneStackedWidget->setCurrentIndex(m_templateSelectionPageIdx);
    m_timer->stop();
    this->okButton()->show();
    this->backButton()->show();
    return;
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    m_mainPaneStackedWidget->setCurrentIndex(m_templateSelectionPageIdx);
  }

  // DLM: m_job->requestStop() might still be working, don't try to delete this here
  //removeWorkingDir();

  OSDialog::on_cancelButton(checked);
}

void ModelDesignWizardDialog::on_backButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_templateSelectionPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_spaceTypeRatiosPageIdx) {
    this->backButton()->setEnabled(false);
    this->okButton()->setText(NEXT_PAGE);
    m_mainPaneStackedWidget->setCurrentIndex(m_templateSelectionPageIdx);
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // Nothing specific here
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    this->okButton()->setEnabled(true);
    this->okButton()->setText(ACCEPT_CHANGES);
    this->backButton()->setEnabled(false);
    m_mainPaneStackedWidget->setCurrentIndex(m_templateSelectionPageIdx);
  }
}

void ModelDesignWizardDialog::on_okButton(bool checked) {
  if (m_mainPaneStackedWidget->currentIndex() == m_templateSelectionPageIdx) {
    m_mainPaneStackedWidget->setCurrentIndex(m_spaceTypeRatiosPageIdx);
    this->backButton()->setEnabled(true);
    this->okButton()->setText(GENERATE_MODEL);
  } else if (m_mainPaneStackedWidget->currentIndex() == m_spaceTypeRatiosPageIdx) {
    runMeasure();
  } else if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    // N/A
    OS_ASSERT(false);
  } else if (m_mainPaneStackedWidget->currentIndex() == m_outputPageIdx) {
    // reload the model
    requestReload();
  }
}

void ModelDesignWizardDialog::requestReload() {
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

void ModelDesignWizardDialog::closeEvent(QCloseEvent* event) {
  //DLM: don't do this here in case we are going to load the model
  //removeWorkingDir();

  // DLM: do not allow closing window while running
  if (m_mainPaneStackedWidget->currentIndex() == m_runningPageIdx) {
    event->ignore();
    return;
  }

  event->accept();
}

void ModelDesignWizardDialog::disableOkButton(bool disable) {
  this->okButton()->setDisabled(disable);
}

void ModelDesignWizardDialog::showAdvancedOutput() {
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
