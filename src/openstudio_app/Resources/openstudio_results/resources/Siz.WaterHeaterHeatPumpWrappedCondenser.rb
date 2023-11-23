# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::WaterHeaterHeatPumpWrappedCondenser
  def performanceCharacteristics
    effs = []
    effs += fan.performanceCharacteristics
    return effs
  end
end
