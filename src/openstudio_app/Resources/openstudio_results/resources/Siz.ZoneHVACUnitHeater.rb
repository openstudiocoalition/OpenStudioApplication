# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACUnitHeater
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxAirFlowRate
    if maximumSupplyAirFlowRate.is_initialized
      maximumSupplyAirFlowRate
    else
      autosizedMaximumSupplyAirFlowRate
    end
  end

  def maxWaterFlowRate
    heatingCoil.maxWaterFlowRate
  end

  def maxHeatingCapacityAutosized
    heatingCoil.maxHeatingCapacityAutosized
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
    heatingCoil.maxWaterFlowRateAutosized
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    effs += heatingCoil.performanceCharacteristics
    return effs
  end
end
