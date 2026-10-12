/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HeatPumpAirToWaterPlacement.hpp"

#include <openstudio/model/HeatPumpAirToWaterCooling.hpp>
#include <openstudio/model/HeatPumpAirToWaterCooling_Impl.hpp>
#include <openstudio/model/HeatPumpAirToWaterHeating.hpp>
#include <openstudio/model/HeatPumpAirToWaterHeating_Impl.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Node_Impl.hpp>
#include <openstudio/model/SizingPlant.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Splitter_Impl.hpp>
#include <openstudio/model/StraightComponent.hpp>

#include <openstudio/utilities/core/Compare.hpp>

namespace openstudio {

boost::optional<HeatPumpAirToWaterMode> suggestedHeatPumpAirToWaterMode(const model::PlantLoop& plantLoop) {
  const std::string loopType = plantLoop.sizingPlant().loopType();
  if (istringEqual(loopType, "Heating")) {
    return HeatPumpAirToWaterMode::Heating;
  }
  if (istringEqual(loopType, "Cooling")) {
    return HeatPumpAirToWaterMode::Cooling;
  }
  return boost::none;
}

bool addHeatPumpAirToWaterModeToLoop(const model::HeatPumpAirToWater& heatPump, HeatPumpAirToWaterMode mode, const model::HVACComponent& target) {
  boost::optional<model::StraightComponent> modeComponent;
  if (mode == HeatPumpAirToWaterMode::Heating) {
    if (auto heating = heatPump.heatingOperationMode()) {
      modeComponent = heating.get();
    }
  } else if (auto cooling = heatPump.coolingOperationMode()) {
    modeComponent = cooling.get();
  }
  if (!modeComponent) {
    return false;
  }

  if (auto node = target.optionalCast<model::Node>()) {
    return modeComponent->addToNode(node.get());
  }

  if (auto splitter = target.optionalCast<model::Splitter>()) {
    if (auto plant = splitter->plantLoop()) {
      if (plant->supplyComponent(splitter->handle())) {
        return plant->addSupplyBranchForComponent(modeComponent.get());
      }
    }
  }

  return false;
}

}  // namespace openstudio
