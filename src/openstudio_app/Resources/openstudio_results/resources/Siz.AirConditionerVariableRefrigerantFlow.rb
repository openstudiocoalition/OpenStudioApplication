# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirConditionerVariableRefrigerantFlow
  def maxHeatingCapacity
    if ratedTotalHeatingCapacity.is_initialized
      ratedTotalHeatingCapacity
    else
      autosizedRatedTotalHeatingCapacity
    end
  end

  def maxCoolingCapacity
    if ratedTotalCoolingCapacity.is_initialized
      ratedTotalCoolingCapacity
    else
      autosizedRatedTotalCoolingCapacity
    end
  end

  def maxAirFlowRate
    if evaporativeCondenserAirFlowRate.is_initialized
      evaporativeCondenserAirFlowRate
    else
      autosizedEvaporativeCondenserAirFlowRate
    end
  end

  def maxWaterFlowRate
    if waterCondenserVolumeFlowRate.is_initialized
      waterCondenserVolumeFlowRate
    else
      autosizedWaterCondenserVolumeFlowRate
    end
  end

  def maxHeatingCapacityAutosized
    if ratedTotalHeatingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxCoolingCapacityAutosized
    if ratedTotalCoolingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if evaporativeCondenserAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if waterCondenserVolumeFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [ratedCoolingCOP, 'Rated Cooling COP']
    effs << [ratedHeatingCOP, 'Rated Heating COP']
    effs << [evaporativeCondenserEffectiveness, 'Evaporative Condenser Effectiveness']
    return effs
  end
end
