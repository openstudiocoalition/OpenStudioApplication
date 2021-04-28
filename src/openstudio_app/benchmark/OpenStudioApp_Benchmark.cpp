#include "OpenStudioApp_Benchmark.hpp"

#include "../../model_editor/Application.hpp"
#include "../OpenStudioApp.hpp"
#include "../../openstudio_lib/OSDocument.hpp"
#include "../../openstudio_lib/SpacesSurfacesGridView.hpp"
#include "../../shared_gui_components/WaitDialog.hpp"

#include <QtWidgets>
#include <QtTest/QtTest>

//TestBenchmark::TestBenchmark()
    //: QObject()
//{
//}
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>

using namespace openstudio;

void OpenStudioAppBenchmark::init() {
  //char arg0[] = "programName";
  //char* argv[] = { &arg0[0], NULL};
  //int argc = sizeof(argv) / sizeof(argv[0]) - 1;
  //mApp = new openstudio::OpenStudioApp(argc, argv);
  //openstudio::Application::instance().setApplication(mApp);
  //openstudio::Application::instance().processEvents();


  //openstudio::Application::instance().application(true);
}

void OpenStudioAppBenchmark::simple()
{

    model::Model model = model::exampleModel();
    QVERIFY2(model.getConcreteModelObjects<model::Surface>().size() > 20, "Expected exampleModel to have more than 20 surfaces");

    auto gridView = std::make_shared<SpacesSurfacesGridView>(false, model);

    char arg0[] = "programName";
    char* argv[] = { &arg0[0], NULL};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    openstudio::OpenStudioApp app(argc, argv);
    openstudio::Application::instance().setApplication(&app);

    // app.exec();

    app.waitDialog()->setVisible(true);
    app.processEvents();

    int startSubTabIndex = 2; // Surfaces
    QBENCHMARK {
      app.m_osDocument = std::shared_ptr<OSDocument>(new OSDocument(app.componentLibrary(), app.resourcesPath(), model, "example_model.osm", false, OSDocument::VerticalTabID::SPACES, startSubTabIndex));

      app.connectOSDocumentSignals();
    }

    app.waitDialog()->setVisible(false);

    QString str1 = QLatin1String("This is a test string");
    QString str2 = QLatin1String("This is a test string");

    QCOMPARE(str1.localeAwareCompare(str2), 0);

    QBENCHMARK {
        str1.localeAwareCompare(str2);
    }
}

QTEST_MAIN(OpenStudioAppBenchmark)
