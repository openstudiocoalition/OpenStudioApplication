/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

// IconLibrary is defined in shared_gui_components, but this test lives in openstudio_lib/test/
// because it reuses OpenStudioLibFixture for QApplication setup and Qt resource initialisation.
// shared_gui_components has no test target of its own; a lightweight QApplication-only fixture
// there would suffice if one is ever added.
#include "OpenStudioLibFixture.hpp"

#include "../../shared_gui_components/IconLibrary.hpp"

#include <openstudio/utilities/idd/IddFactory.hxx>

#include <QPixmap>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, IconLibrary_Icon) {
  std::vector<IddObjectType> iddObjectTypes;
  iddObjectTypes.push_back(IddObjectType::OS_AirLoopHVAC_OutdoorAirSystem);
  iddObjectTypes.push_back(IddObjectType::OS_AirLoopHVAC_UnitaryHeatPump_AirToAir);
  iddObjectTypes.push_back(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_NoReheat);
  iddObjectTypes.push_back(IddObjectType::OS_AirTerminal_SingleDuct_VAV_Reheat);
  iddObjectTypes.push_back(IddObjectType::OS_Boiler_HotWater);
  //iddObjectTypes.push_back(IddObjectType::OS_Building);
  //iddObjectTypes.push_back(IddObjectType::OS_BuildingStory);
  iddObjectTypes.push_back(IddObjectType::OS_Chiller_Electric_EIR);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_DX_SingleSpeed);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_Water);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Electric);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Gas);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Water);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_Water_Panel_Radiant);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction_AirBoundary);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction_CfactorUndergroundWall);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction_FfactorGroundFloor);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction_InternalSource);
  //iddObjectTypes.push_back(IddObjectType::OS_Construction_WindowDataFile);
  //iddObjectTypes.push_back(IddObjectType::OS_Daylighting_Control);
  //iddObjectTypes.push_back(IddObjectType::OS_DefaultConstructionSet);
  //iddObjectTypes.push_back(IddObjectType::OS_DefaultScheduleSet);
  //iddObjectTypes.push_back(IddObjectType::OS_DesignSpecification_OutdoorAir);
  //iddObjectTypes.push_back(IddObjectType::OS_ElectricEquipment);
  //iddObjectTypes.push_back(IddObjectType::OS_ElectricEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_EvaporativeCooler_Direct_ResearchSpecial);
  //iddObjectTypes.push_back(IddObjectType::OS_Facility);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_ComponentModel);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_ConstantVolume);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_OnOff);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_SystemModel);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_VariableVolume);
  //iddObjectTypes.push_back(IddObjectType::OS_GasEquipment);
  //iddObjectTypes.push_back(IddObjectType::OS_GasEquipment_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_IlluminanceMap);
  //iddObjectTypes.push_back(IddObjectType::OS_InteriorPartitionSurfaceGroup);
  //iddObjectTypes.push_back(IddObjectType::OS_InteriorPartitionSurfaceGroup);
  //iddObjectTypes.push_back(IddObjectType::OS_InternalMass);
  //iddObjectTypes.push_back(IddObjectType::OS_InternalMass_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_Lights);
  //iddObjectTypes.push_back(IddObjectType::OS_Lights_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_Luminaire);
  //iddObjectTypes.push_back(IddObjectType::OS_Luminaire_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_Material);
  //iddObjectTypes.push_back(IddObjectType::OS_Material_AirGap);
  //iddObjectTypes.push_back(IddObjectType::OS_Material_InfraredTransparent);
  //iddObjectTypes.push_back(IddObjectType::OS_Material_NoMass);
  //iddObjectTypes.push_back(IddObjectType::OS_Material_RoofVegetation);
  //iddObjectTypes.push_back(IddObjectType::OS_OtherEquipment);
  //iddObjectTypes.push_back(IddObjectType::OS_OtherEquipment_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_People);
  //iddObjectTypes.push_back(IddObjectType::OS_People_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_Pump_VariableSpeed);
  //iddObjectTypes.push_back(IddObjectType::OS_Schedule_Compact);
  //iddObjectTypes.push_back(IddObjectType::OS_Schedule_Constant);
  //iddObjectTypes.push_back(IddObjectType::OS_Schedule_FixedInterval);
  //iddObjectTypes.push_back(IddObjectType::OS_Schedule_Ruleset);
  //iddObjectTypes.push_back(IddObjectType::OS_Schedule_VariableInterval);
  //iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_MixedAir);
  //iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_Scheduled);
  //iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_SingleZone_Reheat);
  //iddObjectTypes.push_back(IddObjectType::OS_ShadingSurfaceGroup);
  //iddObjectTypes.push_back(IddObjectType::OS_ShadingSurface);
  //iddObjectTypes.push_back(IddObjectType::OS_Space);
  //iddObjectTypes.push_back(IddObjectType::OS_SpaceInfiltration_DesignFlowRate);
  //iddObjectTypes.push_back(IddObjectType::OS_SpaceType);
  iddObjectTypes.push_back(IddObjectType::OS_Humidifier_Steam_Gas);
  //iddObjectTypes.push_back(IddObjectType::OS_SteamEquipment);
  //iddObjectTypes.push_back(IddObjectType::OS_SteamEquipment_Definition);
  //iddObjectTypes.push_back(IddObjectType::OS_SubSurface);
  //iddObjectTypes.push_back(IddObjectType::OS_Surface);
  iddObjectTypes.push_back(IddObjectType::OS_SwimmingPool_Indoor);
  iddObjectTypes.push_back(IddObjectType::OS_ThermalZone);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Blind);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Gas);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_GasMixture);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Glazing);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Screen);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Shade);
  //iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_EvaporativeCoolerUnit);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_CoolingPanel_RadiantConvective_Water);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump);

  for (IddObjectType iddObjectType : iddObjectTypes) {
    const QPixmap* pixmap = IconLibrary::Instance().findIcon(iddObjectType.value());
    EXPECT_TRUE(pixmap) << "Could not find icon for IddObjectType = '" << iddObjectType.valueName() << "'";
  }
}

