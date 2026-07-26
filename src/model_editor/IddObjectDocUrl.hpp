/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_IDDOBJECTDOCURL_HPP
#define MODELEDITOR_IDDOBJECTDOCURL_HPP

#include <QString>
#include <QHash>
#include <QDebug>
#include <QUrl>

#include "../utilities/OpenStudioApplicationPathHelpers.hpp"
#include <openstudio/utilities/core/Path.hpp>

// Returns the local EnergyPlus Input Output Reference URL for the given OS IDD type name
// (e.g. "OS:ThermalZone"), or an empty string if none is known.
// Points at the documentation bundled alongside the app (see openstudio::energyPlusDocDirectory()).
inline QString iddObjectDocUrl(const QString& iddTypeName) {
  static const QString base =
    QUrl::fromLocalFile(QString::fromStdString(openstudio::toString(openstudio::energyPlusDocDirectory())) + "/").toString();

  // clang-format off
  static const QHash<QString, QString> urlMap{
    // Simulation Parameters
    {"OS:SimulationControl",                    "1.5-group-simulation-parameters.html#simulationcontrol"},
    {"OS:Building",                             "1.5-group-simulation-parameters.html#building"},
    {"OS:Timestep",                             "1.5-group-simulation-parameters.html#timestep"},
    {"OS:RunPeriod",                            "1.7-group-location-climate-weather-file-access.html#runperiod"},
    {"OS:ShadowCalculation",                    "1.5-group-simulation-parameters.html#shadowcalculation"},
    {"OS:SurfaceConvectionAlgorithm:Inside",    "1.5-group-simulation-parameters.html#surfaceconvectionalgorithminside"},
    {"OS:SurfaceConvectionAlgorithm:Outside",   "1.5-group-simulation-parameters.html#surfaceconvectionalgorithmoutside"},
    {"OS:HeatBalanceAlgorithm",                 "1.5-group-simulation-parameters.html#heatbalancealgorithm"},
    {"OS:ZoneAirHeatBalanceAlgorithm",          "1.5-group-simulation-parameters.html#zoneairheatbalancealgorithm"},
    {"OS:ZoneAirContaminantBalance",            "1.5-group-simulation-parameters.html#zoneaircontaminantbalance"},
    {"OS:ConvergenceLimits",                         "1.5-group-simulation-parameters.html#convergencelimits"},
    {"OS:ZoneCapacitanceMultiplier:ResearchSpecial", "1.5-group-simulation-parameters.html#zonecapacitancemultiplierresearchspecial"},

    // Location and Climate
    {"OS:Site:Location",                                              "1.7-group-location-climate-weather-file-access.html#sitelocation"},
    {"OS:Site:GroundTemperature:Undisturbed:KusudaAchenbach",         "1.7-group-location-climate-weather-file-access.html#sitegroundtemperatureundisturbedkusudaachenbach"},
    {"OS:Site:GroundTemperature:BuildingSurface",           "1.7-group-location-climate-weather-file-access.html#sitegroundtemperaturebuildingsurface"},
    {"OS:Site:GroundTemperature:Shallow",                   "1.7-group-location-climate-weather-file-access.html#sitegroundtemperatureshallow"},
    {"OS:Site:GroundTemperature:Deep",                      "1.7-group-location-climate-weather-file-access.html#sitegroundtemperaturedeep"},
    {"OS:Site:GroundTemperature:FCfactorMethod",            "1.7-group-location-climate-weather-file-access.html#sitegroundtemperaturefcfactormethod"},
    {"OS:Site:GroundReflectance",                           "1.7-group-location-climate-weather-file-access.html#sitegroundreflectance"},
    {"OS:Site:WaterMainsTemperature",                       "1.7-group-location-climate-weather-file-access.html#sitewatermainstemperature"},
    {"OS:SizingPeriod:DesignDay",                           "1.7-group-location-climate-weather-file-access.html#sizingperioddesignday"},
    {"OS:DesignDay",                                        "1.7-group-location-climate-weather-file-access.html#sizingperioddesignday"},
    {"OS:RunPeriodControl:DaylightSavingTime",              "1.7-group-location-climate-weather-file-access.html#runperiodcontroldaylightsavingtime"},

    // Schedules
    {"OS:ScheduleTypeLimits",       "1.8-group-schedules.html#scheduletypelimits"},
    {"OS:Schedule:Compact",         "1.8-group-schedules.html#schedulecompact"},
    {"OS:Schedule:Constant",        "1.8-group-schedules.html#scheduleconstant"},
    {"OS:Schedule:Day",             "1.8-group-schedules.html#day-schedules"},
    {"OS:Schedule:Day:Hourly",      "1.8-group-schedules.html#scheduledayhourly"},
    {"OS:Schedule:Day:Interval",    "1.8-group-schedules.html#scheduledayinterval"},
    {"OS:Schedule:Week:Daily",      "1.8-group-schedules.html#scheduleweekdaily"},
    {"OS:Schedule:Week:Compact",    "1.8-group-schedules.html#scheduleweekcompact"},
    {"OS:Schedule:Year",            "1.8-group-schedules.html#scheduleyear"},
    {"OS:Schedule:Ruleset",         "1.8-group-schedules.html#scheduleyear"},
    {"OS:Schedule:File",            "1.8-group-schedules.html#schedulefile"},

    // Surface Construction Elements
    {"OS:Material",                                             "1.9-group-surface-construction-elements.html#material"},
    {"OS:Material:NoMass",                                      "1.9-group-surface-construction-elements.html#materialnomass"},
    {"OS:Material:AirGap",                                      "1.9-group-surface-construction-elements.html#materialairgap"},
    {"OS:Material:InfraredTransparent",                         "1.9-group-surface-construction-elements.html#materialinfraredtransparent"},
    {"OS:Material:RoofVegetation",                              "1.9-group-surface-construction-elements.html#materialroofvegetation"},
    {"OS:WindowMaterial:Glazing",                               "1.9-group-surface-construction-elements.html#windowmaterialglazing"},
    {"OS:WindowMaterial:Gas",                                   "1.9-group-surface-construction-elements.html#windowmaterialgas"},
    {"OS:WindowMaterial:GasMixture",                            "1.9-group-surface-construction-elements.html#windowmaterialgasmixture"},
    {"OS:WindowMaterial:SimpleGlazingSystem",                   "1.9-group-surface-construction-elements.html#windowmaterialsimpleglazingsystem"},
    {"OS:WindowMaterial:Blind",                                 "1.9-group-surface-construction-elements.html#windowmaterialblind"},
    {"OS:WindowMaterial:Screen",                                "1.9-group-surface-construction-elements.html#windowmaterialscreen"},
    {"OS:WindowMaterial:Shade",                                 "1.9-group-surface-construction-elements.html#windowmaterialshade"},
    {"OS:Construction",                                         "1.9-group-surface-construction-elements.html#construction-000"},
    {"OS:Construction:InternalSource",                          "1.9-group-surface-construction-elements.html#constructioninternalsource"},
    {"OS:Construction:WindowDataFile",                          "1.9-group-surface-construction-elements.html#constructionwindowdatafile"},
    {"OS:Construction:AirBoundary",                             "1.9-group-surface-construction-elements.html#constructionairboundary"},
    {"OS:Construction:CfactorUndergroundWall",                  "1.9-group-surface-construction-elements.html#constructioncfactorundergroundwall"},
    {"OS:Construction:FfactorGroundFloor",                      "1.9-group-surface-construction-elements.html#constructionffactorgroundfloor"},
    {"OS:WindowMaterial:DaylightRedirectionDevice",             "1.9-group-surface-construction-elements.html#windowmaterialblind"},
    {"OS:WindowMaterial:Glazing:RefractionExtinctionMethod",    "1.9-group-surface-construction-elements.html#windowmaterialglazingrefractionextinctionmethod"},
    {"OS:WindowProperty:FrameAndDivider",                       "1.10-group-thermal-zone-descriptiongeometry.html#windowpropertyframeanddivider"},

    // Thermal Zone Description and Geometry
    {"OS:ThermalZone",                      "1.10-group-thermal-zone-descriptiongeometry.html"},
    {"OS:Space",                            "1.10-group-thermal-zone-descriptiongeometry.html"},
    {"OS:SpaceType",                        "1.10-group-thermal-zone-descriptiongeometry.html"},
    {"OS:BuildingStory",                    "1.10-group-thermal-zone-descriptiongeometry.html"},
    {"OS:Surface",                          "1.10-group-thermal-zone-descriptiongeometry.html#buildingsurfacedetailed"},
    {"OS:SubSurface",                       "1.10-group-thermal-zone-descriptiongeometry.html#fenestrationsurfacedetailed"},
    {"OS:ShadingControl",                   "1.10-group-thermal-zone-descriptiongeometry.html#windowpropertyshadingcontrol"},
    {"OS:InteriorPartitionSurfaceGroup",    "1.10-group-thermal-zone-descriptiongeometry.html"},

    // Daylighting
    {"OS:Daylighting:Control",          "1.15-group-daylighting.html#daylightingcontrols-000"},
    {"OS:DaylightingDevice:Shelf",      "1.15-group-daylighting.html#daylightingdeviceshelf"},
    {"OS:DaylightingDevice:Tubular",    "1.15-group-daylighting.html#daylightingdevicetubular"},
    {"OS:DaylightingDevice:LightWell",  "1.15-group-daylighting.html#daylightingdevicelightwell"},

    // Internal Gains
    {"OS:People",               "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#people"},
    {"OS:Lights",               "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#lights-000"},
    {"OS:ElectricEquipment",    "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#electricequipment"},
    {"OS:GasEquipment",         "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#gasequipment"},
    {"OS:HotWaterEquipment",    "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#hotwaterequipment"},
    {"OS:SteamEquipment",       "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#steamequipment"},
    {"OS:OtherEquipment",       "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#otherequipment"},
    {"OS:InternalMass",         "1.10-group-thermal-zone-descriptiongeometry.html#internalmass"},
    {"OS:SwimmingPool:Indoor",  "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html#swimmingpoolindoor"},

    // Exterior Energy Use
    {"OS:Exterior:Lights",          "1.16-group-exterior-energy-use-equipment.html#exteriorlights"},
    {"OS:Exterior:FuelEquipment",   "1.16-group-exterior-energy-use-equipment.html#exteriorfuelequipment"},
    {"OS:Exterior:WaterEquipment",  "1.16-group-exterior-energy-use-equipment.html#exteriorwaterequipment"},
    // Older OpenStudio type names
    {"OS:ExteriorLights",           "1.16-group-exterior-energy-use-equipment.html#exteriorlights"},
    {"OS:ExteriorFuelEquipment",    "1.16-group-exterior-energy-use-equipment.html#exteriorfuelequipment"},
    {"OS:ExteriorWaterEquipment",   "1.16-group-exterior-energy-use-equipment.html#exteriorwaterequipment"},

    // Zone Airflow
    {"OS:SpaceInfiltration:DesignFlowRate",         "1.17-group-airflow.html#zoneinfiltrationdesignflowrate"},
    {"OS:SpaceInfiltration:EffectiveLeakageArea",   "1.17-group-airflow.html#zoneinfiltrationeffectiveleakagearea"},
    {"OS:ZoneVentilation:DesignFlowRate",           "1.17-group-airflow.html#zoneventilationdesignflowrate"},
    {"OS:ZoneVentilation:WindandStackOpenArea",     "1.17-group-airflow.html#zoneventilationwindandstackopenarea"},
    {"OS:ZoneMixing",                               "1.17-group-airflow.html#zonemixing"},
    {"OS:ZoneCrossMixing",                          "1.17-group-airflow.html#zonecrossmixing"},

    // Design Objects / Sizing
    {"OS:DesignSpecification:OutdoorAir",           "1.18-group-design-objects.html#designspecificationoutdoorair"},
    {"OS:DesignSpecification:ZoneAirDistribution",  "1.18-group-design-objects.html#designspecificationzoneairdistribution"},
    {"OS:Sizing:Zone",                              "1.18-group-design-objects.html#sizingzone"},
    {"OS:Sizing:System",                            "1.18-group-design-objects.html#sizingsystem"},
    {"OS:Sizing:Plant",                             "1.18-group-design-objects.html#sizingplant"},
    {"OS:Sizing:Parameters",                        "1.18-group-design-objects.html#sizingparameters"},

    // Zone HVAC Controls and Thermostats
    {"OS:ThermostatSetpoint:DualSetpoint",      "1.36-group---zone-controls---thermostats-and-humidistats.html#thermostatsetpointdualsetpoint"},
    {"OS:ThermostatSetpoint:SingleHeating",     "1.36-group---zone-controls---thermostats-and-humidistats.html#thermostatsetpointsingleheating"},
    {"OS:ThermostatSetpoint:SingleCooling",     "1.36-group---zone-controls---thermostats-and-humidistats.html#thermostatsetpointsinglecooling"},
    {"OS:ZoneControl:Thermostat",               "1.36-group---zone-controls---thermostats-and-humidistats.html#zonecontrolthermostat"},
    {"OS:ZoneControl:ContaminantController",    "1.36-group---zone-controls---thermostats-and-humidistats.html#zonecontrolcontaminantcontroller"},
    {"OS:ZoneControl:Humidistat",               "1.36-group---zone-controls---thermostats-and-humidistats.html#zonecontrolhumidistat"},
    {"OS:ZoneControl:Thermostat:StagedDualSetpoint", "1.36-group---zone-controls---thermostats-and-humidistats.html#zonecontrolthermostatstageddualsetpoint"},

    // Zone HVAC Equipment Connections
    {"OS:ZoneHVAC:EquipmentList",        "1.29-group---zone-equipment.html#zonehvacequipmentlist"},
    {"OS:ZoneHVAC:EquipmentConnections", "1.29-group---zone-equipment.html#zonehvacequipmentconnections"},

    // Zone HVAC Forced Air Units
    {"OS:ZoneHVAC:PackagedTerminalAirConditioner",          "1.31-group-zone-forced-air-units.html#zonehvacpackagedterminalairconditioner"},
    {"OS:ZoneHVAC:PackagedTerminalHeatPump",                "1.31-group-zone-forced-air-units.html#zonehvacpackagedterminalheatpump"},
    {"OS:ZoneHVAC:WaterToAirHeatPump",                      "1.31-group-zone-forced-air-units.html#zonehvacwatertoairheatpump"},
    {"OS:ZoneHVAC:FourPipeFanCoil",                         "1.31-group-zone-forced-air-units.html#zonehvacfourpipefancoil"},
    {"OS:ZoneHVAC:UnitVentilator",                          "1.31-group-zone-forced-air-units.html#zonehvacunitventilator"},
    {"OS:ZoneHVAC:UnitHeater",                              "1.31-group-zone-forced-air-units.html#zonehvacunitheater"},
    {"OS:ZoneHVAC:IdealLoadsAirSystem",                     "1.31-group-zone-forced-air-units.html#zonehvacidealloadsairsystem"},
    {"OS:ZoneHVAC:EnergyRecoveryVentilator",                "1.31-group-zone-forced-air-units.html#zonehvacenergyrecoveryventilator"},
    {"OS:ZoneHVAC:Dehumidifier:DX",                         "1.31-group-zone-forced-air-units.html#zonehvacdehumidifierdx"},
    {"OS:ZoneHVAC:EvaporativeCoolerUnit",                    "1.31-group-zone-forced-air-units.html#zonehvacevaporativecoolerunit"},
    {"OS:ZoneHVAC:Baseboard:Convective:Water",              "1.34-group-radiative-convective-units.html#zonehvacbaseboardconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:Convective:Electric",           "1.34-group-radiative-convective-units.html#zonehvacbaseboardconvectiveelectric"},
    {"OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow",    "1.31-group-zone-forced-air-units.html#zonehvacterminalunitvariablerefrigerantflow"},

    // Zone HVAC Radiative / Cooling Panels
    {"OS:ZoneHVAC:CoolingPanel:RadiantConvective:Water",  "1.34-group-radiative-convective-units.html#zonehvaccoolingpanelradiantconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:RadiantConvective:Water",     "1.34-group-radiative-convective-units.html#zonehvacbaseboardradiantconvectivewater"},
    {"OS:ZoneHVAC:Baseboard:RadiantConvective:Electric","1.34-group-radiative-convective-units.html#zonehvacbaseboardradiantconvectiveelectric"},
    {"OS:ZoneHVAC:HighTemperatureRadiant",              "1.34-group-radiative-convective-units.html#zonehvachightemperatureradiant"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:VariableFlow",  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantvariableflow"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:ConstantFlow",  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantconstantflow"},
    {"OS:ZoneHVAC:LowTemperatureRadiant:Electric",      "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantelectric"},

    // Air Terminals
    {"OS:AirTerminal:SingleDuct:VAV:Reheat",                    "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductvavreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:NoReheat",                  "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductvavnoreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:HeatAndCool:Reheat",        "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductvavheatandcoolreheat"},
    {"OS:AirTerminal:SingleDuct:VAV:HeatAndCool:NoReheat",      "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductvavheatandcoolnoreheat"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:Reheat",         "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumereheat"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:NoReheat",       "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumenoreheat"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:CooledBeam",     "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumecooledbeam"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:FourPipeBeam",   "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumefourpipebeam"},
    {"OS:AirTerminal:SingleDuct:ParallelPIU:Reheat",            "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductparallelpiureheat"},
    {"OS:AirTerminal:SingleDuct:SeriesPIU:Reheat",              "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductseriespiureheat"},
    {"OS:AirTerminal:SingleDuct:InletSideMixer",                "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductmixer"},
    {"OS:AirTerminal:DualDuct:VAV",                             "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminaldualductvav"},
    {"OS:AirTerminal:DualDuct:VAV:OutdoorAir",                  "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminaldualductvavoutdoorair"},
    {"OS:AirTerminal:DualDuct:ConstantVolume",                  "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminaldualductconstantvolume"},
    {"OS:AirTerminal:FourPipeInduction",                                   "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumefourpipeinduction"},
    {"OS:AirTerminal:SingleDuct:ConstantVolume:FourPipeInduction",         "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumefourpipeinduction"},

    // Fans
    {"OS:Fan:ConstantVolume",   "1.43-group-fans.html#fanconstantvolume"},
    {"OS:Fan:VariableVolume",   "1.43-group-fans.html#fanvariablevolume"},
    {"OS:Fan:OnOff",            "1.43-group-fans.html#fanonoff"},
    {"OS:Fan:ZoneExhaust",      "1.43-group-fans.html#fanzoneexhaust"},
    {"OS:Fan:SystemModel",      "1.43-group-fans.html#fansystemmodel"},
    {"OS:Fan:ComponentModel",   "1.43-group-fans.html#fancomponentmodel"},

    // Coils - Cooling
    {"OS:Coil:Cooling:DX:SingleSpeed",                                      "1.42-group-heating-and-cooling-coils.html#coilcoolingdxsinglespeed"},
    {"OS:Coil:Cooling:DX:TwoSpeed",                                         "1.42-group-heating-and-cooling-coils.html#coilcoolingdxtwospeed"},
    {"OS:Coil:Cooling:DX:MultiSpeed",                                       "1.42-group-heating-and-cooling-coils.html#coilcoolingdxmultispeed"},
    {"OS:Coil:Cooling:DX:VariableSpeed",                                    "1.42-group-heating-and-cooling-coils.html#coilcoolingdxvariablespeed"},
    {"OS:Coil:Cooling:DX:VariableRefrigerantFlow",                          "1.42-group-heating-and-cooling-coils.html#coilcoolingdxvariablerefrigerantflow"},
    {"OS:Coil:Cooling:DX:VariableRefrigerantFlow:FluidTemperatureControl",  "1.42-group-heating-and-cooling-coils.html#coilcoolingdxvariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:Coil:Cooling:Water",                                               "1.42-group-heating-and-cooling-coils.html#coilcoolingwater"},
    {"OS:Coil:Cooling:Water:DetailedGeometry",                              "1.42-group-heating-and-cooling-coils.html#coilcoolingwaterdetailedgeometry"},
    {"OS:Coil:Cooling:WaterToAirHeatPump:EquationFit",                      "1.42-group-heating-and-cooling-coils.html#coilcoolingwatertoairheatpumpequationfit"},
    {"OS:Coil:Cooling:WaterToAirHeatPump:VariableSpeedEquationFit",         "1.42-group-heating-and-cooling-coils.html#coilcoolingwatertoairheatpumpvariablespeedequationfit"},
    {"OS:Coil:Cooling:FourPipeBeam",                                        "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumefourpipebeam"},
    {"OS:Coil:Cooling:DX:TwoStageWithHumidityControlMode",                 "1.42-group-heating-and-cooling-coils.html#coilcoolingdxtwostagewithhumiditycontrolmode"},
    {"OS:Coil:Cooling:DX:MultiSpeed:StageData",                            "1.42-group-heating-and-cooling-coils.html#coilcoolingdxmultispeed"},
    {"OS:Coil:Cooling:Water:Panel:Radiant",                                 "1.34-group-radiative-convective-units.html#zonehvaccoolingpanelradiantconvectivewater"},
    {"OS:Coil:Cooling:LowTemperatureRadiant:ConstantFlow",                  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantconstantflow"},
    {"OS:Coil:Cooling:LowTemperatureRadiant:VariableFlow",                  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantvariableflow"},
    {"OS:CoilPerformance:DX:Cooling",                                        "1.42-group-heating-and-cooling-coils.html#coilperformancedxcooling"},

    // Coils - Heating
    {"OS:Coil:Heating:Gas",                                                 "1.42-group-heating-and-cooling-coils.html#coilheatinggas-000"},
    {"OS:Coil:Heating:Fuel",                                                "1.42-group-heating-and-cooling-coils.html#coilheatinggas-000"},
    {"OS:Coil:Heating:Electric",                                            "1.42-group-heating-and-cooling-coils.html#coilheatingelectric"},
    {"OS:Coil:Heating:Water",                                               "1.42-group-heating-and-cooling-coils.html#coilheatingwater"},
    {"OS:Coil:Heating:DX:SingleSpeed",                                      "1.42-group-heating-and-cooling-coils.html#coilheatingdxsinglespeed"},
    {"OS:Coil:Heating:DX:MultiSpeed",                                       "1.42-group-heating-and-cooling-coils.html#coilheatingdxmultispeed"},
    {"OS:Coil:Heating:DX:VariableSpeed",                                    "1.42-group-heating-and-cooling-coils.html#coilheatingdxvariablespeed"},
    {"OS:Coil:Heating:DX:VariableRefrigerantFlow",                          "1.42-group-heating-and-cooling-coils.html#coilheatingdxvariablerefrigerantflow"},
    {"OS:Coil:Heating:DX:VariableRefrigerantFlow:FluidTemperatureControl",  "1.42-group-heating-and-cooling-coils.html#coilheatingdxvariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:Coil:Heating:WaterToAirHeatPump:EquationFit",                      "1.42-group-heating-and-cooling-coils.html#coilheatingwatertoairheatpumpequationfit"},
    {"OS:Coil:Heating:WaterToAirHeatPump:VariableSpeedEquationFit",         "1.42-group-heating-and-cooling-coils.html#coilheatingwatertoairheatpumpvariablespeedequationfit"},
    {"OS:Coil:Heating:Desuperheater",                                       "1.42-group-heating-and-cooling-coils.html#coilheatingdesuperheater"},
    {"OS:Coil:Heating:FourPipeBeam",                                        "1.30-group-zone-HVAC-air-loop-terminal-units.html#airterminalsingleductconstantvolumefourpipebeam"},
    {"OS:Coil:Heating:Gas:MultiStage",                                      "1.42-group-heating-and-cooling-coils.html#coilheatinggasmultistage"},
    {"OS:Coil:Heating:Gas:MultiStage:StageData",                            "1.42-group-heating-and-cooling-coils.html#coilheatinggasmultistage"},
    {"OS:Coil:Heating:LowTemperatureRadiant:ConstantFlow",                  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantconstantflow"},
    {"OS:Coil:Heating:LowTemperatureRadiant:VariableFlow",                  "1.34-group-radiative-convective-units.html#zonehvaclowtemperatureradiantvariableflowdesign"},
    {"OS:Coil:Heating:Water:Baseboard",                                     "1.34-group-radiative-convective-units.html#zonehvacbaseboardconvectivewater"},
    {"OS:Coil:Heating:Water:Baseboard:Radiant",                             "1.34-group-radiative-convective-units.html#zonehvacbaseboardradiantconvectivewater"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump",                             "1.42-group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumppumped"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump:Pumped",                      "1.42-group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumppumped"},
    {"OS:Coil:WaterHeating:AirToWaterHeatPump:Wrapped",                     "1.42-group-heating-and-cooling-coils.html#coilwaterheatingairtowaterheatpumpwrapped"},
    {"OS:Coil:WaterHeating:Desuperheater",                                  "1.42-group-heating-and-cooling-coils.html#coilwaterheatingdesuperheater"},

    // Evaporative Coolers
    {"OS:EvaporativeCooler:Direct:ResearchSpecial",     "1.52-group-evaporative-coolers.html#evaporativecoolerdirectresearchspecial"},
    {"OS:EvaporativeCooler:Indirect:ResearchSpecial",   "1.52-group-evaporative-coolers.html#evaporativecoolerindirectresearchspecial"},
    {"OS:EvaporativeCooler:Direct:CelDekPad",           "1.52-group-evaporative-coolers.html#evaporativecoolerdirectceldekpad"},
    {"OS:EvaporativeCooler:Indirect:CelDekPad",         "1.52-group-evaporative-coolers.html#evaporativecoolerindirectceldekpad"},
    {"OS:EvaporativeCooler:Indirect:WetCoil",           "1.52-group-evaporative-coolers.html#evaporativecoolerindirectwetcoil"},

    // Humidifiers
    {"OS:Humidifier:Steam:Electric", "1.44-group-humidifiers-and-dehumidifiers.html#humidifiersteamelectric"},
    {"OS:Humidifier:Steam:Gas",      "1.44-group-humidifiers-and-dehumidifiers.html#humidifiersteamgas"},

    // Unitary Equipment
    {"OS:AirLoopHVAC:UnitaryHeatPump:AirToAir",            "1.32-group-unitary-equipment.html#airloophvacunitaryheatpumpairtoair"},
    {"OS:AirLoopHVAC:UnitaryHeatPump:AirToAir:MultiSpeed",  "1.32-group-unitary-equipment.html#airloophvacunitaryheatpumpairtoairmultispeed"},
    {"OS:AirLoopHVAC:UnitarySystem",                        "1.32-group-unitary-equipment.html#airloophvacunitarysystem"},
    {"OS:AirLoopHVAC:UnitaryHeatCool:VAVChangeoverBypass",  "1.32-group-unitary-equipment.html#airloophvacunitaryheatcoolvavchangeoverbypass"},

    // Variable Refrigerant Flow
    {"OS:AirConditioner:VariableRefrigerantFlow",                               "1.33-group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflow"},
    {"OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl",       "1.33-group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflowfluidtemperaturecontrol"},
    {"OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl:HR",    "1.33-group-variable-refrigerant-flow-equipment.html#airconditionervariablerefrigerantflowfluidtemperaturecontrolhr"},

    // Air Distribution / AirLoop
    {"OS:AirLoopHVAC",                      "1.26-group-air-distribution.html#group-air-distribution"},
    {"OS:AirLoopHVAC:OutdoorAirSystem",     "1.26-group-air-distribution.html#airloophvacoutdoorairsystem"},
    {"OS:OutdoorAir:Mixer",                 "1.26-group-air-distribution.html#outdoorairmixer"},
    {"OS:Controller:OutdoorAir",            "1.51-group-controllers.html#controlleroutdoorair"},
    {"OS:Controller:MechanicalVentilation",              "1.51-group-controllers.html#controllermechanicalventilation"},
    {"OS:ZoneHVAC:EnergyRecoveryVentilator:Controller",  "1.51-group-controllers.html#zonehvacenergyrecoveryventilatorcontroller"},
    {"OS:AirLoopHVAC:ZoneMixer",            "1.37-group-air-path.html#airloophvaczonemixer"},
    {"OS:AirLoopHVAC:ZoneSplitter",         "1.37-group-air-path.html#airloophvaczonesplitter"},
    {"OS:AirLoopHVAC:ReturnPlenum",         "1.37-group-air-path.html#airloophvacreturnplenum"},
    {"OS:AirLoopHVAC:SupplyPlenum",         "1.37-group-air-path.html#airloophvacsupplyplenum"},

    // Setpoint Managers
    {"OS:SetpointManager:Scheduled",                    "1.50-group-setpoint-managers.html#setpointmanagerscheduled"},
    {"OS:SetpointManager:Scheduled:DualSetpoint",       "1.50-group-setpoint-managers.html#setpointmanagerscheduleddualsetpoint"},
    {"OS:SetpointManager:MixedAir",                     "1.50-group-setpoint-managers.html#setpointmanagermixedair"},
    {"OS:SetpointManager:OutdoorAirReset",              "1.50-group-setpoint-managers.html#setpointmanageroutdoorairreset"},
    {"OS:SetpointManager:SingleZone:Reheat",            "1.50-group-setpoint-managers.html#setpointmanagersinglezonereheat"},
    {"OS:SetpointManager:SingleZone:Heating",           "1.50-group-setpoint-managers.html#setpointmanagersinglezoneheating"},
    {"OS:SetpointManager:SingleZone:Cooling",           "1.50-group-setpoint-managers.html#setpointmanagersinglezonecooling"},
    {"OS:SetpointManager:Warmest",                      "1.50-group-setpoint-managers.html#setpointmanagerwarmest"},
    {"OS:SetpointManager:Coldest",                      "1.50-group-setpoint-managers.html#setpointmanagercoldest"},
    {"OS:SetpointManager:FollowOutdoorAirTemperature",  "1.50-group-setpoint-managers.html#setpointmanagerfollowoutdoorairtemperature"},
    {"OS:SetpointManager:FollowGroundTemperature",      "1.50-group-setpoint-managers.html#setpointmanagerfollowgroundtemperature"},
    {"OS:SetpointManager:CondenserEnteringReset",       "1.50-group-setpoint-managers.html#setpointmanagercondenserenteringreset"},
    {"OS:SetpointManager:WarmestTemperatureFlow",       "1.50-group-setpoint-managers.html#setpointmanagerwarmesttemperatureflow"},
    {"OS:SetpointManager:OutdoorAirPretreat",           "1.50-group-setpoint-managers.html#setpointmanageroutdoorairpretreat"},
    {"OS:SetpointManager:MultiZone:Heating:Average",    "1.50-group-setpoint-managers.html#setpointmanagermultizoneheatingaverage"},
    {"OS:SetpointManager:MultiZone:Cooling:Average",    "1.50-group-setpoint-managers.html#setpointmanagermultizonecoolingaverage"},
    {"OS:SetpointManager:FollowSystemNodeTemperature",              "1.50-group-setpoint-managers.html#setpointmanagerfollowsystemnodetemperature"},
    {"OS:SetpointManager:MultiZone:Humidity:Maximum",               "1.50-group-setpoint-managers.html#setpointmanagermultizonehumiditymaximum"},
    {"OS:SetpointManager:MultiZone:Humidity:Minimum",               "1.50-group-setpoint-managers.html#setpointmanagermultizonehumidityminimum"},
    {"OS:SetpointManager:MultiZone:MaximumHumidity:Average",        "1.50-group-setpoint-managers.html#setpointmanagermultizonemaximumhumidityaverage"},
    {"OS:SetpointManager:MultiZone:MinimumHumidity:Average",        "1.50-group-setpoint-managers.html#setpointmanagermultizoneminimumhumidityaverage"},
    {"OS:SetpointManager:SingleZone:Humidity:Maximum",              "1.50-group-setpoint-managers.html#setpointmanagersinglezonehumiditymaximum"},
    {"OS:SetpointManager:SingleZone:Humidity:Minimum",              "1.50-group-setpoint-managers.html#setpointmanagersinglezonehumidityminimum"},
    {"OS:SetpointManager:SingleZone:OneStageCooling",               "1.50-group-setpoint-managers.html#setpointmanagersinglezoneonestagecooling"},
    {"OS:SetpointManager:SingleZone:OneStageHeating",               "1.50-group-setpoint-managers.html#setpointmanagersinglezoneonestageheating"},
    {"OS:SetpointManager:SystemNodeReset:Humidity",                 "1.50-group-setpoint-managers.html#setpointmanagersystemnoderesethumidity"},
    {"OS:SetpointManager:SystemNodeReset:Temperature",              "1.50-group-setpoint-managers.html#setpointmanagersystemnoderesettemperature"},

    // Pumps
    {"OS:Pump:VariableSpeed",           "1.40-group-pumps.html#pumpvariablespeed"},
    {"OS:Pump:ConstantSpeed",           "1.40-group-pumps.html#pumpconstantspeed"},
    {"OS:HeaderedPumps:VariableSpeed",  "1.40-group-pumps.html#headeredpumpsvariablespeed"},
    {"OS:HeaderedPumps:ConstantSpeed",  "1.40-group-pumps.html#headeredpumpsconstantspeed"},

    // Solar Collectors
    {"OS:SolarCollector:FlatPlate:Water",               "1.39-group-solar-collectors.html#solarcollectorflatplatewater"},
    {"OS:SolarCollector:IntegralCollectorStorage",       "1.39-group-solar-collectors.html#solarcollectorintegralcollectorstorage"},
    {"OS:SolarCollector:UnglazedTranspired",             "1.39-group-solar-collectors.html#solarcollectorunglazedtranspired"},
    {"OS:SolarCollector:FlatPlate:PhotovoltaicThermal",  "1.39-group-solar-collectors.html#solarcollectorflatplatephotovoltaicthermal"},
    {"OS:SolarCollectorPerformance:FlatPlate",           "1.39-group-solar-collectors.html#solarcollectorperformanceflatplate"},
    {"OS:SolarCollectorPerformance:IntegralCollectorStorage", "1.39-group-solar-collectors.html#solarcollectorperformanceintegralcollectorstorage"},

    // Plant Heating and Cooling Equipment
    {"OS:Boiler:HotWater",                          "1.23-group-plant-equipment.html#boilerhotwater"},
    {"OS:Boiler:Steam",                             "1.23-group-plant-equipment.html#boilersteam"},
    {"OS:Chiller:Electric:EIR",                     "1.23-group-plant-equipment.html#chillerelectriceir"},
    {"OS:Chiller:Electric:ReformulatedEIR",         "1.23-group-plant-equipment.html#chillerelectricreformulatedeir"},
    {"OS:Chiller:Absorption",                       "1.23-group-plant-equipment.html#chillerabsorption"},
    {"OS:Chiller:Absorption:Indirect",              "1.23-group-plant-equipment.html#chillerabsorptionindirect"},
    {"OS:Chiller:Absorption:Direct",                "1.23-group-plant-equipment.html#chillerabsorption"},
    {"OS:ChillerHeater:Absorption:DirectFired",     "1.23-group-plant-equipment.html#chillerheaterabsorptiondirectfired"},
    {"OS:DistrictCooling",                          "1.23-group-plant-equipment.html#districtcooling"},
    {"OS:DistrictHeating",                          "1.23-group-plant-equipment.html#districtheating"},
    {"OS:DistrictHeating:Water",                    "1.23-group-plant-equipment.html#districtheating"},
    {"OS:HeatPump:PlantLoop:EIR:Cooling",                  "1.23-group-plant-equipment.html#plhp_eir_cooling"},
    {"OS:HeatPump:PlantLoop:EIR:Heating",                  "1.23-group-plant-equipment.html#plhp_eir_heating"},
    {"OS:HeatPump:WaterToWater:EquationFit:Cooling",       "1.23-group-plant-equipment.html#heatpumpwatertowaterequationfitcooling"},
    {"OS:HeatPump:WaterToWater:EquationFit:Heating",       "1.23-group-plant-equipment.html#heatpumpwatertowaterequationfitheating"},
    {"OS:HeatPump:AirToWater:FuelFired:Cooling",           "1.23-group-plant-equipment.html#plhp_fuelfired"},
    {"OS:HeatPump:AirToWater:FuelFired:Heating",           "1.23-group-plant-equipment.html#plhp_fuelfired"},
    {"OS:CentralHeatPumpSystem",                           "1.23-group-plant-equipment.html#centralheatpumpsystem"},
    {"OS:CentralHeatPumpSystem:Module",                    "1.23-group-plant-equipment.html#centralheatpumpsystem"},
    {"OS:ChillerHeaterPerformance:Electric:EIR",           "1.23-group-plant-equipment.html#chillerheaterperformancelectriceir"},
    {"OS:PlantComponent:TemperatureSource",                "1.23-group-plant-equipment.html#plantcomponenttemperaturesource"},
    {"OS:PlantComponent:UserDefined",                      "1.48-group-user-defined-hvac-and-plant-component-models.html#plantcomponentuserdefined"},

    // Cooling Towers and Fluid Coolers
    {"OS:CoolingTower:SingleSpeed",             "1.25-group-condenser-equipment.html#coolingtowersinglespeed"},
    {"OS:CoolingTower:TwoSpeed",                "1.25-group-condenser-equipment.html#coolingtowertwospeed"},
    {"OS:CoolingTower:VariableSpeed",           "1.25-group-condenser-equipment.html#coolingtowervariablespeed"},
    {"OS:CoolingTower:VariableSpeed:Merkel",    "1.25-group-condenser-equipment.html#coolingtowervariablespeedmerkel"},
    {"OS:CoolingTowerPerformance:CoolTools",    "1.25-group-condenser-equipment.html#coolingtowerperformancecooltools"},
    {"OS:CoolingTowerPerformance:YorkCalc",     "1.25-group-condenser-equipment.html#coolingtowerperformanceyorkcalc"},
    {"OS:EvaporativeFluidCooler:SingleSpeed",   "1.25-group-condenser-equipment.html#evaporativefluidcoolersinglespeed"},
    {"OS:EvaporativeFluidCooler:TwoSpeed",      "1.25-group-condenser-equipment.html#evaporativefluidcoolertwospeed"},
    {"OS:FluidCooler:SingleSpeed",              "1.25-group-condenser-equipment.html#fluidcoolersinglespeed"},
    {"OS:FluidCooler:TwoSpeed",                 "1.25-group-condenser-equipment.html#fluidcoolertwospeed"},

    // Heat Recovery
    {"OS:HeatExchanger:AirToAir:SensibleAndLatent",     "1.53-group-heat-recovery.html#heatexchangerairtoairsensibleandlatent"},
    {"OS:HeatExchanger:FluidToFluid",                   "1.25-group-condenser-equipment.html#heatexchangerfluidtofluid"},
    {"OS:HeatExchanger:Desiccant:BalancedFlow",         "1.53-group-heat-recovery.html#heatexchangerdesiccantbalancedflow"},

    // Condenser Equipment and Ground Heat Exchangers
    {"OS:GroundHeatExchanger:Vertical",         "1.25-group-condenser-equipment.html#groundheatexchangersystem"},
    {"OS:GroundHeatExchanger:HorizontalTrench", "1.25-group-condenser-equipment.html#groundheatexchangerhorizontaltrench"},
    {"OS:GroundHeatExchanger:Slinky",           "1.25-group-condenser-equipment.html#groundheatexchangerslinky"},

    // Water Heaters and Thermal Storage
    {"OS:WaterHeater:Mixed",                        "1.24-group-water-heaters.html#waterheatermixed"},
    {"OS:WaterHeater:Stratified",                   "1.24-group-water-heaters.html#waterheaterstratified"},
    {"OS:WaterHeater:HeatPump",                     "1.24-group-water-heaters.html#waterheaterheatpumppumpedcondenser"},
    {"OS:WaterHeater:HeatPump:PumpedCondenser",     "1.24-group-water-heaters.html#waterheaterheatpumppumpedcondenser"},
    {"OS:WaterHeater:HeatPump:WrappedCondenser",    "1.24-group-water-heaters.html#waterheaterheatpumpwrappedcondenser"},
    {"OS:WaterHeater:Sizing",                       "1.24-group-water-heaters.html#waterheatersizing"},
    {"OS:ThermalStorage:Ice:Detailed",              "1.24-group-water-heaters.html#thermalstorageicedetailed"},
    {"OS:ThermalStorage:ChilledWater:Stratified",   "1.24-group-water-heaters.html#thermalstoragechilledwaterstratified"},

    // Water Systems
    {"OS:WaterUse:Equipment",   "1.56-group-water-systems.html#wateruseequipment"},
    {"OS:WaterUse:Connections", "1.56-group-water-systems.html#wateruseconnections"},

    // Refrigeration
    {"OS:Refrigeration:AirChiller",               "1.35-group-refrigeration.html#refrigerationairchiller"},
    {"OS:Refrigeration:Case",                     "1.35-group-refrigeration.html#refrigerationcase"},
    {"OS:Refrigeration:Compressor",               "1.35-group-refrigeration.html#refrigerationcompressor"},
    {"OS:Refrigeration:CompressorRack",           "1.35-group-refrigeration.html#refrigerationcompressorrack"},
    {"OS:Refrigeration:Condenser:AirCooled",      "1.35-group-refrigeration.html#refrigerationcondenseraircooled"},
    {"OS:Refrigeration:Condenser:Cascade",        "1.35-group-refrigeration.html#refrigerationcondensercascade"},
    {"OS:Refrigeration:Condenser:EvaporativeCooled", "1.35-group-refrigeration.html#refrigerationcondenserevaporativecooled"},
    {"OS:Refrigeration:Condenser:WaterCooled",    "1.35-group-refrigeration.html#refrigerationcondenserwatercooled"},
    {"OS:Refrigeration:DefrostCycleParameters",   "1.35-group-refrigeration.html#refrigerationwalkin"},
    {"OS:Refrigeration:GasCooler:AirCooled",      "1.35-group-refrigeration.html#refrigerationgascooleraircooled"},
    {"OS:Refrigeration:SecondarySystem",          "1.35-group-refrigeration.html#refrigerationsecondarysystem"},
    {"OS:Refrigeration:Subcooler:LiquidSuction",  "1.35-group-refrigeration.html#refrigerationsubcooler"},
    {"OS:Refrigeration:Subcooler:Mechanical",     "1.35-group-refrigeration.html#refrigerationsubcooler"},
    {"OS:Refrigeration:System",                   "1.35-group-refrigeration.html#refrigerationsystem"},
    {"OS:Refrigeration:TranscriticalSystem",      "1.35-group-refrigeration.html#refrigerationtranscriticalsystem"},
    {"OS:Refrigeration:WalkIn",                   "1.35-group-refrigeration.html#refrigerationwalkin"},
    {"OS:Refrigeration:WalkIn:ZoneBoundary",      "1.35-group-refrigeration.html#refrigerationwalkin"},

    // Node / Branch Management
    {"OS:Pipe:Adiabatic", "1.20-group-node-branch-management.html#pipeadiabatic"},
    {"OS:Pipe:Indoor",    "1.20-group-node-branch-management.html#pipeindoor"},
    {"OS:Pipe:Outdoor",   "1.20-group-node-branch-management.html#pipeoutdoor"},
    {"OS:Duct",           "1.20-group-node-branch-management.html#duct"},

    // Plant / Condenser Control
    {"OS:PlantLoop",                                        "1.21-group-plant-condenser-loops.html#plantloop"},
    {"OS:CondenserLoop",                                    "1.21-group-plant-condenser-loops.html#condenserloop"},
    {"OS:PlantEquipmentList",                               "1.22-group-plant-condenser-control.html#plantequipmentlist"},
    {"OS:PlantEquipmentOperation:CoolingLoad",              "1.22-group-plant-condenser-control.html#plantequipmentoperationcoolingload"},
    {"OS:PlantEquipmentOperation:HeatingLoad",              "1.22-group-plant-condenser-control.html#plantequipmentoperationheatingload"},
    {"OS:PlantEquipmentOperation:ComponentSetpoint",        "1.22-group-plant-condenser-control.html#plantequipmentoperationcomponentsetpoint"},
    {"OS:PlantEquipmentOperation:OutdoorDryBulb",           "1.22-group-plant-condenser-control.html#plantequipmentoperationoutdoordrybulb"},
    {"OS:PlantEquipmentOperation:OutdoorWetBulb",           "1.22-group-plant-condenser-control.html#plantequipmentoperationoutdoorwetbulb"},
    {"OS:PlantEquipmentOperation:ThermalEnergyStorage",     "1.22-group-plant-condenser-control.html#plantequipmentoperationthermalenergystorage"},
    {"OS:PlantEquipmentOperation:Uncontrolled",             "1.22-group-plant-condenser-control.html#plantequipmentoperationuncontrolled"},
    {"OS:TemperingValve",                                   "1.41-plant-condenser-flow-control.html#temperingvalve"},
    {"OS:LoadProfile:Plant",                                "1.38-group-non-zone-equipment.html#loadprofileplant"},
    {"OS:AvailabilityManagerAssignmentList",                "1.26-group-air-distribution.html#availabilitymanagerassignmentlist"},
    {"OS:AvailabilityManager:Scheduled",                    "1.49-group-system-availability-managers.html#availabilitymanagerscheduled"},
    {"OS:AvailabilityManager:ScheduledOn",                  "1.49-group-system-availability-managers.html#availabilitymanagerscheduledon"},
    {"OS:AvailabilityManager:ScheduledOff",                 "1.49-group-system-availability-managers.html#availabilitymanagerscheduledoff"},
    {"OS:AvailabilityManager:NightCycle",                   "1.49-group-system-availability-managers.html#availabilitymanagernightcycle"},
    {"OS:AvailabilityManager:DifferentialThermostat",       "1.49-group-system-availability-managers.html#availabilitymanagerdifferentialthermostat"},
    {"OS:AvailabilityManager:OptimumStart",                 "1.49-group-system-availability-managers.html#availabilitymanageroptimumstart"},
    {"OS:AvailabilityManager:NightVentilation",             "1.49-group-system-availability-managers.html#availabilitymanagernightventilation"},
    {"OS:AvailabilityManager:HybridVentilation",            "1.49-group-system-availability-managers.html#availabilitymanagerhybridventilation"},
    {"OS:AvailabilityManager:LowTemperatureTurnOn",         "1.49-group-system-availability-managers.html#availabilitymanagerlowtemperatureturnon"},
    {"OS:AvailabilityManager:HighTemperatureTurnOff",       "1.49-group-system-availability-managers.html#availabilitymanagerhightemperatureturnoff"},
    {"OS:AvailabilityManager:LowTemperatureTurnOff",        "1.49-group-system-availability-managers.html#availabilitymanagerlowtemperatureturnoff"},
    {"OS:AvailabilityManager:HighTemperatureTurnOn",        "1.49-group-system-availability-managers.html#availabilitymanagerhightemperatureturnon"},

    // Energy Management System
    {"OS:EnergyManagementSystem:Sensor",                    "1.45-group-energy-management-system-ems.html#energymanagementsystemsensor"},
    {"OS:EnergyManagementSystem:Actuator",                  "1.45-group-energy-management-system-ems.html#energymanagementsystemactuator"},
    {"OS:EnergyManagementSystem:Program",                   "1.45-group-energy-management-system-ems.html#energymanagementsystemprogram"},
    {"OS:EnergyManagementSystem:ProgramCallingManager",     "1.45-group-energy-management-system-ems.html#energymanagementsystemprogramcallingmanager"},
    {"OS:EnergyManagementSystem:GlobalVariable",            "1.45-group-energy-management-system-ems.html#energymanagementsystemglobalvariable"},
    {"OS:EnergyManagementSystem:OutputVariable",            "1.45-group-energy-management-system-ems.html#energymanagementsystemoutputvariable"},
    {"OS:EnergyManagementSystem:TrendVariable",             "1.45-group-energy-management-system-ems.html#energymanagementsystemtrendvariable"},
    {"OS:EnergyManagementSystem:InternalVariable",          "1.45-group-energy-management-system-ems.html#energymanagementsysteminternalvariable"},
    {"OS:EnergyManagementSystem:ConstructionIndexVariable", "1.45-group-energy-management-system-ems.html#energymanagementsystemconstructionindexvariable"},

    // Performance Curves
    {"OS:Curve:Linear",             "1.58-group---performance-curves.html#curvelinear"},
    {"OS:Curve:Quadratic",          "1.58-group---performance-curves.html#curvequadratic"},
    {"OS:Curve:Cubic",              "1.58-group---performance-curves.html#curvecubic"},
    {"OS:Curve:Quartic",            "1.58-group---performance-curves.html#curvequartic"},
    {"OS:Curve:Exponent",           "1.58-group---performance-curves.html#curveexponent"},
    {"OS:Curve:Bicubic",            "1.58-group---performance-curves.html#curvebicubic"},
    {"OS:Curve:Biquadratic",        "1.58-group---performance-curves.html#curvebiquadratic"},
    {"OS:Curve:QuadraticLinear",    "1.58-group---performance-curves.html#curvequadraticlinear"},
    {"OS:Curve:CubicLinear",        "1.58-group---performance-curves.html#curvecubiclinear"},
    {"OS:Curve:Triquadratic",       "1.58-group---performance-curves.html#curvetriquadratic"},
    {"OS:Curve:FanPressureRise",    "1.58-group---performance-curves.html#curvefanpressurerise"},
    {"OS:Curve:ExponentialDecay",          "1.58-group---performance-curves.html#curveexponentialdecay"},
    {"OS:Curve:ExponentialSkewNormal",     "1.58-group---performance-curves.html#curveexponentialskewnormal"},
    {"OS:Curve:Sigmoid",                   "1.58-group---performance-curves.html#curvesigmoid"},
    {"OS:Curve:QuadLinear",                "1.58-group---performance-curves.html#curvequadlinear"},
    {"OS:Curve:QuintLinear",               "1.58-group---performance-curves.html#curvequintlinear"},
    {"OS:Curve:RectangularHyperbola1",     "1.58-group---performance-curves.html#curverectangularhyperbola1"},
    {"OS:Curve:RectangularHyperbola2",     "1.58-group---performance-curves.html#curverectangularhyperbola2"},
    {"OS:Table:IndependentVariable",     "1.59-group-performance-tables.html#tableindependentvariable"},
    {"OS:Table:IndependentVariableList", "1.59-group-performance-tables.html#tableindependentvariablelist"},
    {"OS:Table:Lookup",                  "1.59-group-performance-tables.html#tablelookup"},

    // Output
    {"OS:Output:Diagnostics",                "1.5-group-simulation-parameters.html#outputdiagnostics"},
    {"OS:OutputControl:ReportingTolerances", "5.1-group-reports.html#outputcontrolreportingtolerances"},
    {"OS:Output:JSON",                       "5.1-group-reports.html#outputjson"},
    {"OS:Output:Table:SummaryReports",       "7.5-outputtablesummaryreports.html#outputtablesummaryreports"},
    {"OS:OutputControl:ResilienceSummaries", "5.1-group-reports.html#OutputControlResilienceSummaries"},

    // Life Cycle Cost
    {"OS:LifeCycleCost:Parameters",         "3.17-lifecyclecostparameters.html#lifecyclecostparameters"},
    {"OS:LifeCycleCost:UsePriceEscalation", "3.20-lifecyclecostusepriceescalation.html#lifecyclecostusepriceescalation"},
  };
  // clang-format on

  auto it = urlMap.constFind(iddTypeName);
  if (it != urlMap.constEnd()) {
    return base + it.value();
  }
  qWarning() << "Cannot find doc url for: " + iddTypeName;
  return {};
}

// Returns the local EnergyPlus Input Output Reference page URL for a given
// OpenStudio IDD group name (e.g. "OpenStudio Simulation"), or an empty string
// if the group has no known page. Group names match \group
// declarations in OpenStudio.idd. Groups that span multiple EnergyPlus
// chapters (e.g. "OpenStudio HVAC") are omitted.
inline QString iddGroupDocUrl(const QString& groupName) {
  static const QString base =
    QUrl::fromLocalFile(QString::fromStdString(openstudio::toString(openstudio::energyPlusDocDirectory())) + "/").toString();

  // clang-format off
  static const QHash<QString, QString> groupMap{
    {"OpenStudio Core",                              "1.5-group-simulation-parameters.html"},
    {"OpenStudio Simulation",                        "1.5-group-simulation-parameters.html"},
    {"OpenStudio Site",                              "1.7-group-location-climate-weather-file-access.html"},
    {"OpenStudio Materials",                         "1.9-group-surface-construction-elements.html"},
    {"OpenStudio Constructions",                     "1.9-group-surface-construction-elements.html"},
    {"OpenStudio Space Load Definitions",            "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html"},
    {"OpenStudio Exterior Equipment Definitions",    "1.16-group-exterior-energy-use-equipment.html"},
    {"OpenStudio Schedules",                         "1.8-group-schedules.html"},
    {"OpenStudio Geometry",                          "1.10-group-thermal-zone-descriptiongeometry.html"},
    {"OpenStudio Space Loads",                       "1.14-group-internal-gains-people-lights-other-internal-zone-equipment.html"},
    {"OpenStudio Exterior Equipment",                "1.16-group-exterior-energy-use-equipment.html"},
    {"OpenStudio Lighting Simulation",               "1.15-group-daylighting.html"},
    {"OpenStudio Refrigeration",                     "1.35-group-refrigeration.html"},
    {"Solar Collectors",                             "1.39-group-solar-collectors.html"},
    {"Energy Management System (EMS)",               "1.45-group-energy-management-system-ems.html"},
    {"User Defined HVAC and Plant Component Models", "1.48-group-user-defined-hvac-and-plant-component-models.html"},
  };
  // clang-format on

  auto it = groupMap.constFind(groupName);
  if (it != groupMap.constEnd()) {
    return base + it.value();
  }
  qWarning() << "Cannot find doc url for: " + groupName;
  return {};
}

#endif  // MODELEDITOR_IDDOBJECTDOCURL_HPP
