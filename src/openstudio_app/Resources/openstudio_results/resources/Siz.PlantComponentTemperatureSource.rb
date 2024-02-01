# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::PlantComponentTemperatureSource
  def maxWaterFlowRate
    if designVolumeFlowRate.is_initialized
      designVolumeFlowRate
    else
      autosizedDesignVolumeFlowRate
    end
  end

  def maxWaterFlowRateAutosized
    if designVolumeFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
