/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "DefaultConstructionSetsController.hpp"
#include "DefaultConstructionSetsView.hpp"

#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/DefaultSurfaceConstructions.hpp>
#include <openstudio/model/DefaultSubSurfaceConstructions.hpp>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

DefaultConstructionSetsController::DefaultConstructionSetsController(const model::Model& model)
  : ModelSubTabController(new DefaultConstructionSetsView(model, "Default Constructions", false), model) {}

DefaultConstructionSetsController::~DefaultConstructionSetsController() = default;

void DefaultConstructionSetsController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  switch (iddObjectType.value()) {
    case IddObjectType::OS_DefaultConstructionSet:
      openstudio::model::DefaultConstructionSet(this->model());
      break;
    default:
      LOG_FREE_AND_THROW("DefaultConstructionSetsController", "Unknown IddObjectType '" << iddObjectType.valueName() << "'");
  }
}

void DefaultConstructionSetsController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->model());
}

void DefaultConstructionSetsController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  boost::optional<model::DefaultConstructionSet> dcs = modelObject.optionalCast<model::DefaultConstructionSet>();
  if (dcs) {
    boost::optional<model::DefaultSurfaceConstructions> dsc = dcs->defaultExteriorSurfaceConstructions();
    if (dsc && (dsc->directUseCount(true) == 1)) {
      dsc->remove();
    }

    dsc = dcs->defaultInteriorSurfaceConstructions();
    if (dsc && (dsc->directUseCount(true) == 1)) {
      dsc->remove();
    }

    dsc = dcs->defaultGroundContactSurfaceConstructions();
    if (dsc && (dsc->directUseCount(true) == 1)) {
      dsc->remove();
    }

    boost::optional<model::DefaultSubSurfaceConstructions> dssc = dcs->defaultExteriorSubSurfaceConstructions();
    if (dssc && (dssc->directUseCount(true) == 1)) {
      dssc->remove();
    }

    dssc = dcs->defaultInteriorSubSurfaceConstructions();
    if (dssc && (dssc->directUseCount(true) == 1)) {
      dssc->remove();
    }
  }

  modelObject.remove();
}

void DefaultConstructionSetsController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void DefaultConstructionSetsController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  if (iddObjectType == IddObjectType::OS_DefaultConstructionSet) {
    this->model().purgeUnusedResourceObjects(IddObjectType::OS_DefaultConstructionSet);
    this->model().purgeUnusedResourceObjects(IddObjectType::OS_DefaultSurfaceConstructions);
    this->model().purgeUnusedResourceObjects(IddObjectType::OS_DefaultSubSurfaceConstructions);
  } else {
    this->model().purgeUnusedResourceObjects(iddObjectType);
  }
}

void DefaultConstructionSetsController::onDrop(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::DefaultConstructionSet>()) {
      if (this->fromComponentLibrary(itemId)) {
        modelObject->clone(this->model());
      }
    }
  }
}

void DefaultConstructionSetsController::onInspectItem(OSItem* item) {}

}  // namespace openstudio
