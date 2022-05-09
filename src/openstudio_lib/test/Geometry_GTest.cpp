/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Building.hpp>
#include <openstudio/model/Building_Impl.hpp>
#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/Facility.hpp>
#include <openstudio/model/Facility_Impl.hpp>
#include <openstudio/model/FloorplanJSForwardTranslator.hpp>
#include <openstudio/model/ModelMerger.hpp>
#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/DefaultScheduleSet_Impl.hpp>
#include <openstudio/model/Site.hpp>
#include <openstudio/model/Site_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/ThreeJSReverseTranslator.hpp>
#include <openstudio/model/ThreeJSForwardTranslator.hpp>

#include <openstudio/utilities/core/Filesystem.hpp>
#include <openstudio/utilities/geometry/FloorplanJS.hpp>
#include <openstudio/utilities/geometry/ThreeJS.hpp>

#include "../../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <boost/algorithm/string/predicate.hpp>

#include <memory>
#include <algorithm>

using namespace openstudio;

// these methods duplicate code from FloorspaceEditor in GeometryEditorView.cpp

boost::optional<FloorplanJS> loadFloorPlanJS(const openstudio::path& floorplanPath) {

  openstudio::filesystem::ifstream ifs(floorplanPath);
  OS_ASSERT(ifs.is_open());
  std::string contents((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  ifs.close();
  boost::optional<FloorplanJS> floorplan = FloorplanJS::load(contents);
  OS_ASSERT(floorplan);

  return floorplan;
}

boost::optional<FloorplanJS> updateFloorplan(boost::optional<FloorplanJS>& floorplan, const openstudio::model::Model& model) {

  boost::optional<model::Building> building = model.getOptionalUniqueModelObject<model::Building>();

  // update with current model content
  // at this point you may have removed objects in the app, so tell updateFloorplanJS to remove missing objects
  model::FloorplanJSForwardTranslator ft;
  floorplan = ft.updateFloorplanJS(*floorplan, model, true);

  // update north axis from model, floorspace js northAxis is opposite of EnergyPlus's
  if (building) {
    floorplan->setNorthAxis(-building->northAxis());
  }

  return floorplan;
}

boost::optional<model::Model> getExportModel(const boost::optional<FloorplanJS>& floorplan, model::Model& model,
                                             std::map<UUID, UUID>& exportModelHandleMapping) {
  OS_ASSERT(floorplan);

  ThreeScene scene = floorplan->toThreeScene(true);

  model::ThreeJSReverseTranslator rt;
  boost::optional<model::Model> exportModel = rt.modelFromThreeJS(scene);

  if (exportModel) {

    std::string originalBuildingName = model.getUniqueModelObject<model::Building>().nameString();
    std::string originalSiteName = model.getUniqueModelObject<model::Site>().nameString();
    boost::optional<model::DefaultConstructionSet> originalDefaultConstructionSet =
      model.getUniqueModelObject<model::Building>().defaultConstructionSet();

    // set north axis, floorspace js northAxis is opposite of EnergyPlus's
    model::Building building = exportModel->getUniqueModelObject<model::Building>();
    building.setName(originalBuildingName);
    building.setNorthAxis(-floorplan->northAxis());

    // synchronize latitude and longitude, floorspace does not set elevation when locating on map
    model::Site site = exportModel->getUniqueModelObject<model::Site>();
    site.setName(originalSiteName);
    double latitude = floorplan->latitude();
    double longitude = floorplan->longitude();
    if ((latitude != 0) && (longitude != 0)) {
      site.setLatitude(latitude);
      site.setLongitude(latitude);
    }

    exportModelHandleMapping = rt.handleMapping();

    // User cannot edit thermal zone for plenums in floorspace, a new thermal zone will always be created for each plenum
    // if the associated space has a thermal zone in FloorplanJS::toThreeScene.  This was probably a bad decision, since
    // the user cannot see or edit this zone we should not create it.  Created floorspace.js/issues/388 to better attach
    // information related to plenum zones

    // the exported plenum space is not matched to an existing space, so existing space will be replaced by exported space
    // we can throw away the new plenum zone, it will not be mapped to anything
    bool removedPlenumZones = false;
    for (const auto& exportSpace : exportModel->getConcreteModelObjects<model::Space>()) {
      if (boost::algorithm::ends_with(exportSpace.nameString(), "Plenum")) {
        boost::optional<model::ThermalZone> exportThermalZone = exportSpace.thermalZone();
        if (exportThermalZone && (exportThermalZone->spaces().size() == 1u)) {
          exportThermalZone->remove();
          removedPlenumZones = true;
        }
      }
    }

    if (removedPlenumZones) {
      // attempt to match thermal zones for existing plenums with new plenums
      for (const auto& space : model.getConcreteModelObjects<model::Space>()) {
        boost::optional<model::Space> abovePlenumSpace;
        boost::optional<model::Space> belowPlenumSpace;
        for (const auto& surface : space.surfaces()) {
          if (istringEqual(surface.surfaceType(), "RoofCeiling")) {
            auto adjacentSurface = surface.adjacentSurface();
            if (adjacentSurface) {
              abovePlenumSpace = adjacentSurface->space();
              if (abovePlenumSpace && !abovePlenumSpace->isPlenum() && !boost::algorithm::icontains(abovePlenumSpace->nameString(), "Plenum")) {
                abovePlenumSpace.reset();
              }
            }
          } else if (istringEqual(surface.surfaceType(), "Floor")) {
            auto adjacentSurface = surface.adjacentSurface();
            if (adjacentSurface) {
              belowPlenumSpace = adjacentSurface->space();
              if (belowPlenumSpace && !belowPlenumSpace->isPlenum() && !boost::algorithm::icontains(belowPlenumSpace->nameString(), "Plenum")) {
                belowPlenumSpace.reset();
              }
            }
          }
        }

        boost::optional<model::Space> exportSpace;
        boost::optional<model::Space> exportAbovePlenumSpace;
        boost::optional<model::Space> exportBelowPlenumSpace;
        auto exportSpaceHandle = exportModelHandleMapping.find(space.handle());
        if (exportSpaceHandle != exportModelHandleMapping.end()) {
          exportSpace = exportModel->getModelObject<model::Space>(exportSpaceHandle->second);
          if (exportSpace) {
            for (const auto& surface : exportSpace->surfaces()) {
              if (istringEqual(surface.surfaceType(), "RoofCeiling")) {
                auto adjacentSurface = surface.adjacentSurface();
                if (adjacentSurface) {
                  exportAbovePlenumSpace = adjacentSurface->space();
                  if (exportAbovePlenumSpace && !boost::algorithm::ends_with(exportAbovePlenumSpace->nameString(), "Plenum")) {
                    exportAbovePlenumSpace.reset();
                  }
                }
              } else if (istringEqual(surface.surfaceType(), "Floor")) {
                auto adjacentSurface = surface.adjacentSurface();
                if (adjacentSurface) {
                  exportBelowPlenumSpace = adjacentSurface->space();
                  if (exportBelowPlenumSpace && !boost::algorithm::ends_with(exportBelowPlenumSpace->nameString(), "Plenum")) {
                    exportBelowPlenumSpace.reset();
                  }
                }
              }
            }
          }
        }

        if (abovePlenumSpace && exportAbovePlenumSpace) {
          boost::optional<model::ThermalZone> thermalZone = abovePlenumSpace->thermalZone();
          if (thermalZone) {
            auto exportThermalZoneHandle = exportModelHandleMapping.find(thermalZone->handle());
            if (exportThermalZoneHandle != exportModelHandleMapping.end()) {
              boost::optional<model::ThermalZone> exportThermalZone =
                exportModel->getModelObject<model::ThermalZone>(exportThermalZoneHandle->second);
              if (exportThermalZone) {
                exportAbovePlenumSpace->setThermalZone(*exportThermalZone);
              }
            }
          }
        }

        if (belowPlenumSpace && exportBelowPlenumSpace) {
          boost::optional<model::ThermalZone> thermalZone = belowPlenumSpace->thermalZone();
          if (thermalZone) {
            auto exportThermalZoneHandle = exportModelHandleMapping.find(thermalZone->handle());
            if (exportThermalZoneHandle != exportModelHandleMapping.end()) {
              boost::optional<model::ThermalZone> exportThermalZone =
                exportModel->getModelObject<model::ThermalZone>(exportThermalZoneHandle->second);
              if (exportThermalZone) {
                exportBelowPlenumSpace->setThermalZone(*exportThermalZone);
              }
            }
          }
        }
      }
    }

    // manually add mappings between current model and new model for Site, Facility, and Building objects
    exportModelHandleMapping[model.getUniqueModelObject<model::Site>().handle()] = exportModel->getUniqueModelObject<model::Site>().handle();
    exportModelHandleMapping[model.getUniqueModelObject<model::Facility>().handle()] = exportModel->getUniqueModelObject<model::Facility>().handle();
    exportModelHandleMapping[model.getUniqueModelObject<model::Building>().handle()] = exportModel->getUniqueModelObject<model::Building>().handle();

    // restore properties on unique objects
    if (originalDefaultConstructionSet) {
      auto it = exportModelHandleMapping.find(originalDefaultConstructionSet->handle());
      if (it != exportModelHandleMapping.end()) {
        boost::optional<model::DefaultConstructionSet> exportDefaultConstructionSet =
          exportModel->getModelObject<model::DefaultConstructionSet>(it->second);
        if (exportDefaultConstructionSet) {
          building.setDefaultConstructionSet(*exportDefaultConstructionSet);
        }
      }
    }
  }

  return exportModel;
}

TEST_F(OpenStudioLibFixture, Geometry_SingleSpace_DefaultConstructionSet) {

  openstudio::path floorplanPath =
    getOpenStudioApplicationSourceDirectory() / openstudio::toPath("src/openstudio_lib/test/floorplans/single_space.json");
  ASSERT_TRUE(exists(floorplanPath));

  boost::optional<FloorplanJS> floorplan = loadFloorPlanJS(floorplanPath);
  ASSERT_TRUE(floorplan);

  model::Model model;
  model::Building building = model.getUniqueModelObject<model::Building>();
  model::Building site = model.getUniqueModelObject<model::Building>();

  model::DefaultConstructionSet defaultConstructionSet(model);
  model::SpaceType spaceType(model);
  model::DefaultScheduleSet defaultScheduleSet(model);

  building.setDefaultConstructionSet(defaultConstructionSet);
  building.setSpaceType(spaceType);
  building.setDefaultScheduleSet(defaultScheduleSet);

  std::string originalBuildingName = building.nameString();
  std::string originalSiteName = site.nameString();

  floorplan = updateFloorplan(floorplan, model);

  std::map<UUID, UUID> exportModelHandleMapping;
  boost::optional<model::Model> exportModel = getExportModel(floorplan, model, exportModelHandleMapping);
  ASSERT_TRUE(exportModel);

  model::ModelMerger mm;
  mm.mergeModels(model, *exportModel, exportModelHandleMapping);

  EXPECT_EQ(building.nameString(), originalBuildingName);
  EXPECT_EQ(site.nameString(), originalSiteName);

  auto defaultConstructionSets = model.getConcreteModelObjects<model::DefaultConstructionSet>();
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();
  auto defaultScheduleSets = model.getConcreteModelObjects<model::DefaultScheduleSet>();

  EXPECT_EQ(defaultConstructionSets.size(), 1u);
  EXPECT_EQ(spaceTypes.size(), 1u);
  EXPECT_EQ(defaultScheduleSets.size(), 1u);

  ASSERT_TRUE(building.defaultConstructionSet());
  EXPECT_EQ(building.defaultConstructionSet()->handle(), defaultConstructionSet.handle());
  ASSERT_TRUE(building.spaceType());
  EXPECT_EQ(building.spaceType()->handle(), spaceType.handle());
  ASSERT_TRUE(building.defaultScheduleSet());
  EXPECT_EQ(building.defaultScheduleSet()->handle(), defaultScheduleSet.handle());

  floorplan = updateFloorplan(floorplan, model);
}

TEST_F(OpenStudioLibFixture, Geometry_SingleSpace_Plenum) {

  openstudio::path floorplanPath =
    getOpenStudioApplicationSourceDirectory() / openstudio::toPath("src/openstudio_lib/test/floorplans/single_space_plenum.json");
  ASSERT_TRUE(exists(floorplanPath));

  boost::optional<FloorplanJS> floorplan = loadFloorPlanJS(floorplanPath);
  ASSERT_TRUE(floorplan);

  model::Model model;

  floorplan = updateFloorplan(floorplan, model);

  std::map<UUID, UUID> exportModelHandleMapping;
  boost::optional<model::Model> exportModel = getExportModel(floorplan, model, exportModelHandleMapping);
  ASSERT_TRUE(exportModel);

  model::ModelMerger mm;
  mm.mergeModels(model, *exportModel, exportModelHandleMapping);

  auto spaces = model.getConcreteModelObjects<model::Space>();
  auto zones = model.getConcreteModelObjects<model::ThermalZone>();

  ASSERT_EQ(spaces.size(), 2u);
  ASSERT_EQ(zones.size(), 0u);

  for (auto& space : spaces) {
    EXPECT_FALSE(space.thermalZone());
    model::ThermalZone zone(model);
    EXPECT_TRUE(space.setThermalZone(zone));
  }

  floorplan = updateFloorplan(floorplan, model);

  exportModelHandleMapping.clear();
  exportModel = getExportModel(floorplan, model, exportModelHandleMapping);
  ASSERT_TRUE(exportModel);

  mm.mergeModels(model, *exportModel, exportModelHandleMapping);

  spaces = model.getConcreteModelObjects<model::Space>();
  zones = model.getConcreteModelObjects<model::ThermalZone>();

  ASSERT_EQ(spaces.size(), 2u);
  ASSERT_EQ(zones.size(), 2u);
  for (auto& space : spaces) {
    EXPECT_TRUE(space.thermalZone());
  }
}
