# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirTerminalSingleDuctParallelPIUReheat
  def maxHeatingCapacity
    reheatCoil.maxHeatingCapacity
  end

  def maxAirFlowRate
    if maximumPrimaryAirFlowRate.is_initialized
      maximumPrimaryAirFlowRate
    else
      autosizedMaximumPrimaryAirFlowRate
    end
  end

  def maxWaterFlowRate
    reheatCoil.maxWaterFlowRate
  end

  def maxHeatingCapacityAutosized
    reheatCoil.maxHeatingCapacityAutosized
  end

  def maxAirFlowRateAutosized
    if maximumPrimaryAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    reheatCoil.maxWaterFlowRateAutosized
  end

  def performanceCharacteristics
    effs = []
    effs += fan.performanceCharacteristics
    effs += reheatCoil.performanceCharacteristics
    return effs
  end
end
