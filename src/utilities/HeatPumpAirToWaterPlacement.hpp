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
enum class HeatPumpAirToWaterMode
{
  Heating,
  Cooling
};

/// The mode suggested by the loop's Sizing:Plant loop type ("Heating" or "Cooling"), or none for
/// any other loop type. Only a suggestion: a new PlantLoop defaults to "Heating" until its sizing
/// is edited, so an untouched chilled water loop also reports "Heating".
boost::optional<HeatPumpAirToWaterMode> suggestedHeatPumpAirToWaterMode(const model::PlantLoop& plantLoop);

/// Places the wrapper's `mode` operation mode on `target`, which is either a plant loop supply-side
/// Node or a plant loop supply-side Splitter (adding a new supply branch). Returns false, leaving the
/// model unchanged, if the wrapper has no such mode or the mode cannot be added at `target`.
bool addHeatPumpAirToWaterModeToLoop(const model::HeatPumpAirToWater& heatPump, HeatPumpAirToWaterMode mode, const model::HVACComponent& target);

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
