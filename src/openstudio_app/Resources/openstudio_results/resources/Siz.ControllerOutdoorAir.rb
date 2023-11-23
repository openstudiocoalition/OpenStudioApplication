# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ControllerOutdoorAir
  def maxAirFlowRate
    if minimumOutdoorAirFlowRate.is_initialized
      minimumOutdoorAirFlowRate
    else
      autosizedMinimumOutdoorAirFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if minimumOutdoorAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
