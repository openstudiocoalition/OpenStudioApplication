# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ModelObject
  # Convert object to a concrete class if possible
  # @return [Object, nil] returns nil if the object was already
  # of its concrete class, returns the object casted to its concrete
  # class if it was casted successfully.
  def cast_to_concrete_type
    # puts "Casting '#{name}' to concrete type from #{caller_locations(1,1)[0].label}"
    # Get the class type and the IDD object type
    comp_class = self.class.name.to_s.gsub('OpenStudio::Model::', '')
    comp_type = iddObject.type.valueDescription.gsub('OS:', '').delete(':')
    case comp_type
    when 'CoilCoolingLowTemperatureRadiantVariableFlow'
      comp_type = 'CoilCoolingLowTempRadiantVarFlow'
    when 'CoilHeatingLowTemperatureRadiantVariableFlow'
      comp_type = 'CoilHeatingLowTempRadiantVarFlow'
    when 'ZoneHVACLowTemperatureRadiantVariableFlow'
      comp_type = 'ZoneHVACLowTempRadiantVarFlow'
    end
    # If the class type and the IDD object type are identical,
    # this means that the object is already of the concrete class.
    if comp_class == comp_type
      # puts "INFO: #{name} of type #{comp_type} is already concrete"
      return nil
    end
    # Cast the object to its concrete class type
    cast_method = "to_#{comp_type}"
    if respond_to?(cast_method)
      return public_send(cast_method).get
    else
      puts "ERROR: #{name} of type #{comp_type} cannot be made concrete using #{cast_method}"
      # raise "ERROR: #{name} of type #{comp_type} cannot be made concrete using #{cast_method}"
      return nil
    end
  end
end
