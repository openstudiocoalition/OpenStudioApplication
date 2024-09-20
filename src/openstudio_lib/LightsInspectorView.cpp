/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LightsInspectorView.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include <openstudio/model/LightsDefinition.hpp>
#include <openstudio/model/LightsDefinition_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

LightsDefinitionInspectorView::LightsDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  m_isIP = isIP;

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

  m_nameEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_nameEdit, 1, 0, 1, 3);

  // Lighting Level

  label = new QLabel("Lighting Power: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  m_lightingLevelEdit = new OSQuantityEdit2("W", "W", "W", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_lightingLevelEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_lightingLevelEdit, 3, 0);

  // Watts Per Space Floor Area

  label = new QLabel("Watts Per Space Floor Area: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 1);

  m_wattsPerSpaceFloorAreaEdit = new OSQuantityEdit2("W/m^2", "W/m^2", "W/ft^2", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_wattsPerSpaceFloorAreaEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_wattsPerSpaceFloorAreaEdit, 3, 1);

  // Watts Per Person

  label = new QLabel("Watts Per Person: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 2);

  m_wattsPerPersonEdit = new OSQuantityEdit2("W/person", "W/person", "W/person", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_wattsPerPersonEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_wattsPerPersonEdit, 3, 2);

  // Fraction Radiant

  label = new QLabel("Fraction Radiant: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_fractionRadiantEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_fractionRadiantEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionRadiantEdit, 5, 0);

  // Fraction Visible

  label = new QLabel("Fraction Visible: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 1);

  m_fractionVisibleEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_fractionVisibleEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionVisibleEdit, 5, 1);

  // Return Air Fraction

  label = new QLabel("Return Air Fraction: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_returnAirFractionEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LightsDefinitionInspectorView::toggleUnitsClicked, m_returnAirFractionEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_returnAirFractionEdit, 7, 0);

  // Stretch

  mainGridLayout->setRowStretch(8, 100);

  mainGridLayout->setColumnStretch(3, 100);
}

void LightsDefinitionInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void LightsDefinitionInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto lightsDefinition = modelObject.cast<model::LightsDefinition>();
  attach(lightsDefinition);
  refresh();
}

void LightsDefinitionInspectorView::onUpdate() {
  refresh();
}

void LightsDefinitionInspectorView::attach(const openstudio::model::LightsDefinition& lightsDefinition) {
  m_lightsDefinition = lightsDefinition;
  // m_nameEdit->bind(lightsDefinition,"name");

  m_nameEdit->bind(*m_lightsDefinition, OptionalStringGetter(std::bind(&model::LightsDefinition::name, m_lightsDefinition.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::LightsDefinition::setName, m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_lightingLevelEdit->bind(lightsDefinition,"lightingLevel",m_isIP);
  m_lightingLevelEdit->bind(
    m_isIP, *m_lightsDefinition, OptionalDoubleGetter(std::bind(&model::LightsDefinition::lightingLevel, m_lightsDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setLightingLevel),
                                            m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_wattsPerSpaceFloorAreaEdit->bind(lightsDefinition,"wattsperSpaceFloorArea",m_isIP);
  m_wattsPerSpaceFloorAreaEdit->bind(m_isIP, *m_lightsDefinition,
                                     OptionalDoubleGetter(std::bind(&model::LightsDefinition::wattsperSpaceFloorArea, m_lightsDefinition.get_ptr())),
                                     boost::optional<DoubleSetter>(std::bind(
                                       static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setWattsperSpaceFloorArea),
                                       m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_wattsPerPersonEdit->bind(lightsDefinition,"wattsperPerson",m_isIP);
  m_wattsPerPersonEdit->bind(
    m_isIP, *m_lightsDefinition, OptionalDoubleGetter(std::bind(&model::LightsDefinition::wattsperPerson, m_lightsDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setWattsperPerson),
                                            m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_fractionRadiantEdit->bind(lightsDefinition,"fractionRadiant",m_isIP);
  m_fractionRadiantEdit->bind(
    m_isIP, *m_lightsDefinition, OptionalDoubleGetter(std::bind(&model::LightsDefinition::fractionRadiant, m_lightsDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setFractionRadiant),
                                            m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_fractionVisibleEdit->bind(lightsDefinition,"fractionVisible",m_isIP);
  m_fractionVisibleEdit->bind(
    m_isIP, *m_lightsDefinition, OptionalDoubleGetter(std::bind(&model::LightsDefinition::fractionVisible, m_lightsDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setFractionVisible),
                                            m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  // m_returnAirFractionEdit->bind(lightsDefinition,"returnAirFraction",m_isIP);
  m_returnAirFractionEdit->bind(
    m_isIP, *m_lightsDefinition, OptionalDoubleGetter(std::bind(&model::LightsDefinition::returnAirFraction, m_lightsDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::LightsDefinition::*)(double)>(&model::LightsDefinition::setReturnAirFraction),
                                            m_lightsDefinition.get_ptr(), std::placeholders::_1)));

  this->stackedWidget()->setCurrentIndex(1);
}

void LightsDefinitionInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_lightingLevelEdit->unbind();
  m_wattsPerSpaceFloorAreaEdit->unbind();
  m_wattsPerPersonEdit->unbind();

  m_lightsDefinition = boost::none;
}

void LightsDefinitionInspectorView::refresh() {}

void LightsDefinitionInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
