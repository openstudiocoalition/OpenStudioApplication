/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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

  auto gridView = std::make_shared<SpacesSurfacesGridView>(false, false, model);
  auto* gridController = getGridController(gridView.get());
  auto* objectSelector = getObjectSelector(gridController);

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
    EXPECT_EQ(spaceObj, surfaceObj.space().get());

    i += 1;
  }

  // Row = 1, Col = 1 is Surface Name Line Edits, Has SubRow
  gridRow = 1;
  col = 2;

  obj = objectSelector->getObject(gridRow - 1, gridRow, col, boost::none);
  EXPECT_FALSE(obj);

  i = 0;
  for (const auto& surface : spaceObj.surfaces()) {
    obj = objectSelector->getObject(gridRow - 1, gridRow, col, i);
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->optionalCast<model::Surface>());
    auto surfaceObj = obj->cast<model::Surface>();
    ASSERT_TRUE(surfaceObj.space());
    EXPECT_EQ(spaceObj, surfaceObj.space().get());

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
  EXPECT_EQ(16U, selectableObjects.size());

  selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(16U, selectedObjects.size());

  i = 0;
  for (const auto& surface : spaceObj.surfaces()) {
    obj = objectSelector->getObject(gridRow - 1, gridRow, col, i);
    ASSERT_TRUE(obj);
    ASSERT_TRUE(obj->optionalCast<model::Surface>());
    auto surfaceObj = obj->cast<model::Surface>();
    ASSERT_TRUE(surfaceObj.space());
    EXPECT_EQ(spaceObj, surfaceObj.space().get());

    i += 1;
  }
}
