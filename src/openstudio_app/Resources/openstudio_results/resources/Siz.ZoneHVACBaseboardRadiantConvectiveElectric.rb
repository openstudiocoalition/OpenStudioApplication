# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACBaseboardRadiantConvectiveElectric
  def maxHeatingCapacity
    if heatingDesignCapacity.is_initialized
      heatingDesignCapacity
    else
      autosizedHeatingDesignCapacity
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

  def performanceCharacteristics
    effs = []
    effs << [efficiency, 'Efficiency']
    return effs
  end
end
