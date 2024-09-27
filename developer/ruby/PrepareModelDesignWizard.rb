require 'openstudio'
require 'json'
# require 'openstudio/extension/core/os_lib_model_generation'
# require 'openstudio-standards'
require File.join(ENV['HOME'], 'Software/Others/openstudio_gems/openstudio-standards/lib/openstudio-standards.rb')

ROOT_DIR = File.absolute_path(File.join(File.dirname(__FILE__), "../../"))
JSON_PATH = File.join(ROOT_DIR, "src", "openstudio_lib", "library", "ModelDesignWizard.json")

extended = true
# https://github.com/NREL/openstudio-standards/blob/9fca7da0fd6ea83fad05e603291add43140cb4ba/lib/openstudio-standards/create_typical/space_type_ratios.rb#L12
# @param whole_building [Boolean] use a whole building space type for Office types
# Thing is that the ratios are all 0.99, so for now I won't bother
# This was fixed on master: https://github.com/NREL/openstudio-standards/commit/48812df5a142bf2496e8fc945da3293177ec1072
whole_building = false

h = {}

["DOE", "DEER"].each do |std|
  h[std] = {}
  ["building_types", "templates", "climate_zones"].each do |info|
    h[std][info] = OpenstudioStandards::CreateTypical.send("get_#{std.downcase}_#{info}", extended).to_a
  end

  h[std]["space_types"] = {}
  h[std]["templates"].each do |template|
    h[std]["space_types"][template] = {}
    h[std]["building_types"].each do |building_type|
      space_types = OpenstudioStandards::CreateTypical.get_space_types_from_building_type(building_type, template: template, whole_building: whole_building)
      h[std]["space_types"][template][building_type] = space_types
    end
  end

  h[std]["building_form_defaults"] = {}
  h[std]["building_types"].each do |building_type|
    h[std]["building_form_defaults"][building_type] = OpenstudioStandards::Geometry::building_form_defaults(building_type)
  end
end

File.write(JSON_PATH, JSON.pretty_generate(h))
