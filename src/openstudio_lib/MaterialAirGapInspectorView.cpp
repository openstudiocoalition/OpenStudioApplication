/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MaterialAirGapInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/AirGap.hpp>
#include <openstudio/model/AirGap_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// MaterialAirGapInspectorView

MaterialAirGapInspectorView::MaterialAirGapInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP), m_thermalResistance(nullptr) {
  createLayout();
}

void MaterialAirGapInspectorView::createLayout() {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();

  QLabel* label = nullptr;

  // Name

  label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_nameEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 3);

  ++row;

  // Standards Information

  m_standardsInformationWidget = new StandardsInformationMaterialWidget(m_isIP, mainGridLayout, row);

  ++row;

  // Thermal Resistance

  label = new QLabel("Thermal Resistance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_thermalResistance = new OSQuantityEdit2("m^2*K/W", "m^2*K/W", "ft^2*h*R/Btu", m_isIP);
  connect(this, &MaterialAirGapInspectorView::toggleUnitsClicked, m_thermalResistance, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_thermalResistance, row++, 0, 1, 3);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void MaterialAirGapInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void MaterialAirGapInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto airGap = modelObject.cast<model::AirGap>();
  attach(airGap);
  refresh();
}

void MaterialAirGapInspectorView::onUpdate() {
  refresh();
}

void MaterialAirGapInspectorView::attach(openstudio::model::AirGap& airGap) {
  m_airGap = airGap;

  // m_nameEdit->bind(airGap,"name");
  m_nameEdit->bind(*m_airGap, OptionalStringGetter(std::bind(&model::AirGap::name, m_airGap.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::AirGap::setName, m_airGap.get_ptr(), std::placeholders::_1)));

  // m_thermalResistance->bind(airGap,"thermalResistance",m_isIP);
  m_thermalResistance->bind(
    m_isIP, *m_airGap, DoubleGetter(std::bind(&model::AirGap::thermalResistance, m_airGap.get_ptr())),
    //static_cast<void(Client::*)(int)>(&Client::foobar)
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::AirGap::*)(double)>(&model::AirGap::setThermalResistance), m_airGap.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::AirGap::resetThermalResistance, m_airGap.get_ptr())));

  m_standardsInformationWidget->attach(airGap);

  this->stackedWidget()->setCurrentIndex(1);
}

void MaterialAirGapInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_thermalResistance->unbind();

  m_airGap = boost::none;

  m_standardsInformationWidget->detach();
}

void MaterialAirGapInspectorView::refresh() {}

}  // namespace openstudio
