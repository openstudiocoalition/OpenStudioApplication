# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneVentilationDesignFlowRate
  def performanceCharacteristics
    effs = []
    effs << [fanPressureRise, 'Fan Pressure Rise']
    effs << [fanTotalEfficiency, 'Fan Total Efficiency']
    return effs
  end
end
