require 'openstudio'
require 'oga'

ROOT_DIR = File.absolute_path(File.join(File.dirname(__FILE__), "../../"))
POLICY_PATH = File.join(ROOT_DIR, 'src', 'openstudio_lib', 'library', 'OpenStudioPolicy.xml')
VALID_ACCESSES = ["LOCKED", "HIDDEN", "FREE"]
FACTORY = OpenStudio::IddFileAndFactoryWrapper.new("OpenStudio".to_IddFileType)

# Helper to locate a given IddObject via Name in the current IDD
# by using IddFileAndFactoryWrapper
#
# @param object_name [String] eg 'OS:ZoneHVAC:TerminalUnit:VariableRefrigerantFlow'
# @return [OpenStudio::IddObject] the IddObject
# raises if not found
def locate_object_in_current_idd(object_name)
  obj = FACTORY.getObject(object_name)
  raise "Cannot locate #{object_name} in current IDD" if obj.empty?
  return obj.get
end

# Scans all fields for an IddObject and reutrn hash of {fieldname: index}
#
# @param iddObject [OpenStudio::IddObject] the IddObject to scan
# @return [Hash] : keys are the field names, values are the indices (0-indexed)
def get_fields(iddObject)
  num_fields = iddObject.numFields + iddObject.properties.numExtensible
  fields = []
  fields_h = {}
  for i in 0..num_fields-1
    f = iddObject.getField(i).get
    fields << [f.name.to_s]
    fields_h[f.name.to_s] = i
  end
  return fields_h
end

def get_objects_with_curves_or_node_names
  h = {}
  FACTORY.objects.each do |obj|
    cleaned_name = obj.name.gsub(':', '_')
    get_fields(obj).each do |field, index|
      if field.end_with?('Curve Name') || field.end_with?('Curve') || field.end_with?('Node') || field.end_with?('Node Name') || field.end_with?('Actuator')
        if !h.has_key?(cleaned_name)
          h[cleaned_name] = []
        end
        h[cleaned_name] << field
      end
    end
  end
  return h
end


handle = File.open(POLICY_PATH)
doc = Oga.parse_xml(handle); nil;
handle.close

foundIddObjectTypeNames = {}

doc.xpath('//POLICY').each do |policy|
  raise "Unexpected number of policy attributes (#{policy.attributes.size}) for #{policy}" if policy.attributes.size > 1
  raise if policy.attributes.first.name != "IddObjectType"
  iddObjectTypeName = policy.attributes.first.value
  iddObject = locate_object_in_current_idd(iddObjectTypeName)
  raise "Duplicate policy for '#{iddObjectTypeName}'" if foundIddObjectTypeNames.has_key?(iddObjectTypeName)

  foundIddObjectTypeNames[iddObjectTypeName] = []
  iddFields = get_fields(iddObject)
  policy.xpath("rule").each do |rule|

    iddField = rule.get("IddField")
    raise if iddField.nil?
    access = rule.get("Access")
    raise if access.nil?

    raise "Unexpected number of rule attributes (#{rule.attributes.size}) for #{policy}" if rule.attributes.size != 2

    raise "Invalid access '#{access}' for '#{iddField}' in '#{iddObjectTypeName}'" if !VALID_ACCESSES.include?(access)
    puts "Field '#{iddField}' not found for #{iddObjectTypeName}" if !iddFields.include?(iddField)
    foundIddObjectTypeNames[iddObjectTypeName] << iddField
  end
end; nil;



h = get_objects_with_curves_or_node_names()
missing_objs = h.keys - foundIddObjectTypeNames.keys
if !missing_objs.empty?
  puts "These objects have curves or node names, consider adding them to OpenStudioPolicy.xml: #{missing_objs}"
end
puts "\n\n"
missing_objs.each do |obj_name|
  puts "  <POLICY IddObjectType=\"#{obj_name}\">"
  missing_curve_fields = h[obj_name]
  missing_curve_fields.each do |field|
    puts "    <rule IddField=\"#{field}\" Access=\"LOCKED\"/>"
  end
  puts "  </POLICY>"
end
puts "\n\n"


h.each do |obj_name, keys|
  next if !foundIddObjectTypeNames.has_key?(obj_name)
  missing_curve_fields = h[obj_name] - foundIddObjectTypeNames[obj_name]
  next if missing_curve_fields.empty?
  puts "For '#{obj_name}', consider adding these:"
  missing_curve_fields.each do |field|
    if field.include?('Node Name')
      access = "HIDDEN"
    else
      access= "LOCKED"
    end
    puts "    <rule IddField=\"#{field}\" Access=\"#{access}\"/>"
  end
end
