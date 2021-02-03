# p = File.join(ENV["HOME"], "Software/Others/OSApp-build-release/OpenStudio-3.1.1/OpenStudio-3.1.1-alpha+3d83fc2b65-Linux/usr/local/openstudio-3.1.1-alpha/Ruby/openstudio.rb")
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

h = HumidifierSteamGas.new(m)
h.setName("Humidifier Steam Gas")
h.setAvailabilitySchedule(alwaysOn)
h.setInletWaterTemperatureOption("FixedInletWaterTemperature")

# Remove the alwaysOn it created by default, and the schedule type lim
sch = m.getScheduleConstantByName("Always On Discrete")
if !sch.empty?
  sch = sch.get
  sch_lim = sch.scheduleTypeLimits.get
  sch_lim.remove()
  sch.remove()
end

m.save('hvac_library.osm', true)
