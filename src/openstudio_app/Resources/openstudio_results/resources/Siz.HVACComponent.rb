# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::HVACComponent
  def maxHeatingCapacity
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxHeatingCapacity
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxCoolingCapacity
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxCoolingCapacity
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRate
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxAirFlowRate
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxWaterFlowRate
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxWaterFlowRate
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def ratedPower
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.ratedPower
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxHeatingCapacityAutosized
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxHeatingCapacityAutosized
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxCoolingCapacityAutosized
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxCoolingCapacityAutosized
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRateAutosized
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxAirFlowRateAutosized
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def maxWaterFlowRateAutosized
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.maxWaterFlowRateAutosized
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def ratedPowerAutosized
    comp = cast_to_concrete_type
    return OpenStudio::OptionalDouble.new if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.ratedPowerAutosized
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return OpenStudio::OptionalDouble.new
    end
  end

  def performanceCharacteristics
    comp = cast_to_concrete_type
    return [] if comp.nil?
    if comp.respond_to?(__method__.to_s)
      return comp.performanceCharacteristics
    else
      # puts "#{__method__.to_s} not implemented for #{iddObject.type.name}"
      return []
    end
  end
end
