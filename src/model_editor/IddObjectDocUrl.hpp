/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_IDDOBJECTDOCURL_HPP
#define MODELEDITOR_IDDOBJECTDOCURL_HPP

#include <QString>
#include <QHash>

// Returns the BigLadder EnergyPlus I/O Reference URL for the given OS IDD type name
// (e.g. "OS:ThermalZone"), or an empty string if none is known.
// Base URL version is controlled by ENERGYPLUS_VERSION_MAJOR/MINOR in FindOpenStudioSDK.cmake.
inline QString iddObjectDocUrl(const QString& iddTypeName) {
  static const QString base = QStringLiteral(BIGLADDERSOFTWARE_DOC_BASE_URL);

  // clang-format off
  static const QHash<QString, QString> urlMap{
    // Simulation Parameters
    {"OS:SimulationControl",                    "group-simulation-parameters.html#simulationcontrol"},
    {"OS:Building",                             "group-simulation-parameters.html#building"},
    {"OS:Timestep",                             "group-simulation-parameters.html#timestep"},
    {"OS:RunPeriod",                            "group-simulation-parameters.html#runperiod"},
    {"OS:ShadowCalculation",                    "group-simulation-parameters.html#shadowcalculation"},
    {"OS:SurfaceConvectionAlgorithm:Inside",    "group-simulation-parameters.html#surfaceconvectionalgorithminside"},
    {"OS:SurfaceConvectionAlgorithm:Outside",   "group-simulation-parameters.html#surfaceconvectionalgorithmoutside"},
    {"OS:HeatBalanceAlgorithm",                 "group-simulation-parameters.html#heatbalancealgorithm"},
    {"OS:ZoneAirHeatBalanceAlgorithm",          "group-simulation-parameters.html#zoneairheatbalancealgorithm"},
    {"OS:ZoneAirContaminantBalance",            "group-simulation-parameters.html#zoneaircontaminantbalance"},

    // Location and Climate
    {"OS:Site:Location",                                    "group-location-climate-weather-file-access.html#sitelocation"},
    {"OS:Site:GroundTemperature:BuildingSurface",           "group-location-climate-weather-file-access.html#sitegroundtemperaturebuildingsurface"},
    {"OS:Site:GroundTemperature:FCfactorMethod",            "group-location-climate-weather-file-access.html#sitegroundtemperaturefcfactormethod"},
    {"OS:Site:GroundReflectance",                           "group-location-climate-weather-file-access.html#sitegroundreflectance"},
    {"OS:Site:WaterMainsTemperature",                       "group-location-climate-weather-file-access.html#sitewatermainstemperature"},
    {"OS:SizingPeriod:DesignDay",                           "group-location-climate-weather-file-access.html#sizingperioddesignday"},
    {"OS:DesignDay",                                        "group-location-climate-weather-file-access.html#sizingperioddesignday"},

    // Schedules
    {"OS:ScheduleTypeLimits",       "group-schedules.html#scheduletypelimits"},
    {"OS:Schedule:Compact",         "group-schedules.html#schedulecompact"},
    {"OS:Schedule:Constant",        "group-schedules.html#scheduleconstant"},
    {"OS:Schedule:Day",             "group-schedules.html#day-schedules"},
    {"OS:Schedule:Day:Hourly",      "group-schedules.html#scheduledayhourly"},
    {"OS:Schedule:Day:Interval",    "group-schedules.html#scheduledayinterval"},
    {"OS:Schedule:Week:Daily",      "group-schedules.html#scheduleweekdaily"},
    {"OS:Schedule:Week:Compact",    "group-schedules.html#scheduleweekcompact"},
    {"OS:Schedule:Year",            "group-schedules.html#scheduleyear"},
    {"OS:Schedule:Ruleset",         "group-schedules.html#scheduleyear"},

    // Surface Construction Elements
    {"OS:Material",                                             "group-surface-construction-elements.html#material"},
    {"OS:Material:NoMass",                                      "group-surface-construction-elements.html#materialnomass"},
    {"OS:Material:AirGap",                                      "group-surface-construction-elements.html#materialaingap"},
    {"OS:Material:InfraredTransparent",                         "group-surface-construction-elements.html#materialinfraredtransparent"},
    {"OS:Material:RoofVegetation",                              "group-surface-construction-elements.html#materialroofvegetation"},
    {"OS:WindowMaterial:Glazing",                               "group-surface-construction-elements.html#windowmaterialglazing"},
    {"OS:WindowMaterial:Gas",                                   "group-surface-construction-elements.html#windowmaterialgas"},
    {"OS:WindowMaterial:GasMixture",                            "group-surface-construction-elements.html#windowmaterialgasmixture"},
    {"OS:WindowMaterial:SimpleGlazingSystem",                   "group-surface-construction-elements.html#windowmaterialsimpleglazingsystem"},
    {"OS:WindowMaterial:Blind",                                 "group-surface-construction-elements.html#windowmaterialblind"},
    {"OS:WindowMaterial:Screen",                                "group-surface-construction-elements.html#windowmaterialscreen"},
    {"OS:WindowMaterial:Shade",                                 "group-surface-construction-elements.html#windowmaterialshade"},
    {"OS:Construction",                                         "group-surface-construction-elements.html#construction"},
    {"OS:Construction:InternalSource",                          "group-surface-construction-elements.html#constructioninternalsource"},
    {"OS:Construction:WindowDataFile",                          "group-surface-construction-elements.html#constructionwindowdatafile"},
    {"OS:WindowProperty:FrameAndDivider",                       "group-surface-construction-elements.html#windowpropertyframeanddivider"},

    // Thermal Zone Description and Geometry
    {"OS:ThermalZone",                      "group-thermal-zone-description-geometry.html"},
    {"OS:Space",                            "group-thermal-zone-description-geometry.html"},
    {"OS:SpaceType",                        "group-thermal-zone-description-geometry.html"},
    {"OS:BuildingStory",                    "group-thermal-zone-description-geometry.html"},
    {"OS:Surface",                          "group-thermal-zone-description-geometry.html#buildingsurface-detailed"},
    {"OS:SubSurface",                       "group-thermal-zone-description-geometry.html#fenestrationsurface-detailed"},
    {"OS:ShadingControl",                   "group-thermal-zone-description-geometry.html#shadingcontrol"},
    {"OS:InteriorPartitionSurface",         "group-thermal-zone-description-geometry.html#interiorpartitionsurface-detailed"},
    {"OS:InteriorPartitionSurfaceGroup",    "group-thermal-zone-description-geometry.html"},

    // Daylighting
    {"OS:Daylighting:Control",          "group-daylighting.html#daylightingcontrols"},
    {"OS:DaylightingDevice:Shelf",      "group-daylighting.html#daylightingdeviceshelf"},
    {"OS:DaylightingDevice:Tubular",    "group-daylighting.html#daylightingdevicetubular"},
    {"OS:DaylightingDevice:LightWell",  "group-daylighting.html#daylightingdevicelightwell"},

    // Internal Gains
    {"OS:People",               "group-internal-gains-people-lights-other.html#people"},
    {"OS:Lights",               "group-internal-gains-people-lights-other.html#lights"},
    {"OS:ElectricEquipment",    "group-internal-gains-people-lights-other.html#electricequipment"},
    {"OS:GasEquipment",         "group-internal-gains-people-lights-other.html#gasequipment"},
    {"OS:HotWaterEquipment",    "group-internal-gains-people-lights-other.html#hotwatersupportequipment"},
    {"OS:SteamEquipment",       "group-internal-gains-people-lights-other.html#steamequipment"},
    {"OS:OtherEquipment",       "group-internal-gains-people-lights-other.html#otherequipment"},
    {"OS:InternalMass",         "group-internal-gains-people-lights-other.html#internalmass"},

    // Exterior Energy Use
    {"OS:Exterior:Lights",          "group-exterior-energy-use-equipment.html#exteriorlights"},
    {"OS:Exterior:FuelEquipment",   "group-exterior-energy-use-equipment.html#exteriorfuelequipment"},
    {"OS:Exterior:WaterEquipment",  "group-exterior-energy-use-equipment.html#exteriorwaterequipment"},
    // Older OpenStudio type names
    {"OS:ExteriorLights",           "group-exterior-energy-use-equipment.html#exteriorlights"},
    {"OS:ExteriorFuelEquipment",    "group-exterior-energy-use-equipment.html#exteriorfuelequipment"},
    {"OS:ExteriorWaterEquipment",   "group-exterior-energy-use-equipment.html#exteriorwaterequipment"},

    // Zone Airflow
    {"OS:SpaceInfiltration:DesignFlowRate",         "group-airflow.html#zoneinfiltrationdesignflowrate"},
    {"OS:SpaceInfiltration:EffectiveLeakageArea",   "group-airflow.html#zoneinfiltrationeffectiveleakagearea"},
    {"OS:ZoneVentilation:DesignFlowRate",           "group-airflow.html#zoneventilationdesignflowrate"},
    {"OS:ZoneVentilation:WindandStackOpenArea",     "group-airflow.html#zoneventilationwindandstackopenarea"},
    {"OS:ZoneMixing",                               "group-airflow.html#zonemixing"},
    {"OS:ZoneCrossMixing",                          "group-airflow.html#zonecrossmixing"},

    // Design Objects / Sizing
    {"OS:DesignSpecification:OutdoorAir",           "group-design-objects.html#designspecificationoutdoorair"},
    {"OS:DesignSpecification:ZoneAirDistribution",  "group-design-objects.html#designspecificationzoneairdistribution"},
    {"OS:Sizing:Zone",                              "group-design-objects.html#sizingzone"},
    {"OS:Sizing:System",                            "group-design-objects.html#sizingsystem"},
    {"OS:Sizing:Plant",                             "group-design-objects.html#sizingplant"},
    {"OS:Sizing:Parameters",                        "group-design-objects.html#sizingparameters"},

    // Zone HVAC Controls and Thermostats
    {"OS:ThermostatSetpoint:DualSetpoint",      "group-zone-controls-thermostats.html#thermostatsetpointdualsetpoint"},
    {"OS:ThermostatSetpoint:SingleHeating",     "group-zone-controls-thermostats.html#thermostatsetpointsingleheating"},
    {"OS:ThermostatSetpoint:SingleCooling",     "group-zone-controls-thermostats.html#thermostatsetpointsinglecooling"},
    {"OS:ZoneControl:Thermostat",               "group-zone-controls-thermostats.html#zonecontrolthermostat"},
    {"OS:ZoneControl:ContaminantController",    "group-zone-controls-thermostats.html#zonecontrolcontaminantcontroller"},
    {"OS:ZoneControl:Humidistat",               "group-zone-controls-thermostats.html#zonecontrolhumidistat"},
    {"OS:ZoneControl:Thermostat:StagedDualSetpoint", "group-zone-controls-thermostats.html#zonecontrolthermostatstageddualsetpoint"},

    // Zone HVAC Equipment Connections
    {"OS:ZoneHVAC:EquipmentList",        "group-zone-equipment.html#zonehvacequipmentlist"},
    {"OS:ZoneHVAC:EquipmentConnections", "group-zone-equipment.html#zonehvacequipmentconnections"},

    // Zone HVAC Forced Air Units
    {"OS:ZoneHVAC:PackagedTerminalAirConditioner",          "group-zone-forced-air-units.html#zonehvacpackagedterminalairconditioner"},
    {"OS:ZoneHVAC:PackagedTerminalHeatPump",                "group-zone-forced-air-units.html#zonehvacpackagedterminalheatpump"},
    {"OS:ZoneHVAC:WaterToAirHeatPump",                      "group-zone-forced-air-units.html#zonehvacwatertoairheatpump"},
    {"OS:ZoneHVAC:FourPipeFanCoil",                         "group-zone-forced-air-units.html#zonehvacfourpipefancoil"},
    {"OS:ZoneHVAC:UnitVentilator",                          "group-zone-forced-air-units.html#zonehvacunitventilator"},
    {"OS:ZoneHVAC:UnitHeater",                              "group-zone-forced-air-units.html#zonehvacunitheater"},
    {"OS:ZoneHVAC:IdealLoadsAirSystem",                     "group-zone-forced-air-units.html#zonehvacidealloadsairsystem"},
    {"OS:ZoneHVAC:EnergyRecoveryVentilator",                "group-zone-forced-air-units.html#zonehvacenergyrecoveryventilator"},
    {"OS:ZoneHVAC:Dehumidifier:DX",                         "group-zone-forced-air-units.html#zonehvacdehumidifierdx"},
    {"OS:ZoneHVAC:EvaporativeCoolerUnit",                    "group-zone-forced-air-units.html#zonehvacevaporativecoolerunit"},
    {"OS:ZoneHVAC:Baseboard:Convective:Water",              "group-radiative-convective-units.html#zonehvacbaseboardconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:Convective:Electric",           "group-radiative-convective-units.html#zonehvacbaseboardconvectiveelectric"},
    {"OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow",    "group-zone-forced-air-units.html#zonehvacterminalunitvariablerefrigerantflow"},

    // Zone HVAC Radiative / Cooling Panels
    {"OS:ZoneHVAC:CoolingPanel:RadiantConvective:Water",  "group-radiative-convective-units.html#zonehvaccoolingpanelradiantconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:RadiantConvective:Water",     "group-radiative-convective-units.html#zonehvacbaseboardradiantconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:RadiantConvective:Electric","group-radiative-convective-units.html#zonehvacbaseboardradiantconvectiveelectric"},
    {"OS:ZoneHVAC:HighTemperatureRadiant",              "group-radiative-convective-units.html#zonehvachightemperatureradiant"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:VariableFlow",  "group-radiative-convective-units.html#zonehvaclowtemperatureradiantvariableflow"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:ConstantFlow",  "group-radiative-convective-units.html#zonehvaclowtemperatureradiantconstantflow"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:Electric",      "group-radiative-convective-units.html#zonehvaclowtemperatureradiantelectric"},

    // Air Terminals
    {"OS:AirTerminal:SingleDuct:VAV:Reheat",                    "group-air-distribution-equipment.html#airterminalsingleductvavreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:NoReheat",                  "group-air-distribution-equipment.html#airterminalsingleductvavnoreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:HeatAndCool:Reheat",        "group-air-distribution-equipment.html#airterminalsingleductvavheatandcoolreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:HeatAndCool:NoReheat",      "group-air-distribution-equipment.html#airterminalsingleductvavheatandcoolnoreheat"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:Reheat",         "group-air-distribution-equipment.html#airterminalsingleductconstantvolumereheat"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:NoReheat",       "group-air-distribution-equipment.html#airterminalsingleductuncontrolled"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:CooledBeam",     "group-air-distribution-equipment.html#airterminalsingleductconstantvolumecooledbeam"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:FourPipeBeam",   "group-air-distribution-equipment.html#airterminalsingleductconstantvolumefourpipebeam"},
    {"OS:AirTerminal:SingleDuct:ParallelPIU:Reheat",            "group-air-distribution-equipment.html#airterminalsingleductparallelpiu"},
    {"OS:AirTerminal:SingleDuct:SeriesPIU:Reheat",              "group-air-distribution-equipment.html#airterminalsingleductseriespiu"},
    {"OS:AirTerminal:SingleDuct:InletSideMixer",                "group-air-distribution-equipment.html#airterminalsingleductinletsidemixer"},
    {"OS:AirTerminal:DualDuct:VAV",                             "group-air-distribution-equipment.html#airterminaldualductvav"},
    {"OS:AirTerminal:DualDuct:VAV:OutdoorAir",                  "group-air-distribution-equipment.html#airterminaldualductvavoutdoorair"},
    {"OS:AirTerminal:DualDuct:ConstantVolume",                  "group-air-distribution-equipment.html#airterminaldualductconstantvolume"},
    {"OS:AirTerminal:FourPipeInduction",                        "group-air-distribution-equipment.html#airterminalfourpipeinduction"},

    // Fans
    {"OS:Fan:ConstantVolume",   "group-fans.html#fanconstantvolume"},
    {"OS:Fan:VariableVolume",   "group-fans.html#fanvariablevolume"},
    {"OS:Fan:OnOff",            "group-fans.html#fanonoff"},
    {"OS:Fan:ZoneExhaust",      "group-fans.html#fanzoneexhaust"},
    {"OS:Fan:SystemModel",      "group-fans.html#fansystemmodel"},

    // Coils - Cooling
    {"OS:Coil:Cooling:DX:SingleSpeed",                                      "group-heating-and-cooling-coils.html#coilcoolingdxsinglespeed"},
    {"OS:Coil:Cooling:DX:TwoSpeed",                                         "group-heating-and-cooling-coils.html#coilcoolingdxtwospeed"},
    {"OS:Coil:Cooling:DX:MultiSpeed",                                       "group-heating-and-cooling-coils.html#coilcoolingdxmultispeed"},
    {"OS:Coil:Cooling:DX:VariableSpeed",                                    "group-heating-and-cooling-coils.html#coilcoolingdxvariablespeed"},
    {"OS:Coil:Cooling:DX:VariableRefrigerantFlow",                          "group-heating-and-cooling-coils.html#coilcoolingdxvariablerefrigerantflow"},
    {"OS:Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl",  "group-heating-and-cooling-coils.html#coilcoolingdxvariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:Coil:Cooling:Water",                                               "group-heating-and-cooling-coils.html#coilcoolingwater"},
    {"OS:Coil:Cooling:Water:DetailedGeometry",                              "group-heating-and-cooling-coils.html#coilcoolingwaterdetailedgeometry"},
    {"OS:Coil:Cooling:WaterToAirHeatPump:EquationFit",                      "group-heating-and-cooling-coils.html#coilcoolingwatertoairheatpumpequationfit"},
    {"OS:Coil:Cooling:WaterToAirHeatPump:VariableSpeedEquationFit",         "group-heating-and-cooling-coils.html#coilcoolingwatertoairheatpumpvariablespeedequationfit"},
    {"OS:Coil:Cooling:FourPipeBeam",                                        "group-heating-and-cooling-coils.html#coilcoolingfourpipebeam"},
    {"OS:Coil:Cooling:DX:TwoStageWithHumidityControlMode",                 "group-heating-and-cooling-coils.html#coilcoolingdxtwostagewithhumiditycontrolmode"},
    {"OS:Coil:Cooling:DX:MultiSpeed:StageData",                            "group-heating-and-cooling-coils.html#coilcoolingdxmultispeed"},
    {"OS:Coil:Cooling:Water:Panel:Radiant",                                 "group-radiative-convective-units.html#zonehvaccoolingpanelradiantconvectivewater"},

    // Coils - Heating
    {"OS:Coil:Heating:Gas",                                                 "group-heating-and-cooling-coils.html#coilheatinggas-000"},
    {"OS:Coil:Heating:Fuel",                                                "group-heating-and-cooling-coils.html#coilheatingfuel"},
    {"OS:Coil:Heating:Electric",                                            "group-heating-and-cooling-coils.html#coilheatingelectric"},
    {"OS:Coil:Heating:Water",                                               "group-heating-and-cooling-coils.html#coilheatingwater"},
    {"OS:Coil:Heating:DX:SingleSpeed",                                      "group-heating-and-cooling-coils.html#coilheatingdxsinglespeed"},
    {"OS:Coil:Heating:DX:MultiSpeed",                                       "group-heating-and-cooling-coils.html#coilheatingdxmultispeed"},
    {"OS:Coil:Heating:DX:VariableSpeed",                                    "group-heating-and-cooling-coils.html#coilheatingdxvariablespeed"},
    {"OS:Coil:Heating:DX:VariableRefrigerantFlow",                          "group-heating-and-cooling-coils.html#coilheatingdxvariablerefrigerantflow"},
    {"OS:Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl",  "group-heating-and-cooling-coils.html#coilheatingdxvariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:Coil:Heating:WaterToAirHeatPump:EquationFit",                      "group-heating-and-cooling-coils.html#coilheatingwatertoairheatpumpequationfit"},
    {"OS:Coil:Heating:WaterToAirHeatPump:VariableSpeedEquationFit",         "group-heating-and-cooling-coils.html#coilheatingwatertoairheatpumpvariablespeedequationfit"},
    {"OS:Coil:Heating:Desuperheater",                                       "group-heating-and-cooling-coils.html#coilheatingdesuperheater"},
    {"OS:Coil:Heating:FourPipeBeam",                                        "group-heating-and-cooling-coils.html#coilheatingfourpipebeam"},
    {"OS:Coil:Heating:Gas:MultiStage",                                      "group-heating-and-cooling-coils.html#coilheatinggasmultistage"},
    {"OS:Coil:Heating:Gas:MultiStage:StageData",                            "group-heating-and-cooling-coils.html#coilheatinggasmultistage"},
    {"OS:Coil:Heating:LowTemperatureRadiant:ConstantFlow",                  "group-radiative-convective-units.html#zonehvaclowtemperatureradiantconstantflow"},
    {"OS:Coil:Heating:LowTemperatureRadiant:VariableFlow",                  "group-radiative-convective-units.html#zonehvaclowtemperatureradiantvariableflowdesign"},
    {"OS:Coil:Heating:Water:Baseboard:Radiant",                             "group-radiative-convective-units.html#zonehvacbaseboardradiantconvectivewater"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump",                             "group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumppumped"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump:Pumped",                      "group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumppumped"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump:Wrapped",                     "group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumpwrapped"},
    {"OS:Coil:WaterHeating:Desuperheater",                                  "group-heating-and-cooling-coils.html#coilwaterheatingdesuperheater"},

    // Evaporative Coolers
    {"OS:EvaporativeCooler:Direct:ResearchSpecial",     "group-evaporative-coolers.html#evaporativecoolerdirectresearchspecial"},
    {"OS:EvaporativeCooler:Indirect:ResearchSpecial",   "group-evaporative-coolers.html#evaporativecoolerindirectresearchspecial"},
    {"OS:EvaporativeCooler:Direct:CelDekPad",           "group-evaporative-coolers.html#evaporativecoolerdirectceldeckpad"},
    {"OS:EvaporativeCooler:Indirect:CelDekPad",         "group-evaporative-coolers.html#evaporativecoolerindirectceldeckpad"},
    {"OS:EvaporativeCooler:Indirect:WetCoil",           "group-evaporative-coolers.html#evaporativecoolerindirectwetcoil"},

    // Humidifiers
    {"OS:Humidifier:Steam:Electric", "group-humidifiers-and-dehumidifiers.html#humidifiersteamelectric"},
    {"OS:Humidifier:Steam:Gas",      "group-humidifiers-and-dehumidifiers.html#humidifiersteamgas"},

    // Unitary Equipment
    {"OS:AirLoopHVAC:UnitaryHeatPump:AirToAir",            "group-unitary-equipment.html#airloophvacunitaryheatpumpairtoair"},
    {"OS:AirLoopHVAC:UnitaryHeatPump:AirToAir:MultiSpeed",  "group-unitary-equipment.html#airloophvacunitaryheatpumpairtoairmultispeed"},
    {"OS:AirLoopHVAC:UnitarySystem",                        "group-unitary-equipment.html#airloophvacunitarysystem"},
    {"OS:AirLoopHVAC:UnitaryHeatCool:VAVChangeoverBypass",  "group-unitary-equipment.html#airloophvacunitaryheatcoolvavchangeoverbypass"},

    // Variable Refrigerant Flow
    {"OS:AirConditioner:VariableRefrigerantFlow",                               "group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflow"},
    {"OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl",       "group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl:HR",    "group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflowfluidtemperaturecontrolhr"},

    // Air Distribution / AirLoop
    {"OS:AirLoopHVAC",                      "group-air-distribution.html#group-air-distribution"},
    {"OS:AirLoopHVAC:OutdoorAirSystem",     "group-air-distribution.html#airloophvacoutdoorairsystem"},
    {"OS:OutdoorAir:Mixer",                 "group-airflow.html#outdoorairmixer"},
    {"OS:Controller:OutdoorAir",            "group-controllers.html#controlleroutdoorair"},
    {"OS:Controller:MechanicalVentilation",              "group-controllers.html#controllermechanicalventilation"},
    {"OS:ZoneHVAC:EnergyRecoveryVentilator:Controller",  "group-controllers.html#zonehvacenergyrecoveryventilatorcontroller"},
    {"OS:AirLoopHVAC:ZoneMixer",            "group-airflow.html#airloophvaczonemixer"},
    {"OS:AirLoopHVAC:ZoneSplitter",         "group-airflow.html#airloophvaczonesplitter"},
    {"OS:AirLoopHVAC:ReturnPlenum",         "group-airflow.html#airloophvacreturnplenum"},
    {"OS:AirLoopHVAC:SupplyPlenum",         "group-airflow.html#airloophvacsupplyplenum"},

    // Setpoint Managers
    {"OS:SetpointManager:Scheduled",                    "group-setpoint-managers.html#setpointmanagerscheduled"},
    {"OS:SetpointManager:Scheduled:DualSetpoint",       "group-setpoint-managers.html#setpointmanagerscheduleddualsetpoint"},
    {"OS:SetpointManager:MixedAir",                     "group-setpoint-managers.html#setpointmanagermixedair"},
    {"OS:SetpointManager:OutdoorAirReset",              "group-setpoint-managers.html#setpointmanageroutdoorarreset"},
    {"OS:SetpointManager:SingleZone:Reheat",            "group-setpoint-managers.html#setpointmanagersinglezonereheat"},
    {"OS:SetpointManager:SingleZone:Heating",           "group-setpoint-managers.html#setpointmanagersinglezoneheating"},
    {"OS:SetpointManager:SingleZone:Cooling",           "group-setpoint-managers.html#setpointmanagersinglezonecooling"},
    {"OS:SetpointManager:Warmest",                      "group-setpoint-managers.html#setpointmanagerwarmest"},
    {"OS:SetpointManager:Coldest",                      "group-setpoint-managers.html#setpointmanagercoldest"},
    {"OS:SetpointManager:FollowOutdoorAirTemperature",  "group-setpoint-managers.html#setpointmanagerfollowoutdoorairtemperature"},
    {"OS:SetpointManager:FollowGroundTemperature",      "group-setpoint-managers.html#setpointmanagerfollowgroundtemperature"},
    {"OS:SetpointManager:CondenserEnteringReset",       "group-setpoint-managers.html#setpointmanagercondenserenteringreset"},
    {"OS:SetpointManager:WarmestTemperatureFlow",       "group-setpoint-managers.html#setpointmanagerwarmesttemperatureflow"},
    {"OS:SetpointManager:OutdoorAirPretreat",           "group-setpoint-managers.html#setpointmanageroutdoorairpretreat"},
    {"OS:SetpointManager:MultiZone:Heating:Average",    "group-setpoint-managers.html#setpointmanagermultizoneheataverage"},
    {"OS:SetpointManager:MultiZone:Cooling:Average",    "group-setpoint-managers.html#setpointmanagermultizonecoolaverage"},
    {"OS:SetpointManager:FollowSystemNodeTemperature",  "group-setpoint-managers.html#setpointmanagerfollowsystemnodetemperature"},

    // Pumps
    {"OS:Pump:VariableSpeed",           "group-pumps.html#pumpvariablespeed"},
    {"OS:Pump:ConstantSpeed",           "group-pumps.html#pumpconstantspeed"},
    {"OS:HeaderedPumps:VariableSpeed",  "group-pumps.html#headeredpumpsvariablespeed"},
    {"OS:HeaderedPumps:ConstantSpeed",  "group-pumps.html#headeredpumpsconstantspeed"},

    // Solar Collectors
    {"OS:SolarCollector:FlatPlate:Water",               "group-solar-collectors.html#solarcollectorflatplatewater"},
    {"OS:SolarCollector:IntegralCollectorStorage",       "group-solar-collectors.html#solarcollectorintegralcollectorstorage"},
    {"OS:SolarCollector:UnglazedTranspired",             "group-solar-collectors.html#solarcollectorunglazedtranspired"},
    {"OS:SolarCollector:FlatPlate:PhotovoltaicThermal",  "group-solar-collectors.html#solarcollectorflatplatephotovoltaicthermal"},

    // Plant Heating and Cooling Equipment
    {"OS:Boiler:HotWater",                          "group-plant-equipment.html#boilerhotwater"},
    {"OS:Boiler:Steam",                             "group-plant-equipment.html#boilersteam"},
    {"OS:Chiller:Electric:EIR",                     "group-plant-equipment.html#chillerelectriceir"},
    {"OS:Chiller:Electric:ReformulatedEIR",         "group-plant-equipment.html#chillerelectricreformulatedeir"},
    {"OS:Chiller:Absorption:Indirect",              "group-plant-equipment.html#chillerabsorptionindirect"},
    {"OS:Chiller:Absorption:Direct",                "group-plant-equipment.html#chillerabsorptiondirect"},
    {"OS:ChillerHeater:Absorption:DirectFired",     "group-plant-equipment.html#chillerheaterabsorptiondirectfired"},
    {"OS:DistrictCooling",                          "group-plant-equipment.html#districtcooling"},
    {"OS:DistrictHeating",                          "group-plant-equipment.html#districtheating"},
    {"OS:DistrictHeating:Water",                    "group-plant-equipment.html#districtheating"},
    {"OS:HeatPump:PlantLoop:EIR:Cooling",                  "group-plant-equipment.html#heatpumpplantloopeircooling"},
    {"OS:HeatPump:PlantLoop:EIR:Heating",                  "group-plant-equipment.html#heatpumpplantloopeirheating"},
    {"OS:HeatPump:WaterToWater:EquationFit:Cooling",       "group-plant-equipment.html#heatpumpwatertowaterequationfitcooling"},
    {"OS:HeatPump:WaterToWater:EquationFit:Heating",       "group-plant-equipment.html#heatpumpwatertowaterequationfitheating"},
    {"OS:HeatPump:AirToWater:FuelFired:Cooling",           "group-plant-equipment.html#plhp_fuelfired"},
    {"OS:HeatPump:AirToWater:FuelFired:Heating",           "group-plant-equipment.html#plhp_fuelfired"},
    {"OS:HeatPump:AirToWater",                             "group-plant-equipment.html#plhp_air_to_water"},
    {"OS:HeatPump:AirToWater:Cooling",                     "group-plant-equipment.html#plhp_air_to_water"},
    {"OS:HeatPump:AirToWater:Heating",                     "group-plant-equipment.html#plhp_air_to_water"},
    {"OS:CentralHeatPumpSystem",                           "group-plant-equipment.html#centralheatpumpsystem"},
    {"OS:ChillerHeaterPerformance:Electric:EIR",           "group-plant-equipment.html#chillerheaterperformancelectriceir"},

    // Cooling Towers and Fluid Coolers
    {"OS:CoolingTower:SingleSpeed",             "group-condenser-equipment.html#coolingtowersinglespeed"},
    {"OS:CoolingTower:TwoSpeed",                "group-condenser-equipment.html#coolingtowertwospeed"},
    {"OS:CoolingTower:VariableSpeed:Merkel",    "group-condenser-equipment.html#coolingtowervariablespeedmerkel"},
    {"OS:EvaporativeFluidCooler:SingleSpeed",   "group-condenser-equipment.html#evaporativefluidcoolersinglespeed"},
    {"OS:EvaporativeFluidCooler:TwoSpeed",      "group-condenser-equipment.html#evaporativefluidcoolertwospeed"},
    {"OS:FluidCooler:SingleSpeed",              "group-condenser-equipment.html#fluidcoolersinglespeed"},
    {"OS:FluidCooler:TwoSpeed",                 "group-condenser-equipment.html#fluidcoolertwospeed"},

    // Heat Recovery
    {"OS:HeatExchanger:AirToAir:SensibleAndLatent",     "group-heat-recovery.html#heatexchangerairtoairsensibleandlatent"},
    {"OS:HeatExchanger:FluidToFluid",                   "group-heat-recovery.html#heatexchangerfluidtofluid"},
    {"OS:HeatExchanger:Desiccant:BalancedFlow",         "group-heat-recovery.html#heatexchangerdesiccantbalancedflow"},

    // Condenser Equipment and Ground Heat Exchangers
    {"OS:GroundHeatExchanger:Vertical",         "group-condenser-equipment.html#groundheatexchangervertical"},
    {"OS:GroundHeatExchanger:HorizontalTrench", "group-condenser-equipment.html#groundheatexchangerhorizontaltrench"},
    {"OS:GroundHeatExchanger:Slinky",           "group-condenser-equipment.html#groundheatexchangerslinky"},

    // Water Heaters and Thermal Storage
    {"OS:WaterHeater:Mixed",                        "group-water-heaters.html#waterheatermixed"},
    {"OS:WaterHeater:Stratified",                   "group-water-heaters.html#waterheaterstratified"},
    {"OS:WaterHeater:HeatPump",                     "group-water-heaters.html#waterheaterheatpump"},
    {"OS:WaterHeater:HeatPump:PumpedCondenser",     "group-water-heaters.html#waterheaterheatpumppumpedcondenser"},
    {"OS:WaterHeater:HeatPump:WrappedCondenser",    "group-water-heaters.html#waterheaterheatpumpwrappedcondenser"},
    {"OS:WaterHeater:Sizing",                       "group-water-heaters.html#waterheatersizing"},
    {"OS:ThermalStorage:Ice:Detailed",              "group-water-heaters.html#thermalstorageicedetailed"},
    {"OS:ThermalStorage:ChilledWater:Stratified",   "group-water-heaters.html#thermalstoragechilledwaterstratified"},

    // Water Systems
    {"OS:WaterUse:Equipment",   "group-water-systems.html#wateruseequipment"},
    {"OS:WaterUse:Connections", "group-water-systems.html#wateruseconnections"},

    // Refrigeration
    {"OS:Refrigeration:AirChiller",               "group-refrigeration.html#refrigerationairchiller"},
    {"OS:Refrigeration:Case",                     "group-refrigeration.html#refrigerationcase"},
    {"OS:Refrigeration:Compressor",               "group-refrigeration.html#refrigerationcompressor"},
    {"OS:Refrigeration:CompressorRack",           "group-refrigeration.html#refrigerationcompressorrack"},
    {"OS:Refrigeration:Condenser:AirCooled",      "group-refrigeration.html#refrigerationcondenseraircooled"},
    {"OS:Refrigeration:Condenser:Cascade",        "group-refrigeration.html#refrigerationcondensercascade"},
    {"OS:Refrigeration:Condenser:EvaporativeCooled", "group-refrigeration.html#refrigerationcondenserevaporativecooled"},
    {"OS:Refrigeration:Condenser:WaterCooled",    "group-refrigeration.html#refrigerationcondenserwatercooled"},
    {"OS:Refrigeration:DefrostCycleParameters",   "group-refrigeration.html#refrigerationwalkin"},
    {"OS:Refrigeration:GasCooler:AirCooled",      "group-refrigeration.html#refrigerationgascooleraircooled"},
    {"OS:Refrigeration:SecondarySystem",          "group-refrigeration.html#refrigerationsecondarysystem"},
    {"OS:Refrigeration:Subcooler:LiquidSuction",  "group-refrigeration.html#refrigerationsubcooler"},
    {"OS:Refrigeration:Subcooler:Mechanical",     "group-refrigeration.html#refrigerationsubcooler"},
    {"OS:Refrigeration:System",                   "group-refrigeration.html#refrigerationsystem"},
    {"OS:Refrigeration:TranscriticalSystem",      "group-refrigeration.html#refrigerationtranscriticalsystem"},
    {"OS:Refrigeration:WalkIn",                   "group-refrigeration.html#refrigerationwalkin"},
    {"OS:Refrigeration:WalkIn:ZoneBoundary",      "group-refrigeration.html#refrigerationwalkin"},

    // Node / Branch Management
    {"OS:Pipe:Adiabatic", "group-node-branch-management.html#pipeadiabatic"},

    // Plant / Condenser Control
    {"OS:PlantLoop",                                        "group-plant-condenser-control.html#plantloop"},
    {"OS:CondenserLoop",                                    "group-plant-condenser-control.html#condenserloop"},
    {"OS:PlantEquipmentList",                               "group-plant-condenser-control.html#plantequipmentlist"},
    {"OS:PlantEquipmentOperation:CoolingLoad",              "group-plant-condenser-control.html#plantequipmentoperationcoolingload"},
    {"OS:PlantEquipmentOperation:HeatingLoad",              "group-plant-condenser-control.html#plantequipmentoperationheatingload"},
    {"OS:PlantEquipmentOperation:ComponentSetpoint",        "group-plant-condenser-control.html#plantequipmentoperationcomponentsetpoint"},
    {"OS:PlantEquipmentOperation:OutdoorDryBulb",           "group-plant-condenser-control.html#plantequipmentoperationoutdoordrybulb"},
    {"OS:PlantEquipmentOperation:OutdoorWetBulb",           "group-plant-condenser-control.html#plantequipmentoperationoutdoorwetbulb"},
    {"OS:PlantEquipmentOperation:ThermalEnergyStorage",     "group-plant-condenser-control.html#plantequipmentoperationthermalenergystorage"},
    {"OS:PlantEquipmentOperation:Uncontrolled",             "group-plant-condenser-control.html#plantequipmentoperationuncontrolled"},
    {"OS:AvailabilityManagerAssignmentList",                "group-system-availability-managers.html#availabilitymanagerassignmentlist"},
    {"OS:AvailabilityManager:Scheduled",                    "group-system-availability-managers.html#availabilitymanagerscheduled"},
    {"OS:AvailabilityManager:ScheduledOn",                  "group-system-availability-managers.html#availabilitymanagerscheduledon"},
    {"OS:AvailabilityManager:ScheduledOff",                 "group-system-availability-managers.html#availabilitymanagerscheduledoff"},
    {"OS:AvailabilityManager:NightCycle",                   "group-system-availability-managers.html#availabilitymanagernightcycle"},
    {"OS:AvailabilityManager:DifferentialThermostat",       "group-system-availability-managers.html#availabilitymanagerdifferentialthermostat"},
    {"OS:AvailabilityManager:OptimumStart",                 "group-system-availability-managers.html#availabilitymanageroptimumstart"},
    {"OS:AvailabilityManager:NightVentilation",             "group-system-availability-managers.html#availabilitymanagernightventilation"},
    {"OS:AvailabilityManager:HybridVentilation",            "group-system-availability-managers.html#availabilitymanagerhybridventilation"},
    {"OS:AvailabilityManager:LowTemperatureTurnOn",         "group-system-availability-managers.html#availabilitymanagerlowtemperatureturnon"},
    {"OS:AvailabilityManager:HighTemperatureTurnOff",       "group-system-availability-managers.html#availabilitymanagerhightemperatureturnoff"},
    {"OS:AvailabilityManager:LowTemperatureTurnOff",        "group-system-availability-managers.html#availabilitymanagerlowtemperatureturnoff"},
    {"OS:AvailabilityManager:HighTemperatureTurnOn",        "group-system-availability-managers.html#availabilitymanagerhightemperatureturnon"},

    // Energy Management System
    {"OS:EnergyManagementSystem:Sensor",                    "group-energy-management-system-ems.html#energymanagementsystemsensor"},
    {"OS:EnergyManagementSystem:Actuator",                  "group-energy-management-system-ems.html#energymanagementsystemactuator"},
    {"OS:EnergyManagementSystem:Program",                   "group-energy-management-system-ems.html#energymanagementsystemprogram"},
    {"OS:EnergyManagementSystem:ProgramCallingManager",     "group-energy-management-system-ems.html#energymanagementsystemprogramcallingmanager"},
    {"OS:EnergyManagementSystem:GlobalVariable",            "group-energy-management-system-ems.html#energymanagementsystemglobalvariable"},
    {"OS:EnergyManagementSystem:OutputVariable",            "group-energy-management-system-ems.html#energymanagementsystemoutputvariable"},
    {"OS:EnergyManagementSystem:TrendVariable",             "group-energy-management-system-ems.html#energymanagementsystemtrendvariable"},
    {"OS:EnergyManagementSystem:InternalVariable",          "group-energy-management-system-ems.html#energymanagementsysteminternalvariable"},
    {"OS:EnergyManagementSystem:ConstructionIndexVariable", "group-energy-management-system-ems.html#energymanagementsystemconstructionindexvariable"},

    // Performance Curves
    {"OS:Curve:Linear",             "group-performance-curves.html#curvelinear"},
    {"OS:Curve:Quadratic",          "group-performance-curves.html#curvequadratic"},
    {"OS:Curve:Cubic",              "group-performance-curves.html#curvecubic"},
    {"OS:Curve:Quartic",            "group-performance-curves.html#curvequartic"},
    {"OS:Curve:Exponent",           "group-performance-curves.html#curveexponent"},
    {"OS:Curve:Bicubic",            "group-performance-curves.html#curvebicubic"},
    {"OS:Curve:Biquadratic",        "group-performance-curves.html#curvebiquadratic"},
    {"OS:Curve:QuadraticLinear",    "group-performance-curves.html#curvequadraticlinear"},
    {"OS:Curve:CubicLinear",        "group-performance-curves.html#curvecubiclinear"},
    {"OS:Curve:Triquadratic",       "group-performance-curves.html#curvetriquadratic"},
    {"OS:Curve:FanPressureRise",    "group-performance-curves.html#curvefanpressurerise"},
    {"OS:Curve:ExponentialDecay",   "group-performance-curves.html#curveexponentialdecay"},
    {"OS:Curve:Sigmoid",            "group-performance-curves.html#curvesigmoid"},
    {"OS:Table:IndependentVariable",     "group-performance-tables.html#tableindependentvariable"},
    {"OS:Table:IndependentVariableList", "group-performance-tables.html#tableindependentvariablelist"},
    {"OS:Table:Lookup",                  "group-performance-tables.html#tablelookup"},
  };
  // clang-format on

  auto it = urlMap.constFind(iddTypeName);
  if (it != urlMap.constEnd()) {
    return base + it.value();
  }
  return QString();
}

#endif  // MODELEDITOR_IDDOBJECTDOCURL_HPP
