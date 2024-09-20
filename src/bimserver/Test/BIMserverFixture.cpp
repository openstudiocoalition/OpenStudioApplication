/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BIMserverFixture.hpp"

//#include <resources.hxx>

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

void BIMserverFixture::SetUp() {}

void BIMserverFixture::TearDown() {}

void BIMserverFixture::SetUpTestCase() {
  // set up logging
  openstudio::Logger::instance().standardOutLogger().disable();
  logFile = std::shared_ptr<openstudio::FileLogSink>(new openstudio::FileLogSink(openstudio::toPath("./BIMserverFixture.log")));
}

void BIMserverFixture::TearDownTestCase() {
  logFile->disable();
}

std::shared_ptr<openstudio::FileLogSink> BIMserverFixture::logFile;
