/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionAirBoundaryInspectorView.hpp"
#include "StandardsInformationConstructionWidget.hpp"

#include "OSItem.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/OSComboBox.hpp"

#include <openstudio/model/ConstructionAirBoundary.hpp>
#include <openstudio/model/ConstructionAirBoundary_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QComboBox>

namespace openstudio {

ConstructionAirBoundaryInspectorView::ConstructionAirBoundaryInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent),
    m_isIP(isIP),
    m_nameEdit(nullptr),
    m_standardsInformationWidget(nullptr),
    m_airExchangeMethod(nullptr),
    m_simpleMixingAirChangesPerHour(nullptr),
    m_scheduleDropZone(nullptr) {
  createLayout();
}

void ConstructionAirBoundaryInspectorView::createLayout() {
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

  // Air Exchange Method

  label = new QLabel("Air Exchange Method: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_airExchangeMethod = new OSComboBox2(nullptr, false);  // editable = false
  mainGridLayout->addWidget(m_airExchangeMethod, row, 0);

  ++row;

  // Simple Mixing Air Changes per Hour

  label = new QLabel("Simple Mixing Air Changes per Hour: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_simpleMixingAirChangesPerHour = new OSQuantityEdit2("1/hr", "1/hr", "1/hr", m_isIP);
  mainGridLayout->addWidget(m_simpleMixingAirChangesPerHour, row, 0);

  ++row;

  // Simple Mixing Schedule

  //label = new QLabel("Simple Mixing Schedule: ");
  //label->setObjectName("H2");
  //mainGridLayout->addWidget(label, row, 0);

  //++row;

  //m_scheduleDropZone = new OSDropZone("", "", "", m_isIP);
  //mainGridLayout->addWidget(m_scheduleDropZone, row, 0);

  //++row;

  // Stretch

  mainGridLayout->setRowStretch(row, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void ConstructionAirBoundaryInspectorView::onClearSelection() {
  detach();

  this->stackedWidget()->setCurrentIndex(0);
}

void ConstructionAirBoundaryInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto construction = modelObject.cast<model::ConstructionAirBoundary>();
  attach(construction);

  this->stackedWidget()->setCurrentIndex(1);
}

void ConstructionAirBoundaryInspectorView::onUpdate() {}

void ConstructionAirBoundaryInspectorView::attach(const openstudio::model::ConstructionAirBoundary& constructionAirBoundary) {
  m_constructionAirBoundary = constructionAirBoundary;

  m_nameEdit->bind(*m_constructionAirBoundary,
                   OptionalStringGetter(std::bind(&model::ConstructionAirBoundary::name, m_constructionAirBoundary.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::ConstructionAirBoundary::setName, m_constructionAirBoundary.get_ptr(), std::placeholders::_1)));

  m_airExchangeMethod->bind<std::string>(
    *m_constructionAirBoundary, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    &model::ConstructionAirBoundary::airExchangeMethodValues,
    std::bind(&model::ConstructionAirBoundary::airExchangeMethod, m_constructionAirBoundary.get_ptr()),
    std::bind(&model::ConstructionAirBoundary::setAirExchangeMethod, m_constructionAirBoundary.get_ptr(), std::placeholders::_1),
    boost::optional<NoFailAction>(std::bind(&model::ConstructionAirBoundary::resetAirExchangeMethod, m_constructionAirBoundary.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ConstructionAirBoundary::isAirExchangeMethodDefaulted, m_constructionAirBoundary.get_ptr())));

  m_simpleMixingAirChangesPerHour->bind(
    m_isIP, *m_constructionAirBoundary,
    DoubleGetter(std::bind(&model::ConstructionAirBoundary::simpleMixingAirChangesPerHour, m_constructionAirBoundary.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::ConstructionAirBoundary::*)(double)>(&model::ConstructionAirBoundary::setSimpleMixingAirChangesPerHour),
                m_constructionAirBoundary.get_ptr(), std::placeholders::_1)));

  m_standardsInformationWidget->attach(m_constructionAirBoundary.get());
}

void ConstructionAirBoundaryInspectorView::detach() {
  m_nameEdit->unbind();
  m_airExchangeMethod->unbind();
  m_simpleMixingAirChangesPerHour->unbind();

  m_standardsInformationWidget->detach();

  m_constructionAirBoundary = boost::none;
}

}  // namespace openstudio
