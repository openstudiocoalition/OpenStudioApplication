/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "../OpenStudioApplicationPathHelpers.hpp"

using namespace openstudio;

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(OpenStudioApplicationPathHelpers, Strings) {
  path applicationPath = getOpenStudioApplicationPath();
  path applicationDir = getOpenStudioApplicationDirectory();
  path openstudioModule = getOpenStudioApplicationModule();
  path openstudioModuleDirectory = getOpenStudioApplicationModuleDirectory();
  path openstudioCLI = getOpenStudioCoreCLI();
  //path energyplusDirectory = getEnergyPlusDirectory();
  // path energyplusExecutable = getEnergyPlusExecutable();

  EXPECT_TRUE(exists(applicationPath));
  EXPECT_TRUE(exists(applicationDir));
  EXPECT_TRUE(exists(openstudioModule));
  EXPECT_TRUE(exists(openstudioModuleDirectory));
  EXPECT_TRUE(exists(openstudioCLI));
  //EXPECT_TRUE(exists(energyplusDirectory));
  // EXPECT_TRUE(exists(energyplusExecutable));
}
