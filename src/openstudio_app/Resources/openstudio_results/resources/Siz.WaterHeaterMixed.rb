# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::WaterHeaterMixed
  def maxHeatingCapacity
    if heaterMaximumCapacity.is_initialized
      heaterMaximumCapacity
    else
      autosizedHeaterMaximumCapacity
    end
  end

  def maxWaterFlowRate
    if useSideDesignFlowRate.is_initialized
      useSideDesignFlowRate
    else
      autosizedUseSideDesignFlowRate
    end
  end

  def maxHeatingCapacityAutosized
    if heaterMaximumCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if useSideDesignFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [heaterThermalEfficiency, 'Heater Thermal Efficiency']
    effs << [useSideEffectiveness, 'Use Side Effectiveness']
    effs << [sourceSideEffectiveness, 'Source Side Effectiveness']
    return effs
  end
end
