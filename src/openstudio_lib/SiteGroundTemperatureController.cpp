/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SiteGroundTemperatureController.hpp"

#include "SiteGroundTemperatureMonthlyWidget.hpp"
#include "OSItemSelectorButtons.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

SiteGroundTemperatureController::SiteGroundTemperatureController(bool isIP, const model::Model& model)
  : ModelSubTabController(new SiteGroundTemperatureMonthlyView(isIP, model), model) {
  subTabView()->itemSelectorButtons()->hideAddButton();
  subTabView()->itemSelectorButtons()->hideRemoveButton();
  subTabView()->itemSelectorButtons()->hideCopyButton();
  subTabView()->itemSelectorButtons()->hidePurgeButton();
  subTabView()->itemSelectorButtons()->hideDropZone();
}

void SiteGroundTemperatureController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  model::Model model = this->model();
  if (iddObjectType == model::SiteGroundTemperatureBuildingSurface::iddObjectType()) {
    model.getUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
  } else if (iddObjectType == model::SiteGroundTemperatureShallow::iddObjectType()) {
    model.getUniqueModelObject<model::SiteGroundTemperatureShallow>();
  }
}

void SiteGroundTemperatureController::onCopyObject(const openstudio::model::ModelObject& /*modelObject*/) {
  // not applicable for unique objects
}

void SiteGroundTemperatureController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void SiteGroundTemperatureController::onReplaceObject(openstudio::model::ModelObject /*modelObject*/, const OSItemId& /*replacementItemId*/) {
  // not yet implemented
}

void SiteGroundTemperatureController::onPurgeObjects(const openstudio::IddObjectType& /*iddObjectType*/) {
  // purge button is disabled
}

void SiteGroundTemperatureController::onDrop(const OSItemId& /*itemId*/) {
  // drop zone is hidden
}

void SiteGroundTemperatureController::onInspectItem(OSItem* item) {
  subTabView()->inspectorView()->selectItem(item);
}

}  // namespace openstudio
