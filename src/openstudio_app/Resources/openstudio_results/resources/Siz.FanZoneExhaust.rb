# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::FanZoneExhaust
  def performanceCharacteristics
    effs = []
    effs << [fanEfficiency, 'Fan Efficiency']
    effs << [pressureRise, 'Pressure Rise']
    return effs
  end
end
