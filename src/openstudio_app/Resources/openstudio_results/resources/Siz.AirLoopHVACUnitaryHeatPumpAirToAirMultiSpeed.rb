# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirLoopHVACUnitaryHeatPumpAirToAirMultiSpeed
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxCoolingCapacity
    coolingCoil.maxCoolingCapacity
  end

  def maxAirFlowRate
    vals = []
    if supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.get
    elsif autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      vals << autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisNeeded.get
    end
    if speed1SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << speed1SupplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSpeed1SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSpeed1SupplyAirFlowRateDuringHeatingOperation.get
    end
    if speed2SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << speed2SupplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSpeed2SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSpeed2SupplyAirFlowRateDuringHeatingOperation.get
    end
    if speed3SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << speed3SupplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSpeed3SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSpeed3SupplyAirFlowRateDuringHeatingOperation.get
    end
    if speed4SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << speed4SupplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSpeed4SupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSpeed4SupplyAirFlowRateDuringHeatingOperation.get
    end
    if speed1SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << speed1SupplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSpeed1SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSpeed1SupplyAirFlowRateDuringCoolingOperation.get
    end
    if speed2SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << speed2SupplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSpeed2SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSpeed2SupplyAirFlowRateDuringCoolingOperation.get
    end
    if speed3SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << speed3SupplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSpeed3SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSpeed3SupplyAirFlowRateDuringCoolingOperation.get
    end
    if speed4SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << speed4SupplyAirFlowRateDuringCoolingOperation.get
    elsif autosizedSpeed4SupplyAirFlowRateDuringCoolingOperation.is_initialized
      vals << autosizedSpeed4SupplyAirFlowRateDuringCoolingOperation.get
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
    if supplyAirFlowRateWhenNoCoolingorHeatingisNeeded.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed1SupplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed2SupplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed3SupplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed4SupplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed1SupplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed2SupplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed3SupplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif speed4SupplyAirFlowRateDuringCoolingOperation.is_initialized
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
