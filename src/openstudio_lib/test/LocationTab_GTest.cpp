/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../LocationTabView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/DesignDay.hpp>
#include <openstudio/model/DesignDay_Impl.hpp>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, SortableDesignDay) {

  model::Model model = model::exampleModel();
  auto designDays = model.getConcreteModelObjects<model::DesignDay>();
  std::sort(designDays.begin(), designDays.end(), WorkspaceObjectNameLess());

  ASSERT_EQ(2u, designDays.size());
  auto dd = designDays[0];

  EXPECT_EQ("Sizing Period Design Day 1", dd.nameString());
  SortableDesignDay sdd(dd);
  EXPECT_EQ("", sdd.type());
  EXPECT_EQ(0, sdd.permil());
  EXPECT_EQ(0, sdd.sortablePermil());

  dd.setName("El Paso International Ap Ut Ann Clg .4% Condns DB = > MWB");
  sdd = SortableDesignDay(dd);
  EXPECT_EQ("Cooling", sdd.type());
  EXPECT_EQ(4, sdd.permil());
  EXPECT_EQ(4, sdd.sortablePermil());

  dd.setName("El Paso International Ap Ut Ann Clg .4 % Condns DB = > MWB");
  sdd = SortableDesignDay(dd);
  EXPECT_EQ("Cooling", sdd.type());
  EXPECT_EQ(4, sdd.permil());
  EXPECT_EQ(4, sdd.sortablePermil());

  dd.setName("Buffalo Niagara Intl Ap Ann Htg 99.6% Condns DB");
  sdd = SortableDesignDay(dd);
  EXPECT_EQ("Heating", sdd.type());
  EXPECT_EQ(996, sdd.permil());
  EXPECT_EQ(4, sdd.sortablePermil());

}