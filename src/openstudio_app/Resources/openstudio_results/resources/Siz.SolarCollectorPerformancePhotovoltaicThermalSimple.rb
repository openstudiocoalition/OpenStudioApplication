# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::SolarCollectorPerformancePhotovoltaicThermalSimple
  def performanceCharacteristics
    effs = []
    effs << [thermalConversionEfficiency, 'Thermal Conversion Efficiency']
    return effs
  end
end
