/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionCfactorUndergroundWallInspectorView.hpp"
#include "StandardsInformationConstructionWidget.hpp"

#include "OSItem.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/OSComboBox.hpp"

#include <openstudio/model/CFactorUndergroundWallConstruction.hpp>
#include <openstudio/model/CFactorUndergroundWallConstruction_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QComboBox>

namespace openstudio {

ConstructionCfactorUndergroundWallInspectorView::ConstructionCfactorUndergroundWallInspectorView(bool isIP, const openstudio::model::Model& model,
                                                                                                 QWidget* parent)
  : ModelObjectInspectorView(model, true, parent),
    m_isIP(isIP),
    m_nameEdit(nullptr),
    m_standardsInformationWidget(nullptr),
    m_cfactorEdit(nullptr),
    m_heightEdit(nullptr) {
  createLayout();
}

void ConstructionCfactorUndergroundWallInspectorView::createLayout() {
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

  m_standardsInformationWidget = new StandardsInformationConstructionWidget(m_isIP, mainGridLayout, row);
  m_standardsInformationWidget->hideFenestration();
  m_standardsInformationWidget->disableFenestration();

  ++row;

  // C-Factor

  label = new QLabel("C-Factor: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_cfactorEdit = new OSQuantityEdit2("W/m^2*K", "W/m^2*K", "Btu/ft^2*hr*R", m_isIP);
  connect(this, &ConstructionCfactorUndergroundWallInspectorView::toggleUnitsClicked, m_cfactorEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_cfactorEdit, row, 0);

  ++row;

  // Height

  label = new QLabel("Height: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_heightEdit = new OSQuantityEdit2("m", "m", "ft", m_isIP);
  connect(this, &ConstructionCfactorUndergroundWallInspectorView::toggleUnitsClicked, m_heightEdit, &OSQuantityEdit2::onUnitSystemChange);
  mainGridLayout->addWidget(m_heightEdit, row, 0);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(row, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void ConstructionCfactorUndergroundWallInspectorView::onClearSelection() {
  detach();

  this->stackedWidget()->setCurrentIndex(0);
}

void ConstructionCfactorUndergroundWallInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto construction = modelObject.cast<model::CFactorUndergroundWallConstruction>();
  attach(construction);

  this->stackedWidget()->setCurrentIndex(1);
}

void ConstructionCfactorUndergroundWallInspectorView::onUpdate() {}

void ConstructionCfactorUndergroundWallInspectorView::attach(
  const openstudio::model::CFactorUndergroundWallConstruction& cFactorUndergroundWallConstruction) {
  m_cFactorUndergroundWallConstruction = cFactorUndergroundWallConstruction;

  // m_nameEdit->bind(cFactorUndergroundWallConstruction,"name");
  m_nameEdit->bind(
    *m_cFactorUndergroundWallConstruction,
    OptionalStringGetter(std::bind(&model::CFactorUndergroundWallConstruction::name, m_cFactorUndergroundWallConstruction.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(
      std::bind(&model::CFactorUndergroundWallConstruction::setName, m_cFactorUndergroundWallConstruction.get_ptr(), std::placeholders::_1)));

  // m_cfactorEdit->bind(cFactorUndergroundWallConstruction,"cFactor",m_isIP);
  m_cfactorEdit->bind(m_isIP, *m_cFactorUndergroundWallConstruction,
                      DoubleGetter(std::bind(&model::CFactorUndergroundWallConstruction::cFactor, m_cFactorUndergroundWallConstruction.get_ptr())),
                      boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::CFactorUndergroundWallConstruction::*)(double)>(
                                                                &model::CFactorUndergroundWallConstruction::setCFactor),
                                                              m_cFactorUndergroundWallConstruction.get_ptr(), std::placeholders::_1)));

  // m_heightEdit->bind(cFactorUndergroundWallConstruction,"height",m_isIP);
  m_heightEdit->bind(m_isIP, *m_cFactorUndergroundWallConstruction,
                     DoubleGetter(std::bind(&model::CFactorUndergroundWallConstruction::height, m_cFactorUndergroundWallConstruction.get_ptr())),
                     boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::CFactorUndergroundWallConstruction::*)(double)>(
                                                               &model::CFactorUndergroundWallConstruction::setHeight),
                                                             m_cFactorUndergroundWallConstruction.get_ptr(), std::placeholders::_1)));

  m_standardsInformationWidget->attach(m_cFactorUndergroundWallConstruction.get());
}

void ConstructionCfactorUndergroundWallInspectorView::detach() {
  m_cfactorEdit->unbind();
  m_heightEdit->unbind();

  m_standardsInformationWidget->detach();

  m_cFactorUndergroundWallConstruction = boost::none;
}

}  // namespace openstudio
