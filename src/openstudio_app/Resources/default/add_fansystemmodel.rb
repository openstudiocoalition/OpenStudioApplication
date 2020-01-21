# p = File.join(ENV["HOME"], "Software/Others/OS-build/Products/ruby/openstudio")
# require p
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

# There are meant to match exactly the current "Const Spd Fan" and "Var Spd
# Fan" (Fan:ConstantVolume / Fan:VariableVolume) from the hvac_library.osm
# that you could drag and drop on an AirLoopHVAC
fan_cv = FanSystemModel.new(m)
fan_cv.setAvailabilitySchedule(alwaysOn)
fan_cv.setName("FanSystemModel ConstantVolume")
fan_cv.setFanTotalEfficiency(0.7)
fan_cv.setDesignPressureRise(250.0)
fan_cv.setDesignPowerSizingMethod("TotalEfficiencyAndPressure")

fan_vv = FanSystemModel.new(m)
fan_vv.setName("FanSystemModel VAV")
fan_vv.setAvailabilitySchedule(alwaysOn)
fan_vv.setSpeedControlMethod("Continuous")
fan_vv.setElectricPowerMinimumFlowRateFraction(0)
fan_vv.setDesignPressureRise(1017.592)
fan_vv.setMotorEfficiency(0.93)
fan_vv.setFanTotalEfficiency(0.6045)
fan_vv.setDesignPowerSizingMethod("TotalEfficiencyAndPressure")
c = CurveQuartic.new(m)
c.setName("VAV Fan Curve")
c.setCoefficient1Constant(0.0407598940)
c.setCoefficient2x(0.08804497)
c.setCoefficient3xPOW2(-0.072926120)
c.setCoefficient4xPOW3(0.9437398230)
c.setCoefficient5xPOW4(0)
c.setMaximumCurveOutput(1)
c.setMinimumCurveOutput(0)
c.setInputUnitTypeforX("Dimensionless")
c.setOutputUnitType("Dimensionless")
fan_vv.setElectricPowerFunctionofFlowFractionCurve(c)


# Remove the alwaysOn it created by default, and the schedule type lim
m.getScheduleConstantByName("Always On Discrete").get.remove
m.getScheduleTypeLimitsByName("Always On Discrete Limits").get.remove

m.save('hvac_library.osm', true)
