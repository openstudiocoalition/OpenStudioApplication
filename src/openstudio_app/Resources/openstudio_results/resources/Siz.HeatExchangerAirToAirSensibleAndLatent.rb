# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::HeatExchangerAirToAirSensibleAndLatent
  def maxAirFlowRate
    if nominalSupplyAirFlowRate.is_initialized
      nominalSupplyAirFlowRate
    else
      autosizedNominalSupplyAirFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if nominalSupplyAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [sensibleEffectivenessat100HeatingAirFlow, 'Sensible Effectivenessat 100 Heating Air Flow']
    effs << [latentEffectivenessat100HeatingAirFlow, 'Latent Effectivenessat 100 Heating Air Flow']
    effs << [sensibleEffectivenessat75HeatingAirFlow, 'Sensible Effectivenessat 75 Heating Air Flow']
    effs << [latentEffectivenessat75HeatingAirFlow, 'Latent Effectivenessat 75 Heating Air Flow']
    effs << [sensibleEffectivenessat100CoolingAirFlow, 'Sensible Effectivenessat 100 Cooling Air Flow']
    effs << [latentEffectivenessat100CoolingAirFlow, 'Latent Effectivenessat 100 Cooling Air Flow']
    effs << [sensibleEffectivenessat75CoolingAirFlow, 'Sensible Effectivenessat 75 Cooling Air Flow']
    effs << [latentEffectivenessat75CoolingAirFlow, 'Latent Effectivenessat 75 Cooling Air Flow']
    return effs
  end
end
