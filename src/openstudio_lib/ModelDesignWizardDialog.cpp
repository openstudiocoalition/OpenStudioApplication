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

#include "../shared_gui_components/Buttons.hpp"
#include "../shared_gui_components/BusyWidget.hpp"
#include "../shared_gui_components/EditController.hpp"
#include "../shared_gui_components/EditView.hpp"
#include "../shared_gui_components/LocalLibraryController.hpp"
#include "../shared_gui_components/LocalLibraryView.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
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
#include <openstudio/utilities/filetypes/WorkflowStep_Impl.hpp>
#include <openstudio/utilities/filetypes/WorkflowStepResult.hpp>
#include <openstudio/utilities/time/DateTime.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
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
#include <QIntValidator>
#include <QLocale>

#include <array>
#include <fstream>
#include <vector>
#include <string_view>

#include <fmt/format.h>

#define FAILED_ARG_TEXT "<FONT COLOR = RED>Failed to Show Arguments<FONT COLOR = BLACK> <br> <br>Reason(s): <br> <br>"

#define ACCEPT_CHANGES openstudio::ModelDesignWizardDialog::tr("Accept Changes")
#define GENERATE_MODEL openstudio::ModelDesignWizardDialog::tr("Generate Model")
#define NEXT_PAGE openstudio::ModelDesignWizardDialog::tr("Next Page")

