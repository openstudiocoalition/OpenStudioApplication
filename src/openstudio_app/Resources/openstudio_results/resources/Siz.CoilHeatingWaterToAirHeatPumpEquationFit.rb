# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilHeatingWaterToAirHeatPumpEquationFit
  def maxHeatingCapacity
    if ratedHeatingCapacity.is_initialized
      ratedHeatingCapacity
    else
      autosizedRatedHeatingCapacity
    end
  end

  def maxAirFlowRate
    if ratedAirFlowRate.is_initialized
      ratedAirFlowRate
    else
      autosizedRatedAirFlowRate
    end
  end

  def maxWaterFlowRate
    if ratedWaterFlowRate.is_initialized
      ratedWaterFlowRate
    else
      autosizedRatedWaterFlowRate
    end
  end

  def maxHeatingCapacityAutosized
    if ratedHeatingCapacity.is_initialized
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

  def maxWaterFlowRateAutosized
    if ratedWaterFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
