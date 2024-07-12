# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::PhotovoltaicPerformanceSimple
  def performanceCharacteristics
    effs = []
    effs << [fixedEfficiency, 'Fixed Efficiency']
    return effs
  end
end
