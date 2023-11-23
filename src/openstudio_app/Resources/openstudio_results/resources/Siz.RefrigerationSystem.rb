# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::RefrigerationSystem
  def performanceCharacteristics
    effs = []
    effs << [shellandCoilIntercoolerEffectiveness, 'Shelland Coil Intercooler Effectiveness']
    return effs
  end
end
