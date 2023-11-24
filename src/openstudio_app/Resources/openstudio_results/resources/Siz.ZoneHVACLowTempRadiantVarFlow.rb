# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACLowTempRadiantVarFlow
  def maxWaterFlowRate
    vals = []
    if coolingCoil.maxWaterFlowRate.is_initialized
      vals << coolingCoil.maxWaterFlowRate.get
    end
    if heatingCoil.maxWaterFlowRate.is_initialized
      vals << heatingCoil.maxWaterFlowRate.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxWaterFlowRateAutosized
    if coolingCoil.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif heatingCoil.maxWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += heatingCoil.performanceCharacteristics
    effs += coolingCoil.performanceCharacteristics
    return effs
  end
end
