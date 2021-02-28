/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2020, OpenStudio Coalition and other contributors. All rights reserved.
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
#include "../shared_gui_components/OSGridController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>

using namespace openstudio;

class TestGridController : public OSGridController
{

 public:
  TestGridController() 
  : OSGridController()
  {}

  TestGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                     const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects)
  {}

  virtual ~TestGridController() {}

  virtual void refreshModelObjects() override {}

  virtual void onItemDropped(const OSItemId& itemId) override {}

 protected:

  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override {}

  virtual QString getColor(const model::ModelObject& modelObject) override {return "";}

};

TEST_F(OpenStudioLibFixture, OptionalAssumptions) {
  model::Model model;
  model::Space space1(model);
  model::Space space2(model);

  model::Space space1_copy = space1;

  EXPECT_EQ(2u, model.getConcreteModelObjects<model::Space>().size());

  boost::optional<model::Space> optSpace1;
  boost::optional<model::Space> optSpace2;

  EXPECT_TRUE(optSpace1 == optSpace1);
  EXPECT_TRUE(optSpace1 == optSpace2);
  EXPECT_TRUE(optSpace1 != space1);

  optSpace1 = space1;
  optSpace2 = space1_copy;

  EXPECT_TRUE(optSpace1 == optSpace1);
  EXPECT_TRUE(optSpace1 == optSpace2);
  EXPECT_TRUE(optSpace1 == space1);
  EXPECT_TRUE(optSpace1 == space1_copy);
  
  optSpace1 = space1;
  optSpace2 = space2;

  EXPECT_TRUE(optSpace1 == optSpace1);
  EXPECT_TRUE(optSpace1 != optSpace2);
}

TEST_F(OpenStudioLibFixture, ObjectSelector) {

  TestGridController grid;
  ObjectSelector selector(&grid);  
}


TEST_F(OpenStudioLibFixture, SpacesSurfacesGridView) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();
  auto surfaces = model.getConcreteModelObjects<model::Surface>();

  EXPECT_EQ(4u, spaces.size());
  EXPECT_EQ(1u, spaceTypes.size());
  EXPECT_EQ(24u, surfaces.size());

  SpacesSurfacesGridView gridView(false, model);
  auto gridController = getGridController(gridView);
  auto objectSelector = gridController->getObjectSelector();
  
  processEvents();

  auto selectableObjects = objectSelector->selectableObjects();
  EXPECT_EQ(surfaces.size(), selectableObjects.size());

  auto selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(0u, selectedObjects.size());

  for (const auto &surface: surfaces) {
    EXPECT_TRUE(objectSelector->containsObject(surface));
  }
  for (const auto &space: spaces) {
    EXPECT_TRUE(objectSelector->containsObject(space));
  }

  // Headers
  auto objRow0Col0 = objectSelector->getObject(0, 0, boost::none);
  EXPECT_FALSE(objRow0Col0);

  auto objRow0Col1 = objectSelector->getObject(0, 1, boost::none);
  EXPECT_FALSE(objRow0Col1);

  auto widRow0Col0 = objectSelector->getWidget(0, 0, boost::none);
  EXPECT_TRUE(widRow0Col0);

  auto widRow0Col1 = objectSelector->getWidget(0, 1, boost::none);
  EXPECT_TRUE(widRow0Col1);

  // First row
  auto objRow1Col0 = objectSelector->getObject(1, 0, boost::none);
  ASSERT_TRUE(objRow1Col0);
  ASSERT_TRUE(objRow1Col0->optionalCast<model::Space>());
  auto spaceRow1 = objRow1Col0->cast<model::Space>();

  auto objRow1Col1 = objectSelector->getObject(1, 1, boost::none);
  EXPECT_FALSE(objRow1Col1);

  auto widRow1Col0 = objectSelector->getWidget(1, 0, boost::none);
  EXPECT_TRUE(widRow1Col0);

  auto widRow1Col1 = objectSelector->getWidget(1, 1, boost::none);
  EXPECT_FALSE(widRow1Col1);

  unsigned i = 0;
  for (const auto& surface : spaceRow1.surfaces()) {
    auto objRow1Col1Sub = objectSelector->getObject(1, 1, i);
    ASSERT_TRUE(objRow1Col1Sub);
    ASSERT_TRUE(objRow1Col1Sub->optionalCast<model::Surface>());
    auto surfaceRow1 = objRow1Col1Sub->cast<model::Surface>();
    ASSERT_TRUE(surfaceRow1.space());
    EXPECT_EQ(spaceRow1, surfaceRow1.space());

    auto widRow1Col1Sub0 = objectSelector->getWidget(1, 1, i);
    EXPECT_TRUE(widRow1Col1Sub0);

    i += 1;
  }

  objectSelector->selectAllVisible();

  processEvents();

  selectableObjects = objectSelector->selectableObjects();
  EXPECT_EQ(surfaces.size(), selectableObjects.size());

  selectedObjects = objectSelector->selectedObjects();
  EXPECT_EQ(surfaces.size(), selectedObjects.size());

  processEvents();
  /*
  std::vector<model::ModelObject> selectedObjects() const;
  std::vector<QString> categories();

  std::vector<std::pair<QString, std::vector<QString>>> categoriesAndFields();

  virtual void categorySelected(int index);

  virtual int rowCount() const;

  virtual int columnCount() const;



  QWidget* cell(int rowIndex, int columnIndex);

  model::ModelObject modelObject(int rowIndex);

  virtual std::vector<QWidget*> row(int rowIndex);

  void selectRow(int rowIndex, bool select);

  int rowIndexFromModelIndex(int modelIndex);

  // Return a new widget at a "top level" row and column specified by arguments.
  // There might be sub rows within the specified location.
  // In that case a QWidget with sub rows (inner grid layout) will be returned.
  QWidget* widgetAt(int row, int column);

  // Call this function on a model update
  virtual void refreshModelObjects() = 0;

  void connectToModel();

  void disconnectFromModel();

  std::shared_ptr<ObjectSelector> getObjectSelector() const {
    return m_objectSelector;
  }

  IddObjectType m_iddObjectType;

  std::vector<model::ModelObject> m_modelObjects;

  std::vector<model::ModelObject> m_inheritedModelObjects;

  model::Model& model() {
    return m_model;
  }

  OSGridView* gridView();
*/
}