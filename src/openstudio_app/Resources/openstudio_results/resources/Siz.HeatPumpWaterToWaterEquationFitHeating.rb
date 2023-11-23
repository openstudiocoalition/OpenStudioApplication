# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::HeatPumpWaterToWaterEquationFitHeating
  def maxHeatingCapacity
    if ratedHeatingCapacity.is_initialized
      ratedHeatingCapacity
    else
      autosizedRatedHeatingCapacity
    end
  end

  def maxWaterFlowRate
    if referenceLoadSideFlowRate.is_initialized
      referenceLoadSideFlowRate
    else
      autosizedReferenceLoadSideFlowRate
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

  def maxWaterFlowRateAutosized
    if referenceLoadSideFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
