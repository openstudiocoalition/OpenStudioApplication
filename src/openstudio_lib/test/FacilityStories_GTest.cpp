/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
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

  auto gridView = std::make_shared<FacilityStoriesGridView>(false, m);
  auto gridController = getGridController(gridView.get());
  auto objectSelector = getObjectSelector(gridController);

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

  gridController->setObjectFilter([](const model::ModelObject& obj) -> bool { return true; });

  for (const auto& locationInfoPair : locationToInfoMap) {
    if (locationInfoPair.first->modelRow >= 0) {
      EXPECT_TRUE(locationInfoPair.second->isVisible());
    }
  }
}
