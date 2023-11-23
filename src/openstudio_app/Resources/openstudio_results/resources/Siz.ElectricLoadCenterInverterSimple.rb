# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ElectricLoadCenterInverterSimple
  def performanceCharacteristics
    effs = []
    effs << [inverterEfficiency, 'Inverter Efficiency']
    return effs
  end
end
