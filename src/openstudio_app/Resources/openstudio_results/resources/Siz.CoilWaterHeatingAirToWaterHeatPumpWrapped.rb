# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilWaterHeatingAirToWaterHeatPumpWrapped
  def performanceCharacteristics
    effs = []
    effs << [ratedCOP, 'Rated COP']
    return effs
  end
end
