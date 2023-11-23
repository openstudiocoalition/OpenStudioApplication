# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::FanVariableVolume
  def maxAirFlowRate
    if maximumFlowRate.is_initialized
      maximumFlowRate
    else
      autosizedMaximumFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if maximumFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def performanceCharacteristics
    effs = []
    effs << [fanEfficiency, 'Fan Efficiency']
    effs << [pressureRise, 'Pressure Rise']
    effs << [motorEfficiency, 'Motor Efficiency']
    return effs
  end
end
