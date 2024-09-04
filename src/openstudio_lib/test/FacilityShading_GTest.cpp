/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../FacilityShadingGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ShadingSurfaceGroup.hpp>
#include <openstudio/model/ShadingSurfaceGroup_Impl.hpp>
#include <openstudio/model/ShadingSurface.hpp>

#include <memory>
#include <algorithm>
#include <utilities/core/Compare.hpp>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, FacilityShadingGridView) {

  model::Model model = model::exampleModel();
  auto shadingGroups = model.getConcreteModelObjects<model::ShadingSurfaceGroup>();
  std::sort(shadingGroups.begin(), shadingGroups.end(), WorkspaceObjectNameLess());

  // includes one space shading group (not shown on this tab), and one site and one building which are seen
  ASSERT_EQ(3u, shadingGroups.size());

  auto it = std::find_if(shadingGroups.begin(), shadingGroups.end(),
                         [](const model::ShadingSurfaceGroup& s) { return openstudio::istringEqual("Space", s.shadingSurfaceType()); });
  ASSERT_NE(it, shadingGroups.end());

  it = std::find_if(shadingGroups.begin(), shadingGroups.end(),
                    [](const model::ShadingSurfaceGroup& s) { return openstudio::istringEqual("Building", s.shadingSurfaceType()); });
  ASSERT_NE(it, shadingGroups.end());
  auto shadingGroup1 = *it;

  auto gridView = std::make_shared<FacilityShadingGridView>(false, false, model);
  auto* gridController = getGridController(gridView.get());
  auto* objectSelector = getObjectSelector(gridController);

  processEvents();

  auto modelObjects = gridController->modelObjects();
  ASSERT_EQ(2u, modelObjects.size());
  EXPECT_EQ(shadingGroup1.handle(), modelObjects[0].handle());

  auto selectableObjectsSet = objectSelector->selectableObjects();
  EXPECT_EQ(shadingGroups.size() - 1, selectableObjectsSet.size());  // Excludes the "Space" one

  auto selectedObjectsSet = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjectsSet.size());

  // What you select is the shading surface, not the group
  objectSelector->setObjectSelected(shadingGroup1.shadingSurfaces().front(), true);

  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(shadingGroup1.shadingSurfaces().front().handle(), selectedObjectsSet.begin()->handle());
}
