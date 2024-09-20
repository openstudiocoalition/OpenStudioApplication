/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleConstantInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSDoubleEdit.hpp"

#include <openstudio/model/ScheduleConstant.hpp>
#include <openstudio/model/ScheduleConstant_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// ScheduleConstantInspectorView

ScheduleConstantInspectorView::ScheduleConstantInspectorView(const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  createLayout();
}

void ScheduleConstantInspectorView::createLayout() {
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
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 2);

  ++row;

  // Value

  label = new QLabel(" Value: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_value = new OSDoubleEdit2();
  mainGridLayout->addWidget(m_value, row++, 0, 1, 1);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(0, 1);
  mainGridLayout->setColumnStretch(1, 1);
  mainGridLayout->setColumnStretch(2, 1);
}

void ScheduleConstantInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void ScheduleConstantInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto sch = modelObject.cast<model::ScheduleConstant>();
  attach(sch);
  refresh();
}

void ScheduleConstantInspectorView::onUpdate() {
  refresh();
}

void ScheduleConstantInspectorView::attach(openstudio::model::ScheduleConstant& sch) {
  m_sch = sch;

  // m_nameEdit->bind(sch,"name");
  m_nameEdit->bind(
    *m_sch, OptionalStringGetter(std::bind(&model::ScheduleConstant::name, m_sch.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::ScheduleConstant::setName, m_sch.get_ptr(), std::placeholders::_1)));

  // m_value->bind(sch,"value",m_isIP);
  m_value->bind(*m_sch, DoubleGetter(std::bind(&model::ScheduleConstant::value, m_sch.get_ptr())),
                //static_cast<void(Client::*)(int)>(&Client::foobar)
                boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::ScheduleConstant::*)(double)>(&model::ScheduleConstant::setValue),
                                                        m_sch.get_ptr(), std::placeholders::_1)));

  this->stackedWidget()->setCurrentIndex(1);
}

void ScheduleConstantInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_value->unbind();

  m_sch = boost::none;
}

void ScheduleConstantInspectorView::refresh() {}

}  // namespace openstudio
