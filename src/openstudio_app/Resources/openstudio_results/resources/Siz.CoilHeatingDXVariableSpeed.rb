# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilHeatingDXVariableSpeed
  def maxHeatingCapacity
    if ratedHeatingCapacityAtSelectedNominalSpeedLevel.is_initialized
      ratedHeatingCapacityAtSelectedNominalSpeedLevel
    else
      autosizedRatedHeatingCapacityAtSelectedNominalSpeedLevel
    end
  end

  def maxAirFlowRate
    if ratedAirFlowRateAtSelectedNominalSpeedLevel.is_initialized
      ratedAirFlowRateAtSelectedNominalSpeedLevel
    else
      autosizedRatedAirFlowRateAtSelectedNominalSpeedLevel
    end
  end

  def maxHeatingCapacityAutosized
    if ratedHeatingCapacityAtSelectedNominalSpeedLevel.is_initialized
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
end
