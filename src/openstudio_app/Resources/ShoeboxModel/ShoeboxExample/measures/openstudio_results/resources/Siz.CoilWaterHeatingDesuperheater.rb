# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilWaterHeatingDesuperheater
  def performanceCharacteristics
    effs = []
    effs << [ratedHeatReclaimRecoveryEfficiency, 'Rated Heat Reclaim Recovery Efficiency']
    return effs
  end
end
