/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HotWaterEquipmentInspectorView.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include <openstudio/model/HotWaterEquipmentDefinition.hpp>
#include <openstudio/model/HotWaterEquipmentDefinition_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

HotWaterEquipmentDefinitionInspectorView::HotWaterEquipmentDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_nameEdit(new OSLineEdit2()), m_isIP(isIP) {

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

  // Design Level

  label = new QLabel("Design Level: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  m_designLevelEdit = new OSQuantityEdit2("W", "W", "W", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_designLevelEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_designLevelEdit, 3, 0);

  // Watts Per Space Floor Area

  label = new QLabel("Watts Per Space Floor Area: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 1);

  m_wattsPerSpaceFloorAreaEdit = new OSQuantityEdit2("W/m^2", "W/m^2", "W/ft^2", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_wattsPerSpaceFloorAreaEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_wattsPerSpaceFloorAreaEdit, 3, 1);

  // Watts Per Person

  label = new QLabel("Watts Per Person: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 2);

  m_wattsPerPersonEdit = new OSQuantityEdit2("W/person", "W/person", "W/person", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_wattsPerPersonEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_wattsPerPersonEdit, 3, 2);

  // Fraction Latent

  label = new QLabel("Fraction Latent: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_fractionLatentEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_fractionLatentEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionLatentEdit, 5, 0);

  // Fraction Radiant

  label = new QLabel("Fraction Radiant: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 1);

  m_fractionRadiantEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_fractionRadiantEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionRadiantEdit, 5, 1);

  // Fraction Lost

  label = new QLabel("Fraction Lost: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_fractionLostEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked, m_fractionLostEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionLostEdit, 7, 0);

  // Stretch

  mainGridLayout->setRowStretch(8, 100);

  mainGridLayout->setColumnStretch(3, 100);
}

void HotWaterEquipmentDefinitionInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void HotWaterEquipmentDefinitionInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto hotwaterEquipmentDefinition = modelObject.cast<model::HotWaterEquipmentDefinition>();
  attach(hotwaterEquipmentDefinition);
  refresh();
}

void HotWaterEquipmentDefinitionInspectorView::onUpdate() {
  refresh();
}

void HotWaterEquipmentDefinitionInspectorView::attach(const openstudio::model::HotWaterEquipmentDefinition& hotwaterEquipmentDefinition) {
  m_hotwaterEquipmentDefinition = hotwaterEquipmentDefinition;

  // m_nameEdit->bind(hotwaterEquipmentDefinition,"name");
  m_nameEdit->bind(*m_hotwaterEquipmentDefinition,
                   OptionalStringGetter(std::bind(&model::HotWaterEquipmentDefinition::name, m_hotwaterEquipmentDefinition.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::HotWaterEquipmentDefinition::setName, m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  // m_designLevelEdit->bind(hotwaterEquipmentDefinition,"designLevel",m_isIP);
  m_designLevelEdit->bind(m_isIP, *m_hotwaterEquipmentDefinition,
                          OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::designLevel, m_hotwaterEquipmentDefinition.get_ptr())),
                          boost::optional<DoubleSetter>(std::bind(
                            static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setDesignLevel),
                            m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  // m_wattsPerSpaceFloorAreaEdit->bind(hotwaterEquipmentDefinition,"wattsperSpaceFloorArea",m_isIP);
  m_wattsPerSpaceFloorAreaEdit->bind(
    m_isIP, *m_hotwaterEquipmentDefinition,
    OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::wattsperSpaceFloorArea, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setWattsperSpaceFloorArea),
                m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  // m_wattsPerPersonEdit->bind(hotwaterEquipmentDefinition,"wattsperPerson",m_isIP);
  m_wattsPerPersonEdit->bind(
    m_isIP, *m_hotwaterEquipmentDefinition,
    OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::wattsperPerson, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setWattsperPerson),
                m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)));

  // m_fractionLatentEdit->bind(hotwaterEquipmentDefinition,"fractionLatent",m_isIP);
  m_fractionLatentEdit->bind(
    m_isIP, *m_hotwaterEquipmentDefinition,
    OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::fractionLatent, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setFractionLatent),
                m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HotWaterEquipmentDefinition::resetFractionLatent, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HotWaterEquipmentDefinition::isFractionLatentDefaulted, m_hotwaterEquipmentDefinition.get_ptr())));

  // m_fractionRadiantEdit->bind(hotwaterEquipmentDefinition,"fractionRadiant",m_isIP);
  m_fractionRadiantEdit->bind(
    m_isIP, *m_hotwaterEquipmentDefinition,
    OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::fractionRadiant, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setFractionRadiant),
                m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HotWaterEquipmentDefinition::resetFractionRadiant, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HotWaterEquipmentDefinition::isFractionRadiantDefaulted, m_hotwaterEquipmentDefinition.get_ptr())));

  // m_fractionLostEdit->bind(hotwaterEquipmentDefinition,"fractionLost",m_isIP);
  m_fractionLostEdit->bind(
    m_isIP, *m_hotwaterEquipmentDefinition,
    OptionalDoubleGetter(std::bind(&model::HotWaterEquipmentDefinition::fractionLost, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HotWaterEquipmentDefinition::*)(double)>(&model::HotWaterEquipmentDefinition::setFractionLost),
                m_hotwaterEquipmentDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HotWaterEquipmentDefinition::resetFractionLost, m_hotwaterEquipmentDefinition.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HotWaterEquipmentDefinition::isFractionLostDefaulted, m_hotwaterEquipmentDefinition.get_ptr())));

  this->stackedWidget()->setCurrentIndex(1);
}

void HotWaterEquipmentDefinitionInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_designLevelEdit->unbind();
  m_wattsPerSpaceFloorAreaEdit->unbind();
  m_wattsPerPersonEdit->unbind();
  m_fractionLatentEdit->unbind();
  m_fractionRadiantEdit->unbind();
  m_fractionLostEdit->unbind();

  m_hotwaterEquipmentDefinition = boost::none;
}

void HotWaterEquipmentDefinitionInspectorView::refresh() {}

void HotWaterEquipmentDefinitionInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
