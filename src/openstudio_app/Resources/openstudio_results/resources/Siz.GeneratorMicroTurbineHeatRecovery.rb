# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::GeneratorMicroTurbineHeatRecovery
  def performanceCharacteristics
    effs = []
    effs << [referenceThermalEfficiencyUsingLowerHeatValue, 'Reference Thermal Efficiency Using Lower Heat Value']
    return effs
  end
end
