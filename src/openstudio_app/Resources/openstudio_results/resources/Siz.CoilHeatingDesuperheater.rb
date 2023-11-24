# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilHeatingDesuperheater
  def performanceCharacteristics
    effs = []
    effs << [heatReclaimRecoveryEfficiency, 'Heat Reclaim Recovery Efficiency']
    return effs
  end
end
