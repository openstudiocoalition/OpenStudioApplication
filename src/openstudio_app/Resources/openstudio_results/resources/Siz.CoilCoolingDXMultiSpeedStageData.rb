# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingDXMultiSpeedStageData
  def maxCoolingCapacity
    if grossRatedTotalCoolingCapacity.is_initialized
      grossRatedTotalCoolingCapacity
    else
      autosizedGrossRatedTotalCoolingCapacity
    end
  end

  def maxAirFlowRate
    if ratedAirFlowRate.is_initialized
      ratedAirFlowRate
    else
      autosizedRatedAirFlowRate
    end
  end

  def maxCoolingCapacityAutosized
    if grossRatedTotalCoolingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if ratedAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [grossRatedCoolingCOP, 'Gross Rated Cooling COP']
    effs << [evaporativeCondenserEffectiveness, 'Evaporative Condenser Effectiveness']
    return effs
  end
end
