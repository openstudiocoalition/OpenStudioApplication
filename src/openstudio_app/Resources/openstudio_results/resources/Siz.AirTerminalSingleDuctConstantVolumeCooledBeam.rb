# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirTerminalSingleDuctConstantVolumeCooledBeam
  def maxAirFlowRate
    if supplyAirVolumetricFlowRate.is_initialized
      supplyAirVolumetricFlowRate
    else
      autosizedSupplyAirVolumetricFlowRate
    end
  end

  def maxWaterFlowRate
    if maximumTotalChilledWaterVolumetricFlowRate.is_initialized
      maximumTotalChilledWaterVolumetricFlowRate
    else
      autosizedMaximumTotalChilledWaterVolumetricFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if supplyAirVolumetricFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if maximumTotalChilledWaterVolumetricFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
