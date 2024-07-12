# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingDXVariableSpeedSpeedData
  def performanceCharacteristics
    effs = []
    effs << [referenceUnitGrossRatedCoolingCOP, 'Reference Unit Gross Rated Cooling COP']
    effs << [referenceUnitRatedPadEffectivenessofEvapPrecooling, 'Reference Unit Rated Pad Effectivenessof Evap Precooling']
    return effs
  end
end
