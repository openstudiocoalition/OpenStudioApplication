/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LuminaireInspectorView.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include <openstudio/model/LuminaireDefinition.hpp>
#include <openstudio/model/LuminaireDefinition_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

LuminaireDefinitionInspectorView::LuminaireDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
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
  mainGridLayout->addWidget(m_nameEdit, 1, 0, 1, 2);

  // Lighting Power

  label = new QLabel("Lighting Power: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  m_lightingPowerEdit = new OSQuantityEdit2("W", "W", "W", m_isIP);
  connect(this, &LuminaireDefinitionInspectorView::toggleUnitsClicked, m_lightingPowerEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_lightingPowerEdit, 3, 0);

  // Fraction Radiant

  label = new QLabel("Fraction Radiant: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_fractionRadiantEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LuminaireDefinitionInspectorView::toggleUnitsClicked, m_fractionRadiantEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionRadiantEdit, 5, 0);

  // Fraction Visible

  label = new QLabel("Fraction Visible: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 1);

  m_fractionVisibleEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LuminaireDefinitionInspectorView::toggleUnitsClicked, m_fractionVisibleEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_fractionVisibleEdit, 5, 1);

  // Return Air Fraction

  label = new QLabel("Return Air Fraction: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_returnAirFractionEdit = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &LuminaireDefinitionInspectorView::toggleUnitsClicked, m_returnAirFractionEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_returnAirFractionEdit, 7, 0);

  // Stretch

  mainGridLayout->setRowStretch(8, 100);

  mainGridLayout->setColumnStretch(2, 100);
}

void LuminaireDefinitionInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void LuminaireDefinitionInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto luminiareDefinition = modelObject.cast<model::LuminaireDefinition>();
  attach(luminiareDefinition);
  refresh();
}

void LuminaireDefinitionInspectorView::onUpdate() {
  refresh();
}

void LuminaireDefinitionInspectorView::attach(const openstudio::model::LuminaireDefinition& luminaireDefinition) {
  m_luminaireDefinition = luminaireDefinition;

  // m_nameEdit->bind(luminaireDefinition,"name");
  m_nameEdit->bind(*m_luminaireDefinition, OptionalStringGetter(std::bind(&model::LuminaireDefinition::name, m_luminaireDefinition.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::LuminaireDefinition::setName, m_luminaireDefinition.get_ptr(), std::placeholders::_1)));
  // m_lightingPowerEdit->bind(luminaireDefinition,"lightingPower",m_isIP);
  m_lightingPowerEdit->bind(
    m_isIP, *m_luminaireDefinition, DoubleGetter(std::bind(&model::LuminaireDefinition::lightingPower, m_luminaireDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::LuminaireDefinition::setLightingPower, m_luminaireDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::LuminaireDefinition::resetLightingPower, m_luminaireDefinition.get_ptr())), boost::none,
    boost::none, boost::optional<BasicQuery>(std::bind(&model::LuminaireDefinition::isLightingPowerDefaulted, m_luminaireDefinition.get_ptr())));

  // m_fractionRadiantEdit->bind(luminaireDefinition,"fractionRadiant",m_isIP);
  m_fractionRadiantEdit->bind(
    m_isIP, *m_luminaireDefinition, DoubleGetter(std::bind(&model::LuminaireDefinition::fractionRadiant, m_luminaireDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::LuminaireDefinition::setFractionRadiant, m_luminaireDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::LuminaireDefinition::resetFractionRadiant, m_luminaireDefinition.get_ptr())), boost::none,
    boost::none, boost::optional<BasicQuery>(std::bind(&model::LuminaireDefinition::isFractionRadiantDefaulted, m_luminaireDefinition.get_ptr())));

  // m_fractionVisibleEdit->bind(luminaireDefinition,"fractionVisible",m_isIP);
  m_fractionVisibleEdit->bind(
    m_isIP, *m_luminaireDefinition, DoubleGetter(std::bind(&model::LuminaireDefinition::fractionVisible, m_luminaireDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::LuminaireDefinition::setFractionVisible, m_luminaireDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::LuminaireDefinition::resetFractionVisible, m_luminaireDefinition.get_ptr())), boost::none,
    boost::none, boost::optional<BasicQuery>(std::bind(&model::LuminaireDefinition::isFractionVisibleDefaulted, m_luminaireDefinition.get_ptr())));

  // m_returnAirFractionEdit->bind(luminaireDefinition,"returnAirFraction",m_isIP);
  m_returnAirFractionEdit->bind(
    m_isIP, *m_luminaireDefinition, DoubleGetter(std::bind(&model::LuminaireDefinition::returnAirFraction, m_luminaireDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::LuminaireDefinition::setReturnAirFraction, m_luminaireDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::LuminaireDefinition::resetReturnAirFraction, m_luminaireDefinition.get_ptr())), boost::none,
    boost::none, boost::optional<BasicQuery>(std::bind(&model::LuminaireDefinition::isReturnAirFractionDefaulted, m_luminaireDefinition.get_ptr())));

  this->stackedWidget()->setCurrentIndex(1);
}

void LuminaireDefinitionInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_lightingPowerEdit->unbind();
  m_fractionRadiantEdit->unbind();
  m_fractionVisibleEdit->unbind();
  m_returnAirFractionEdit->unbind();

  m_luminaireDefinition = boost::none;
}

void LuminaireDefinitionInspectorView::refresh() {}

void LuminaireDefinitionInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
