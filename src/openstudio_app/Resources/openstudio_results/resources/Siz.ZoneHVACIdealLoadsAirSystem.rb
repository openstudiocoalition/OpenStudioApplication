# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACIdealLoadsAirSystem
  def maxHeatingCapacity
    if maximumSensibleHeatingCapacity.is_initialized
      maximumSensibleHeatingCapacity
    else
      autosizedMaximumSensibleHeatingCapacity
    end
  end

  def maxCoolingCapacity
    if maximumTotalCoolingCapacity.is_initialized
      maximumTotalCoolingCapacity
    else
      autosizedMaximumTotalCoolingCapacity
    end
  end

  def maxAirFlowRate
    if maximumHeatingAirFlowRate.is_initialized
      maximumHeatingAirFlowRate
    else
      autosizedMaximumHeatingAirFlowRate
    end
  end

  def maxHeatingCapacityAutosized
    if maximumSensibleHeatingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxCoolingCapacityAutosized
    if maximumTotalCoolingCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if maximumHeatingAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [sensibleHeatRecoveryEffectiveness, 'Sensible Heat Recovery Effectiveness']
    effs << [latentHeatRecoveryEffectiveness, 'Latent Heat Recovery Effectiveness']
    return effs
  end
end
