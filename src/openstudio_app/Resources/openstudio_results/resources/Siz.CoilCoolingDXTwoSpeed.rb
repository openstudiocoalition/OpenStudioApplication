# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingDXTwoSpeed
  def maxCoolingCapacity
    if ratedHighSpeedTotalCoolingCapacity.is_initialized
      ratedHighSpeedTotalCoolingCapacity
    else
      autosizedRatedHighSpeedTotalCoolingCapacity
    end
  end

  def maxAirFlowRate
    if ratedHighSpeedAirFlowRate.is_initialized
      ratedHighSpeedAirFlowRate
    else
      autosizedRatedHighSpeedAirFlowRate
    end
  end

  def maxCoolingCapacityAutosized
    if ratedHighSpeedTotalCoolingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if ratedHighSpeedAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [ratedHighSpeedCOP, 'Rated High Speed COP']
    effs << [ratedLowSpeedCOP, 'Rated Low Speed COP']
    effs << [highSpeedEvaporativeCondenserEffectiveness, 'High Speed Evaporative Condenser Effectiveness']
    effs << [lowSpeedEvaporativeCondenserEffectiveness, 'Low Speed Evaporative Condenser Effectiveness']
    return effs
  end
end
