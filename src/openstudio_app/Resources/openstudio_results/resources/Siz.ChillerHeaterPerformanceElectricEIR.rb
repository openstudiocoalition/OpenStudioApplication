# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ChillerHeaterPerformanceElectricEIR
  def maxCoolingCapacity
    if referenceCoolingModeEvaporatorCapacity.is_initialized
      referenceCoolingModeEvaporatorCapacity
    else
      autosizedReferenceCoolingModeEvaporatorCapacity
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
    if referenceCoolingModeEvaporatorCapacity.is_initialized
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

  def performanceCharacteristics
    effs = []
    effs << [referenceCoolingModeCOP, 'Reference Cooling Mode COP']
    # check os version
    if Gem::Version.new(OpenStudio.openStudioVersion) > Gem::Version.new('2.9.1')
      effs << [fractionofCompressorElectricConsumptionRejectedbyCondenser, 'Fraction of Compressor Electric Consumption Rejected by Condenser']
    else
      effs << [compressorMotorEfficiency, 'Compressor Motor Fraction of Compressor Electric Consumption Rejected by Condenser']
    end
    return effs
  end
end
