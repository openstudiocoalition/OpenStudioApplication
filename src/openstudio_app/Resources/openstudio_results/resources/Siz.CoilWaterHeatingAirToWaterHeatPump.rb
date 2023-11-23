# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilWaterHeatingAirToWaterHeatPump
  def maxAirFlowRate
    if ratedEvaporatorAirFlowRate.is_initialized
      ratedEvaporatorAirFlowRate
    else
      autosizedRatedEvaporatorAirFlowRate
    end
  end

  def maxWaterFlowRate
    if ratedCondenserWaterFlowRate.is_initialized
      ratedCondenserWaterFlowRate
    else
      autosizedRatedCondenserWaterFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if ratedEvaporatorAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if ratedCondenserWaterFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [ratedCOP, 'Rated COP']
    return effs
  end
end
