/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionsController.hpp"
#include "ConstructionsView.hpp"

#include <openstudio/model/CFactorUndergroundWallConstruction.hpp>
#include <openstudio/model/Component.hpp>
#include <openstudio/model/Component_Impl.hpp>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/ComponentData_Impl.hpp>
#include <openstudio/model/Construction.hpp>
#include <openstudio/model/ConstructionAirBoundary.hpp>
#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/ConstructionWithInternalSource.hpp>
#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/FFactorGroundFloorConstruction.hpp>
#include <openstudio/model/WindowDataFile.hpp>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

ConstructionsController::ConstructionsController(bool isIP, const model::Model& model)
  : ModelSubTabController(new ConstructionsView(isIP, model), model) {}

ConstructionsController::~ConstructionsController() = default;

void ConstructionsController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  switch (iddObjectType.value()) {
    case IddObjectType::OS_Construction:
      openstudio::model::Construction(this->model());
      break;
    case IddObjectType::OS_Construction_AirBoundary:
      openstudio::model::ConstructionAirBoundary(this->model());
      break;
    case IddObjectType::OS_Construction_InternalSource:
      openstudio::model::ConstructionWithInternalSource(this->model());
      break;
    case IddObjectType::OS_Construction_CfactorUndergroundWall:
      openstudio::model::CFactorUndergroundWallConstruction(this->model());
      break;
    case IddObjectType::OS_Construction_FfactorGroundFloor:
      openstudio::model::FFactorGroundFloorConstruction(this->model());
      break;
    case IddObjectType::OS_Construction_WindowDataFile:
      openstudio::model::WindowDataFile(this->model());
      break;
    default:
      LOG_FREE_AND_THROW("ConstructionsController", "Unknown IddObjectType '" << iddObjectType.valueName() << "'");
  }
}

void ConstructionsController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->model());
}

void ConstructionsController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void ConstructionsController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void ConstructionsController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  this->model().purgeUnusedResourceObjects(iddObjectType);
}

void ConstructionsController::onDrop(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::ConstructionBase>()) {
      if (this->fromComponentLibrary(itemId)) {
        modelObject->clone(this->model());
      }
    }
  } else {
    boost::optional<model::Component> component = this->getComponent(itemId);
    if (component) {
      if (component->primaryObject().optionalCast<model::ModelObject>()) {
        this->model().insertComponent(*component);
      }
    }
  }
}

void ConstructionsController::onInspectItem(OSItem* item) {}

}  // namespace openstudio
