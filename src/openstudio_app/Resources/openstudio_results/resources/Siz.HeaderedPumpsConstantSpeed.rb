# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::HeaderedPumpsConstantSpeed
  def maxWaterFlowRate
    if totalRatedFlowRate.is_initialized
      totalRatedFlowRate
    else
      autosizedTotalRatedFlowRate
    end
  end

  def maxWaterFlowRateAutosized
    if totalRatedFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [ratedPumpHead, 'Rated Pump Head']
    effs << [motorEfficiency, 'Motor Efficiency']
    return effs
  end
end
