require 'openstudio'
require 'json'
# require 'openstudio/extension/core/os_lib_model_generation'
require '/home/julien/Software/Others/openstudio_gems/openstudio-extension-gem/lib/openstudio/extension/core/os_lib_model_generation.rb'

include OsLib_ModelGeneration

ROOT_DIR = File.absolute_path(File.join(File.dirname(__FILE__), "../../"))
JSON_PATH = File.join(ROOT_DIR, "src", "openstudio_lib", "library", "ModelDesignWizard.json")

extended = true
whole_building = true

h = {}

["DOE", "DEER"].each do |std|
  h[std] = {}
  ["building_types", "templates", "climate_zones"].each do |info|
    h[std][info] = eval("get_#{std.downcase}_#{info}(#{extended}).to_a")
  end

  h[std]["space_types"] = {}
  h[std]["templates"].each do |template|
    h[std]["space_types"][template] = {}
    h[std]["building_types"].each do |building_type|
      space_types = get_space_types_from_building_type(building_type, template, whole_building)
      h[std]["space_types"][template][building_type] = space_types
    end
  end
end

File.write(JSON_PATH, JSON.pretty_generate(h))
