/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "WindowMaterialSimpleGlazingSystemInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/SimpleGlazing.hpp>
#include <openstudio/model/SimpleGlazing_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// WindowMaterialSimpleGlazingSystemInspectorView

WindowMaterialSimpleGlazingSystemInspectorView::WindowMaterialSimpleGlazingSystemInspectorView(bool isIP, const openstudio::model::Model& model,
                                                                                               QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP) {
  createLayout();
}

void WindowMaterialSimpleGlazingSystemInspectorView::createLayout() {
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

  // U-Factor

  label = new QLabel("U-Factor: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_uFactor = new OSQuantityEdit2("W/m^2*K", "W/m^2*K", "Btu/ft^2*hr*R", m_isIP);
  connect(this, &WindowMaterialSimpleGlazingSystemInspectorView::toggleUnitsClicked, m_uFactor, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_uFactor, row++, 0, 1, 3);

  // Solar Heat Gain Coefficient

  label = new QLabel("Solar Heat Gain Coefficient: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_solarHeatGainCoefficient = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &WindowMaterialSimpleGlazingSystemInspectorView::toggleUnitsClicked, m_solarHeatGainCoefficient,
          &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_solarHeatGainCoefficient, row++, 0, 1, 3);

  // Visible Transmittance

  label = new QLabel("Visible Transmittance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_visibleTransmittance = new OSQuantityEdit2("", "", "", m_isIP);
  connect(this, &WindowMaterialSimpleGlazingSystemInspectorView::toggleUnitsClicked, m_visibleTransmittance, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_visibleTransmittance, row++, 0, 1, 3);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void WindowMaterialSimpleGlazingSystemInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void WindowMaterialSimpleGlazingSystemInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto simpleGlazing = modelObject.cast<model::SimpleGlazing>();
  attach(simpleGlazing);
  refresh();
}

void WindowMaterialSimpleGlazingSystemInspectorView::onUpdate() {
  refresh();
}

void WindowMaterialSimpleGlazingSystemInspectorView::attach(openstudio::model::SimpleGlazing& simpleGlazing) {
  // m_nameEdit->bind(simpleGlazing,"name");
  m_simpleGlazing = simpleGlazing;
  m_nameEdit->bind(
    *m_simpleGlazing, OptionalStringGetter(std::bind(&model::SimpleGlazing::name, m_simpleGlazing.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::SimpleGlazing::setName, m_simpleGlazing.get_ptr(), std::placeholders::_1)));

  // m_uFactor->bind(simpleGlazing,"uFactor",m_isIP);
  m_uFactor->bind(m_isIP, *m_simpleGlazing, DoubleGetter(std::bind(&model::SimpleGlazing::uFactor, m_simpleGlazing.get_ptr())),
                  boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::SimpleGlazing::*)(double)>(&model::SimpleGlazing::setUFactor),
                                                          m_simpleGlazing.get_ptr(), std::placeholders::_1)));

  // m_solarHeatGainCoefficient->bind(simpleGlazing,"solarHeatGainCoefficient",m_isIP);
  m_solarHeatGainCoefficient->bind(
    m_isIP, *m_simpleGlazing, DoubleGetter(std::bind(&model::SimpleGlazing::solarHeatGainCoefficient, m_simpleGlazing.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::SimpleGlazing::*)(double)>(&model::SimpleGlazing::setSolarHeatGainCoefficient),
                                            m_simpleGlazing.get_ptr(), std::placeholders::_1)));

  // m_visibleTransmittance->bind(simpleGlazing,"visibleTransmittance",m_isIP);
  m_visibleTransmittance->bind(
    m_isIP, *m_simpleGlazing, OptionalDoubleGetter(std::bind(&model::SimpleGlazing::visibleTransmittance, m_simpleGlazing.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::SimpleGlazing::*)(double)>(&model::SimpleGlazing::setVisibleTransmittance),
                                            m_simpleGlazing.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimpleGlazing::resetVisibleTransmittance, m_simpleGlazing.get_ptr())));

  m_standardsInformationWidget->attach(simpleGlazing);

  this->stackedWidget()->setCurrentIndex(1);
}

void WindowMaterialSimpleGlazingSystemInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_uFactor->unbind();
  m_solarHeatGainCoefficient->unbind();
  m_visibleTransmittance->unbind();

  m_simpleGlazing = boost::none;

  m_standardsInformationWidget->detach();
}

void WindowMaterialSimpleGlazingSystemInspectorView::refresh() {}

}  // namespace openstudio
