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