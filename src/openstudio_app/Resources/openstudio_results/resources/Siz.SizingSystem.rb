# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::SizingSystem
  def maxHeatingCapacity
    if heatingDesignCapacity.is_initialized
      heatingDesignCapacity
    else
      autosizedHeatingDesignCapacity
    end
  end

  def maxCoolingCapacity
    if coolingDesignCapacity.is_initialized
      coolingDesignCapacity
    else
      autosizedCoolingDesignCapacity
    end
  end

  def maxAirFlowRate
    if designOutdoorAirFlowRate.is_initialized
      designOutdoorAirFlowRate
    else
      autosizedDesignOutdoorAirFlowRate
    end
  end

  def maxHeatingCapacityAutosized
    if heatingDesignCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxCoolingCapacityAutosized
    if coolingDesignCapacity.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if designOutdoorAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
