# *******************************************************************************
# OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
# See also https://openstudio.net/license
# *******************************************************************************

class OpenStudio::Model::CoilCoolingDXTwoStageWithHumidityControlMode
  def maxCoolingCapacity
    vals = []
    if normalModeStage1CoilPerformance.is_initialized
      cap = normalModeStage1CoilPerformance.get.maxCoolingCapacity
      vals << cap.get if cap.is_initialized
    end
    if normalModeStage1Plus2CoilPerformance.is_initialized
      cap = normalModeStage1Plus2CoilPerformance.get.maxCoolingCapacity
      vals << cap.get if cap.is_initialized
    end
    if dehumidificationMode1Stage1CoilPerformance.is_initialized
      cap = dehumidificationMode1Stage1CoilPerformance.get.maxCoolingCapacity
      vals << cap.get if cap.is_initialized
    end
    if dehumidificationMode1Stage1Plus2CoilPerformance.is_initialized
      cap = dehumidificationMode1Stage1Plus2CoilPerformance.get.maxCoolingCapacity
      vals << cap.get if cap.is_initialized
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxAirFlowRate
    vals = []
    if normalModeStage1CoilPerformance.is_initialized
      cap = normalModeStage1CoilPerformance.get.maxAirFlowRate
      vals << cap.get if cap.is_initialized
    end
    if normalModeStage1Plus2CoilPerformance.is_initialized
      cap = normalModeStage1Plus2CoilPerformance.get.maxAirFlowRate
      vals << cap.get if cap.is_initialized
    end
    if dehumidificationMode1Stage1CoilPerformance.is_initialized
      cap = dehumidificationMode1Stage1CoilPerformance.get.maxAirFlowRate
      vals << cap.get if cap.is_initialized
    end
    if dehumidificationMode1Stage1Plus2CoilPerformance.is_initialized
      cap = dehumidificationMode1Stage1Plus2CoilPerformance.get.maxAirFlowRate
      vals << cap.get if cap.is_initialized
    end
    if vals.size.zero?
      OpenStudio::OptionalDouble.new
    else
      OpenStudio::OptionalDouble.new(vals.max)
    end
  end

  def maxCoolingCapacityAutosized
    if normalModeStage1CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif normalModeStage1Plus2CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif dehumidelsificationMode1Stage1CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif dehumidelsificationMode1Stage1Plus2CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end

  def maxAirFlowRateAutosized
    if normalModeStage1CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif normalModeStage1Plus2CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif dehumidelsificationMode1Stage1CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    elsif dehumidelsificationMode1Stage1Plus2CoilPerformance.is_initialized
      return OpenStudio::OptionalBool.new(false)
      vals << cap.get elsif cap.is_initialized
                        return OpenStudio::OptionalBool.new(false)
    else
      return OpenStudio::OptionalBool.new(true)
    end
  end
end
