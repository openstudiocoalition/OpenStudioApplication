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

#include "../SpacesSurfacesGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>

#include <QComboBox>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, SpacesSurfacesGridView) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();
  auto surfaces = model.getConcreteModelObjects<model::Surface>();

  EXPECT_EQ(4u, spaces.size());
  EXPECT_EQ(1u, spaceTypes.size());
  EXPECT_EQ(24u, surfaces.size());

  auto gridView = std::make_shared<SpacesSurfacesGridView>(false, model);
  auto gridController = getGridController(gridView.get());
  auto objectSelector = getObjectSelector(gridController);

  processEvents();

  auto selectableObjects = objectSelector->selectableObjects();
  EXPECT_EQ(surfaces.size(), selectableObjects.size());

  auto selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjects.size());

  // Headers on row = 0
  unsigned gridRow = 0;
  unsigned col = 0;
  unsigned i = 0;

  // Row = 0, Col = 0 is Space Name header, No SubRow
  gridRow = 0;
  col = 0;

  auto obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  EXPECT_FALSE(obj);

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, 0);
  EXPECT_FALSE(obj);

  // Row = 0, Col = 1 is All header, No SubRow
  gridRow = 0;
  col = 1;

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  EXPECT_FALSE(obj);

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, 0);
  EXPECT_FALSE(obj);

  // First row of objects on row = 1

  // Row = 1, Col = 0 is Space, No SubRow
  gridRow = 1;
  col = 0;

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  ASSERT_TRUE(obj);
  ASSERT_TRUE(obj->optionalCast<model::Space>());
  auto spaceObj = obj->cast<model::Space>();

  // Row = 1, Col = 1 is Surface Select Check Boxes, Has SubRow
  gridRow = 1;
  col = 1;

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  EXPECT_FALSE(obj);

  i = 0;
  for (const auto& surface : spaceObj.surfaces()) {
    obj = objectSelector->getObject(gridRow - 1, gridRow, col, i);
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->optionalCast<model::Surface>());
    auto surfaceObj = obj->cast<model::Surface>();
    ASSERT_TRUE(surfaceObj.space());
    EXPECT_EQ(spaceObj, surfaceObj.space());
    EXPECT_EQ(surface.handle(), surfaceObj.handle());

    i += 1;
  }

  // Row = 1, Col = 1 is Surface Name Line Edits, Has SubRow
  gridRow = 1;
  col = 2;

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  EXPECT_FALSE(obj);

  i = 0;
  for (const auto& surface: spaceObj.surfaces()) {
    obj = objectSelector->getObject(gridRow - 1, gridRow, col, i);
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->optionalCast<model::Surface>());
    auto surfaceObj = obj->cast<model::Surface>();
    ASSERT_TRUE(surfaceObj.space());
    EXPECT_EQ(spaceObj, surfaceObj.space());
    EXPECT_EQ(surface.handle(), surfaceObj.handle());

    i += 1;
  }

  // selection
  objectSelector->selectAll();

  processEvents();

  selectableObjects = objectSelector->selectableObjects();
  EXPECT_EQ(surfaces.size(), selectableObjects.size());

  selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(surfaces.size(), selectedObjects.size());

  processEvents();

  EXPECT_EQ("All", gridView->m_surfaceTypeFilter->currentText());
  gridView->m_surfaceTypeFilter->setCurrentText("Wall");
  EXPECT_EQ("Wall", gridView->m_surfaceTypeFilter->currentText());

  processEvents();

  selectableObjects = objectSelector->selectableObjects();
  EXPECT_EQ(16u, selectableObjects.size());

  selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(16u, selectedObjects.size());

  i = 0;
  for (const auto& surface: spaceObj.surfaces()) {
    obj = objectSelector->getObject(gridRow - 1, gridRow, col, i);
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->optionalCast<model::Surface>());
    auto surfaceObj = obj->cast<model::Surface>();
    ASSERT_TRUE(surfaceObj.space());
    EXPECT_EQ(spaceObj, surfaceObj.space());
    EXPECT_EQ(surface.handle(), surfaceObj.handle());

    i += 1;
  }
}