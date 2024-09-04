/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionsView.hpp"

#include "ConstructionCfactorUndergroundWallInspectorView.hpp"
#include "ConstructionFfactorGroundFloorInspectorView.hpp"
#include "ConstructionInspectorView.hpp"
#include "ConstructionAirBoundaryInspectorView.hpp"
#include "ConstructionInternalSourceInspectorView.hpp"
#include "ConstructionWindowDataFileInspectorView.hpp"
#include "ModelObjectTypeListView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>
#include <QStackedWidget>

namespace openstudio {

ConstructionsView::ConstructionsView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(new ModelObjectTypeListView(ConstructionsView::modelObjectTypesAndNames(), model, true, OSItemType::ListItem, false, parent),
                    new ConstructionsInspectorView(isIP, model, parent), false, parent) {
  connect(this, &ConstructionsView::toggleUnitsClicked, modelObjectInspectorView(), &ModelObjectInspectorView::toggleUnitsClicked);
}

std::vector<std::pair<IddObjectType, std::string>> ConstructionsView::modelObjectTypesAndNames() {
  std::vector<std::pair<IddObjectType, std::string>> result;
  result.push_back(std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction, "Constructions"));
  result.push_back(std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction_AirBoundary, "Air Boundary Constructions"));
  result.push_back(std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction_InternalSource, "Internal Source Constructions"));
  result.push_back(
    std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction_CfactorUndergroundWall, "C-factor Underground Wall Constructions"));
  result.push_back(
    std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction_FfactorGroundFloor, "F-factor Ground Floor Constructions"));
  // Not currently supported
  //result.push_back(std::make_pair<IddObjectType, std::string>(IddObjectType::OS_Construction_WindowDataFile, "Window Data File Constructions"));

  return result;
}

ConstructionsInspectorView::ConstructionsInspectorView(bool isIP, const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, false, parent), m_isIP(isIP) {}

void ConstructionsInspectorView::onClearSelection() {
  QWidget* widget = this->stackedWidget()->currentWidget();
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  if (modelObjectInspectorView) {
    modelObjectInspectorView->clearSelection();
  }

  this->stackedWidget()->setCurrentIndex(0);
}

void ConstructionsInspectorView::onUpdate() {}

void ConstructionsInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_Construction:
      this->showConstructionInspector(modelObject);
      break;
    case IddObjectType::OS_Construction_AirBoundary:
      this->showAirBoundaryInspector(modelObject);
      break;
    case IddObjectType::OS_Construction_CfactorUndergroundWall:
      this->showCfactorUndergroundWallInspector(modelObject);
      break;
    case IddObjectType::OS_Construction_FfactorGroundFloor:
      this->showFfactorGroundFloorInspector(modelObject);
      break;
    case IddObjectType::OS_Construction_InternalSource:
      this->showInternalSourceInspector(modelObject);
      break;
    case IddObjectType::OS_Construction_WindowDataFile:
      this->showWindowDataFileInspector(modelObject);
      break;
    default:
      showDefaultView();
  }
}

void ConstructionsInspectorView::showConstructionInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionInspectorView = new ConstructionInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionInspectorView, &ConstructionInspectorView::toggleUnitsClicked);

  constructionInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionInspectorView);
}

void ConstructionsInspectorView::showAirBoundaryInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionAirBoundaryInspectorView = new ConstructionAirBoundaryInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionAirBoundaryInspectorView,
          &ConstructionAirBoundaryInspectorView::toggleUnitsClicked);

  constructionAirBoundaryInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionAirBoundaryInspectorView);
}

void ConstructionsInspectorView::showCfactorUndergroundWallInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionCfactorUndergroundWallInspectorView = new ConstructionCfactorUndergroundWallInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionCfactorUndergroundWallInspectorView,
          &ConstructionCfactorUndergroundWallInspectorView::toggleUnitsClicked);

  constructionCfactorUndergroundWallInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionCfactorUndergroundWallInspectorView);
}

void ConstructionsInspectorView::showFfactorGroundFloorInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionFfactorGroundFloorInspectorView = new ConstructionFfactorGroundFloorInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionFfactorGroundFloorInspectorView,
          &ConstructionFfactorGroundFloorInspectorView::toggleUnitsClicked);

  constructionFfactorGroundFloorInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionFfactorGroundFloorInspectorView);
}

void ConstructionsInspectorView::showInternalSourceInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionInternalSourceInspectorView = new ConstructionInternalSourceInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionInternalSourceInspectorView,
          &ConstructionInternalSourceInspectorView::toggleUnitsClicked);

  constructionInternalSourceInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionInternalSourceInspectorView);
}

void ConstructionsInspectorView::showWindowDataFileInspector(const openstudio::model::ModelObject& modelObject) {
  auto* constructionWindowDataFileInspectorView = new ConstructionWindowDataFileInspectorView(m_isIP, m_model);
  connect(this, &ConstructionsInspectorView::toggleUnitsClicked, constructionWindowDataFileInspectorView,
          &ConstructionWindowDataFileInspectorView::toggleUnitsClicked);

  constructionWindowDataFileInspectorView->selectModelObject(modelObject);

  this->showInspector(constructionWindowDataFileInspectorView);
}

void ConstructionsInspectorView::showInspector(QWidget* widget) {
  if (QWidget* _widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(_widget);

    delete _widget;
  }

  this->stackedWidget()->addWidget(widget);
}

void ConstructionsInspectorView::showDefaultView() {
  if (QWidget* widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(widget);

    delete widget;
  }
}

/****************** SLOTS ******************/

void ConstructionsInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
