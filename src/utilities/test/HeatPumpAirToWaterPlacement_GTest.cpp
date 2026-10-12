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
#include <openstudio/model/HeatPumpAirToWaterHeatingSpeedData.hpp>
#include <openstudio/model/HeatPumpAirToWaterHeatingSpeedData_Impl.hpp>
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

TEST(HeatPumpAirToWaterPlacement, ModeFindsItsWrapper) {
  Model model;
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);

  auto heatingWrapper = heatPumpAirToWaterForMode(heatPump.heatingOperationMode().get());
  ASSERT_TRUE(heatingWrapper);
  EXPECT_EQ(heatPump.handle(), heatingWrapper->handle());

  auto coolingWrapper = heatPumpAirToWaterForMode(heatPump.coolingOperationMode().get());
  ASSERT_TRUE(coolingWrapper);
  EXPECT_EQ(heatPump.handle(), coolingWrapper->handle());

  // The wrapper itself, an orphaned mode, and unrelated objects have no wrapper
  EXPECT_FALSE(heatPumpAirToWaterForMode(heatPump));
  HeatPumpAirToWaterHeating orphan(model);
  EXPECT_FALSE(heatPumpAirToWaterForMode(orphan));
  BoilerHotWater boiler(model);
  EXPECT_FALSE(heatPumpAirToWaterForMode(boiler));
}

TEST(HeatPumpAirToWaterPlacement, RemovingFirstModeDisconnectsItFromItsLoopOnly) {
  Model model;
  PlantLoop hotWaterLoop(model);
  PlantLoop chilledWaterLoop(model);
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);
  HeatPumpAirToWaterHeating heating = heatPump.heatingOperationMode().get();
  HeatPumpAirToWaterCooling cooling = heatPump.coolingOperationMode().get();
  ASSERT_TRUE(hotWaterLoop.addSupplyBranchForComponent(heating));
  ASSERT_TRUE(chilledWaterLoop.addSupplyBranchForComponent(cooling));

  EXPECT_TRUE(removeHeatPumpAirToWaterModeFromLoop(heating));
  EXPECT_FALSE(heating.plantLoop());
  EXPECT_TRUE(cooling.plantLoop());
  // The mode stays in the model, still attached to its wrapper
  ASSERT_TRUE(heatPump.heatingOperationMode());
  EXPECT_EQ(heating.handle(), heatPump.heatingOperationMode()->handle());

  // Anything else is left for the normal removal
  BoilerHotWater boiler(model);
  EXPECT_FALSE(removeHeatPumpAirToWaterModeFromLoop(boiler));
  EXPECT_FALSE(removeHeatPumpAirToWaterModeFromLoop(heatPump));
  EXPECT_FALSE(boiler.handle().isNull());
  EXPECT_FALSE(heatPump.handle().isNull());
}

TEST(HeatPumpAirToWaterPlacement, RemovingLastModeOnALoopRemovesTheWholeUnit) {
  Model model;
  PlantLoop hotWaterLoop(model);
  PlantLoop chilledWaterLoop(model);
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);
  HeatPumpAirToWaterHeating heating = heatPump.heatingOperationMode().get();
  HeatPumpAirToWaterCooling cooling = heatPump.coolingOperationMode().get();
  ASSERT_TRUE(heating.addSpeed(HeatPumpAirToWaterHeatingSpeedData(model)));
  ASSERT_TRUE(hotWaterLoop.addSupplyBranchForComponent(heating));
  ASSERT_TRUE(chilledWaterLoop.addSupplyBranchForComponent(cooling));

  EXPECT_TRUE(removeHeatPumpAirToWaterModeFromLoop(cooling));
  EXPECT_FALSE(heatPump.handle().isNull());
  EXPECT_TRUE(removeHeatPumpAirToWaterModeFromLoop(heating));

  EXPECT_TRUE(heatPump.handle().isNull());
  EXPECT_TRUE(heating.handle().isNull());
  EXPECT_TRUE(cooling.handle().isNull());
  EXPECT_TRUE(model.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().empty());
  EXPECT_FALSE(hotWaterLoop.handle().isNull());
  EXPECT_FALSE(chilledWaterLoop.handle().isNull());
}

TEST(HeatPumpAirToWaterPlacement, RemovingOnlyPlacedModeRemovesTheWholeUnit) {
  Model model;
  PlantLoop hotWaterLoop(model);
  HeatPumpAirToWater heatPump = makeHeatPumpWithBothModes(model);
  HeatPumpAirToWaterHeating heating = heatPump.heatingOperationMode().get();
  ASSERT_TRUE(hotWaterLoop.addSupplyBranchForComponent(heating));

  EXPECT_TRUE(removeHeatPumpAirToWaterModeFromLoop(heating));
  EXPECT_TRUE(heatPump.handle().isNull());
  EXPECT_TRUE(model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().empty());
}
