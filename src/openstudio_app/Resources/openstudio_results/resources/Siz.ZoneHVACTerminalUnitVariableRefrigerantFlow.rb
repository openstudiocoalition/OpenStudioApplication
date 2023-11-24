# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACTerminalUnitVariableRefrigerantFlow
  def maxHeatingCapacity
    if heatingCoil.is_initialized
      heatingCoil.get.maxHeatingCapacity
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxCoolingCapacity
    if coolingCoil.is_initialized
      coolingCoil.get.maxCoolingCapacity
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRate
    vals = []
    if supplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << supplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSupplyAirFlowRateDuringCoolingOperation.get
    end
    if supplyAirFlowRateWhenNoCoolingisNeeded.is_initialized
      vals << supplyAirFlowRateWhenNoCoolingisNeeded.get
    elsif autosizedSupplyAirFlowRateWhenNoCoolingisNeeded.is_initialized
      vals << autosizedSupplyAirFlowRateWhenNoCoolingisNeeded.get
    end
    if supplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << supplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSupplyAirFlowRateDuringHeatingOperation.get
    end
    if supplyAirFlowRateWhenNoHeatingisNeeded.is_initialized
      vals << supplyAirFlowRateWhenNoHeatingisNeeded.get
    elsif autosizedSupplyAirFlowRateWhenNoHeatingisNeeded.is_initialized
      vals << autosizedSupplyAirFlowRateWhenNoHeatingisNeeded.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxWaterFlowRate
    vals = []
    if coolingCoil.is_initialized
      if coolingCoil.get.maxWaterFlowRate.is_initialized
        vals << coolingCoil.get.maxWaterFlowRate.get
      end
    end
    if heatingCoil.is_initialized
      if heatingCoil.get.maxWaterFlowRate.is_initialized
        vals << heatingCoil.get.maxWaterFlowRate.get
      end
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxHeatingCapacityAutosized
    if heatingCoil.is_initialized
      heatingCoil.get.maxHeatingCapacityAutosized
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxCoolingCapacityAutosized
    if coolingCoil.is_initialized
      coolingCoil.get.maxCoolingCapacityAutosized
    else
      OpenStudio::OptionalDouble.new
    end
  end

  def maxAirFlowRateAutosized
    if supplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateWhenNoCoolingisNeeded.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateWhenNoHeatingisNeeded.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxWaterFlowRateAutosized
    if coolingCoil.is_initialized
      if coolingCoil.get.maxWaterFlowRate.is_initialized
        return OpenStudio::OptionalBool.new(false)
      end
    end
    if heatingCoil.is_initialized
      if heatingCoil.get.maxWaterFlowRate.is_initialized
        return OpenStudio::OptionalBool.new(false)
      end
    end
    return OpenStudio::OptionalBool.new(true)
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    effs += coolingCoil.get.performanceCharacteristics if coolingCoil.is_initialized
    effs += heatingCoil.get.performanceCharacteristics if heatingCoil.is_initialized
    return effs
  end
end
