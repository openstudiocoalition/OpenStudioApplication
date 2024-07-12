# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingDXSingleSpeed
  def maxCoolingCapacity
    if ratedTotalCoolingCapacity.is_initialized
      ratedTotalCoolingCapacity
    else
      autosizedRatedTotalCoolingCapacity
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
    if ratedTotalCoolingCapacity.is_initialized
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
    effs << [ratedCOP, 'Rated COP']
    effs << [evaporativeCondenserEffectiveness, 'Evaporative Condenser Effectiveness']
    return effs
  end
end
