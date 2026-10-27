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
#include <openstudio/model/Loop.hpp>
#include <openstudio/model/StraightComponent.hpp>
#include <openstudio/model/StraightComponent_Impl.hpp>

#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

boost::optional<HeatPumpAirToWaterType> heatPumpAirToWaterType(const model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_HeatPump_AirToWater:
      return HeatPumpAirToWaterType::Wrapper;
    case IddObjectType::OS_HeatPump_AirToWater_Heating:
      return HeatPumpAirToWaterType::Heating;
    case IddObjectType::OS_HeatPump_AirToWater_Cooling:
      return HeatPumpAirToWaterType::Cooling;
    default:
      return boost::none;
  }
}

boost::optional<HeatPumpAirToWaterType> suggestedHeatPumpAirToWaterMode(const model::PlantLoop& plantLoop) {
  const std::string loopType = plantLoop.sizingPlant().loopType();
  if (istringEqual(loopType, "Heating")) {
    return HeatPumpAirToWaterType::Heating;
  }
  if (istringEqual(loopType, "Cooling")) {
    return HeatPumpAirToWaterType::Cooling;
  }
  return boost::none;
}

bool addHeatPumpAirToWaterModeToLoop(const model::HeatPumpAirToWater& heatPump, HeatPumpAirToWaterType mode, const model::HVACComponent& target) {
  boost::optional<model::StraightComponent> modeComponent;
  if (mode == HeatPumpAirToWaterType::Heating) {
    if (auto heating = heatPump.heatingOperationMode()) {
      modeComponent = heating.get();
    }
  } else if (mode == HeatPumpAirToWaterType::Cooling) {
    if (auto cooling = heatPump.coolingOperationMode()) {
      modeComponent = cooling.get();
    }
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

boost::optional<model::HeatPumpAirToWater> heatPumpAirToWaterForMode(const model::ModelObject& modelObject) {
  const boost::optional<HeatPumpAirToWaterType> type = heatPumpAirToWaterType(modelObject);
  if (type == HeatPumpAirToWaterType::Heating) {
    return modelObject.cast<model::HeatPumpAirToWaterHeating>().heatPumpAirToWater();
  }
  if (type == HeatPumpAirToWaterType::Cooling) {
    return modelObject.cast<model::HeatPumpAirToWaterCooling>().heatPumpAirToWater();
  }
  return boost::none;
}

bool removeHeatPumpAirToWaterModeFromLoop(const model::ModelObject& modelObject) {
  boost::optional<model::HeatPumpAirToWater> heatPump = heatPumpAirToWaterForMode(modelObject);
  if (!heatPump) {
    return false;
  }
  auto mode = modelObject.cast<model::StraightComponent>();
  if (mode.loop()) {
    mode.removeFromLoop();
  }

  boost::optional<model::StraightComponent> otherMode;
  if (heatPumpAirToWaterType(modelObject) == HeatPumpAirToWaterType::Heating) {
    if (auto cooling = heatPump->coolingOperationMode()) {
      otherMode = cooling.get();
    }
  } else if (auto heating = heatPump->heatingOperationMode()) {
    otherMode = heating.get();
  }
  if (!otherMode || !otherMode->loop()) {
    heatPump->remove();
  }
  return true;
}

}  // namespace openstudio
