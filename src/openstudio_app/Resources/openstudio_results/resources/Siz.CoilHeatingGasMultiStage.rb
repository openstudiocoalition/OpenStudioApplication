# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilHeatingGasMultiStage
  def maxHeatingCapacity
    stages.last.maxHeatingCapacity
  end

  def maxHeatingCapacityAutosized
    stages.last.maxHeatingCapacityAutosized
  end
end