TEST_F(OpenStudioLibFixture, IconLibrary_MiniIcon) {
  std::vector<IddObjectType> iddObjectTypes;
  iddObjectTypes.push_back(IddObjectType::OS_AirLoopHVAC_OutdoorAirSystem);
  iddObjectTypes.push_back(IddObjectType::OS_AirLoopHVAC_UnitaryHeatPump_AirToAir);
  iddObjectTypes.push_back(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_NoReheat);
  iddObjectTypes.push_back(IddObjectType::OS_AirTerminal_SingleDuct_VAV_Reheat);
  iddObjectTypes.push_back(IddObjectType::OS_Boiler_HotWater);
  iddObjectTypes.push_back(IddObjectType::OS_Building);
  iddObjectTypes.push_back(IddObjectType::OS_BuildingStory);
  iddObjectTypes.push_back(IddObjectType::OS_Chiller_Electric_EIR);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_DX_SingleSpeed);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_Water);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Electric);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Gas);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Heating_Water);
  iddObjectTypes.push_back(IddObjectType::OS_Coil_Cooling_Water_Panel_Radiant);
  iddObjectTypes.push_back(IddObjectType::OS_Construction);
  iddObjectTypes.push_back(IddObjectType::OS_Construction_AirBoundary);
  iddObjectTypes.push_back(IddObjectType::OS_Construction_CfactorUndergroundWall);
  iddObjectTypes.push_back(IddObjectType::OS_Construction_FfactorGroundFloor);
  iddObjectTypes.push_back(IddObjectType::OS_Construction_InternalSource);
  iddObjectTypes.push_back(IddObjectType::OS_Construction_WindowDataFile);
  iddObjectTypes.push_back(IddObjectType::OS_Daylighting_Control);
  iddObjectTypes.push_back(IddObjectType::OS_DefaultConstructionSet);
  iddObjectTypes.push_back(IddObjectType::OS_DefaultScheduleSet);
  iddObjectTypes.push_back(IddObjectType::OS_DesignSpecification_OutdoorAir);
  iddObjectTypes.push_back(IddObjectType::OS_ElectricEquipment);
  iddObjectTypes.push_back(IddObjectType::OS_ElectricEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_EvaporativeCooler_Direct_ResearchSpecial);
  iddObjectTypes.push_back(IddObjectType::OS_Facility);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_ComponentModel);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_ConstantVolume);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_OnOff);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_SystemModel);
  iddObjectTypes.push_back(IddObjectType::OS_Fan_VariableVolume);
  iddObjectTypes.push_back(IddObjectType::OS_GasEquipment);
  iddObjectTypes.push_back(IddObjectType::OS_GasEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_HotWaterEquipment);
  iddObjectTypes.push_back(IddObjectType::OS_HotWaterEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_IlluminanceMap);
  iddObjectTypes.push_back(IddObjectType::OS_InteriorPartitionSurfaceGroup);
  iddObjectTypes.push_back(IddObjectType::OS_InteriorPartitionSurfaceGroup);
  iddObjectTypes.push_back(IddObjectType::OS_InternalMass);
  iddObjectTypes.push_back(IddObjectType::OS_InternalMass_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_Lights);
  iddObjectTypes.push_back(IddObjectType::OS_Lights_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_Luminaire);
  iddObjectTypes.push_back(IddObjectType::OS_Luminaire_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_Material);
  iddObjectTypes.push_back(IddObjectType::OS_Material_AirGap);
  iddObjectTypes.push_back(IddObjectType::OS_Material_InfraredTransparent);
  iddObjectTypes.push_back(IddObjectType::OS_Material_NoMass);
  iddObjectTypes.push_back(IddObjectType::OS_Material_RoofVegetation);
  iddObjectTypes.push_back(IddObjectType::OS_OtherEquipment);
  iddObjectTypes.push_back(IddObjectType::OS_OtherEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_People);
  iddObjectTypes.push_back(IddObjectType::OS_People_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_Pump_VariableSpeed);
  iddObjectTypes.push_back(IddObjectType::OS_Schedule_Compact);
  iddObjectTypes.push_back(IddObjectType::OS_Schedule_Constant);
  iddObjectTypes.push_back(IddObjectType::OS_Schedule_FixedInterval);
  iddObjectTypes.push_back(IddObjectType::OS_Schedule_Ruleset);
  iddObjectTypes.push_back(IddObjectType::OS_Schedule_VariableInterval);
  iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_MixedAir);
  iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_Scheduled);
  iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_SingleZone_Reheat);
  iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_SystemNodeReset_Temperature);
  iddObjectTypes.push_back(IddObjectType::OS_SetpointManager_SystemNodeReset_Humidity);
  iddObjectTypes.push_back(IddObjectType::OS_ShadingSurfaceGroup);
  iddObjectTypes.push_back(IddObjectType::OS_ShadingSurface);
  iddObjectTypes.push_back(IddObjectType::OS_Space);
  iddObjectTypes.push_back(IddObjectType::OS_SpaceInfiltration_DesignFlowRate);
  iddObjectTypes.push_back(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea);
  iddObjectTypes.push_back(IddObjectType::OS_SpaceType);
  iddObjectTypes.push_back(IddObjectType::OS_Humidifier_Steam_Gas);
  iddObjectTypes.push_back(IddObjectType::OS_SteamEquipment);
  iddObjectTypes.push_back(IddObjectType::OS_SteamEquipment_Definition);
  iddObjectTypes.push_back(IddObjectType::OS_SubSurface);
  iddObjectTypes.push_back(IddObjectType::OS_Surface);
  iddObjectTypes.push_back(IddObjectType::OS_SwimmingPool_Indoor);
  iddObjectTypes.push_back(IddObjectType::OS_ThermalZone);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Blind);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Gas);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_GasMixture);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Glazing);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Screen);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_Shade);
  iddObjectTypes.push_back(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_EvaporativeCoolerUnit);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_CoolingPanel_RadiantConvective_Water);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner);
  iddObjectTypes.push_back(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump);

  for (IddObjectType iddObjectType : iddObjectTypes) {
    const QPixmap* pixmap = IconLibrary::Instance().findMiniIcon(iddObjectType.value());
    EXPECT_TRUE(pixmap) << "Could not find mini-icon for IddObjectType = '" << iddObjectType.valueName() << "'";
  }
}

