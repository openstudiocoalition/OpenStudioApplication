# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingWaterToAirHeatPumpVariableSpeedEquationFit
  def maxCoolingCapacity
    if grossRatedTotalCoolingCapacityAtSelectedNominalSpeedLevel.is_initialized
      grossRatedTotalCoolingCapacityAtSelectedNominalSpeedLevel
    else
      autosizedGrossRatedTotalCoolingCapacityAtSelectedNominalSpeedLevel
    end
  end

  def maxAirFlowRate
    if ratedAirFlowRateAtSelectedNominalSpeedLevel.is_initialized
      ratedAirFlowRateAtSelectedNominalSpeedLevel
    else
      autosizedRatedAirFlowRateAtSelectedNominalSpeedLevel
    end
  end

  def maxWaterFlowRate
    if ratedWaterFlowRateAtSelectedNominalSpeedLevel.is_initialized
      ratedWaterFlowRateAtSelectedNominalSpeedLevel
    else
      autosizedRatedWaterFlowRateAtSelectedNominalSpeedLevel
    end
  end

  def maxCoolingCapacityAutosized
    if grossRatedTotalCoolingCapacityAtSelectedNominalSpeedLevel.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if ratedAirFlowRateAtSelectedNominalSpeedLevel.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if ratedWaterFlowRateAtSelectedNominalSpeedLevel.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
