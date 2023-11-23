# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingWaterToAirHeatPumpVariableSpeedEquationFitSpeedData
  def performanceCharacteristics
    effs = []
    effs << [referenceUnitGrossRatedCoolingCOP, 'Reference Unit Gross Rated Cooling COP']
    return effs
  end
end
