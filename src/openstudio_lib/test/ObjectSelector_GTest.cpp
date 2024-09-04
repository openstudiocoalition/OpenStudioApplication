/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>

#include <memory>

using namespace openstudio;

class TestGridController : public OSGridController
{

 public:
  TestGridController() : OSGridController() {}

  TestGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                     const std::vector<model::ModelObject>& modelObjects)
    : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {}

  virtual ~TestGridController() = default;

  virtual void refreshModelObjects() override {}

  virtual void onItemDropped(const OSItemId& itemId) override {}

 protected:
  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override {}

  virtual QString getColor(const model::ModelObject& modelObject) override {
    return "";
  }
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

TEST_F(OpenStudioLibFixture, OSObjectSelector) {

  TestGridController grid;
  OSObjectSelector selector(&grid);
}
