/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../SpacesSpacesGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, SpacesSpacesGridView) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), WorkspaceObjectNameLess());
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();

  ASSERT_EQ(4u, spaces.size());
  ASSERT_EQ(1u, spaceTypes.size());

  auto space1 = spaces[0];

  auto gridView = std::make_shared<SpacesSpacesGridView>(false, false, model);
  auto* gridController = getGridController(gridView.get());
  auto* objectSelector = getObjectSelector(gridController);

  processEvents();

  auto modelObjects = gridController->modelObjects();
  ASSERT_EQ(4u, modelObjects.size());
  EXPECT_EQ(space1.handle(), modelObjects[0].handle());

  auto selectableObjectsSet = objectSelector->selectableObjects();
  EXPECT_EQ(spaces.size(), selectableObjectsSet.size());

  auto selectedObjectsSet = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjectsSet.size());

  objectSelector->setObjectSelected(space1, true);

  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(space1.handle(), selectedObjectsSet.begin()->handle());
}
