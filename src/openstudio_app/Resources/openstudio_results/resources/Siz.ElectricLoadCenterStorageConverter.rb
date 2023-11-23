# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ElectricLoadCenterStorageConverter
  def performanceCharacteristics
    effs = []
    effs << [simpleFixedEfficiency, 'Simple Fixed Efficiency']
    return effs
  end
end
