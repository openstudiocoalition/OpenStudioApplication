/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "WorkflowController.hpp"
#include "WorkflowView.hpp"
#include "MeasureDragData.hpp"
#include "MeasureManager.hpp"
#include "EditController.hpp"
#include "BaseApp.hpp"
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"
#include "../openstudio_lib/MainWindow.hpp"
#include "LocalLibraryController.hpp"
#include "WorkflowTools.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/energyplus/ForwardTranslator.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/core/Containers.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep_Impl.hpp>

#include <QByteArray>
#include <QDialog>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QRadioButton>
#include <utility>

namespace openstudio {

namespace measuretab {

WorkflowController::WorkflowController(BaseApp* t_baseApp) {
  QSharedPointer<WorkflowSectionItem> workflowSectionItem;

  workflowSectionItem = QSharedPointer<WorkflowSectionItem>(new WorkflowSectionItem(MeasureType::ModelMeasure, "OpenStudio Measures", t_baseApp));
  addItem(workflowSectionItem);

  workflowSectionItem =
    QSharedPointer<WorkflowSectionItem>(new WorkflowSectionItem(MeasureType::EnergyPlusMeasure, "EnergyPlus Measures", t_baseApp));
  addItem(workflowSectionItem);

  workflowSectionItem = QSharedPointer<WorkflowSectionItem>(new WorkflowSectionItem(MeasureType::ReportingMeasure, "Reporting Measures", t_baseApp));
  addItem(workflowSectionItem);
}

void WorkflowController::addItem(QSharedPointer<OSListItem> item) {
  if (QSharedPointer<WorkflowSectionItem> workflowSectionItem = item.dynamicCast<WorkflowSectionItem>()) {
    m_workflowSectionItems.push_back(workflowSectionItem);
    workflowSectionItem->setController(this);
    workflowSectionItem->workflowStepController()->setSelectionController(selectionController());
  }
}

QSharedPointer<OSListItem> WorkflowController::itemAt(int i) {
  if (i >= 0 && i < count()) {
    return m_workflowSectionItems[i];
  }

  return {};
}

int WorkflowController::count() {
  return m_workflowSectionItems.size();
}

WorkflowSectionItem::WorkflowSectionItem(const MeasureType& measureType, const QString& label, BaseApp* t_baseApp)
  : m_label(label), m_measureType(measureType), m_workflowStepController(QSharedPointer<MeasureStepController>::create(measureType, t_baseApp)) {}

QString WorkflowSectionItem::label() const {
  return m_label;
}

QSharedPointer<WorkflowStepController> WorkflowSectionItem::workflowStepController() const {
  return m_workflowStepController;
}

WorkflowSectionItemDelegate::WorkflowSectionItemDelegate() = default;

QWidget* WorkflowSectionItemDelegate::view(QSharedPointer<OSListItem> dataSource) {
  if (QSharedPointer<WorkflowSectionItem> workflowSectionItem = dataSource.objectCast<WorkflowSectionItem>()) {
    QSharedPointer<WorkflowStepController> workflowStepController = workflowSectionItem->workflowStepController();

    if (QSharedPointer<MeasureStepController> measureStepController = qSharedPointerCast<MeasureStepController>(workflowStepController)) {

      QSharedPointer<MeasureStepItemDelegate> measureStepItemDelegate = QSharedPointer<MeasureStepItemDelegate>(new MeasureStepItemDelegate());

      MeasureType measureType = measureStepController->measureType();

      QString acceptedMimeType = MeasureDragData::mimeType(measureType);

      auto* workflowSectionView = new WorkflowSectionView(measureType);

      workflowSectionView->content->newMeasureDropZone->setAcceptedMimeType(acceptedMimeType);

      connect(workflowSectionView->content->newMeasureDropZone, &NewMeasureDropZone::dataDropped, measureStepController.data(),
              &MeasureStepController::addItemForDroppedMeasure);

      workflowSectionView->content->workflowStepsView->setListController(measureStepController);
      workflowSectionView->content->workflowStepsView->setDelegate(measureStepItemDelegate);

      workflowSectionView->header->label->setText(workflowSectionItem->label());

      return workflowSectionView;
    }
  }

  return new QWidget();
}

WorkflowStepController::WorkflowStepController(openstudio::BaseApp* /*t_app*/) {}

MeasureStepController::MeasureStepController(MeasureType measureType, openstudio::BaseApp* t_baseApp)
  : WorkflowStepController(t_baseApp), m_measureType(measureType), m_app(t_baseApp) {}

MeasureType MeasureStepController::measureType() const {
  return m_measureType;
}

std::vector<MeasureStep> MeasureStepController::measureSteps() const {
  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
  std::vector<MeasureStep> result = workflowJSON.getMeasureSteps(m_measureType);
  return result;
}

QSharedPointer<OSListItem> MeasureStepController::itemAt(int i) {
  std::vector<MeasureStep> steps = this->measureSteps();

  if (i >= 0 && i < (int)steps.size()) {
    MeasureStep step = steps[i];

    QSharedPointer<MeasureStepItem> item = QSharedPointer<MeasureStepItem>(new MeasureStepItem(m_measureType, step, m_app));

    item->setController(this);

    return item;
  }

  return QSharedPointer<MeasureStepItem>();
}

int MeasureStepController::count() {
  return measureSteps().size();
}

void MeasureStepController::removeItemForStep(MeasureStep step) {
  std::vector<MeasureStep> oldMeasureSteps = measureSteps();

  bool didRemove = false;
  bool canDeleteMeasure = true;
  std::vector<MeasureStep> newMeasureSteps;
  newMeasureSteps.reserve(oldMeasureSteps.size());
  for (const auto& oldMeasureStep : oldMeasureSteps) {
    if (oldMeasureStep == step) {
      didRemove = true;
    } else {
      if (oldMeasureStep.measureDirName() == step.measureDirName()) {
        canDeleteMeasure = false;
      }
      newMeasureSteps.push_back(oldMeasureStep);
    }
  }

  if (didRemove) {
    // remove the measure
    if (canDeleteMeasure) {
      boost::optional<openstudio::path> measureDir = m_app->currentModel()->workflowJSON().findMeasure(step.measureDirName());
      if (measureDir && openstudio::filesystem::exists(*measureDir)) {
        openstudio::filesystem::remove_all(*measureDir);
      }
    }

    // set the new steps
    m_app->currentModel()->workflowJSON().setMeasureSteps(m_measureType, newMeasureSteps);

    emit modelReset();
  }
}

void MeasureStepController::addItemForDroppedMeasure(QDropEvent* event) {
  // accept the event to make the icon refresh
  event->accept();

  const QMimeData* mimeData = event->mimeData();

  QByteArray byteArray;

  byteArray = mimeData->data(MeasureDragData::mimeType(m_measureType));

  MeasureDragData measureDragData(byteArray);

  UUID id = measureDragData.id();

  std::shared_ptr<OSDocument> document = nullptr;
  if (dynamic_cast<OSAppBase*>(m_app)) {
    document = dynamic_cast<OSAppBase*>(m_app)->currentDocument();
    document->disable();
  }

  boost::optional<BCLMeasure> projectMeasure;
  try {

    // Get the measure, will throw if error occurs
    projectMeasure = m_app->measureManager().insertReplaceMeasure(id);

  } catch (const std::exception& e) {

    QString errorMessage("Failed to add measure: \n\n");
    errorMessage += QString::fromStdString(e.what());
    QMessageBox::information(m_app->mainWidget(), QString("Failed to add measure"), errorMessage);

    if (document) {
      document->enable();
    }
    return;
  }
  OS_ASSERT(projectMeasure);

  if (projectMeasure->measureType() != m_measureType) {
    QString errorMessage("Failed to add measure at this workflow location.");
    QMessageBox::information(m_app->mainWidget(), QString("Failed to add measure"), errorMessage);

    if (document) {
      document->enable();
    }
    return;
  }

  // Since we set the measure_paths, we only neeed to reference the name of the directory (=last level directory name)
  // eg: /path/to/measure_folder => measure_folder
  MeasureStep measureStep(toString(getLastLevelDirectoryName(projectMeasure->directory())));
  try {
    /* std::vector<measure::OSArgument> arguments = */ m_app->measureManager().getArguments(*projectMeasure);
  } catch (const std::exception& e) {
    QString errorMessage("Failed to compute arguments for measure: \n\n");
    errorMessage += QString::fromStdString(e.what());
    QMessageBox::information(m_app->mainWidget(), QString("Failed to add measure"), errorMessage);

    if (document) {
      document->enable();
    }
    return;
  }

  // the new measure
  std::string name = m_app->measureManager().suggestMeasureName(*projectMeasure);
  // DLM: moved to WorkflowStepResult
  //measureStep.setMeasureId(projectMeasure->uid());
  //measureStep.setVersionId(projectMeasure->versionId());
  //std::vector<std::string> tags = projectMeasure->tags();
  //if (!tags.empty()){
  //  measureStep.setTaxonomy(tags[0]);
  //}
  measureStep.setName(name);
  //measureStep.setDisplayName(name); // DLM: TODO
  measureStep.setDescription(projectMeasure->description());
  measureStep.setModelerDescription(projectMeasure->modelerDescription());

  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();

  std::vector<MeasureStep> newMeasureSteps = workflowJSON.getMeasureSteps(m_measureType);
  newMeasureSteps.push_back(measureStep);
  bool test = workflowJSON.setMeasureSteps(m_measureType, newMeasureSteps);
  OS_ASSERT(test);

  //workflowJSON.save();

  if (document) {
    document->enable();
  }

  emit modelReset();
}

void MeasureStepController::moveUp(MeasureStep step) {
  std::vector<MeasureStep> oldMeasureSteps = measureSteps();

  bool found = false;
  size_t i = 0;
  size_t n = oldMeasureSteps.size();
  for (i = 0; i < n; ++i) {
    if (oldMeasureSteps[i] == step) {
      found = true;
      break;
    }
  }

  if (found && i > 0) {
    // swap i with i-1
    MeasureStep temp = oldMeasureSteps[i - 1];
    oldMeasureSteps[i - 1] = oldMeasureSteps[i];
    oldMeasureSteps[i] = temp;

    m_app->currentModel()->workflowJSON().setMeasureSteps(m_measureType, oldMeasureSteps);

    emit itemChanged(i - 1);
    emit itemChanged(i);
  }
}

void MeasureStepController::moveDown(MeasureStep step) {
  std::vector<MeasureStep> oldMeasureSteps = measureSteps();

  bool found = false;
  size_t i = 0;
  size_t n = oldMeasureSteps.size();
  for (i = 0; i < n; ++i) {
    if (oldMeasureSteps[i] == step) {
      found = true;
      break;
    }
  }

  if (found && i < (n - 1)) {
    // swap i with i+1
    MeasureStep temp = oldMeasureSteps[i + 1];
    oldMeasureSteps[i + 1] = oldMeasureSteps[i];
    oldMeasureSteps[i] = temp;

    m_app->currentModel()->workflowJSON().setMeasureSteps(m_measureType, oldMeasureSteps);

    emit itemChanged(i);
    emit itemChanged(i + 1);
  }
}

MeasureStepItem::MeasureStepItem(MeasureType measureType, MeasureStep step, openstudio::BaseApp* t_baseApp)
  : m_measureType(measureType),
    m_step(step),
    m_app(t_baseApp)

{}

QString MeasureStepItem::name() const {
  QString result;
  if (boost::optional<std::string> name_ = m_step.name()) {
    return result = QString::fromStdString(name_.get());
  }
  return result;
}

//QString MeasureStepItem::displayName() const
//{
//  // DLM: TODO, add display name
//  QString result;
//  if (boost::optional<std::string> name = m_step.name()){
//    return result = QString::fromStdString(*name);
//  }
//  return result;
//}

MeasureType MeasureStepItem::measureType() const {
  OptionalBCLMeasure bclMeasure_ = this->bclMeasure();
  OS_ASSERT(bclMeasure_);
  return bclMeasure_->measureType();
}

MeasureLanguage MeasureStepItem::measureLanguage() const {
  OptionalBCLMeasure bclMeasure_ = this->bclMeasure();
  OS_ASSERT(bclMeasure_);
  return bclMeasure_->measureLanguage();
}

MeasureStep MeasureStepItem::measureStep() const {
  return m_step;
}

QString MeasureStepItem::description() const {
  QString result;
  if (boost::optional<std::string> description_ = m_step.description()) {
    return result = QString::fromStdString(description_.get());
  }
  return result;
}

QString MeasureStepItem::modelerDescription() const {
  QString result;
  if (boost::optional<std::string> modelerDescription_ = m_step.modelerDescription()) {
    return result = QString::fromStdString(modelerDescription_.get());
  }
  return result;
}

QString MeasureStepItem::scriptFileName() const {
  QString result;
  if (OptionalBCLMeasure bclMeasure_ = this->bclMeasure()) {
    if (boost::optional<path> primaryScriptPath_ = bclMeasure_->primaryScriptPath()) {
      result = toQString(primaryScriptPath_.get());
    }
  }
  return result;
}

OptionalBCLMeasure MeasureStepItem::bclMeasure() const {
  return m_app->currentModel()->workflowJSON().getBCLMeasure(m_step);
}

std::vector<measure::OSArgument> MeasureStepItem::arguments() const {
  std::vector<measure::OSArgument> result;

  // get arguments from the BCL Measure (computed using the current model)
  if (auto bclMeasure_ = this->bclMeasure()) {
    result = m_app->measureManager().getArguments(bclMeasure_.get());
  }

  // fill in with any arguments in this WorkflowJSON
  for (auto& argument : result) {
    boost::optional<Variant> variant = m_step.getArgument(argument.name());
    if (variant) {

      VariantType variantType = variant->variantType();
      if (variantType == VariantType::Boolean) {
        argument.setValue(variant->valueAsBoolean());
      } else if (variantType == VariantType::Integer) {
        argument.setValue(variant->valueAsInteger());
      } else if (variantType == VariantType::Double) {
        argument.setValue(variant->valueAsDouble());
      } else if (variantType == VariantType::String) {
        argument.setValue(variant->valueAsString());
      }
    }
  }

  return result;
}

bool MeasureStepItem::hasIncompleteArguments() const {
  return (!incompleteArguments().empty());
}

std::vector<measure::OSArgument> MeasureStepItem::incompleteArguments() const {
  std::vector<measure::OSArgument> result;

  // find any required arguments without a value
  for (const auto& argument : arguments()) {
    if (argument.required() && !argument.hasDefaultValue()) {
      boost::optional<Variant> variant = m_step.getArgument(argument.name());
      if (!variant) {
        result.push_back(argument);
      }
    }
  }

  return result;
}

void MeasureStepItem::remove() {
  // if this step is being edited, clear the edit controller
  MeasureStepItem* measureStepItem = m_app->editController()->measureStepItem();
  if (measureStepItem) {
    if (measureStepItem->measureStep() == m_step) {
      m_app->editController()->reset();
    }
  }

  qobject_cast<MeasureStepController*>(controller())->removeItemForStep(m_step);
}

void MeasureStepItem::moveUp() {
  qobject_cast<MeasureStepController*>(controller())->moveUp(m_step);
}

void MeasureStepItem::moveDown() {
  qobject_cast<MeasureStepController*>(controller())->moveDown(m_step);
}

void MeasureStepItem::setName(const QString& name) {
  m_step.setName(name.toStdString());

  emit nameChanged(name);
}

//void MeasureStepItem::setDisplayName(const QString & displayName)
//{
//  m_step.setName(displayName.toStdString());
//
//  emit displayNameChanged(displayName);
//}

void MeasureStepItem::setDescription(const QString& description) {
  m_step.setDescription(description.toStdString());
}

void MeasureStepItem::setArgument(const measure::OSArgument& argument) {
  if (argument.hasValue()) {

    if (argument.type() == measure::OSArgumentType::Boolean) {
      m_step.setArgument(argument.name(), argument.valueAsBool());
    } else if ((argument.type() == measure::OSArgumentType::Double) || (argument.type() == measure::OSArgumentType::Quantity)) {
      m_step.setArgument(argument.name(), argument.valueAsDouble());
    } else if (argument.type() == measure::OSArgumentType::Integer) {
      m_step.setArgument(argument.name(), argument.valueAsInteger());
    } else if (argument.type() == measure::OSArgumentType::Path) {
      m_step.setArgument(argument.name(), openstudio::toString(argument.valueAsPath()));
    } else {
      m_step.setArgument(argument.name(), argument.valueAsString());
    }
  } else {
    m_step.removeArgument(argument.name());
  }

  emit argumentsChanged(hasIncompleteArguments());
}

void MeasureStepItem::setSelected(bool isSelected) {
  OSListItem::setSelected(isSelected);

  // Ugly hack to prevent the controller from doing this for tab 2.
  // Please somebody fix, perhaps be using a new signal from OSItemSelectionController.
  if (!controller()->selectionController()->allowMultipleSelections()) {
    if (isSelected) {
      m_app->chooseHorizontalEditTab();
      m_app->editController()->setMeasureStepItem(this, m_app);

    } else {
      m_app->editController()->reset();
    }
  }
}

MeasureStepItemDelegate::MeasureStepItemDelegate() = default;

QWidget* MeasureStepItemDelegate::view(QSharedPointer<OSListItem> dataSource) {
  if (QSharedPointer<MeasureStepItem> measureStepItem = dataSource.objectCast<MeasureStepItem>()) {
    auto* workflowStepView = new WorkflowStepView();

    const MeasureLanguage measureLanguage = measureStepItem->measureLanguage();

    const QString measureLangStr = toQString(measureLanguage.valueName());
    if (measureStepItem->measureType() == MeasureType::ModelMeasure) {
      workflowStepView->workflowStepButton->measureTypeBadge->setPixmap(
        QPixmap(QString(":/images/openstudio_measure_icon_%1.png").arg(measureLangStr))
          .scaled(15, 15, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
      workflowStepView->workflowStepButton->measureTypeBadge->setVisible(true);
    } else if (measureStepItem->measureType() == MeasureType::EnergyPlusMeasure) {
      workflowStepView->workflowStepButton->measureTypeBadge->setPixmap(
        QPixmap(QString(":/images/energyplus_measure_icon_%1.png").arg(measureLangStr))
          .scaled(15, 15, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
      workflowStepView->workflowStepButton->measureTypeBadge->setVisible(true);
    } else if (measureStepItem->measureType() == MeasureType::ReportingMeasure) {
      workflowStepView->workflowStepButton->measureTypeBadge->setPixmap(
        QPixmap(QString(":/images/report_measure_icon_%1.png").arg(measureLangStr)).scaled(15, 15, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
      workflowStepView->workflowStepButton->measureTypeBadge->setVisible(true);
    }

    workflowStepView->workflowStepButton->nameLabel->setText(measureStepItem->name());

    workflowStepView->workflowStepButton->nameLabel->setText(measureStepItem->name());

    connect(measureStepItem.data(), &MeasureStepItem::nameChanged, workflowStepView->workflowStepButton->nameLabel, &QLabel::setText);

    // Remove

    connect(workflowStepView->removeButton, &QPushButton::clicked, measureStepItem.data(), &MeasureStepItem::remove);

    // Selection

    workflowStepView->workflowStepButton->setHasEmphasis(measureStepItem->isSelected());

    connect(workflowStepView->workflowStepButton, &WorkflowStepButton::clicked, measureStepItem.data(), &MeasureStepItem::toggleSelected);

    connect(measureStepItem.data(), &MeasureStepItem::selectedChanged, workflowStepView->workflowStepButton, &WorkflowStepButton::setHasEmphasis);

    const bool useClassicCLI =
      OSAppBase::instance()->currentDocument() == nullptr ? false : OSAppBase::instance()->currentDocument()->mainWindow()->useClassicCLI();
    if (useClassicCLI && (measureLanguage == MeasureLanguage::Python)) {
      workflowStepView->workflowStepButton->errorLabel->setToolTip(
        "Python Measures are not supported in the Classic CLI.\nYou can change CLI version using 'Preferences->Use Classic CLI'.");
      workflowStepView->workflowStepButton->errorLabel->setVisible(true);
    } else {
      try {
        // Warning Icon
        workflowStepView->workflowStepButton->cautionLabel->setVisible(measureStepItem->hasIncompleteArguments());
        connect(measureStepItem.data(), &MeasureStepItem::argumentsChanged, workflowStepView->workflowStepButton->cautionLabel, &QLabel::setVisible);
      } catch (const std::exception& e) {
        workflowStepView->workflowStepButton->errorLabel->setToolTip(e.what());
        workflowStepView->workflowStepButton->errorLabel->setVisible(true);
      }
    }

    // Up and down buttons

    connect(workflowStepView->upButton, &QPushButton::clicked, measureStepItem.data(), &MeasureStepItem::moveUp);

    connect(workflowStepView->downButton, &QPushButton::clicked, measureStepItem.data(), &MeasureStepItem::moveDown);

    return workflowStepView;
  }

  return new QWidget();
}

}  // namespace measuretab

}  // namespace openstudio
