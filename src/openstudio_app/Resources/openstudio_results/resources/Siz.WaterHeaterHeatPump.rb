# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::WaterHeaterHeatPump
  def maxWaterFlowRate
    if condenserWaterFlowRate.is_initialized
      condenserWaterFlowRate
    else
      autosizedCondenserWaterFlowRate
    end
  end

  def maxWaterFlowRateAutosized
    if condenserWaterFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += fan.performanceCharacteristics
    return effs
  end
end
