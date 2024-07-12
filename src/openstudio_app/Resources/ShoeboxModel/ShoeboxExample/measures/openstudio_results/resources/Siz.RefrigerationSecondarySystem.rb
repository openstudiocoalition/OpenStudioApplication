# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::RefrigerationSecondarySystem
  def performanceCharacteristics
    effs = []
    effs << [totalPumpHead, 'Total Pump Head']
    return effs
  end
end
