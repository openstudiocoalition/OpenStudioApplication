# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilHeatingWaterToAirHeatPumpVariableSpeedEquationFitSpeedData
  def performanceCharacteristics
    effs = []
    effs << [referenceUnitGrossRatedHeatingCOP, 'Reference Unit Gross Rated Heating COP']
    return effs
  end
end
