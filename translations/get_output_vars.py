import sys
import os

sdk_python = r"C:\Users\ml\openstudioapplication\OpenStudio-3.11.0\OpenStudio-3.11.0+241b8abb4d-Windows\Python"
sys.path.insert(0, sdk_python)

os.add_dll_directory(r"C:\Users\ml\openstudioapplication\OpenStudio-3.11.0\OpenStudio-3.11.0+241b8abb4d-Windows\bin")

import openstudiomodel as osm

model = osm.Model()

# Add one instance of every concrete model object type so outputVariableNames() is populated
for iddType in osm.IddObjectType.getValues():
    try:
        factory = osm.ModelObjectFactory()
        obj = model.getOptionalModelObject(iddType)
    except:
        pass

# Collect all output variable names from every model object
all_vars = set()
for mo in model.getModelObjects(osm.ModelObject.iddObjectType()):
    try:
        for v in mo.outputVariableNames():
            all_vars.add(v)
    except:
        pass

# Also try creating one of each type and querying
concrete_types = [
    osm.AirLoopHVAC, osm.ThermalZone, osm.Space, osm.Surface, osm.SubSurface,
    osm.People, osm.Lights, osm.ElectricEquipment, osm.GasEquipment,
    osm.InternalMassDefinition, osm.ZoneVentilationDesignFlowRate,
    osm.CoilCoolingDXSingleSpeed, osm.CoilHeatingGas, osm.CoilHeatingWater,
    osm.CoilCoolingWater, osm.FanConstantVolume, osm.FanVariableVolume,
    osm.PumpVariableSpeed, osm.PumpConstantSpeed, osm.ChillerElectricEIR,
    osm.BoilerHotWater, osm.CoolingTowerSingleSpeed, osm.PlantLoop,
    osm.WaterHeaterMixed, osm.WaterUseConnections, osm.WaterUseEquipment,
]

for cls in concrete_types:
    try:
        obj = cls(model)
        for v in obj.outputVariableNames():
            all_vars.add(v)
    except Exception as e:
        pass

sorted_vars = sorted(all_vars)
print(f"Total unique output variables: {len(sorted_vars)}")
for v in sorted_vars:
    print(v)
