# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACPackagedTerminalAirConditioner
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxCoolingCapacity
    coolingCoil.maxCoolingCapacity
  end

  def maxAirFlowRate
    vals = []
    if supplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << supplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSupplyAirFlowRateDuringCoolingOperation.get
    end
    if supplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << supplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSupplyAirFlowRateDuringHeatingOperation.get
    end
    if supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.get
    elsif autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisNeeded.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

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

  def maxHeatingCapacityAutosized
    heatingCoil.maxHeatingCapacityAutosized
  end

  def maxCoolingCapacityAutosized
    coolingCoil.maxCoolingCapacityAutosized
  end

  def maxAirFlowRateAutosized
    if supplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
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
    effs += supplyAirFan.performanceCharacteristics
    effs += heatingCoil.performanceCharacteristics
    effs += coolingCoil.performanceCharacteristics
    return effs
  end
end
