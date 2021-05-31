require 'openstudio'

include OpenStudio::Model

# Helper to load a model in one line
# It will raise if the path (or the model) isn't valid
#
# @param path [String] The path to the osm
# @return [OpenStudio::Model::Model] the resulting model.
def osload(path)
  translator = OpenStudio::OSVersion::VersionTranslator.new
  ospath = OpenStudio::Path.new(path)
  model = translator.loadModel(ospath)
  if model.empty?
      raise "Path '#{path}' is not a valid path to an OpenStudio Model"
  else
      model = model.get
  end
  return model
end

m = osload('hvac_library.osm')

alwaysOn = m.getScheduleConstantByName("Always On Discrete hvac_library").get

fan = FanComponentModel.new(m)

fan.setName('FanComponentModel')

# Availability Schedule Name: Required Object
fan.setAvailabilitySchedule(alwaysOn)

beltMaxEffMedium = OpenStudio::Model::CurveQuartic.new(m)
beltMaxEffMedium.setName('BeltMaxEffMedium')
beltMaxEffMedium.setCoefficient1Constant(-0.09504)
beltMaxEffMedium.setCoefficient2x(0.03415)
beltMaxEffMedium.setCoefficient3xPOW2(-0.008897)
beltMaxEffMedium.setCoefficient4xPOW3(0.001159)
beltMaxEffMedium.setCoefficient5xPOW4(-6.132e-05)
beltMaxEffMedium.setMinimumValueofx(-1.2)
beltMaxEffMedium.setMaximumValueofx(6.2)
beltMaxEffMedium.setMinimumCurveOutput(-4.6)
beltMaxEffMedium.setMaximumCurveOutput(0.0)
# beltMaxEffMedium.setInputUnitTypeforX("")
# beltMaxEffMedium.setOutputUnitType("")

beltPartLoadRegion1 = OpenStudio::Model::CurveRectangularHyperbola2.new(m)
beltPartLoadRegion1.setName('BeltPartLoadRegion1')
beltPartLoadRegion1.setCoefficient1C1(0.920797)
beltPartLoadRegion1.setCoefficient2C2(0.0262686)
beltPartLoadRegion1.setCoefficient3C3(0.151594)
beltPartLoadRegion1.setMinimumValueofx(0.0)
beltPartLoadRegion1.setMaximumValueofx(1.0)
beltPartLoadRegion1.setMinimumCurveOutput(0.01)
beltPartLoadRegion1.setMaximumCurveOutput(1.0)
# beltPartLoadRegion1.setInputUnitTypeforx("")
# beltPartLoadRegion1.setOutputUnitType("")

beltPartLoadRegion2 = OpenStudio::Model::CurveExponentialDecay.new(m)
beltPartLoadRegion2.setName('BeltPartLoadRegion2')
beltPartLoadRegion2.setCoefficient1C1(1.011965)
beltPartLoadRegion2.setCoefficient2C2(-0.339038)
beltPartLoadRegion2.setCoefficient3C3(-3.43626)
beltPartLoadRegion2.setMinimumValueofx(0.0)
beltPartLoadRegion2.setMaximumValueofx(1.0)
beltPartLoadRegion2.setMinimumCurveOutput(0.01)
beltPartLoadRegion2.setMaximumCurveOutput(1.0)
# beltPartLoadRegion2.setInputUnitTypeforx("")
# beltPartLoadRegion2.setOutputUnitType("")

beltPartLoadRegion3 = OpenStudio::Model::CurveRectangularHyperbola2.new(m)
beltPartLoadRegion3.setName('BeltPartLoadRegion3')
beltPartLoadRegion3.setCoefficient1C1(1.037778)
beltPartLoadRegion3.setCoefficient2C2(0.0103068)
beltPartLoadRegion3.setCoefficient3C3(-0.0268146)
beltPartLoadRegion3.setMinimumValueofx(0.0)
beltPartLoadRegion3.setMaximumValueofx(1.0)
beltPartLoadRegion3.setMinimumCurveOutput(0.01)
beltPartLoadRegion3.setMaximumCurveOutput(1.0)
# beltPartLoadRegion3.setInputUnitTypeforx("")
# beltPartLoadRegion3.setOutputUnitType("")

motorMaxEffAvg = OpenStudio::Model::CurveRectangularHyperbola1.new(m)
motorMaxEffAvg.setName('MotorMaxEffAvg')
motorMaxEffAvg.setCoefficient1C1(0.29228)
motorMaxEffAvg.setCoefficient2C2(3.368739)
motorMaxEffAvg.setCoefficient3C3(0.762471)
motorMaxEffAvg.setMinimumValueofx(0.0)
motorMaxEffAvg.setMaximumValueofx(7.6)
motorMaxEffAvg.setMinimumCurveOutput(0.01)
motorMaxEffAvg.setMaximumCurveOutput(1.0)
# motorMaxEffAvg.setInputUnitTypeforx("")
# motorMaxEffAvg.setOutputUnitType("")

motorPartLoad = OpenStudio::Model::CurveRectangularHyperbola2.new(m)
motorPartLoad.setName('MotorPartLoad')
motorPartLoad.setCoefficient1C1(1.137209)
motorPartLoad.setCoefficient2C2(0.0502359)
motorPartLoad.setCoefficient3C3(-0.0891503)
motorPartLoad.setMinimumValueofx(0.0)
motorPartLoad.setMaximumValueofx(1.0)
motorPartLoad.setMinimumCurveOutput(0.01)
motorPartLoad.setMaximumCurveOutput(1.0)
# motorPartLoad.setInputUnitTypeforx("")
# motorPartLoad.setOutputUnitType("")

vFDPartLoad = OpenStudio::Model::CurveRectangularHyperbola2.new(m)
vFDPartLoad.setName('VFDPartLoad')
vFDPartLoad.setCoefficient1C1(0.987405)
vFDPartLoad.setCoefficient2C2(0.0155361)
vFDPartLoad.setCoefficient3C3(-0.0059365)
vFDPartLoad.setMinimumValueofx(0.0)
vFDPartLoad.setMaximumValueofx(1.0)
vFDPartLoad.setMinimumCurveOutput(0.01)
vFDPartLoad.setMaximumCurveOutput(1.0)
# vFDPartLoad.setInputUnitTypeforx("")
# vFDPartLoad.setOutputUnitType("")


fan.setMaximumBeltEfficiencyCurve(beltMaxEffMedium)
fan.setNormalizedBeltEfficiencyCurveRegion1(beltPartLoadRegion1)
fan.setNormalizedBeltEfficiencyCurveRegion2(beltPartLoadRegion2)
fan.setNormalizedBeltEfficiencyCurveRegion3(beltPartLoadRegion3)
fan.setMaximumMotorEfficiencyCurve(motorMaxEffAvg)
fan.setNormalizedMotorEfficiencyCurve(motorPartLoad)
fan.setVFDEfficiencyCurve(vFDPartLoad)


# Remove the alwaysOn it created by default, and the schedule type lim
sch = m.getScheduleConstantByName("Always On Discrete").get
sch_lim = sch.scheduleTypeLimits.get
sch_lim.remove()
sch.remove()

m.save('hvac_library.osm', true)
