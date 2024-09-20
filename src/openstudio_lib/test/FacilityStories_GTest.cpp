/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../FacilityStoriesGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/BuildingStory.hpp>
#include <openstudio/model/BuildingStory_Impl.hpp>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, FacilityStoriesGridView) {

  model::Model m = model::exampleModel();
  model::BuildingStory s1(m);
  model::BuildingStory s2(m);

  auto stories = m.getConcreteModelObjects<model::BuildingStory>();
  std::sort(stories.begin(), stories.end(), WorkspaceObjectNameLess());

  ASSERT_EQ(3u, stories.size());

  auto story1 = stories[0];
  story1.setNominalZCoordinate(0);
  auto story2 = stories[1];
  story2.setNominalZCoordinate(3);
  auto story3 = stories[2];
  story3.setNominalZCoordinate(6);

  auto gridView = std::make_shared<FacilityStoriesGridView>(false, false, m);
  auto* gridController = getGridController(gridView.get());
  auto* objectSelector = getObjectSelector(gridController);

  processEvents();

  auto modelObjects = gridController->modelObjects();
  ASSERT_EQ(3u, modelObjects.size());
  EXPECT_EQ(story1.handle(), modelObjects[0].handle());
  EXPECT_EQ(0, modelObjects[0].cast<model::BuildingStory>().nominalZCoordinate().get());
  EXPECT_EQ(story2.handle(), modelObjects[1].handle());
  EXPECT_EQ(3, modelObjects[1].cast<model::BuildingStory>().nominalZCoordinate().get());
  EXPECT_EQ(story3.handle(), modelObjects[2].handle());
  EXPECT_EQ(6, modelObjects[2].cast<model::BuildingStory>().nominalZCoordinate().get());

  auto selectableObjectsSet = objectSelector->selectableObjects();
  EXPECT_EQ(stories.size(), selectableObjectsSet.size());

  auto selectedObjectsSet = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjectsSet.size());

  objectSelector->setObjectSelected(story1, true);

  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(story1.handle(), selectedObjectsSet.begin()->handle());

  auto locationToInfoMap = getGridCellLocationToInfoMap(objectSelector);
  for (const auto& locationInfoPair : locationToInfoMap) {
    if (locationInfoPair.first->modelRow >= 0) {
      EXPECT_TRUE(locationInfoPair.second->isVisible());
    }
  }

  double lowerLimit = 2;
  double upperLimit = 4;
  gridController->setObjectFilter([upperLimit, lowerLimit](const model::ModelObject& obj) -> bool {
    boost::optional<model::BuildingStory> story = obj.optionalCast<model::BuildingStory>();
    if (story) {
      auto nominalZCoordinate = story->nominalZCoordinate();
      if (nominalZCoordinate) {
        if (*nominalZCoordinate >= upperLimit || *nominalZCoordinate <= lowerLimit) {
          return false;
        }
      }
    }
    return true;
  });

  for (const auto& locationInfoPair : locationToInfoMap) {
    if (locationInfoPair.first->modelRow == 0) {
      EXPECT_FALSE(locationInfoPair.second->isVisible());
    } else if (locationInfoPair.first->modelRow == 1) {
      EXPECT_TRUE(locationInfoPair.second->isVisible());
    } else if (locationInfoPair.first->modelRow == 2) {
      EXPECT_FALSE(locationInfoPair.second->isVisible());
    }
  }

  gridController->setObjectFilter([upperLimit, lowerLimit](const model::ModelObject& obj) -> bool { return true; });

  for (const auto& locationInfoPair : locationToInfoMap) {
    if (locationInfoPair.first->modelRow >= 0) {
      EXPECT_TRUE(locationInfoPair.second->isVisible());
    }
  }
}
