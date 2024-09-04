/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "WaterUseEquipmentInspectorView.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include "ModelObjectItem.hpp"
#include <openstudio/model/WaterUseEquipmentDefinition.hpp>
#include <openstudio/model/WaterUseEquipmentDefinition_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/utilities/idd/OS_WaterUse_Equipment_Definition_FieldEnums.hxx>
#include <openstudio/utilities/core/Assert.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

// TargetTemperatureScheduleVC

boost::optional<model::WaterUseEquipmentDefinition> TargetTemperatureScheduleVC::waterUseEquipmentDefinition() const {
  boost::optional<model::WaterUseEquipmentDefinition> result;

  if (m_modelObject) {
    result = m_modelObject->optionalCast<model::WaterUseEquipmentDefinition>();
  }

  return result;
}

void TargetTemperatureScheduleVC::onChangeRelationship(const model::ModelObject& /*modelObject*/, int index, Handle /*newHandle*/,
                                                       Handle /*oldHandle*/) {
  if (index == OS_WaterUse_Equipment_DefinitionFields::TargetTemperatureScheduleName) {
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> TargetTemperatureScheduleVC::makeVector() {
  std::vector<OSItemId> result;
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    boost::optional<model::Schedule> schedule = def->targetTemperatureSchedule();
    if (schedule) {
      result.push_back(modelObjectToItemId(*schedule, false));
    }
  }
  return result;
}

void TargetTemperatureScheduleVC::onRemoveItem(OSItem* /*item*/) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    def->resetTargetTemperatureSchedule();
  }
}

void TargetTemperatureScheduleVC::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

void TargetTemperatureScheduleVC::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
    if (modelObject) {
      if (modelObject->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          modelObject = modelObject->clone(m_modelObject->model());
        }
        def->setTargetTemperatureSchedule(modelObject->cast<model::Schedule>());
      }
    }
  }
}

// SensibleFractionScheduleVC

boost::optional<model::WaterUseEquipmentDefinition> SensibleFractionScheduleVC::waterUseEquipmentDefinition() const {
  boost::optional<model::WaterUseEquipmentDefinition> result;

  if (m_modelObject) {
    result = m_modelObject->optionalCast<model::WaterUseEquipmentDefinition>();
  }

  return result;
}

void SensibleFractionScheduleVC::onChangeRelationship(const model::ModelObject& /*modelObject*/, int index, Handle /*newHandle*/,
                                                      Handle /*oldHandle*/) {
  if (index == OS_WaterUse_Equipment_DefinitionFields::SensibleFractionScheduleName) {
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> SensibleFractionScheduleVC::makeVector() {
  std::vector<OSItemId> result;
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    if (boost::optional<model::Schedule> schedule = def->sensibleFractionSchedule()) {
      result.push_back(modelObjectToItemId(*schedule, false));
    }
  }
  return result;
}

void SensibleFractionScheduleVC::onRemoveItem(OSItem* /*item*/) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    def->resetSensibleFractionSchedule();
  }
}

void SensibleFractionScheduleVC::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

void SensibleFractionScheduleVC::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
    if (modelObject) {
      if (modelObject->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          modelObject = modelObject->clone(m_modelObject->model());
        }
        def->setSensibleFractionSchedule(modelObject->cast<model::Schedule>());
      }
    }
  }
}

// LatentFractionScheduleVC

boost::optional<model::WaterUseEquipmentDefinition> LatentFractionScheduleVC::waterUseEquipmentDefinition() const {
  boost::optional<model::WaterUseEquipmentDefinition> result;

  if (m_modelObject) {
    result = m_modelObject->optionalCast<model::WaterUseEquipmentDefinition>();
  }

  return result;
}

void LatentFractionScheduleVC::onChangeRelationship(const model::ModelObject& /*modelObject*/, int index, Handle /*newHandle*/,
                                                    Handle /*oldHandle*/) {
  if (index == OS_WaterUse_Equipment_DefinitionFields::LatentFractionScheduleName) {
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> LatentFractionScheduleVC::makeVector() {
  std::vector<OSItemId> result;
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    boost::optional<model::Schedule> schedule = def->latentFractionSchedule();
    if (schedule) {
      result.push_back(modelObjectToItemId(*schedule, false));
    }
  }
  return result;
}

void LatentFractionScheduleVC::onRemoveItem(OSItem* /*item*/) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    def->resetLatentFractionSchedule();
  }
}

void LatentFractionScheduleVC::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

void LatentFractionScheduleVC::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::WaterUseEquipmentDefinition> def = this->waterUseEquipmentDefinition()) {
    if (boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId)) {
      if (modelObject->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          modelObject = modelObject->clone(m_modelObject->model());
        }
        def->setLatentFractionSchedule(modelObject->cast<model::Schedule>());
      }
    }
  }
}

