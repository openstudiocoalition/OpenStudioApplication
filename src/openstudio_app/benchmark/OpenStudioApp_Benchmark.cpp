#include "OpenStudioApp_Benchmark.hpp"
#include "../../model_editor/Application.hpp"
#include "../OpenStudioApp.hpp"
#include <QtWidgets>
#include <qtest.h>


//TestBenchmark::TestBenchmark()
    //: QObject()
//{
//}

void TestBenchmark::init() {
  int argc = 0;
  char ** argv;
  mApp = new openstudio::OpenStudioApp(argc, argv);
  openstudio::Application::instance().setApplication(mApp);
}

void TestBenchmark::simple()
{
    QString str1 = QLatin1String("This is a test string");
    QString str2 = QLatin1String("This is a test string");

    QCOMPARE(str1.localeAwareCompare(str2), 0);

    QBENCHMARK {
        str1.localeAwareCompare(str2);
    }
}

QTEST_MAIN(TestBenchmark)