TEST_F(OpenStudioLibFixture, IconLibrary_PlantLoopHeatPumps) {
  // findIcon/findMiniIcon fall back to the Catchall "missing icon" rather than returning null, so
  // check the plant loop heat pump types are actually mapped and their images load
  const std::vector<IddObjectType> iddObjectTypes{
    IddObjectType::OS_HeatPump_PlantLoop_EIR_Heating,
    IddObjectType::OS_HeatPump_PlantLoop_EIR_Cooling,
    IddObjectType::OS_HeatPump_AirToWater_FuelFired_Heating,
    IddObjectType::OS_HeatPump_AirToWater_FuelFired_Cooling,
    IddObjectType::OS_HeatPump_AirToWater,
    IddObjectType::OS_HeatPump_AirToWater_Heating,
    IddObjectType::OS_HeatPump_AirToWater_Cooling,
  };
  const QPixmap* missingIcon = IconLibrary::Instance().findIcon(IddObjectType(IddObjectType::Catchall).value());
  const QPixmap* missingMiniIcon = IconLibrary::Instance().findMiniIcon(IddObjectType(IddObjectType::Catchall).value());

  for (const IddObjectType& iddObjectType : iddObjectTypes) {
    const QPixmap* icon = IconLibrary::Instance().findIcon(iddObjectType.value());
    EXPECT_NE(missingIcon, icon) << "No icon mapped for IddObjectType = '" << iddObjectType.valueName() << "'";
    EXPECT_FALSE(icon->isNull()) << "Icon failed to load for IddObjectType = '" << iddObjectType.valueName() << "'";

    const QPixmap* miniIcon = IconLibrary::Instance().findMiniIcon(iddObjectType.value());
    EXPECT_NE(missingMiniIcon, miniIcon) << "No mini-icon mapped for IddObjectType = '" << iddObjectType.valueName() << "'";
    EXPECT_FALSE(miniIcon->isNull()) << "Mini-icon failed to load for IddObjectType = '" << iddObjectType.valueName() << "'";
  }
}

