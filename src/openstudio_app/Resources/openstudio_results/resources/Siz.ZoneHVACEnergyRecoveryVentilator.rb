# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACEnergyRecoveryVentilator
  def maxAirFlowRate
    if supplyAirFlowRate.is_initialized
      supplyAirFlowRate
    else
      autosizedSupplyAirFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if supplyAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs += supplyAirFan.performanceCharacteristics
    return effs
  end
end
