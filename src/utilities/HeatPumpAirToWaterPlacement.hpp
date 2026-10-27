/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OSAPP_UTILITIES_HEATPUMPAIRTOWATERPLACEMENT_HPP
#define OSAPP_UTILITIES_HEATPUMPAIRTOWATERPLACEMENT_HPP

#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HeatPumpAirToWater.hpp>
#include <openstudio/model/PlantLoop.hpp>

#include <boost/optional.hpp>

namespace openstudio {

/// HeatPumpAirToWater is a wrapper that cannot itself be placed on a loop (its addToNode() always
/// returns false): its heating and cooling operation modes are the StraightComponents that sit on
/// the hot water and chilled water loops, and the wrapper is only forward translated through them.
/// Dropping a wrapper onto a loop therefore means placing one of its modes there instead.
enum class HeatPumpAirToWaterType
{
  Wrapper,
  Heating,
  Cooling
};

/// Which of the three HeatPumpAirToWater object types `modelObject` is, or none for anything else.
/// Only compares the IddObjectType, so it is cheap enough to call on every object of a loop.
boost::optional<HeatPumpAirToWaterType> heatPumpAirToWaterType(const model::ModelObject& modelObject);

/// The mode (Heating or Cooling) suggested by the loop's Sizing:Plant loop type, or none for any other
/// loop type. Only a suggestion: a new PlantLoop defaults to "Heating" until its sizing is edited, so an
/// untouched chilled water loop also reports "Heating".
boost::optional<HeatPumpAirToWaterType> suggestedHeatPumpAirToWaterMode(const model::PlantLoop& plantLoop);

/// Places the wrapper's `mode` (Heating or Cooling) operation mode on `target`, which is either a plant
/// loop supply-side Node or a plant loop supply-side Splitter (adding a new supply branch). Returns false,
/// leaving the model unchanged, if `mode` is Wrapper, the wrapper has no such mode, or the mode cannot be
/// added at `target`.
bool addHeatPumpAirToWaterModeToLoop(const model::HeatPumpAirToWater& heatPump, HeatPumpAirToWaterType mode, const model::HVACComponent& target);

/// If `modelObject` is a HeatPumpAirToWater heating or cooling operation mode that belongs to a wrapper,
/// returns that wrapper; otherwise none.
boost::optional<model::HeatPumpAirToWater> heatPumpAirToWaterForMode(const model::ModelObject& modelObject);

/// A mode owned by a wrapper can't be removed on its own (isRemovable() is false), so deleting it from a loop
/// disconnects it instead, leaving the wrapper and its other mode intact. Once neither mode is on a loop, the
/// wrapper is removed together with both modes and their speed data, since the GUI has no other way to delete it.
/// Returns true if `modelObject` is such a mode; false for anything else, which should be removed as usual.
bool removeHeatPumpAirToWaterModeFromLoop(const model::ModelObject& modelObject);

}  // namespace openstudio

#endif  // OSAPP_UTILITIES_HEATPUMPAIRTOWATERPLACEMENT_HPP
