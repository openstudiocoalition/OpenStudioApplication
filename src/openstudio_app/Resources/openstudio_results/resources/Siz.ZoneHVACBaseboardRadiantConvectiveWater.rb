# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACBaseboardRadiantConvectiveWater
  def maxHeatingCapacity
    heatingCoil.maxHeatingCapacity
  end

  def maxWaterFlowRate
    heatingCoil.maxWaterFlowRate
  end

  def maxHeatingCapacityAutosized
    heatingCoil.maxHeatingCapacityAutosized
  end

  def maxWaterFlowRateAutosized
    heatingCoil.maxWaterFlowRateAutosized
  end

  def performanceCharacteristics
    effs = []
    effs += heatingCoil.performanceCharacteristics
    return effs
  end
end
