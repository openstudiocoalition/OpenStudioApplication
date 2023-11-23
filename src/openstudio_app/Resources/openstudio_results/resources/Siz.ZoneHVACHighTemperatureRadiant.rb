# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ZoneHVACHighTemperatureRadiant
  def performanceCharacteristics
    effs = []
    effs << [combustionEfficiency, 'Combustion Efficiency']
    return effs
  end
end
