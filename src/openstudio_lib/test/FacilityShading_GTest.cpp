/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

  auto gridView = std::make_shared<FacilityShadingGridView>(false, model);
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
