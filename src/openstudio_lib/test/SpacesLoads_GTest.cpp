/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../SpacesLoadsGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/ElectricEquipment.hpp>

#include <memory>
#include <algorithm>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, SpacesLoadsGridView) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), WorkspaceObjectNameLess());
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();

  ASSERT_EQ(4u, spaces.size());
  ASSERT_EQ(1u, spaceTypes.size());

  auto it = std::find_if(spaces.begin(), spaces.end(), [](const auto& s) { return s.nameString() == "Space 4"; });
  ASSERT_NE(it, spaces.end());
  auto space4 = *it;
  ASSERT_EQ(1u, space4.electricEquipment().size());

  auto gridView = std::make_shared<SpacesLoadsGridView>(false, false, model);
  auto* gridController = getGridController(gridView.get());
  auto* objectSelector = getObjectSelector(gridController);

  processEvents();

  auto modelObjects = gridController->modelObjects();
  ASSERT_EQ(4u, modelObjects.size());
  EXPECT_EQ(space4.handle(), modelObjects[3].handle());

  // There is only one hard-assigned load (the printer) that can be selected, the rest is inherited
  auto selectableObjectsSet = objectSelector->selectableObjects();
  EXPECT_EQ(1u, selectableObjectsSet.size());

  auto selectedObjectsSet = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjectsSet.size());

  objectSelector->setObjectSelected(space4.electricEquipment().front(), true);

  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(space4.electricEquipment().front().handle(), selectedObjectsSet.begin()->handle());
}