namespace openstudio {

ModelDesignWizardDialog::ModelDesignWizardDialog(bool isIP, QWidget* parent)
  : OSDialog(false, parent),
    m_isIP(isIP),
    m_mainPaneStackedWidget(nullptr),
    m_rightPaneStackedWidget(nullptr),
    m_argumentsFailedTextEdit(nullptr),
    m_jobItemView(nullptr),
    m_timer(nullptr),
    m_showAdvancedOutput(nullptr),
    m_advancedOutputDialog(nullptr) {
  setWindowTitle("Model Design Wizard");
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

  m_positiveIntValidator = new QIntValidator();
  m_positiveIntValidator->setBottom(0.0);
  m_positiveIntValidator->setLocale(lo);

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

  m_tempWorkflowJSON.setOswDir(m_workingDir);
  m_tempWorkflowJSON.saveAs(m_workingDir / toPath("temp.osw"));
  app->currentModel()->setWorkflowJSON(m_tempWorkflowJSON);

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

  const std::vector<std::pair<QString, boost::optional<BCLMeasure>>> measuresToAdd{
    {"Create Bar From Space Type Ratios", app->currentDocument()->createBarFromSpaceTypeRatiosMeasure()},
    // {"Create Typical Building from Model", app->currentDocument()->createTypicalBuildingFromModelMeasure()},
  };

  for (const auto& [measureName, bclMeasure_] : measuresToAdd) {
    if (!bclMeasure_) {
      QMessageBox::warning(parent, tr("Measure Not Found"), tr("Could not find or download ") + QString("'%1'.").arg(measureName));
      return;
    }
    m_tempWorkflowJSON.addMeasurePath(bclMeasure_->directory().parent_path());

    MeasureStep step(toString(getLastLevelDirectoryName(bclMeasure_->directory())));
    step.setName(bclMeasure_->displayName());
    step.setDescription(bclMeasure_->description());
    step.setModelerDescription(bclMeasure_->modelerDescription());

    m_tempWorkflowJSON.addMeasurePath(bclMeasure_->directory().parent_path());

    steps.push_back(step);
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
  return {770, 560};
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

  ++row;
  m_useIPCheckBox = new QCheckBox("Use IP Units");
  mainGridLayout->addWidget(m_useIPCheckBox, row, 0, 1, 1);
  m_useIPCheckBox->setChecked(m_isIP);
  connect(m_useIPCheckBox, &QCheckBox::stateChanged, this, [this](int state) { m_isIP = state == Qt::Checked; });

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
    qDebug() << "Populate Space Type Ratios";
    populateSpaceTypeRatiosPage();
    populateOtherParamsPage();
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

void ModelDesignWizardDialog::populateSpaceTypeComboBox(QComboBox* comboBox, QString buildingType) {

  comboBox->blockSignals(true);

  comboBox->clear();

  comboBox->addItem("");

  if (buildingType.isEmpty()) {
    buildingType = m_primaryBuildingTypeComboBox->currentText();
  } else {
    const QString selectedStandardType = m_standardTypeComboBox->currentText();
    const QString selectedStandard = m_targetStandardComboBox->currentText();

    for (const QString& temp : m_supportJsonObject[selectedStandardType]
                                 .toObject()["space_types"]
                                 .toObject()[selectedStandard]
                                 .toObject()[buildingType]
                                 .toObject()
                                 .keys()) {
      comboBox->addItem(temp);
    }
  }

  comboBox->setCurrentIndex(0);

  comboBox->blockSignals(false);
}

QGridLayout* ModelDesignWizardDialog::spaceTypeRatiosMainLayout() const {
  return m_spaceTypeRatiosMainLayout;
}

QString ModelDesignWizardDialog::selectedPrimaryBuildingType() const {
  return m_primaryBuildingTypeComboBox->currentText();
}

bool ModelDesignWizardDialog::isIP() const {
  return m_isIP;
}

void ModelDesignWizardDialog::addSpaceTypeRatioRow(const QString& buildingType, const QString& spaceType, double ratio, bool tweakStretch) {

  if (tweakStretch) {
    m_spaceTypeRatiosMainLayout->setRowMinimumHeight(m_spaceTypeRatiosMainLayout->rowCount() - 1, 0);
    qDebug() << "addSpaceTypeRatioRow, After setting setRowMinimumHeight at rowCount - 1: " << m_spaceTypeRatiosMainLayout->rowCount();
    m_spaceTypeRatiosMainLayout->setRowStretch(m_spaceTypeRatiosMainLayout->rowCount() - 1, 0);
    qDebug() << "addSpaceTypeRatioRow, After setting stretch at rowCount - 1: " << m_spaceTypeRatiosMainLayout->rowCount();
  }

  qDebug() << "inside: " << m_spaceTypeRatiosMainLayout;
  qDebug() << "inside: " << m_spaceTypeRatiosMainLayout->rowCount();

  auto* spaceTypeRow = new SpaceTypeRatioRow(this, buildingType, spaceType, ratio);
  m_spaceTypeRatioRows.push_back(spaceTypeRow);
  spaceTypeRow->vectorPos = m_spaceTypeRatioRows.size() - 1;

  // populateBuildingTypeComboBox(spaceTypeRow->buildingTypeComboBox);
  // populateSpaceTypeComboBox(spaceTypeRow->spaceTypeComboBox, buildingType);
  connect(m_useIPCheckBox, &QCheckBox::stateChanged, spaceTypeRow->spaceTypeFloorAreaEdit,
          [this, spaceTypeRow](int state) { spaceTypeRow->onUnitSystemChange(static_cast<bool>(state)); });

  connect(m_showAdvancedOutput, &QPushButton::clicked, this, &ModelDesignWizardDialog::showAdvancedOutput);

  connect(spaceTypeRow->deleteRowButton, &QPushButton::clicked, [this, spaceTypeRow]() { removeSpaceTypeRatioRow(spaceTypeRow); });

  if (tweakStretch) {
    const int rowCount = m_spaceTypeRatiosMainLayout->rowCount();

    m_spaceTypeRatiosMainLayout->setRowMinimumHeight(rowCount, 100);
    qDebug() << "removeSpaceTypeRatioRow, After setting setRowMinimumHeight at rowCount: " << m_spaceTypeRatiosMainLayout->rowCount();
    m_spaceTypeRatiosMainLayout->setRowStretch(rowCount, 100);
    qDebug() << "removeSpaceTypeRatioRow, After setting stretch at rowCount: " << m_spaceTypeRatiosMainLayout->rowCount();
  }
}

void ModelDesignWizardDialog::removeSpaceTypeRatioRow(SpaceTypeRatioRow* row) {

  qDebug() << "\nremoveSpaceTypeRatioRow, Original rowCount: " << m_spaceTypeRatiosMainLayout->rowCount();

  qDebug() << "Removing row at gridLayoutRowIndex=" << row->gridLayoutRowIndex << " and vectorPos=" << row->vectorPos;
  for (int i = 0; auto* spaceTypeRatioRow : m_spaceTypeRatioRows) {
    qDebug() << "* " << i++ << "gridLayoutRowIndex=" << spaceTypeRatioRow->gridLayoutRowIndex << " and vectorPos=" << spaceTypeRatioRow->vectorPos;
  }
  auto it = std::next(m_spaceTypeRatioRows.begin(), row->vectorPos);

#if 1

  //  m_spaceTypeRatiosMainLayout->removeWidget(row->buildingTypeComboBox);
  m_spaceTypeRatiosMainLayout->removeWidget(row->buildingTypeComboBox);
  delete row->buildingTypeComboBox;
  m_spaceTypeRatiosMainLayout->removeWidget(row->spaceTypeComboBox);
  delete row->spaceTypeComboBox;
  m_spaceTypeRatiosMainLayout->removeWidget(row->spaceTypeRatioEdit);
  delete row->spaceTypeRatioEdit;
  m_spaceTypeRatiosMainLayout->removeWidget(row->spaceTypeFloorAreaEdit);
  delete row->spaceTypeFloorAreaEdit;
  m_spaceTypeRatiosMainLayout->removeWidget(row->deleteRowButton);
  delete row->deleteRowButton;

  m_spaceTypeRatiosMainLayout->setRowMinimumHeight(row->gridLayoutRowIndex, 0);
  m_spaceTypeRatiosMainLayout->setRowStretch(row->gridLayoutRowIndex, 0);

#elif 0
  QLayoutItem* child = nullptr;

  while ((child = m_spaceTypeRatiosMainLayout->takeAt(index)) != nullptr) {
    QWidget* widget = child->widget();

    OS_ASSERT(widget);

    delete widget;
    // Using deleteLater is actually slower than calling delete directly on the widget
    // deleteLater also introduces a strange redraw issue where the select all check box
    // is not redrawn, after being checked.
    //widget->deleteLater();

    delete child;
  }
#endif
  m_spaceTypeRatioRows.erase(it);
  qDebug() << "removeSpaceTypeRatioRow, Final rowCount: " << m_spaceTypeRatiosMainLayout->rowCount();
  for (int i = 0; auto* spaceTypeRatioRow : m_spaceTypeRatioRows) {
    spaceTypeRatioRow->vectorPos = i++;
  }
  recalculateTotalBuildingRatio(true);
}

SpaceTypeRatioRow::SpaceTypeRatioRow(ModelDesignWizardDialog* parent, const QString& buildingType, const QString& spaceType, double ratio)
  : buildingTypeComboBox(new QComboBox()),
    spaceTypeComboBox(new QComboBox()),
    spaceTypeRatioEdit(new openstudio::OSNonModelObjectQuantityEdit("", "", "", false)),
    spaceTypeFloorAreaEdit(new openstudio::OSNonModelObjectQuantityEdit("ft^2", "m^2", "ft^2", parent->isIP())),
    deleteRowButton(new openstudio::RemoveButton()),
    gridLayoutRowIndex(parent->spaceTypeRatiosMainLayout()->rowCount()) {

  spaceTypeRatioEdit->setFixedPrecision(4);
  spaceTypeFloorAreaEdit->setFixedPrecision(2);

  int col = 0;

  parent->spaceTypeRatiosMainLayout()->addWidget(buildingTypeComboBox, gridLayoutRowIndex, col++, 1, 1);
  parent->populateBuildingTypeComboBox(buildingTypeComboBox);
  buildingTypeComboBox->setCurrentText(buildingType);

  parent->spaceTypeRatiosMainLayout()->addWidget(spaceTypeComboBox, gridLayoutRowIndex, col++, 1, 1);
  parent->populateSpaceTypeComboBox(spaceTypeComboBox, buildingType);
  spaceTypeComboBox->setCurrentText(spaceType);
  const bool isConnected =
    QComboBox::connect(buildingTypeComboBox, &QComboBox::currentTextChanged, spaceTypeComboBox,
                       [this, parent](const QString& buildingType) { parent->populateSpaceTypeComboBox(spaceTypeComboBox, buildingType); });

  spaceTypeRatioEdit->setMinimumValue(0.0);
  spaceTypeRatioEdit->setMaximumValue(1.0);
  spaceTypeRatioEdit->enableClickFocus();
  // connect(this, &SpaceTypeRatioRow::onUnitSystemChange, spaceTypeRatioEdit, &OSNonModelObjectQuantityEdit::onUnitSystemChange);
  // ModelDesignWizardDialog::connect(parent, &ModelDesignWizardDialog::onUnitSystemChange, spaceTypeRatioEdit,
  //                                 &OSNonModelObjectQuantityEdit::onUnitSystemChange);

  parent->spaceTypeRatiosMainLayout()->addWidget(spaceTypeRatioEdit, gridLayoutRowIndex, col++, 1, 1);

  spaceTypeFloorAreaEdit->setMinimumValue(0.0);
  // spaceTypeFloorAreaEdit->enableClickFocus();
  spaceTypeFloorAreaEdit->setLocked(true);

  parent->spaceTypeRatiosMainLayout()->addWidget(spaceTypeFloorAreaEdit, gridLayoutRowIndex, col++, 1, 1);
  // connect(this, &SpaceTypeRatioRow::onUnitSystemChange, spaceTypeFloorAreaEdit, &OSNonModelObjectQuantityEdit::onUnitSystemChange);

  OSNonModelObjectQuantityEdit::connect(spaceTypeRatioEdit, &OSNonModelObjectQuantityEdit::valueChanged, [this, parent](double newValue) {
    recalculateFloorArea(parent->totalBuildingFloorArea());
    parent->recalculateTotalBuildingRatio(false);
  });
  parent->spaceTypeRatiosMainLayout()->addWidget(deleteRowButton, gridLayoutRowIndex, col++, 1, 1);

  spaceTypeRatioEdit->setDefault(ratio);

  // const bool isConnected = QObject::connect(buildingTypeComboBox, &QComboBox::currentTextChanged, [this, &spaceTypeComboBox](const QString& text) {
  //   parent->populateSpaceTypeComboBox(spaceTypeComboBox, text);
  // });
}

void ModelDesignWizardDialog::recalculateTotalBuildingRatio(bool forceToOne) {
  double totalRatio = 0;
  for (auto* spaceTypeRatioRow : m_spaceTypeRatioRows) {
    totalRatio += spaceTypeRatioRow->spaceTypeRatioEdit->currentValue();
  }
  if (forceToOne) {
    for (auto* spaceTypeRatioRow : m_spaceTypeRatioRows) {
      spaceTypeRatioRow->spaceTypeRatioEdit->blockSignals(true);
      spaceTypeRatioRow->spaceTypeRatioEdit->setCurrentValue(spaceTypeRatioRow->spaceTypeRatioEdit->currentValue() / totalRatio);
      spaceTypeRatioRow->spaceTypeRatioEdit->blockSignals(false);
      spaceTypeRatioRow->spaceTypeRatioEdit->refreshTextAndLabel();
    }
    totalRatio = 1.0;
  }

  m_totalBuildingRatioEdit->setCurrentValue(totalRatio);
}

void ModelDesignWizardDialog::recalculateSpaceTypeFloorAreas() {
  const double totalFloorArea = m_totalBuildingFloorAreaEdit->currentValue();
  for (auto* spaceTypeRatioRow : m_spaceTypeRatioRows) {
    const double ratio = spaceTypeRatioRow->spaceTypeRatioEdit->currentValue();
    spaceTypeRatioRow->spaceTypeFloorAreaEdit->blockSignals(true);
    spaceTypeRatioRow->spaceTypeFloorAreaEdit->setCurrentValue(totalFloorArea * ratio);
    spaceTypeRatioRow->spaceTypeFloorAreaEdit->blockSignals(false);
  }
}

double ModelDesignWizardDialog::totalBuildingFloorArea() const {
  return m_totalBuildingFloorAreaEdit->currentValue();
}

void SpaceTypeRatioRow::onUnitSystemChange(bool isIP) {
  // spaceTypeRatioEdit->onUnitSystemChange(isIP);
  spaceTypeFloorAreaEdit->onUnitSystemChange(isIP);
}

void SpaceTypeRatioRow::recalculateFloorArea(double totalBuildingFloorArea) {
  const double floorArea = spaceTypeRatioEdit->currentValue() * totalBuildingFloorArea;
  spaceTypeFloorAreaEdit->setCurrentValue(floorArea);
}

std::string SpaceTypeRatioRow::toArgumentString() const {
  return fmt::format("{} | {} => {}", toString(buildingTypeComboBox->currentText()), toString(spaceTypeComboBox->currentText()),
                     toString(spaceTypeRatioEdit->currentValue()));
}

std::string ModelDesignWizardDialog::spaceTypeRatiosString() const {
  std::vector<std::string> argumentsStrings;
  argumentsStrings.reserve(m_spaceTypeRatioRows.size());
  std::transform(m_spaceTypeRatioRows.cbegin(), m_spaceTypeRatioRows.cend(), std::back_inserter(argumentsStrings),
                 [](const auto& row) { return row->toArgumentString(); });

  return fmt::format("{}", fmt::join(argumentsStrings, ", "));
}

void ModelDesignWizardDialog::populateOtherParamsPage() {
  const QString selectedStandardType = m_standardTypeComboBox->currentText();
  const QString selectedPrimaryBuildingType = m_primaryBuildingTypeComboBox->currentText();

  const QJsonObject defaultSpaceTypeRatios =
    m_supportJsonObject[selectedStandardType].toObject()["building_form_defaults"].toObject()[selectedPrimaryBuildingType].toObject();

  if (defaultSpaceTypeRatios.isEmpty()) {
    qDebug() << "Object is empty";
    return;
  }

  // Reset the m_currentValue, and set the new default from JSON
  m_aspectRatioEdit->resetValue();
  m_aspectRatioEdit->setDefault(defaultSpaceTypeRatios["aspect_ratio"].toDouble(1.0));

  m_wwrEdit->resetValue();
  m_wwrEdit->setDefault(defaultSpaceTypeRatios["wwr"].toDouble(0.4));

  m_floorHeightEdit->resetValue();
  m_floorHeightEdit->setDefault(defaultSpaceTypeRatios["typical_story"].toDouble(10.0));
}

void ModelDesignWizardDialog::populateSpaceTypeRatiosPage() {

  // TODO: I get a crash when I go back to the templateSelection page and change the PrimaryBuildingType
  if (m_spaceTypeRatiosMainLayout) {
    while (QLayoutItem* item = m_spaceTypeRatiosMainLayout->takeAt(0)) {
      Q_ASSERT(!item->layout());  // otherwise the layout will leak
      delete item->widget();
      delete item;
    }
    delete m_spaceTypeRatiosMainLayout;
  }

  if (auto* existingLayout = m_spaceTypeRatiosPageWidget->layout()) {
    // Reparent the layout to a temporary widget, so we can install the new one, and this one will get deleted because we don't have a reference to it anymore
    QWidget().setLayout(existingLayout);
  }

  m_spaceTypeRatioRows.clear();

  m_spaceTypeRatiosMainLayout = new QGridLayout();
  m_spaceTypeRatiosMainLayout->setContentsMargins(7, 7, 7, 7);
  m_spaceTypeRatiosMainLayout->setSpacing(14);
  m_spaceTypeRatiosPageWidget->setLayout(m_spaceTypeRatiosMainLayout);

  int row = m_spaceTypeRatiosMainLayout->rowCount();
  qDebug() << "Original rowCount" << row;
  {
    int col = 0;

    {
      auto* totalBuildingFloorAreaLabel = new QLabel("Total Building Floor Area:");
      totalBuildingFloorAreaLabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(totalBuildingFloorAreaLabel, row, col++, 1, 1);
    }
    {
      m_totalBuildingFloorAreaEdit = new openstudio::OSNonModelObjectQuantityEdit("ft^2", "m^2", "ft^2", m_isIP);
      m_totalBuildingFloorAreaEdit->setMinimumValue(0.0);
      m_totalBuildingFloorAreaEdit->enableClickFocus();
      m_totalBuildingFloorAreaEdit->setFixedPrecision(2);
      m_totalBuildingFloorAreaEdit->setDefault(10000.0);
      m_spaceTypeRatiosMainLayout->addWidget(m_totalBuildingFloorAreaEdit, row, col++, 1, 1);
      connect(m_useIPCheckBox, &QCheckBox::stateChanged, m_totalBuildingFloorAreaEdit, &OSNonModelObjectQuantityEdit::onUnitSystemChange);
      connect(m_totalBuildingFloorAreaEdit, &OSNonModelObjectQuantityEdit::valueChanged, [this]() { recalculateSpaceTypeFloorAreas(); });
    }
    {
      auto* totalBuildingRatioLabel = new QLabel("Total Ratio:");
      totalBuildingRatioLabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(totalBuildingRatioLabel, row, col++, 1, 1);
    }
    {
      m_totalBuildingRatioEdit = new openstudio::OSNonModelObjectQuantityEdit("", "", "", m_isIP);
      m_totalBuildingRatioEdit->setLocked(true);
      m_totalBuildingRatioEdit->setFixedPrecision(4);
      m_spaceTypeRatiosMainLayout->addWidget(m_totalBuildingRatioEdit, row, col++, 1, 1);
    }
    {
      auto* normalizeToOneButton = new openstudio::AddButton();  // TODO: replace with another icon
      m_spaceTypeRatiosMainLayout->addWidget(normalizeToOneButton, row, col++, 1, 1);
      connect(normalizeToOneButton, &QPushButton::clicked, [this]() { recalculateTotalBuildingRatio(true); });
    }
  }

  ++row;

  auto* addRowButton = new openstudio::AddButton();
  m_spaceTypeRatiosMainLayout->addWidget(addRowButton, row, 0, 1, 1);
  connect(addRowButton, &QPushButton::clicked, [this]() { addSpaceTypeRatioRow(); });

  // TODO: add a way to add / delete rows, so one could pick from another building type for eg

  ++row;
  {
    int col = 0;
    {
      auto* buildingTypelabel = new QLabel("Building Type:");
      buildingTypelabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(buildingTypelabel, row, col++, 1, 1);
    }
    {
      auto* spaceTypelabel = new QLabel("Space Type:");
      spaceTypelabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(spaceTypelabel, row, col++, 1, 1);
    }
    {
      auto* ratioLabel = new QLabel("Ratio:");
      ratioLabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(ratioLabel, row, col++, 1, 1);
    }
    {
      auto* floorAreaLabel = new QLabel("Area:");
      floorAreaLabel->setObjectName("H2");
      m_spaceTypeRatiosMainLayout->addWidget(floorAreaLabel, row, col++, 1, 1);
    }
  }

  qDebug() << "rowCount just before inserting spaceTypeRatio rows" << row;

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
      const QString spaceType = it.key();
      const double ratio = it.value().toObject()["ratio"].toDouble();
      qDebug() << "before: " << m_spaceTypeRatiosMainLayout;
      qDebug() << "before: " << m_spaceTypeRatiosMainLayout->rowCount();
      addSpaceTypeRatioRow(selectedPrimaryBuildingType, spaceType, ratio, false);
    }
  }

  m_spaceTypeRatiosMainLayout->setRowStretch(m_spaceTypeRatiosMainLayout->rowCount(), 100);
}

QWidget* ModelDesignWizardDialog::createSpaceTypeRatiosPage() {
  m_spaceTypeRatiosPageWidget = new QWidget();

  return m_spaceTypeRatiosPageWidget;
}

QWidget* ModelDesignWizardDialog::createOtherBarParamsPage() {

  auto* widget = new QWidget();
  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  widget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();
  int col = 0;

  {
    auto* label = new QLabel("Other Create Bar parameters");
    label->setObjectName("H1");
    mainGridLayout->addWidget(label, row, col++, 1, 1);
  }

  ++row;
  col = 0;
  {
    {
      auto* aspectRatioLabel = new QLabel("Aspect Ratio (length/width):");
      aspectRatioLabel->setObjectName("H2");
      mainGridLayout->addWidget(aspectRatioLabel, row, col++, 1, 1);
    }
    {
      m_aspectRatioEdit = new openstudio::OSNonModelObjectQuantityEdit("", "", "", false);
      m_aspectRatioEdit->setMinimumValue(1.0);
      m_aspectRatioEdit->enableClickFocus();
      m_aspectRatioEdit->setFixedPrecision(1);
      m_aspectRatioEdit->setDefault(1.0);
      mainGridLayout->addWidget(m_aspectRatioEdit, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* wwrLabel = new QLabel("Window To Wall Ratio:");
      wwrLabel->setObjectName("H2");
      mainGridLayout->addWidget(wwrLabel, row, col++, 1, 1);
    }
    {
      m_wwrEdit = new openstudio::OSNonModelObjectQuantityEdit("", "", "", false);
      m_wwrEdit->setMinimumValue(0.0);
      m_wwrEdit->setMaximumValue(1.0);
      m_wwrEdit->enableClickFocus();
      m_wwrEdit->setFixedPrecision(1);
      m_wwrEdit->setDefault(0.4);
      mainGridLayout->addWidget(m_wwrEdit, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* floorHeightLabel = new QLabel("Floor to Floor Height:");
      floorHeightLabel->setObjectName("H2");
      mainGridLayout->addWidget(floorHeightLabel, row, col++, 1, 1);
    }

    {
      m_floorHeightEdit = new openstudio::OSNonModelObjectQuantityEdit("ft", "m", "ft", m_isIP);
      m_floorHeightEdit->setMinimumValue(1.0);
      m_floorHeightEdit->enableClickFocus();
      m_floorHeightEdit->setFixedPrecision(2);
      m_floorHeightEdit->setDefault(10.0);
      mainGridLayout->addWidget(m_floorHeightEdit, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* storiesAboveGradeLabel = new QLabel("Number of Stories Above Grade:");
      storiesAboveGradeLabel->setObjectName("H2");
      mainGridLayout->addWidget(storiesAboveGradeLabel, row, col++, 1, 1);
    }
    {
      m_numStoriesAboveGradeLineEdit = new QLineEdit();
      m_numStoriesAboveGradeLineEdit->setValidator(m_positiveIntValidator);
      m_numStoriesAboveGradeLineEdit->setText("1");
      mainGridLayout->addWidget(m_numStoriesAboveGradeLineEdit, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* storiesBelowGradeLabel = new QLabel("Number of Stories Below Grade:");
      storiesBelowGradeLabel->setObjectName("H2");
      mainGridLayout->addWidget(storiesBelowGradeLabel, row, col++, 1, 1);
    }
    {
      m_numStoriesBelowGradeLineEdit = new QLineEdit();
      m_numStoriesBelowGradeLineEdit->setValidator(m_positiveIntValidator);
      m_numStoriesBelowGradeLineEdit->setText("0");
      mainGridLayout->addWidget(m_numStoriesBelowGradeLineEdit, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* floorMultiplierLabel = new QLabel("Use a Floor Multiplier?");
      floorMultiplierLabel->setObjectName("H2");
      mainGridLayout->addWidget(floorMultiplierLabel, row, col++, 1, 1);
    }
    {
      m_floorMultiplierSwitch = new QPushButton();
      m_floorMultiplierSwitch->setAcceptDrops(false);
      m_floorMultiplierSwitch->setFlat(true);
      m_floorMultiplierSwitch->setFixedSize(63, 21);
      m_floorMultiplierSwitch->setCheckable(true);
      m_floorMultiplierSwitch->setEnabled(false);
      m_floorMultiplierSwitch->setObjectName("OnOffSliderButton");
      mainGridLayout->addWidget(m_floorMultiplierSwitch, row, col++, 1, 1);
    }
  }

  ++row;
  col = 0;
  {
    {
      auto* midStoryAdiabLabel = new QLabel("Make Mid Stories adiabatic?");
      midStoryAdiabLabel->setObjectName("H2");
      mainGridLayout->addWidget(midStoryAdiabLabel, row, col++, 1, 1);
    }
    {
      m_midStoryAdiabSwitch = new QPushButton();
      m_midStoryAdiabSwitch->setAcceptDrops(false);
      m_midStoryAdiabSwitch->setFlat(true);
      m_midStoryAdiabSwitch->setFixedSize(63, 21);
      m_midStoryAdiabSwitch->setCheckable(true);
      m_midStoryAdiabSwitch->setEnabled(false);
      m_midStoryAdiabSwitch->setObjectName("OnOffSliderButton");
      mainGridLayout->addWidget(m_midStoryAdiabSwitch, row, col++, 1, 1);
      // connect(m_midStoryAdiabSwitch, &QPushButton::clicked, [this](bool checked) { m_midStoryAdiab = checked; });
    }
  }

  // connect(m_numStoriesAboveGradeLineEdit, &QLineEdit::editingFinished, [this]() {
  //   bool ok = false;
  //   int numStoriesAboveGrade = m_numStoriesAboveGradeLineEdit->text().toInt(&ok);
  //   if (ok) {
  //     bool enabled = numStoriesAboveGrade > 3;
  //     m_floorMultiplierSwitch->setEnabled(enabled);
  //     m_midStoryAdiabSwitch->setEnabled(enabled);
  //   }
  // });

  ++row;
  mainGridLayout->setRowStretch(row, 100);
  mainGridLayout->setColumnStretch(col, 100);
  return widget;
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

  m_otherBarParamsPageIdx = m_mainPaneStackedWidget->addWidget(createOtherBarParamsPage());

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

  m_jobItemView->update("Model Design Wizard", outWorkflowJSON, false);
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
  } else if (m_mainPaneStackedWidget->currentIndex() == m_otherBarParamsPageIdx) {
    this->backButton()->setEnabled(true);
    this->okButton()->setText(NEXT_PAGE);
    m_mainPaneStackedWidget->setCurrentIndex(m_spaceTypeRatiosPageIdx);
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
    this->okButton()->setText(NEXT_PAGE);
  } else if (m_mainPaneStackedWidget->currentIndex() == m_spaceTypeRatiosPageIdx) {
    m_mainPaneStackedWidget->setCurrentIndex(m_otherBarParamsPageIdx);
    this->backButton()->setEnabled(true);
    this->okButton()->setText(GENERATE_MODEL);

    // Force ratio to be 1.0
    recalculateTotalBuildingRatio(true);

    auto step = m_tempWorkflowJSON.workflowSteps().front().cast<openstudio::MeasureStep>();
    // auto step = m_tempWorkflowJSON.currentStep()->cast<openstudio::MeasureStep>();

    step.setArgument("template", toString(m_targetStandardComboBox->currentText()));
    step.setArgument("space_type_hash_string", spaceTypeRatiosString());
    step.setArgument("total_bldg_floor_area", m_totalBuildingFloorAreaEdit->currentValue());
    m_tempWorkflowJSON.save();

  } else if (m_mainPaneStackedWidget->currentIndex() == m_otherBarParamsPageIdx) {

    auto step = m_tempWorkflowJSON.workflowSteps().front().cast<openstudio::MeasureStep>();
    // auto step = m_tempWorkflowJSON.currentStep()->cast<openstudio::MeasureStep>();

    step.setArgument("ns_to_ew_ratio", m_aspectRatioEdit->currentValue());
    step.setArgument("wwr", m_wwrEdit->currentValue());
    step.setArgument("floor_height", m_floorHeightEdit->currentValue());

    bool ok = false;
    int numStoriesAboveGrade = m_numStoriesAboveGradeLineEdit->text().toInt(&ok);
    step.setArgument("num_stories_above_grade", numStoriesAboveGrade);
    OS_ASSERT(ok);
    ok = false;
    int numStoriesBelowGrade = m_numStoriesBelowGradeLineEdit->text().toInt(&ok);
    OS_ASSERT(ok);
    step.setArgument("num_stories_below_grade", numStoriesBelowGrade);

    if (numStoriesAboveGrade > 3) {
      const std::string story_multiplier = m_floorMultiplierSwitch->isChecked() ? "Basements Ground Mid Top" : "None";
      step.setArgument("story_multiplier", story_multiplier);

      step.setArgument("make_mid_story_surfaces_adiabatic", m_midStoryAdiabSwitch->isChecked());
    }

    // step.setArgument("make_mid_story_surfaces_adiabatic", m_midStoryAdiabCheckBox->checked());

    m_tempWorkflowJSON.save();

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

  // Save a copy of the OSW for inspection of the arguments
  m_tempWorkflowJSON.clone().saveAs(m_workingFilesDir / "create_bar_in.osw");

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
