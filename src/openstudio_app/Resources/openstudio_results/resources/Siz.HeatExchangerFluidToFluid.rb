# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::HeatExchangerFluidToFluid
  def maxWaterFlowRate
    if loopDemandSideDesignFlowRate.is_initialized
      loopDemandSideDesignFlowRate
    else
      autosizedLoopDemandSideDesignFlowRate
    end
  end

  def maxWaterFlowRateAutosized
    if loopDemandSideDesignFlowRate.is_initialized
      # Not autosized if hard size field value present
      return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