TEST_F(OpenStudioLibFixture, IconLibrary_PlantLoopHeatPumpsDistinct) {
  auto icon = [](IddObjectType t) { return IconLibrary::Instance().findIcon(t.value())->toImage(); };
  auto miniIcon = [](IddObjectType t) { return IconLibrary::Instance().findMiniIcon(t.value())->toImage(); };

  // HeatPumpAirToWater and its operation modes are one physical unit, so they share one icon
  for (const IddObjectType& mode :
       {IddObjectType(IddObjectType::OS_HeatPump_AirToWater_Heating), IddObjectType(IddObjectType::OS_HeatPump_AirToWater_Cooling)}) {
    EXPECT_EQ(icon(IddObjectType::OS_HeatPump_AirToWater), icon(mode)) << mode.valueName();
    EXPECT_EQ(miniIcon(IddObjectType::OS_HeatPump_AirToWater), miniIcon(mode)) << mode.valueName();
  }

  // Each heating / cooling pair must be told apart
  const std::vector<std::pair<IddObjectType, IddObjectType>> pairs{
    {IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Heating, IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Cooling},
    {IddObjectType::OS_HeatPump_PlantLoop_EIR_Heating, IddObjectType::OS_HeatPump_PlantLoop_EIR_Cooling},
    {IddObjectType::OS_HeatPump_AirToWater_FuelFired_Heating, IddObjectType::OS_HeatPump_AirToWater_FuelFired_Cooling},
  };
  for (const auto& [heating, cooling] : pairs) {
    EXPECT_NE(icon(heating), icon(cooling)) << heating.valueName();
    EXPECT_NE(miniIcon(heating), miniIcon(cooling)) << heating.valueName();
  }
}
