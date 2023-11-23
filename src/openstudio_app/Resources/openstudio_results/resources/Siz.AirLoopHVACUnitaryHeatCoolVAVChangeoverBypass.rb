# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirLoopHVACUnitaryHeatCoolVAVChangeoverBypass
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxCoolingCapacity
    coolingCoil.maxCoolingCapacity
  end

  def maxAirFlowRate
    vals = []
    if systemAirFlowRateDuringCoolingOperation.is_initialized
      vals << systemAirFlowRateDuringCoolingOperation.get
    elsif autosizedSystemAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSystemAirFlowRateDuringCoolingOperation.get
    end
    if systemAirFlowRateDuringHeatingOperation.is_initialized
      vals << systemAirFlowRateDuringHeatingOperation.get
    elsif autosizedSystemAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSystemAirFlowRateDuringHeatingOperation.get
    end
    if systemAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << systemAirFlowRateWhenNoCoolingorHeatingisNeeded.get
    elsif autosizedSystemAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << autosizedSystemAirFlowRateWhenNoCoolingorHeatingisNeeded.get
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
    coolingCoil.maxCoolingCapacityAutosized
  end

  def maxAirFlowRateAutosized
    if systemAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif systemAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif systemAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    effs += coolingCoil.performanceCharacteristics
    effs += heatingCoil.performanceCharacteristics
    return effs
  end
end
