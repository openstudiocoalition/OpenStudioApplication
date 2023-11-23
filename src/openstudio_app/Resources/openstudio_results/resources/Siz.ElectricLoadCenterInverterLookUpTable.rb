# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ElectricLoadCenterInverterLookUpTable
  def performanceCharacteristics
    effs = []
    effs << [efficiencyAt10PowerAndNominalVoltage, 'Efficiency At 10 Power And Nominal Voltage']
    effs << [efficiencyAt20PowerAndNominalVoltage, 'Efficiency At 20 Power And Nominal Voltage']
    effs << [efficiencyAt30PowerAndNominalVoltage, 'Efficiency At 30 Power And Nominal Voltage']
    effs << [efficiencyAt50PowerAndNominalVoltage, 'Efficiency At 50 Power And Nominal Voltage']
    effs << [efficiencyAt75PowerAndNominalVoltage, 'Efficiency At 75 Power And Nominal Voltage']
    effs << [efficiencyAt100PowerAndNominalVoltage, 'Efficiency At 100 Power And Nominal Voltage']
    return effs
  end
end
