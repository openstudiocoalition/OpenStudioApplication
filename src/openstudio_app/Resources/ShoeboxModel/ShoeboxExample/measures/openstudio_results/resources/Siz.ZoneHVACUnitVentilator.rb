# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACUnitVentilator
  def maxHeatingCapacity
    if heatingCoil.is_initialized
      heatingCoil.get.maxHeatingCapacity
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxCoolingCapacity
    if coolingCoil.is_initialized
      coolingCoil.get.maxCoolingCapacity
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRate
    if maximumSupplyAirFlowRate.is_initialized
      maximumSupplyAirFlowRate
    else
      autosizedMaximumSupplyAirFlowRate
    end
  end

  def maxWaterFlowRate
    vals = []
    if coolingCoil.is_initialized
      if coolingCoil.get.maxWaterFlowRate.is_initialized
        vals << coolingCoil.get.maxWaterFlowRate.get
      end
    end
    if heatingCoil.is_initialized
      if heatingCoil.get.maxWaterFlowRate.is_initialized
        vals << heatingCoil.get.maxWaterFlowRate.get
      end
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxHeatingCapacityAutosized
    if heatingCoil.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxCoolingCapacityAutosized
    if coolingCoil.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if maximumSupplyAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if coolingCoil.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif coolingCoil.get.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif heatingCoil.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif heatingCoil.get.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    effs += heatingCoil.get.performanceCharacteristics if heatingCoil.is_initialized
    effs += coolingCoil.get.performanceCharacteristics if coolingCoil.is_initialized
    return effs
  end
end
