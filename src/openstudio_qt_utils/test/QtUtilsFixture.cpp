/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "QtUtilsFixture.hpp"
#include "../Application.hpp"

#include <QTimer>

int main(int argc, char* argv[]) {
  auto app = openstudio::Application::instance().application(true);

  QTimer::singleShot(0, [&]() {
    ::testing::InitGoogleTest(&argc, argv);
    auto testResult = RUN_ALL_TESTS();
    app->exit(testResult);
  });

  return app->exec();
}

void QtUtilsFixture::SetUp() {}
void QtUtilsFixture::TearDown() {}
void QtUtilsFixture::SetUpTestCase() {}
void QtUtilsFixture::TearDownTestCase() {}

boost::optional<openstudio::FileLogSink> QtUtilsFixture::logFile;
