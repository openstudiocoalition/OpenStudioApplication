/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleOthersView.hpp"
#include "ModelObjectTypeListView.hpp"

#include "ScheduleConstantInspectorView.hpp"
#include "ScheduleCompactInspectorView.hpp"
#include "ScheduleFileInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QStackedWidget>

namespace openstudio {

ScheduleOthersView::ScheduleOthersView(const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(
      new ModelObjectTypeListView(ScheduleOthersView::modelObjectTypesAndNames(), model, true, OSItemType::CollapsibleListHeader, false, parent),
      new ScheduleOthersInspectorView(model, parent), false, parent) {}

void ScheduleOthersView::setCurrentSchedule(const openstudio::model::ModelObject& modelObject) {
  qobject_cast<ScheduleOthersInspectorView*>(modelObjectInspectorView())->setCurrentSchedule(modelObject);
}

std::vector<std::pair<IddObjectType, std::string>> ScheduleOthersView::modelObjectTypesAndNames() {
  return {{
    {IddObjectType::OS_Schedule_Constant, "Schedule Constant"},
    {IddObjectType::OS_Schedule_Compact, "Schedule Compact"},
    {IddObjectType::OS_Schedule_File, "Schedule File"},
  }};
}

ScheduleOthersInspectorView::ScheduleOthersInspectorView(const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, false, parent) {}

void ScheduleOthersInspectorView::onClearSelection() {
  QWidget* widget = this->stackedWidget()->currentWidget();
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  if (modelObjectInspectorView) {
    modelObjectInspectorView->clearSelection();
  }

  this->stackedWidget()->setCurrentIndex(0);
}

void ScheduleOthersInspectorView::onUpdate() {}

void ScheduleOthersInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_Schedule_Constant:
      this->showScheduleConstantView(modelObject);
      break;
    case IddObjectType::OS_Schedule_Compact:
      this->showScheduleCompactView(modelObject);
      break;
    case IddObjectType::OS_Schedule_File:
      this->showScheduleFileView(modelObject);
      break;
    default:
      showDefaultView();
  }
}

void ScheduleOthersInspectorView::setCurrentSchedule(const openstudio::model::ModelObject& modelObject) {
  onSelectModelObject(modelObject);
}

void ScheduleOthersInspectorView::showScheduleConstantView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleConstantInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showScheduleCompactView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleCompactInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showScheduleFileView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleFileInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showInspector(QWidget* widget) {
  if (QWidget* _widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(_widget);

    delete _widget;
  }

  this->stackedWidget()->addWidget(widget);
}

void ScheduleOthersInspectorView::showDefaultView() {
  if (QWidget* widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(widget);

    delete widget;
  }
}

}  // namespace openstudio
