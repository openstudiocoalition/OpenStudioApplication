# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::AirTerminalDualDuctVAV
  def maxAirFlowRate
    if maximumDamperAirFlowRate.is_initialized
      maximumDamperAirFlowRate
    else
      autosizedMaximumDamperAirFlowRate
    end
  end

  def maxAirFlowRateAutosized
    if maximumDamperAirFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
