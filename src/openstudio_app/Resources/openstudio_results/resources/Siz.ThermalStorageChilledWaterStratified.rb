# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ThermalStorageChilledWaterStratified
  def maxWaterFlowRate
    if useSideDesignFlowRate.is_initialized
      useSideDesignFlowRate
    else
      autosizedUseSideDesignFlowRate
    end
  end

  def maxWaterFlowRateAutosized
    if useSideDesignFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [useSideHeatTransferEffectiveness, 'Use Side Heat Transfer Effectiveness']
    effs << [sourceSideHeatTransferEffectiveness, 'Source Side Heat Transfer Effectiveness']
    return effs
  end
end
