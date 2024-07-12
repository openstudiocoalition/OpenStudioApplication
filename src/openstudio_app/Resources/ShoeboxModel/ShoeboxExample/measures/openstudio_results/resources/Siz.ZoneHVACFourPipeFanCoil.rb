# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACFourPipeFanCoil
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxCoolingCapacity
    coolingCoil.maxCoolingCapacity
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
    if coolingCoil.maxWaterFlowRate.is_initialized
      vals << coolingCoil.maxWaterFlowRate.get
    end
    if heatingCoil.maxWaterFlowRate.is_initialized
      vals << heatingCoil.maxWaterFlowRate.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxHeatingCapacityAutosized
    heatingCoil.maxHeatingCapacityAutosized
  end

  def maxCoolingCapacityAutosized
    coolingCoil.maxCoolingCapacityAutosized
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
    if coolingCoil.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif heatingCoil.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    effs += coolingCoil.performanceCharacteristics
    effs += heatingCoil.performanceCharacteristics
    return effs
  end
end
