# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ChillerAbsorption
  def maxCoolingCapacity
    if nominalCapacity.is_initialized
      nominalCapacity
    else
      autosizedNominalCapacity
    end
  end

  def maxWaterFlowRate
    if designChilledWaterFlowRate.is_initialized
      designChilledWaterFlowRate
    else
      autosizedDesignChilledWaterFlowRate
    end
  end

  def maxCoolingCapacityAutosized
    if nominalCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if designChilledWaterFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
