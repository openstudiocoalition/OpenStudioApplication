/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../DesignDayGridView.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSGridView.hpp"
#include "../../shared_gui_components/OSLineEdit.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/DesignDay.hpp>
#include <openstudio/model/DesignDay_Impl.hpp>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, DesignDayGridView) {

  model::Model model = model::exampleModel();
  auto designDays = model.getConcreteModelObjects<model::DesignDay>();
  std::sort(designDays.begin(), designDays.end(), WorkspaceObjectNameLess());

  ASSERT_EQ(2u, designDays.size());
  auto designDay1 = designDays[0];
  auto designDay2 = designDays[1];

  auto ddGridView = std::make_shared<DesignDayGridView>(false, model);
  auto* gridController = getGridController(ddGridView.get());
  auto* gridView = getGridView(ddGridView.get());
  auto* objectSelector = getObjectSelector(gridController);

  int numCategories = 6;
  int numRows = 3;
  int numColumns = 6;
  EXPECT_EQ(numCategories, gridController->categories().size());
  EXPECT_EQ(numRows, gridController->rowCount());
  EXPECT_EQ(numColumns, gridController->columnCount());

  processEvents();

  auto modelObjects = gridController->modelObjects();
  ASSERT_EQ(2u, modelObjects.size());
  EXPECT_EQ(designDay1.handle(), modelObjects[0].handle());

  auto selectableObjectsSet = objectSelector->selectableObjects();
  EXPECT_EQ(designDays.size(), selectableObjectsSet.size());

  auto selectedObjectsSet = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjectsSet.size());

  processEvents();

  checkExpected(objectSelector, gridView, -1, 0, 0, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 1, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 2, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 3, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 4, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 5, boost::none, boost::none, true, true, false, false, false, "");

  checkExpected(objectSelector, gridView, 0, 1, 0, boost::none, designDay1, true, true, false, false, false, "000");   // Name
  checkExpected(objectSelector, gridView, 0, 1, 1, boost::none, designDay1, true, true, false, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 0, 1, 2, boost::none, designDay1, true, true, false, false, false, "0000");  // Day of Month
  checkExpected(objectSelector, gridView, 0, 1, 3, boost::none, designDay1, true, true, false, false, false, "0000");  // Month
  checkExpected(objectSelector, gridView, 0, 1, 4, boost::none, designDay1, true, true, false, false, false, "0000");  // Day Type
  checkExpected(objectSelector, gridView, 0, 1, 5, boost::none, designDay1, true, true, false, false, false, "000");   // DST

  checkExpected(objectSelector, gridView, 1, 2, 0, boost::none, designDay2, true, true, false, false, false, "000");   // Name
  checkExpected(objectSelector, gridView, 1, 2, 1, boost::none, designDay2, true, true, false, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 1, 2, 2, boost::none, designDay2, true, true, false, false, false, "0000");  // Day of Month
  checkExpected(objectSelector, gridView, 1, 2, 3, boost::none, designDay2, true, true, false, false, false, "0000");  // Month
  checkExpected(objectSelector, gridView, 1, 2, 4, boost::none, designDay2, true, true, false, false, false, "0000");  // Day Type
  checkExpected(objectSelector, gridView, 1, 2, 5, boost::none, designDay2, true, true, false, false, false, "000");   // DST

  objectSelector->setObjectSelected(designDay1, true);
  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(designDay1.handle(), selectedObjectsSet.begin()->handle());

  processEvents();

  checkExpected(objectSelector, gridView, -1, 0, 0, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 1, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 2, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 3, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 4, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 5, boost::none, boost::none, true, true, false, false, false, "");

  checkExpected(objectSelector, gridView, 0, 1, 0, boost::none, designDay1, true, true, true, false, false, "000");   // Name
  checkExpected(objectSelector, gridView, 0, 1, 1, boost::none, designDay1, true, true, true, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 0, 1, 2, boost::none, designDay1, true, true, true, false, false, "0000");  // Day of Month
  checkExpected(objectSelector, gridView, 0, 1, 3, boost::none, designDay1, true, true, true, false, false, "0000");  // Month
  checkExpected(objectSelector, gridView, 0, 1, 4, boost::none, designDay1, true, true, true, false, false, "0000");  // Day Type
  checkExpected(objectSelector, gridView, 0, 1, 5, boost::none, designDay1, true, true, true, false, false, "000");   // DST

  checkExpected(objectSelector, gridView, 1, 2, 0, boost::none, designDay2, true, true, false, false, false, "000");   // Name
  checkExpected(objectSelector, gridView, 1, 2, 1, boost::none, designDay2, true, true, false, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 1, 2, 2, boost::none, designDay2, true, true, false, false, false, "0000");  // Day of Month
  checkExpected(objectSelector, gridView, 1, 2, 3, boost::none, designDay2, true, true, false, false, false, "0000");  // Month
  checkExpected(objectSelector, gridView, 1, 2, 4, boost::none, designDay2, true, true, false, false, false, "0000");  // Day Type
  checkExpected(objectSelector, gridView, 1, 2, 5, boost::none, designDay2, true, true, false, false, false, "000");   // DST

  designDay1.remove();
  selectedObjectsSet = objectSelector->selectedObjects();
  ASSERT_EQ(1u, selectedObjectsSet.size());
  EXPECT_EQ(designDay1.handle(), selectedObjectsSet.begin()->handle());

  processEvents();

  checkExpected(objectSelector, gridView, -1, 0, 0, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 1, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 2, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 3, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 4, boost::none, boost::none, true, true, false, false, false, "");
  checkExpected(objectSelector, gridView, -1, 0, 5, boost::none, boost::none, true, true, false, false, false, "");

  checkExpected(objectSelector, gridView, 0, 1, 0, boost::none, designDay1, true, true, true, false, false, "100");   // Name
  checkExpected(objectSelector, gridView, 0, 1, 1, boost::none, designDay1, true, true, true, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 0, 1, 2, boost::none, designDay1, true, true, true, false, false, "1000");  // Day of Month
  checkExpected(objectSelector, gridView, 0, 1, 3, boost::none, designDay1, true, true, true, false, false, "1000");  // Month
  checkExpected(objectSelector, gridView, 0, 1, 4, boost::none, designDay1, true, true, true, false, false, "1000");  // Day Type
  checkExpected(objectSelector, gridView, 0, 1, 5, boost::none, designDay1, true, true, true, false, false, "000");   // DST

  checkExpected(objectSelector, gridView, 1, 2, 0, boost::none, designDay2, true, true, false, false, false, "000");   // Name
  checkExpected(objectSelector, gridView, 1, 2, 1, boost::none, designDay2, true, true, false, true, false, "000");    // Selector
  checkExpected(objectSelector, gridView, 1, 2, 2, boost::none, designDay2, true, true, false, false, false, "0000");  // Day of Month
  checkExpected(objectSelector, gridView, 1, 2, 3, boost::none, designDay2, true, true, false, false, false, "0000");  // Month
  checkExpected(objectSelector, gridView, 1, 2, 4, boost::none, designDay2, true, true, false, false, false, "0000");  // Day Type
  checkExpected(objectSelector, gridView, 1, 2, 5, boost::none, designDay2, true, true, false, false, false, "000");   // DST
}