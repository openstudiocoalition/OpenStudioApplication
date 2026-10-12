/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "../HeatPumpAirToWaterPlacement.hpp"

#include <openstudio/model/BoilerHotWater.hpp>
#include <openstudio/model/BoilerHotWater_Impl.hpp>
#include <openstudio/model/HeatPumpAirToWater.hpp>
#include <openstudio/model/HeatPumpAirToWater_Impl.hpp>
#include <openstudio/model/HeatPumpAirToWaterCooling.hpp>
#include <openstudio/model/HeatPumpAirToWaterCooling_Impl.hpp>
#include <openstudio/model/HeatPumpAirToWaterHeating.hpp>
#include <openstudio/model/HeatPumpAirToWaterHeating_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Node_Impl.hpp>
#include <openstudio/model/PlantLoop.hpp>
#include <openstudio/model/PlantLoop_Impl.hpp>
#include <openstudio/model/SizingPlant.hpp>
#include <openstudio/model/SizingPlant_Impl.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Splitter_Impl.hpp>

using namespace openstudio;
using namespace openstudio::model;

namespace {
HeatPumpAirToWater makeHeatPumpWithBothModes(const Model& model) {
  HeatPumpAirToWater heatPump(model);
  HeatPumpAirToWaterHeating heating(model);
  HeatPumpAirToWaterCooling cooling(model);
  EXPECT_TRUE(heatPump.setHeatingOperationMode(heating));
  EXPECT_TRUE(heatPump.setCoolingOperationMode(cooling));
  return heatPump;
}
}  // namespace

TEST(HeatPumpAirToWaterPlacement, SuggestedModeFollowsSizingPlantLoopType) {
  Model model;
  PlantLoop plantLoop(model);

  // A new PlantLoop's Sizing:Plant defaults to "Heating"
  auto suggested = suggestedHeatPumpAirToWaterMode(plantLoop);
  ASSERT_TRUE(suggested);
  EXPECT_EQ(HeatPumpAirToWaterMode::Heating, suggested.get());

  ASSERT_TRUE(plantLoop.sizingPlant().setLoopType("Cooling"));
  suggested = suggestedHeatPumpAirToWaterMode(plantLoop);
  ASSERT_TRUE(suggested);
  EXPECT_EQ(HeatPumpAirToWaterMode::Cooling, suggested.get());

  ASSERT_TRUE(plantLoop.sizingPlant().setLoopType("Condenser"));
  EXPECT_FALSE(suggestedHeatPumpAirToWaterMode(plantLoop));
}

TEST(HeatPumpAirToWaterPlacement, PlacesBothModesOfOneWrapperOnSeparateLoops) {
  Model model;
  PlantLoop hotWaterLoop(model);
  PlantLoop chilledWaterLoop(model);
  ASSERT_TRUE(chilledWaterLoop.sizingPlant().setLoopType("Cooling"));

  // Mirrors a library drop: the library wrapper is cloned, which clones both of its modes
  HeatPumpAirToWater libraryHeatPump = makeHeatPumpWithBothModes(model);
  auto heatPump = libraryHeatPump.clone(model).cast<HeatPumpAirToWater>();
  ASSERT_TRUE(heatPump.heatingOperationMode());
  ASSERT_TRUE(heatPump.coolingOperationMode());
  EXPECT_NE(libraryHeatPump.heatingOperationMode()->handle(), heatPump.heatingOperationMode()->handle());

  Node hotWaterNode = hotWaterLoop.supplyOutletNode();
  EXPECT_TRUE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Heating, hotWaterNode));
  ASSERT_TRUE(heatPump.heatingLoop());
  EXPECT_EQ(hotWaterLoop.handle(), heatPump.heatingLoop()->handle());
  EXPECT_FALSE(heatPump.coolingLoop());

  Splitter chilledWaterSplitter = chilledWaterLoop.supplySplitter();
  EXPECT_TRUE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Cooling, chilledWaterSplitter));
  ASSERT_TRUE(heatPump.coolingLoop());
  EXPECT_EQ(chilledWaterLoop.handle(), heatPump.coolingLoop()->handle());

  // Both modes still belong to the same wrapper
  EXPECT_EQ(heatPump.handle(), heatPump.heatingOperationMode()->heatPumpAirToWater()->handle());
  EXPECT_EQ(heatPump.handle(), heatPump.coolingOperationMode()->heatPumpAirToWater()->handle());

  // The library wrapper's own modes were not placed
  EXPECT_FALSE(libraryHeatPump.heatingLoop());
  EXPECT_FALSE(libraryHeatPump.coolingLoop());
}

TEST(HeatPumpAirToWaterPlacement, MissingModeIsRejected) {
  Model model;
  PlantLoop plantLoop(model);
  HeatPumpAirToWater heatPump(model);
  HeatPumpAirToWaterHeating heating(model);
  ASSERT_TRUE(heatPump.setHeatingOperationMode(heating));

  Node node = plantLoop.supplyOutletNode();
  const auto supplyComponentCount = plantLoop.supplyComponents().size();
  EXPECT_FALSE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Cooling, node));
  EXPECT_EQ(supplyComponentCount, plantLoop.supplyComponents().size());
  EXPECT_FALSE(heatPump.coolingLoop());
}

TEST(HeatPumpAirToWaterPlacement, DemandSideIsRejected) {
  Model model;
  PlantLoop plantLoop(model);
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);

  Node demandNode = plantLoop.demandInletNode();
  EXPECT_FALSE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Heating, demandNode));

  Splitter demandSplitter = plantLoop.demandSplitter();
  EXPECT_FALSE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Heating, demandSplitter));

  EXPECT_FALSE(heatPump.heatingLoop());
}

TEST(HeatPumpAirToWaterPlacement, NonNodeNonSplitterTargetIsRejected) {
  Model model;
  PlantLoop plantLoop(model);
  BoilerHotWater boiler(model);
  Node node = plantLoop.supplyOutletNode();
  ASSERT_TRUE(boiler.addToNode(node));
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);

  EXPECT_FALSE(addHeatPumpAirToWaterModeToLoop(heatPump, HeatPumpAirToWaterMode::Heating, boiler));
  EXPECT_FALSE(heatPump.heatingLoop());
}
