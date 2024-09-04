/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OpenStudioAppFixture.hpp"

#include "../../model_editor/Application.hpp"
#include <openstudio/utilities/core/Path.hpp>

#include <QTimer>

int main(int argc, char* argv[]) {
  auto app = openstudio::Application::instance().application(false);

  QTimer::singleShot(0, [&]() {
    ::testing::InitGoogleTest(&argc, argv);
    auto testResult = RUN_ALL_TESTS();
    app->exit(testResult);
  });

  return app->exec();
}

void OpenStudioAppFixture::SetUp() {}

void OpenStudioAppFixture::TearDown() {}

void OpenStudioAppFixture::SetUpTestCase() {
  // set up logging
  logFile = openstudio::FileLogSink(openstudio::toPath("./OpenStudioAppFixture.log"));
  logFile->setLogLevel(Debug);
  openstudio::Logger::instance().standardOutLogger().disable();
}

void OpenStudioAppFixture::TearDownTestCase() {}

// static variables
boost::optional<openstudio::FileLogSink> OpenStudioAppFixture::logFile;
