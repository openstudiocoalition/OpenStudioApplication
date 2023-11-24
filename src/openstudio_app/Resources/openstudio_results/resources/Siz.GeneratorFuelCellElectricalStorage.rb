# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::GeneratorFuelCellElectricalStorage
  def performanceCharacteristics
    effs = []
    effs << [nominalChargingEnergeticEfficiency, 'Nominal Charging Energetic Efficiency']
    effs << [nominalDischargingEnergeticEfficiency, 'Nominal Discharging Energetic Efficiency']
    return effs
  end
end
