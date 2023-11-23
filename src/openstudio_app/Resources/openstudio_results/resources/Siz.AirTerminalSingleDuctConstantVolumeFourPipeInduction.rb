# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirTerminalSingleDuctConstantVolumeFourPipeInduction
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxCoolingCapacity
    if coolingCoil.is_initialized
      coolingCoil.get.maxCoolingCapacity
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRate
    if maximumTotalAirFlowRate.is_initialized
      maximumTotalAirFlowRate
    else
      autosizedMaximumTotalAirFlowRate
    end
  end

  def maxWaterFlowRate
    vals = []
    if maximumHotWaterFlowRate.is_initialized
      vals << maximumHotWaterFlowRate.get
    elsif autosizedMaximumHotWaterFlowRate.is_initialized
      vals << autosizedMaximumHotWaterFlowRate.get
    end
    if maximumColdWaterFlowRate.is_initialized
      vals << maximumColdWaterFlowRate.get
    elsif autosizedMaximumColdWaterFlowRate.is_initialized
      vals << autosizedMaximumColdWaterFlowRate.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxHeatingCapacityAutosized
    heatingCoil.maxHeatingCapacityAutosized
  end

  def maxCoolingCapacityAutosized
    if coolingCoil.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if maximumTotalAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if maximumHotWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif maximumColdWaterFlowRate.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += heatingCoil.performanceCharacteristics
    effs += coolingCoil.get.performanceCharacteristics if coolingCoil.is_initialized
    return effs
  end
end
