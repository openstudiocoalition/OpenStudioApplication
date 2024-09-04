/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleSetsView.hpp"
#include "ModelObjectListView.hpp"
#include "OSItem.hpp"
#include "ScheduleSetInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <sstream>

#include <openstudio/utilities/idd/IddEnums.hxx>
namespace openstudio {

ScheduleSetsView::ScheduleSetsView(const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(new ModelObjectListView(IddObjectType::OS_DefaultScheduleSet, model, true, false, parent),
                    new ScheduleSetInspectorView(model, parent), false, parent) {}

std::vector<std::pair<IddObjectType, std::string>> ScheduleSetsView::modelObjectTypesAndNames() {
  std::vector<std::pair<IddObjectType, std::string>> result;
  result.push_back(std::make_pair<IddObjectType, std::string>(IddObjectType::OS_DefaultScheduleSet, "ScheduleSets"));
  return result;
}

ScheduleSetsInspectorView::ScheduleSetsInspectorView(const model::Model& model, QWidget* parent) : ModelObjectInspectorView(model, false, parent) {
  // index of hidden widget is 0
  auto* hiddenWidget = new QWidget();
  int index = this->stackedWidget()->addWidget(hiddenWidget);
  OS_ASSERT(index == 0);

  // index of the default is 1
  auto* defaultInspectorView = new DefaultInspectorView(model, parent);
  index = this->stackedWidget()->addWidget(defaultInspectorView);
  OS_ASSERT(index == 1);

  //ScheduleSetsInspectorView* scheduleSetsInspectorView = new ScheduleSetsInspectorView(model, parent);
  //index = this->stackedWidget()->addWidget(scheduleSetsInspectorView);
  //m_inspectorIndexMap[IddObjectType::OS_ScheduleSets] = index;
}

void ScheduleSetsInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  QWidget* widget = this->stackedWidget()->currentWidget();
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  OS_ASSERT(modelObjectInspectorView);
  modelObjectInspectorView->clearSelection();

  this->stackedWidget()->setCurrentIndex(0);
}

void ScheduleSetsInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  int index = 1;
  auto it = m_inspectorIndexMap.find(modelObject.iddObjectType());
  if (it != m_inspectorIndexMap.end()) {
    index = it->second;
  }

  QWidget* widget = this->stackedWidget()->widget(index);
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  OS_ASSERT(modelObjectInspectorView);
  modelObjectInspectorView->selectModelObject(modelObject);
  this->stackedWidget()->setCurrentIndex(index);
}

void ScheduleSetsInspectorView::onUpdate() {}

}  // namespace openstudio