WaterUseEquipmentDefinitionInspectorView::WaterUseEquipmentDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent),
    m_nameEdit(new OSLineEdit2()),
    m_endUseSubcategoryEdit(new OSLineEdit2()),
    m_targetTemperatureScheduleVC(new TargetTemperatureScheduleVC()),
    m_sensibleFractionScheduleVC(new SensibleFractionScheduleVC()),
    m_latentFractionScheduleVC(new LatentFractionScheduleVC()),
    m_isIP(isIP) {

  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  // Name

  auto* label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 0, 0);

  mainGridLayout->addWidget(m_nameEdit, 1, 0, 1, 3);

  // End Use Subcategory

  label = new QLabel("End Use Subcategory: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  mainGridLayout->addWidget(m_endUseSubcategoryEdit, 3, 0, 1, 3);

  // Peak Flow Rate

  label = new QLabel("Peak Flow Rate: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_peakFlowRateEdit = new OSQuantityEdit2("m^3/s", "m^3/s", "gal/min", m_isIP);
  connect(this, &WaterUseEquipmentDefinitionInspectorView::toggleUnitsClicked, m_peakFlowRateEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_peakFlowRateEdit, 5, 0, 1, 3);

  // Target Temperature Schedule

  label = new QLabel("Target Temperature Schedule: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_targetTemperatureScheduleDZ = new OSDropZone(m_targetTemperatureScheduleVC);
  m_targetTemperatureScheduleDZ->setMinItems(0);
  m_targetTemperatureScheduleDZ->setMaxItems(1);
  m_targetTemperatureScheduleDZ->setItemsAcceptDrops(true);
  mainGridLayout->addWidget(m_targetTemperatureScheduleDZ, 7, 0, 1, 3);

  // Sensible Fraction Schedule

  label = new QLabel("Sensible Fraction Schedule: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 8, 0);

  m_sensibleFractionScheduleDZ = new OSDropZone(m_sensibleFractionScheduleVC);
  m_sensibleFractionScheduleDZ->setMinItems(0);
  m_sensibleFractionScheduleDZ->setMaxItems(1);
  m_sensibleFractionScheduleDZ->setItemsAcceptDrops(true);
  mainGridLayout->addWidget(m_sensibleFractionScheduleDZ, 9, 0, 1, 3);

  // Latent Fraction Schedule

  label = new QLabel("Latent Fraction Schedule: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 10, 0);

  m_latentFractionScheduleDZ = new OSDropZone(m_latentFractionScheduleVC);
  m_latentFractionScheduleDZ->setMinItems(0);
  m_latentFractionScheduleDZ->setMaxItems(1);
  m_latentFractionScheduleDZ->setItemsAcceptDrops(true);
  mainGridLayout->addWidget(m_latentFractionScheduleDZ, 11, 0, 1, 3);

  // Stretch

  mainGridLayout->setRowStretch(12, 100);

  mainGridLayout->setColumnStretch(3, 100);
}

void WaterUseEquipmentDefinitionInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void WaterUseEquipmentDefinitionInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto def = modelObject.cast<model::WaterUseEquipmentDefinition>();
  attach(def);
  refresh();
}

void WaterUseEquipmentDefinitionInspectorView::onUpdate() {
  refresh();
}

void WaterUseEquipmentDefinitionInspectorView::attach(const openstudio::model::WaterUseEquipmentDefinition& waterUseEquipmentDefinition) {
  // m_nameEdit->bind(waterUseEquipmentDefinition,"name");
  m_waterUseEquipmentDefinition = waterUseEquipmentDefinition;
  m_nameEdit->bind(*m_waterUseEquipmentDefinition,
                   OptionalStringGetter(std::bind(&model::WaterUseEquipmentDefinition::name, m_waterUseEquipmentDefinition.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::WaterUseEquipmentDefinition::setName, m_waterUseEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  // m_endUseSubcategoryEdit->bind(waterUseEquipmentDefinition,"endUseSubcategory");
  m_endUseSubcategoryEdit->bind(
    *m_waterUseEquipmentDefinition,
    StringGetter(std::bind(&model::WaterUseEquipmentDefinition::endUseSubcategory, m_waterUseEquipmentDefinition.get_ptr())),
    boost::optional<StringSetter>(
      std::bind(&model::WaterUseEquipmentDefinition::setEndUseSubcategory, m_waterUseEquipmentDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::WaterUseEquipmentDefinition::resetEndUseSubcategory, m_waterUseEquipmentDefinition.get_ptr())),
    boost::optional<BasicQuery>(
      std::bind(&model::WaterUseEquipmentDefinition::isEndUseSubcategoryDefaulted, m_waterUseEquipmentDefinition.get_ptr())));

  // m_peakFlowRateEdit->bind(waterUseEquipmentDefinition,"peakFlowRate",m_isIP);
  m_peakFlowRateEdit->bind(m_isIP, *m_waterUseEquipmentDefinition,
                           DoubleGetter(std::bind(&model::WaterUseEquipmentDefinition::peakFlowRate, m_waterUseEquipmentDefinition.get_ptr())),
                           boost::optional<DoubleSetter>(std::bind(
                             static_cast<bool (model::WaterUseEquipmentDefinition::*)(double)>(&model::WaterUseEquipmentDefinition::setPeakFlowRate),
                             m_waterUseEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  m_targetTemperatureScheduleVC->attach(waterUseEquipmentDefinition);
  m_targetTemperatureScheduleVC->reportItems();

  m_latentFractionScheduleVC->attach(waterUseEquipmentDefinition);
  m_latentFractionScheduleVC->reportItems();

  m_sensibleFractionScheduleVC->attach(waterUseEquipmentDefinition);
  m_sensibleFractionScheduleVC->reportItems();

  this->stackedWidget()->setCurrentIndex(1);
}

void WaterUseEquipmentDefinitionInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_endUseSubcategoryEdit->unbind();
  m_peakFlowRateEdit->unbind();

  m_waterUseEquipmentDefinition = boost::none;

  m_targetTemperatureScheduleVC->detach();
  m_targetTemperatureScheduleVC->reportItems();

  m_latentFractionScheduleVC->detach();
  m_latentFractionScheduleVC->reportItems();

  m_sensibleFractionScheduleVC->detach();
  m_sensibleFractionScheduleVC->reportItems();
}

void WaterUseEquipmentDefinitionInspectorView::refresh() {}

void WaterUseEquipmentDefinitionInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
