/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelEditorFixture.hpp"

#include "../../model_editor/Application.hpp"

#include <openstudio/utilities/core/Path.hpp>

#include <QTimer>

int main(int argc, char* argv[]) {
  Q_INIT_RESOURCE(modeleditorlib);
  auto app = openstudio::Application::instance().application(true);

  QTimer::singleShot(0, [&]() {
    ::testing::InitGoogleTest(&argc, argv);
    auto testResult = RUN_ALL_TESTS();
    app->exit(testResult);
  });

  return app->exec();
}

void ModelEditorFixture::SetUp() {}

void ModelEditorFixture::TearDown() {}

void ModelEditorFixture::SetUpTestCase() {
  // set up logging
  logFile = openstudio::FileLogSink(openstudio::toPath("./ModelEditorFixture.log"));
  logFile->setLogLevel(Debug);
  openstudio::Logger::instance().standardOutLogger().disable();
}

void ModelEditorFixture::TearDownTestCase() {}

// static variables
boost::optional<openstudio::FileLogSink> ModelEditorFixture::logFile;
