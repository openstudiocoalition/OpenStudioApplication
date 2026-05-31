/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleOthersView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include "ScheduleConstantInspectorView.hpp"
#include "ScheduleCompactInspectorView.hpp"
#include "ScheduleFileInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QStackedWidget>
#include <QStringLiteral>
#include <tuple>

namespace openstudio {

ScheduleOthersView::ScheduleOthersView(const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(
      new ModelObjectTypeListView(ScheduleOthersView::modelObjectTypesNamesAndUrls(), model, true, OSItemType::CollapsibleListHeader, false, parent),
      new ScheduleOthersInspectorView(model, parent), false, parent) {}

std::vector<std::tuple<IddObjectType, QString, QString>> ScheduleOthersView::modelObjectTypesNamesAndUrls() {
  static const QString base = QString::fromStdString(openstudio::bigladdersoftwareDocBaseUrl());
  static const QString sch = base + QStringLiteral("group-schedules.html");

  using T = std::tuple<IddObjectType, QString, QString>;
  return {
    T{IddObjectType::OS_Schedule_Constant, tr("Schedule Constant"), sch + "#scheduleconstant"},
    T{IddObjectType::OS_Schedule_Compact, tr("Schedule Compact"), sch + "#schedulecompact"},
    T{IddObjectType::OS_Schedule_File, tr("Schedule File"), sch + "#schedulefile"},
  };
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
