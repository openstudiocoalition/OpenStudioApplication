/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleCompactInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSDoubleEdit.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ScheduleCompact.hpp>
#include <openstudio/model/ScheduleCompact_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QPlainTextEdit>

#include <sstream>

namespace openstudio {

// ScheduleCompactInspectorView

ScheduleCompactInspectorView::ScheduleCompactInspectorView(const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  createLayout();
}

void ScheduleCompactInspectorView::createLayout() {
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
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 1);

  ++row;

  // Value

  label = new QLabel("Content: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_content = new QPlainTextEdit();
  m_content->setReadOnly(true);
  const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  m_content->setFont(f);

  mainGridLayout->addWidget(m_content, row++, 0, 1, 2);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);
  mainGridLayout->setColumnStretch(0, 3);
  mainGridLayout->setColumnStretch(1, 1);
  mainGridLayout->setColumnStretch(2, 1);
}

void ScheduleCompactInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void ScheduleCompactInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto sch = modelObject.cast<model::ScheduleCompact>();
  attach(sch);
  refresh();
}

void ScheduleCompactInspectorView::onUpdate() {
  refresh();
}

void ScheduleCompactInspectorView::attach(openstudio::model::ScheduleCompact& sch) {
  m_sch = sch;

  m_nameEdit->bind(
    *m_sch, OptionalStringGetter(std::bind(&model::ScheduleCompact::name, m_sch.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::ScheduleCompact::setName, m_sch.get_ptr(), std::placeholders::_1)));

  std::stringstream ss;
  sch.print(ss);

  m_content->setPlainText(toQString(ss.str()));

  this->stackedWidget()->setCurrentIndex(1);
}

void ScheduleCompactInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();

  m_sch = boost::none;
}

void ScheduleCompactInspectorView::refresh() {}

}  // namespace openstudio
