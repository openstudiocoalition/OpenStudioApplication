# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirLoopHVACUnitarySystem
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
    if supplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << supplyAirFlowRateDuringHeatingOperation.get
    elsif autosizedSupplyAirFlowRateDuringHeatingOperation.is_initialized
      vals << autosizedSupplyAirFlowRateDuringHeatingOperation.get
    end
    if supplyAirFlowRateWhenNoCoolingorHeatingisRequired.is_initialized
      vals << supplyAirFlowRateWhenNoCoolingorHeatingisRequired.get
    elsif autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisRequired.is_initialized
      vals << autosizedSupplyAirFlowRateWhenNoCoolingorHeatingisRequired.get
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxHeatingCapacityAutosized
    if heatingCoil.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
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
    if supplyAirFlowRateDuringCoolingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateDuringHeatingOperation.is_initialized
      return OpenStudio::OptionalBool.new(false)
    elsif supplyAirFlowRateWhenNoCoolingorHeatingisRequired.is_initialized
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += supplyFan.get.performanceCharacteristics if supplyFan.is_initialized
    effs += heatingCoil.get.performanceCharacteristics if heatingCoil.is_initialized
    effs += coolingCoil.get.performanceCharacteristics if coolingCoil.is_initialized
    return effs
  end
end
