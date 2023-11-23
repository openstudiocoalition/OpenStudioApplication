# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::ElectricLoadCenterStorageSimple
  def performanceCharacteristics
    effs = []
    effs << [nominalEnergeticEfficiencyforCharging, 'Nominal Energetic Efficiencyfor Charging']
    effs << [nominalDischargingEnergeticEfficiency, 'Nominal Discharging Energetic Efficiency']
    return effs
  end
end
