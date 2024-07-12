# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::RefrigerationTranscriticalSystem
  def performanceCharacteristics
    effs = []
    effs << [receiverPressure, 'Receiver Pressure']
    effs << [subcoolerEffectiveness, 'Subcooler Effectiveness']
    return effs
  end
end
