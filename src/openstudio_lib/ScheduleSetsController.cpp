/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScheduleSetsController.hpp"
#include "ScheduleSetsView.hpp"

#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/DefaultScheduleSet_Impl.hpp>

#include <openstudio/utilities/core/Logger.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

ScheduleSetsController::ScheduleSetsController(const model::Model& model) : ModelSubTabController(new ScheduleSetsView(model), model) {}

void ScheduleSetsController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  switch (iddObjectType.value()) {
    case IddObjectType::OS_DefaultScheduleSet:
      openstudio::model::DefaultScheduleSet(this->model());
      break;
    default:
      LOG_FREE_AND_THROW("ScheduleSetsController", "Unknown IddObjectType '" << iddObjectType.valueName() << "'");
  }
}

void ScheduleSetsController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->model());
}

void ScheduleSetsController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void ScheduleSetsController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void ScheduleSetsController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  this->model().purgeUnusedResourceObjects(iddObjectType);
}

void ScheduleSetsController::onDrop(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::DefaultScheduleSet>()) {
      if (this->fromComponentLibrary(itemId)) {
        modelObject->clone(this->model());
      }
    }
  }
}

void ScheduleSetsController::onInspectItem(OSItem* item) {}

}  // namespace openstudio
