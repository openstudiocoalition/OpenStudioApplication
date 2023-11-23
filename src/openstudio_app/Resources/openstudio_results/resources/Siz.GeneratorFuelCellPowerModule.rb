# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::GeneratorFuelCellPowerModule
  def performanceCharacteristics
    effs = []
    effs << [nominalEfficiency, 'Nominal Efficiency']
    return effs
  end
end
