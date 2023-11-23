# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::GeneratorMicroTurbine
  def performanceCharacteristics
    effs = []
    effs << [referenceElectricalEfficiencyUsingLowerHeatingValue, 'Reference Electrical Efficiency Using Lower Heating Value']
    return effs
  end
end
