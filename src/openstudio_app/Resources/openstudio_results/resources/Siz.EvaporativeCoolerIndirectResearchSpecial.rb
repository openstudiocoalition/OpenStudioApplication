# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::EvaporativeCoolerIndirectResearchSpecial
  def maxAirFlowRate
    if primaryDesignAirFlowRate.is_initialized
      primaryDesignAirFlowRate
    else
      autosizedPrimaryDesignAirFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if primaryDesignAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [coolerMaximumEffectiveness, 'Cooler Maximum Effectiveness']
    effs << [secondaryFanTotalEfficiency, 'Secondary Fan Total Efficiency']
    effs << [secondaryFanDeltaPressure, 'Secondary Fan Delta Pressure']
    effs << [coolerDrybulbDesignEffectiveness, 'Cooler Drybulb Design Effectiveness']
    return effs
  end
end
