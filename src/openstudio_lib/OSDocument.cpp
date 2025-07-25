/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSDocument.hpp"

#include "ApplyMeasureNowDialog.hpp"
#include "ConstructionsTabController.hpp"
#include "GeometryTabController.hpp"
#include "FacilityTabController.hpp"
#include "HorizontalTabWidget.hpp"
#include "HVACSystemsTabController.hpp"
#include "InspectorController.hpp"
#include "InspectorView.hpp"
#include "LibraryTabWidget.hpp"
#include "LoadsTabController.hpp"
#include "LocationTabController.hpp"
#include "LocationTabView.hpp"
#include "MainRightColumnController.hpp"
#include "MainWindow.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectTypeListView.hpp"
#include "OSAppBase.hpp"
#include "ResultsTabController.hpp"
#include "ResultsTabView.hpp"
#include "RunTabController.hpp"
#include "RunTabView.hpp"
#include "SchedulesTabController.hpp"
#include "ScriptsTabController.hpp"
#include "ScriptsTabView.hpp"
#include "SimSettingsTabController.hpp"
#include "SimSettingsTabView.hpp"
#include "SpacesTabController.hpp"
#include "SpaceTypesTabController.hpp"
#include "SpaceTypesView.hpp"
#include "SummaryTabController.hpp"
#include "SummaryTabView.hpp"
#include "ThermalZonesTabController.hpp"
#include "VariablesTabController.hpp"
#include "VerticalTabWidget.hpp"
#include "YearSettingsWidget.hpp"

#include "../shared_gui_components/BuildingComponentDialog.hpp"
#include "../shared_gui_components/LocalLibraryController.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/WaitDialog.hpp"

#include "../model_editor/UserSettings.hpp"
#include "../model_editor/Utilities.hpp"

//#include "../analysis/Analysis.hpp"

#include <openstudio/model/FileOperations.hpp>
#include <openstudio/model/Component.hpp>
#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/model/WeatherFile_Impl.hpp>

#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>
#include <openstudio/utilities/bcl/RemoteBCL.hpp>
#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Checksum.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/data/Attribute.hpp>
#include <openstudio/utilities/idf/IdfFile.hpp>
#include <openstudio/utilities/idf/ValidityReport.hpp>
#include <openstudio/utilities/idf/Workspace.hpp>
#include <openstudio/utilities/filetypes/EpwFile.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON_Impl.hpp>

#include <openstudio/osversion/VersionTranslator.hpp>

//#include "../analysis/DataPoint.hpp"
//#include "../analysis/MeasureGroup.hpp"
//#include "../analysis/MeasureGroup_Impl.hpp"
//#include "../analysis/NullMeasure.hpp"
//#include "../analysis/Problem.hpp"
//#include "../analysis/RubyMeasure.hpp"
//#include "../analysis/RubyMeasure_Impl.hpp"

//#include "../runmanager/lib/WorkItem.hpp"

#include "../model_editor/Application.hpp"

#include <openstudio/OpenStudio.hxx>

#include <openstudio/energyplus/ForwardTranslator.hpp>
#include <openstudio/gbxml/ForwardTranslator.hpp>
#include <openstudio/sdd/ForwardTranslator.hpp>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QIcon>
#include <QInputDialog>
#include <QSettings>
#include <QtGlobal>  // Workaround for #659

#include <algorithm>
#include <memory>

#if (defined(_WIN32) || defined(_WIN64))
#  include <windows.h>
#endif

namespace openstudio {

OSDocument::OSDocument(const openstudio::model::Model& library, const openstudio::path& resourcesPath, openstudio::model::OptionalModel model,
                       QString filePath, bool isPlugin, int startTabIndex, int startSubTabIndex)
  : m_compLibrary(library),
    m_resourcesPath(resourcesPath),
    m_onlineMeasuresBclDialog(nullptr),
    m_onlineBclDialog(nullptr),
    m_localLibraryDialog(nullptr),
    m_savePath(std::move(filePath)),
    m_isPlugin(isPlugin) {

  QFile data(":openstudiolib.qss");

  static QString style;
  if (style.isEmpty()) {
    if (data.open(QFile::ReadOnly)) {
      QTextStream styleIn(&data);
      style = styleIn.readAll();
      data.close();
      qobject_cast<QApplication*>(QApplication::instance())->setStyleSheet(style);
    } else {
      LOG_FREE(LogLevel::Error, "OSDocument", "Failed to open openstudiolib.qss");
    }
  }

  m_mainWindow = new MainWindow(m_isPlugin);
  addQObject(m_mainWindow);

  bool initalizeWorkflow = false;
  if (!model) {
    model = openstudio::model::Model();
    initalizeWorkflow = true;
  }

  openstudio::path modelTempDirPath;
  if (!m_savePath.isEmpty()) {
    auto p = toPath(m_savePath);
    modelTempDirPath = model::initializeModel(*model, p);
    m_mainWindow->setWindowFilePath(m_savePath);
    m_mainWindow->setWindowTitle(m_savePath + "[*]");
  } else {
    modelTempDirPath = model::initializeModel(*model);
    m_mainWindow->setWindowTitle("Untitled[*]");
    initalizeWorkflow = true;
  }
  m_modelTempDir = toQString(modelTempDirPath);

  m_verticalId = 0;
  m_subTabIds = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  // Make sure that the vector is the same size as the number of tabs
  OS_ASSERT(m_subTabIds.size() == static_cast<unsigned>(RESULTS_SUMMARY + 1));

  // set the model, this will create widgets
  setModel(*model, false, false);

  // connect signals to main window
  connect(m_mainWindow, &MainWindow::downloadComponentsClicked, this, &OSDocument::openBclDlg);
  connect(m_mainWindow, &MainWindow::openLibDlgClicked, this, &OSDocument::openLibDlg);
  connect(m_mainWindow, &MainWindow::closeClicked, this, &OSDocument::closeClicked);
  connect(m_mainWindow, &MainWindow::verticalTabSelected, this, &OSDocument::onVerticalTabSelected);
  connect(m_mainWindow, &MainWindow::importClicked, this, &OSDocument::importClicked);
  connect(m_mainWindow, &MainWindow::importgbXMLClicked, this, &OSDocument::importgbXMLClicked);
  connect(m_mainWindow, &MainWindow::importSDDClicked, this, &OSDocument::importSDDClicked);
  connect(m_mainWindow, &MainWindow::importIFCClicked, this, &OSDocument::importIFCClicked);
  connect(m_mainWindow, &MainWindow::loadFileClicked, this, &OSDocument::loadFileClicked);
  connect(m_mainWindow, &MainWindow::changeDefaultLibrariesClicked, this, &OSDocument::changeDefaultLibrariesClicked);
  connect(m_mainWindow, &MainWindow::configureExternalToolsClicked, this, &OSDocument::configureExternalToolsClicked);
  connect(m_mainWindow, &MainWindow::changeLanguageClicked, this, &OSDocument::changeLanguageClicked);
  connect(m_mainWindow, &MainWindow::loadLibraryClicked, this, &OSDocument::loadLibraryClicked);
  connect(m_mainWindow, &MainWindow::loadExampleModelClicked, this, &OSDocument::loadExampleModelClicked);
  connect(m_mainWindow, &MainWindow::loadShoeboxModelClicked, this, &OSDocument::loadShoeboxModelClicked);
  connect(m_mainWindow, &MainWindow::newClicked, this, &OSDocument::newClicked);
  connect(m_mainWindow, &MainWindow::exitClicked, this, &OSDocument::exitClicked);
  connect(m_mainWindow, &MainWindow::helpClicked, this, &OSDocument::helpClicked);
  connect(m_mainWindow, &MainWindow::checkForUpdateClicked, this, &OSDocument::checkForUpdateClicked);
  connect(m_mainWindow, &MainWindow::debugWebglClicked, this, &OSDocument::debugWebglClicked);
  connect(m_mainWindow, &MainWindow::aboutClicked, this, &OSDocument::aboutClicked);
  connect(m_mainWindow, &MainWindow::osmDropped, this, &OSDocument::osmDropped);
  connect(m_mainWindow, &MainWindow::exportClicked, this, &OSDocument::exportIdf);
  connect(m_mainWindow, &MainWindow::exportgbXMLClicked, this, &OSDocument::exportgbXML);
  connect(m_mainWindow, &MainWindow::exportSDDClicked, this, &OSDocument::exportSDD);
  connect(m_mainWindow, &MainWindow::saveAsFileClicked, this, &OSDocument::saveAs);
  connect(m_mainWindow, &MainWindow::saveFileClicked, this, &OSDocument::save);
  // Using old-style connect here to avoid including OpenStudioApp files
  auto isConnected = connect(m_mainWindow, SIGNAL(revertFileClicked()), OSAppBase::instance(), SLOT(revertToSaved()));
  OS_ASSERT(isConnected);
  connect(m_mainWindow, &MainWindow::scanForToolsClicked, this, &OSDocument::scanForTools);
  connect(m_mainWindow, &MainWindow::showRunManagerPreferencesClicked, this, &OSDocument::showRunManagerPreferences);
  connect(m_mainWindow, &MainWindow::toggleUnitsClicked, this, &OSDocument::toggleUnitsClicked);
  connect(m_mainWindow, &MainWindow::toggleDisplayAdditionalPropsClicked, this, &OSDocument::toggleDisplayAdditionalPropsClicked);
  connect(m_mainWindow, &MainWindow::applyMeasureClicked, this, &OSDocument::openMeasuresDlg);
  connect(m_mainWindow, &MainWindow::downloadMeasuresClicked, this, &OSDocument::openMeasuresBclDlg);
  connect(m_mainWindow, &MainWindow::changeMyMeasuresDir, this, &OSDocument::openChangeMeasuresDirDlg);
  connect(m_mainWindow, &MainWindow::changeBclLogin, this, &OSDocument::changeBclLogin);
  connect(this, &OSDocument::downloadComponentsClicked, this, &OSDocument::openBclDlg);
  connect(this, &OSDocument::openLibDlgClicked, this, &OSDocument::openLibDlg);

  // update window path after the dialog is shown
  QTimer::singleShot(0, this, &OSDocument::updateWindowFilePath);

  if (initalizeWorkflow) {
    QTimer::singleShot(0, this, &OSDocument::addStandardMeasures);
  }

  if (startTabIndex != m_verticalId) {
    QTimer::singleShot(0, [=, this] {
      this->onVerticalTabSelected(startTabIndex);
      this->updateSubTabSelected(startSubTabIndex);
    });
  }
}

//void OSDocument::showRubyConsole()
//{
//  if (!m_consoleWidget)
//  {
//    m_consoleWidget = boost::shared_ptr<OSConsoleWidget>(new OSConsoleWidget(m_model));
//  }
//
//  m_consoleWidget->show();
//}

OSDocument::~OSDocument() {
  // blockSignals wouldn't work now anyways because of nano signal slot implementation
  // m_model.getImpl<openstudio::model::detail::Model_Impl>()->blockSignals(true);

  disconnect();

  // release the file watchers so can remove model temp dir
  m_mainTabController.reset();
  m_mainRightColumnController.reset();

  model::removeModelTempDir(toPath(m_modelTempDir));
}

void OSDocument::showStartTabAndStartSubTab() {
  m_mainWindow->show();
}

int OSDocument::subTabIndex() const {
  return m_subTabId;
}

void OSDocument::initializeModel() {
  model::initializeModelObjects(m_model);
}

MainWindow* OSDocument::mainWindow() {
  return m_mainWindow;
}

model::Model OSDocument::model() {
  return m_model;
}

void OSDocument::setModel(const model::Model& model, bool modified, bool /*saveCurrentTabs*/) {
  bool wasVisible = m_mainWindow->isVisible();
  m_mainWindow->setVisible(false);
  openstudio::OSAppBase* app = OSAppBase::instance();
  app->waitDialog()->setVisible(true);

  for (int i = 5; i != 0; --i) {
    Application::instance().processEvents();
  }

  m_model = model;

  // convert absolute weather file paths to relative in the model, also copy the epw file to the temp dir
  // remove the weather file object if something goes wrong
  bool weatherFileOk = this->fixWeatherFileInTemp(true);
  if (!weatherFileOk) {
    // weather file in model was reset because the file did not exist or it was not a valid EPW
    modified = true;

    // connect to slot that would show user error dialog
    QTimer::singleShot(0, this, &OSDocument::weatherFileReset);
  }

  m_model.getImpl<model::detail::Model_Impl>()->addWorkspaceObjectPtr.connect<OSAppBase, &OSAppBase::addWorkspaceObjectPtr>(OSAppBase::instance());
  m_model.getImpl<model::detail::Model_Impl>()->removeWorkspaceObjectPtr.connect<OSAppBase, &OSAppBase::removeWorkspaceObjectPtr>(
    OSAppBase::instance());
  m_model.getImpl<model::detail::Model_Impl>()->addWorkspaceObject.connect<OSAppBase, &OSAppBase::addWorkspaceObject>(OSAppBase::instance());
  m_model.getImpl<model::detail::Model_Impl>()->removeWorkspaceObject.connect<OSAppBase, &OSAppBase::removeWorkspaceObject>(OSAppBase::instance());
  m_model.getImpl<model::detail::Model_Impl>()->onChange.connect<OSDocument, &OSDocument::markAsModified>(this);
  m_model.workflowJSON().getImpl<detail::WorkflowJSON_Impl>()->onChange.connect<OSDocument, &OSDocument::markAsModified>(this);

  // Main Right Column

  m_mainRightColumnController = std::make_shared<MainRightColumnController>(m_model, m_resourcesPath);
  connect(this, &OSDocument::toggleUnitsClicked, m_mainRightColumnController.get(), &MainRightColumnController::toggleUnitsClicked);
  connect(this, &OSDocument::toggleDisplayAdditionalPropsClicked, m_mainRightColumnController.get(),
          &MainRightColumnController::toggleDisplayAdditionalPropsClicked);

  m_mainWindow->setMainRightColumnView(m_mainRightColumnController->mainRightColumnView());

  // Main Vertical Tabs

  if (modified) {
    QTimer::singleShot(0, this, &OSDocument::markAsModified);
  } else {
    QTimer::singleShot(0, this, &OSDocument::markAsUnmodified);
  }

  // There are (at least) two situations where setModel
  // is called.
  if (!m_tabButtonsCreated) {
    // New OSDocument, setting model for the first time
    m_tabButtonsCreated = true;
    createTabButtons();
    createTab(m_verticalId);
  } else {
    // Apply Measure Now has happened and we are reloading the model, but the document is not new
    // We could and maybe should just create a new document on apply measure now.
    onVerticalTabSelected(m_verticalId);
  }

  QTimer::singleShot(0, this, &OSDocument::initializeModel);

  app->waitDialog()->setVisible(false);
  m_mainWindow->setVisible(wasVisible);

  QTimer::singleShot(0, this, &OSDocument::showStartTabAndStartSubTab);
}

void OSDocument::weatherFileReset() {
  // TODO: temporary workaround for #659
#ifndef Q_OS_DARWIN
  QMessageBox::warning(mainWindow(), "Missing Weather File",
                       "Invalid weather file object, weather file object has been reset. Please choose another weather file.");
#endif
}

void OSDocument::createTabButtons() {
  // Location
  m_mainWindow->addVerticalTabButton(SITE, "Site", ":/images/on_location_tab.png", ":/images/off_location_tab.png",
                                     ":/images/disabled_location_tab.png");

  // Schedules
  m_mainWindow->addVerticalTabButton(SCHEDULES, "Schedules", ":/images/on_schedules_tab.png", ":/images/off_schedules_tab.png",
                                     ":/images/disabled_schedules_tab.png");

  // Constructions
  m_mainWindow->addVerticalTabButton(CONSTRUCTIONS, "Constructions", ":/images/on_constructions_tab.png", ":/images/off_constructions_tab.png",
                                     ":/images/disabled_constructions_tab.png");

  // Loads
  m_mainWindow->addVerticalTabButton(LOADS, "Loads", ":/images/on_loads_tab.png", ":/images/off_loads_tab.png", ":/images/disabled_loads_tab.png");

  // Space Types
  m_mainWindow->addVerticalTabButton(SPACE_TYPES, "Space Types", ":/images/on_space_types_tab.png", ":/images/off_space_types_tab.png",
                                     ":/images/disabled_space_types_tab.png");

  // Geometry
  m_mainWindow->addVerticalTabButton(GEOMETRY, "Geometry", ":/images/on_geometry_tab.png", ":/images/off_geometry_tab.png",
                                     ":/images/disabled_geometry_tab.png");

  // Facility
  m_mainWindow->addVerticalTabButton(FACILITY, "Facility", ":/images/on_building_tab.png", ":/images/off_building_tab.png",
                                     ":/images/disabled_building_tab.png");

  // Spaces
  m_mainWindow->addVerticalTabButton(SPACES, "Spaces", ":/images/on_spaces_tab.png", ":/images/off_spaces_tab.png",
                                     ":/images/disabled_spaces_tab.png");

  // Thermal Zones
  m_mainWindow->addVerticalTabButton(THERMAL_ZONES, "Thermal Zones", ":/images/on_thermal_zone_tab.png", ":/images/off_thermal_zone_tab.png",
                                     ":/images/disabled_thermal_zone_tab.png");

  // HVAC Systems
  m_mainWindow->addVerticalTabButton(HVAC_SYSTEMS, "HVAC Systems", ":/images/on_hvac_tab.png", ":/images/off_hvac_tab.png",
                                     ":/images/disabled_hvac_tab.png");

  //******************************************************************************************************
  //
  //// Summary
  //
  //m_mainWindow->addVerticalTabButton( BUILDING_SUMMARY,
  //                              "Building Summary",
  //                              ":/images/on_summary_tab.png",
  //                              ":/images/off_summary_tab.png" );
  //
  //******************************************************************************************************

  // Variables
  m_mainWindow->addVerticalTabButton(OUTPUT_VARIABLES, "Output Variables", ":/images/on_var_tab.png", ":/images/off_var_tab.png",
                                     ":/images/disabled_var_tab.png");

  // Sim Settings
  m_mainWindow->addVerticalTabButton(SIMULATION_SETTINGS, "Simulation Settings", ":/images/on_sim_settings_tab.png",
                                     ":/images/off_sim_settings_tab.png", ":/images/disabled_sim_settings_tab.png");

  // Scripts
  m_mainWindow->addVerticalTabButton(RUBY_SCRIPTS, "Measures", ":/images/on_scripts_tab.png", ":/images/off_scripts_tab.png",
                                     ":/images/disabled_scripts_tab.png");

  // Run
  m_mainWindow->addVerticalTabButton(RUN_SIMULATION, "Run Simulation", ":/images/on_run_tab.png", ":/images/off_run_tab.png",
                                     ":/images/disabled_run_tab.png");

  // Results
  m_mainWindow->addVerticalTabButton(RESULTS_SUMMARY, "Results Summary", ":/images/on_results_tab.png", ":/images/off_results_tab.png",
                                     ":/images/disabled_results_tab.png");
}

void OSDocument::createTab(int verticalId) {
  m_mainTabController.reset();

  m_verticalId = verticalId;

  bool isIP = m_mainWindow->displayIP();
  bool displayAdditionalProps = m_mainWindow->displayAdditionalProps();

  switch (verticalId) {
    case SITE:
      // Location

      m_mainTabController = std::shared_ptr<MainTabController>(new LocationTabController(isIP, m_model, m_modelTempDir));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), SITE);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForSiteSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &LocationTabController::toggleUnitsClicked);

      break;

    case SCHEDULES:
      // Schedules

      m_mainTabController = std::shared_ptr<MainTabController>(new SchedulesTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), SCHEDULES);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &SchedulesTabController::toggleUnitsClicked);

      connect(m_mainTabController.get(), &SpaceTypesTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController.get(), &SchedulesTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &SchedulesTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForSchedulesSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case CONSTRUCTIONS:
      // Constructions

      m_mainTabController = std::shared_ptr<MainTabController>(new ConstructionsTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), CONSTRUCTIONS);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &ConstructionsTabController::toggleUnitsClicked);

      connect(m_mainTabController.get(), &ConstructionsTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &ConstructionsTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForConstructionsSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case GEOMETRY:
      // Geometry

      m_mainTabController = std::shared_ptr<MainTabController>(new GeometryTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), GEOMETRY);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &GeometryTabController::toggleUnitsClicked);

      connect(m_mainTabController.get(), &GeometryTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &GeometryTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForGeometrySubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case LOADS:
      // Loads

      m_mainTabController = std::shared_ptr<MainTabController>(new LoadsTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), LOADS);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &LoadsTabController::toggleUnitsClicked);

      connect(m_mainTabController.get(), &LoadsTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &LoadsTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForLoadsSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case SPACE_TYPES:
      // Space Types

      m_mainTabController = std::shared_ptr<MainTabController>(new SpaceTypesTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), SPACE_TYPES);

      connect(m_mainTabController.get(), &SpaceTypesTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController.get(), &SpaceTypesTabController::dropZoneItemSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObjectByItem);

      connect(m_mainTabController.get(), &SpaceTypesTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &SpaceTypesTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainRightColumnController.get(), &MainRightColumnController::itemRemoveClicked, m_mainTabController.get(),
              &SpaceTypesTabController::itemRemoveClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForSpaceTypesSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case FACILITY:
      // Facility

      m_mainTabController = std::shared_ptr<MainTabController>(new FacilityTabController(isIP, displayAdditionalProps, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), FACILITY);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &FacilityTabController::toggleUnitsClicked);

      connect(this, &OSDocument::toggleDisplayAdditionalPropsClicked, m_mainTabController.get(),
              &FacilityTabController::toggleDisplayAdditionalPropsClicked);

      connect(m_mainTabController.get(), &FacilityTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController.get(), &FacilityTabController::dropZoneItemSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObjectByItem);

      connect(m_mainTabController.get(), &FacilityTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &FacilityTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainRightColumnController.get(), &MainRightColumnController::itemRemoveClicked, m_mainTabController.get(),
              &FacilityTabController::itemRemoveClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForFacilitySubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case SPACES:
      // Spaces

      m_mainTabController = std::shared_ptr<MainTabController>(new SpacesTabController(isIP, displayAdditionalProps, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), SPACES);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &SpacesTabController::toggleUnitsClicked);

      connect(this, &OSDocument::toggleDisplayAdditionalPropsClicked, m_mainTabController.get(),
              &SpacesTabController::toggleDisplayAdditionalPropsClicked);

      connect(m_mainTabController.get(), &SpacesTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController.get(), &SpacesTabController::dropZoneItemSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObjectByItem);

      connect(m_mainTabController.get(), &SpacesTabController::downloadComponentsClicked, this, &OSDocument::downloadComponentsClicked);

      connect(m_mainTabController.get(), &SpacesTabController::openLibDlgClicked, this, &OSDocument::openLibDlgClicked);

      connect(m_mainRightColumnController.get(), &MainRightColumnController::itemRemoveClicked, m_mainTabController.get(),
              &SpacesTabController::itemRemoveClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForSpacesSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case THERMAL_ZONES:
      // Thermal Zones

      m_mainTabController = std::shared_ptr<MainTabController>(new ThermalZonesTabController(isIP, displayAdditionalProps, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), THERMAL_ZONES);

      connect(m_mainTabController.get(), &ThermalZonesTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController.get(), &ThermalZonesTabController::dropZoneItemSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObjectByItem);

      connect(this, &OSDocument::toggleUnitsClicked, qobject_cast<ThermalZonesTabController*>(m_mainTabController.get()),
              &ThermalZonesTabController::toggleUnitsClicked);

      connect(this, &OSDocument::toggleDisplayAdditionalPropsClicked, m_mainTabController.get(),
              &ThermalZonesTabController::toggleDisplayAdditionalPropsClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForThermalZonesSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case HVAC_SYSTEMS:
      // HVAC Systems

      m_mainTabController = std::shared_ptr<MainTabController>(new HVACSystemsTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), HVAC_SYSTEMS);

      connect(this, &OSDocument::toggleUnitsClicked, qobject_cast<HVACSystemsTabController*>(m_mainTabController.get()),
              &HVACSystemsTabController::toggleUnitsClicked);

      connect(m_mainTabController.get(), &HVACSystemsTabController::modelObjectSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::inspectModelObject);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForHVACSystemsSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case OUTPUT_VARIABLES:
      // Variables

      m_mainTabController = std::shared_ptr<MainTabController>(new VariablesTabController(m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), OUTPUT_VARIABLES);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForOutputVariablesSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case SIMULATION_SETTINGS:
      // Sim Settings

      m_mainTabController = std::shared_ptr<MainTabController>(new SimSettingsTabController(isIP, m_model));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), SIMULATION_SETTINGS);

      connect(this, &OSDocument::toggleUnitsClicked, m_mainTabController.get(), &SimSettingsTabController::toggleUnitsClicked);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForSimulationSettingsSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case RUBY_SCRIPTS:
      // Scripts

      m_mainTabController = std::shared_ptr<MainTabController>(new ScriptsTabController());
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), RUBY_SCRIPTS);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForScriptsSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      //isConnected = QObject::connect(m_scriptsTabController.get(), SIGNAL(downloadComponentsClicked()), this, SIGNAL(downloadComponentsClicked()));
      //OS_ASSERT(isConnected);

      //isConnected = QObject::connect(m_scriptsTabController.get(), SIGNAL(openLibDlgClicked()), this, SIGNAL(openLibDlgClicked()));
      //OS_ASSERT(isConnected);

      break;

    case RUN_SIMULATION:
      // Run

      m_mainTabController = std::shared_ptr<MainTabController>(
        new RunTabController(m_model, openstudio::toPath(m_savePath), openstudio::toPath(m_modelTempDir)));  //, m_simpleProject->runManager()));
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), RUN_SIMULATION);

      connect(qobject_cast<RunTabController*>(m_mainTabController.get()), &RunTabController::toolsUpdated, this, &OSDocument::markAsModified);

      connect(this, &OSDocument::toolsUpdated, this, &OSDocument::markAsModified);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForRunSimulationSubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    case RESULTS_SUMMARY:
      // Results

      m_mainTabController = std::shared_ptr<MainTabController>(new ResultsTabController());
      m_mainWindow->setView(m_mainTabController->mainContentWidget(), RESULTS_SUMMARY);

      connect(this, &OSDocument::toggleUnitsClicked, qobject_cast<ResultsTabController*>(m_mainTabController.get()),
              &ResultsTabController::onUnitSystemChange);

      connect(this, &OSDocument::treeChanged, static_cast<ResultsTabView*>(m_mainTabController->mainContentWidget()), &ResultsTabView::treeChanged);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, m_mainRightColumnController.get(),
              &MainRightColumnController::configureForResultsSummarySubTab);

      connect(m_mainTabController->mainContentWidget(), &MainTabView::tabSelected, this, &OSDocument::updateSubTabSelected);

      break;

    default:
      // Should never get here
      OS_ASSERT(false);
      break;
  }
}

void OSDocument::markAsModified() {
  m_mainWindow->setWindowModified(true);

  if (m_savePath.isEmpty()) {
    return;
  }

  // this is very expensive to do every time any object changes
  //QString fileName = this->mainWindow()->windowFilePath();
  //QFile testFile(fileName);
  //if (!testFile.exists()) return;

  m_mainWindow->enableRevertToSavedAction(true);
}

void OSDocument::markAsUnmodified() {
  m_mainWindow->setWindowModified(false);

  m_mainWindow->enableRevertToSavedAction(false);
}

void OSDocument::disableTabsDuringRun() {
  if (!m_enableTabsAfterRun) {
    return;  // Already in correct state, no work to be done
  } else {
    m_enableTabsAfterRun = false;
  }

  m_mainWindow->verticalTabWidget()->enableTabButton(SITE, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SCHEDULES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(CONSTRUCTIONS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(LOADS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACE_TYPES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(GEOMETRY, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(FACILITY, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(THERMAL_ZONES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(HVAC_SYSTEMS, m_enableTabsAfterRun);
  //m_mainWindow->verticalTabWidget()->enableTabButton(BUILDING_SUMMARY, m_enableTabsAfterRun); No Summary tab available
  m_mainWindow->verticalTabWidget()->enableTabButton(OUTPUT_VARIABLES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SIMULATION_SETTINGS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUBY_SCRIPTS, m_enableTabsAfterRun);
  //m_mainWindow->verticalTabWidget()->enableTabButton(RUN_SIMULATION, m_enableTabsAfterRun); Run tab aways active
  m_mainWindow->verticalTabWidget()->enableTabButton(RESULTS_SUMMARY, m_enableTabsAfterRun);

  m_mainWindow->verticalTabWidget()->refreshTabButtons();
}

void OSDocument::enableTabsAfterRun() {

  if (m_enableTabsAfterRun) {
    return;  // Already in correct state, no work to be done
  } else {
    m_enableTabsAfterRun = true;
  }

  m_mainWindow->verticalTabWidget()->enableTabButton(SITE, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SCHEDULES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(CONSTRUCTIONS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(LOADS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACE_TYPES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(GEOMETRY, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(FACILITY, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(THERMAL_ZONES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(HVAC_SYSTEMS, m_enableTabsAfterRun);
  //m_mainWindow->verticalTabWidget()->enableTabButton(BUILDING_SUMMARY, m_enableTabsAfterRun); No Summary tab available
  m_mainWindow->verticalTabWidget()->enableTabButton(OUTPUT_VARIABLES, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(SIMULATION_SETTINGS, m_enableTabsAfterRun);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUBY_SCRIPTS, m_enableTabsAfterRun);
  //m_mainWindow->verticalTabWidget()->enableTabButton(RUN_SIMULATION, m_enableTabsAfterRun); Run tab aways active
  m_mainWindow->verticalTabWidget()->enableTabButton(RESULTS_SUMMARY, m_enableTabsAfterRun);

  m_mainWindow->verticalTabWidget()->refreshTabButtons();
}

void OSDocument::disable() {
  m_mainWindow->setEnabled(false);

  m_mainWindow->verticalTabWidget()->enableTabButton(SITE, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(SCHEDULES, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(CONSTRUCTIONS, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(LOADS, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACE_TYPES, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(GEOMETRY, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(FACILITY, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACES, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(THERMAL_ZONES, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(HVAC_SYSTEMS, false);
  //m_mainWindow->verticalTabWidget()->enableTabButton(BUILDING_SUMMARY, false); No Summary tab available
  m_mainWindow->verticalTabWidget()->enableTabButton(OUTPUT_VARIABLES, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(SIMULATION_SETTINGS, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUBY_SCRIPTS, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUN_SIMULATION, false);
  m_mainWindow->verticalTabWidget()->enableTabButton(RESULTS_SUMMARY, false);

  m_mainWindow->verticalTabWidget()->refreshTabButtons();
}

void OSDocument::enable() {
  m_mainWindow->setEnabled(true);

  m_mainWindow->verticalTabWidget()->enableTabButton(SITE, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(SCHEDULES, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(CONSTRUCTIONS, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(LOADS, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACE_TYPES, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(GEOMETRY, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(FACILITY, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(SPACES, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(THERMAL_ZONES, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(HVAC_SYSTEMS, true);
  //m_mainWindow->verticalTabWidget()->enableTabButton(BUILDING_SUMMARY, true); No Summary tab available
  m_mainWindow->verticalTabWidget()->enableTabButton(OUTPUT_VARIABLES, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(SIMULATION_SETTINGS, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUBY_SCRIPTS, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(RUN_SIMULATION, true);
  m_mainWindow->verticalTabWidget()->enableTabButton(RESULTS_SUMMARY, true);

  m_mainWindow->verticalTabWidget()->refreshTabButtons();

  m_mainWindow->promptAnalytics();
}

bool OSDocument::modified() const {
  return m_mainWindow->isWindowModified();
}

QString OSDocument::savePath() const {
  return m_savePath;
}

QString OSDocument::modelTempDir() const {
  return m_modelTempDir;
}

void OSDocument::setSavePath(const QString& savePath) {
  m_savePath = savePath;
  updateWindowFilePath();
}

bool OSDocument::fixWeatherFileInTemp(bool opening) {
  LOG(Debug, "OSDocument::fixWeatherFileInTemp: " << opening);

  // look for existing weather file
  boost::optional<model::WeatherFile> weatherFile = m_model.getOptionalUniqueModelObject<model::WeatherFile>();

  // no weather file, nothing to do
  if (!weatherFile) {
    LOG(Debug, "No weather file");
    return true;
  }

  boost::optional<openstudio::path> weatherFilePath = weatherFile->path();

  // no weather file path, nothing to do
  if (!weatherFilePath) {
    LOG(Debug, "No weather file path");
    return true;
  }

  // no temp dir, nothing to do
  if (m_modelTempDir.isEmpty()) {
    LOG(Debug, "No temp dir");
    return true;
  }

  boost::optional<std::string> weatherFileChecksum = weatherFile->checksum();

  openstudio::path tempResourcesDir = toPath(m_modelTempDir) / toPath("resources/");
  openstudio::path tempFilesDir = toPath(m_modelTempDir) / toPath("resources/files/");

  // was the path in weather file object absolute
  bool epwPathAbsolute = false;

  // absolute path to the weather file in the user's directory
  openstudio::path epwInUserPath;
  boost::optional<std::string> epwInUserPathChecksum;

  // absolute path to the weather file in the temp directory
  openstudio::path epwInTempPath;
  boost::optional<std::string> epwInTempPathChecksum;

  // check if weather file path is absolute
  if (weatherFilePath->is_complete()) {

    // absolute weather file path
    epwPathAbsolute = true;

    epwInUserPath = *weatherFilePath;
    boost::system::error_code ec;
    if (boost::filesystem::exists(epwInUserPath, ec)) {
      epwInUserPathChecksum = checksum(epwInUserPath);
    }

    epwInTempPath = tempFilesDir / epwInUserPath.filename();
    if (boost::filesystem::exists(epwInTempPath, ec)) {
      epwInTempPathChecksum = checksum(epwInTempPath);
    }

  } else {

    // relative weather file path
    epwPathAbsolute = false;

    // Look in temp model "resources" and "resources/files"
    epwInTempPath = tempResourcesDir / *weatherFilePath;
    boost::system::error_code ec;
    if (boost::filesystem::exists(epwInTempPath, ec)) {
      epwInTempPathChecksum = checksum(epwInTempPath);
    } else {
      epwInTempPath = tempFilesDir / *weatherFilePath;
      if (boost::filesystem::exists(epwInTempPath, ec)) {
        epwInTempPathChecksum = checksum(epwInTempPath);
      }
    }

    if (!m_savePath.isEmpty()) {
      // We look where the actual model is saved on disk (non temp folder)
      // eg: /path/to/model.osm
      openstudio::path osmPath = toPath(m_savePath);
      // eg: /path/to/model/
      openstudio::path searchCompanionDir = getCompanionFolder(osmPath);
      // eg: /path/to/model/files/
      openstudio::path searchFilesDir = searchCompanionDir / toPath("files");

      // Expected location is companion_folder/files
      epwInUserPath = searchFilesDir / *weatherFilePath;
      if (boost::filesystem::exists(epwInUserPath, ec)) {
        epwInUserPathChecksum = checksum(epwInUserPath);
      } else {
        // Just in case, we look in the companion_folder
        epwInUserPath = searchCompanionDir / *weatherFilePath;
        if (boost::filesystem::exists(epwInUserPath, ec)) {
          epwInUserPathChecksum = checksum(epwInUserPath);
        }
      }
    }
  }

  // check if we need to do the file copy
  bool doCopy = false;
  openstudio::path copySource;
  openstudio::path copyDest;

  if (epwInUserPathChecksum && epwInTempPathChecksum) {

    // file exists in both places
    if (epwInUserPathChecksum.get() == epwInTempPathChecksum.get()) {
      // file is same in both places
      doCopy = false;

    } else {

      if (opening || epwPathAbsolute) {
        // file in user's path should be preferred to what is in temp dir
        doCopy = true;
        copySource = epwInUserPath;
        copyDest = epwInTempPath;
      } else {
        // do not copy from temp to user dir
        doCopy = false;
      }
    }

  } else if (!epwInUserPathChecksum && !epwInTempPathChecksum) {

    LOG(Debug, "Weather file not found, removing");

    // file does not exist anywhere
    weatherFile->remove();

    // not yet listening to model signals
    if (opening) {
      QTimer::singleShot(0, this, &OSDocument::markAsModified);
    }

    return false;

  } else if (!epwInUserPathChecksum) {

    // file does not exist in user path
    doCopy = false;

  } else if (!epwInTempPathChecksum) {

    // file does not exist in temp path but does exist in user dir
    doCopy = true;
    copySource = epwInUserPath;
    copyDest = epwInTempPath;
  }

  bool checksumMismatch = false;
  if (doCopy) {
    OS_ASSERT(epwInUserPathChecksum);
    if (weatherFileChecksum) {
      checksumMismatch = (epwInUserPathChecksum.get() != weatherFileChecksum.get());
    }
  } else {
    OS_ASSERT(epwInTempPathChecksum);
    if (weatherFileChecksum) {
      checksumMismatch = (epwInTempPathChecksum.get() != weatherFileChecksum.get());
    }
  }

  if (!doCopy && !epwPathAbsolute && !checksumMismatch) {
    // no need to copy file or adjust model
    LOG(Debug, "No need to copy weather file");
    return true;
  }

  // duplicating code in LocationView::onWeatherFileBtnClicked

  if (doCopy) {
    try {
      LOG(Debug, "Start copy weather file from " << copySource << " to " << copyDest);
      boost::filesystem::copy_file(copySource, copyDest, boost::filesystem::copy_options::overwrite_existing);
      LOG(Debug, "Copy weather file complete");
    } catch (...) {
      // copy failed
      LOG(Debug, "Copy weather file failed, removing");
      weatherFile->remove();

      // not yet listening to model signals
      if (opening) {
        QTimer::singleShot(0, this, &OSDocument::markAsModified);
      }

      return false;
    }
  }

  try {
    LOG(Debug, "Verifying weather file at " << epwInTempPath);
    EpwFile epwFile(epwInTempPath);

    weatherFile = openstudio::model::WeatherFile::setWeatherFile(m_model, epwFile);
    OS_ASSERT(weatherFile);

    weatherFile->makeUrlRelative(tempResourcesDir);

    // not yet listening to model signals
    if (opening) {
      QTimer::singleShot(0, this, &OSDocument::markAsModified);
    }

    LOG(Debug, "Verifying weather file complete");

  } catch (...) {
    // epw file not valid
    LOG(Debug, "Verifying weather file failed, removing");
    weatherFile->remove();

    // not yet listening to model signals
    if (opening) {
      QTimer::singleShot(0, this, &OSDocument::markAsModified);
    }

    if (doCopy) {
      LOG(Debug, "Removing weather file at " << copyDest);
      boost::system::error_code ec;
      boost::filesystem::remove_all(copyDest, ec);
      LOG(Debug, "Removing weather file complete");
    }

    return false;
  }

  return true;
}

int OSDocument::verticalTabIndex() {
  return m_mainWindow->verticalTabIndex();
}

void OSDocument::onVerticalTabSelected(int verticalId) {
  m_mainTabId = verticalId;

  if (m_mainTabId != RUBY_SCRIPTS && m_mainRightColumnController->isMyModelTabHidden()) {
    m_mainRightColumnController->hideMyModelTab(false);
  }

  createTab(m_mainTabId);

  m_subTabId = m_subTabIds.at(m_mainTabId);

  m_mainWindow->view()->selectSubTabByIndex(m_subTabId);

  bool enableFileImportActions = true;
  bool enablePreferencesActions = true;
  bool enableComponentsMeasuresActions = true;

  switch (m_mainTabId) {
    case SITE:
      m_mainRightColumnController->configureForSiteSubTab(m_subTabId);
      break;
    case SCHEDULES:
      m_mainRightColumnController->configureForSchedulesSubTab(m_subTabId);
      break;
    case CONSTRUCTIONS:
      m_mainRightColumnController->configureForConstructionsSubTab(m_subTabId);
      break;
    case LOADS:
      m_mainRightColumnController->configureForLoadsSubTab(m_subTabId);
      break;
    case SPACE_TYPES:
      m_mainRightColumnController->configureForSpaceTypesSubTab(m_subTabId);
      break;
    case GEOMETRY:
      m_mainRightColumnController->configureForGeometrySubTab(m_subTabId);
      enableFileImportActions = false;
      enablePreferencesActions = false;
      enableComponentsMeasuresActions = false;
      break;
    case FACILITY:
      m_mainRightColumnController->configureForFacilitySubTab(m_subTabId);
      break;
    case SPACES:
      m_mainRightColumnController->configureForSpacesSubTab(m_subTabId);
      break;
    case THERMAL_ZONES:
      m_mainRightColumnController->configureForThermalZonesSubTab(m_subTabId);
      break;
    case HVAC_SYSTEMS:
      m_mainRightColumnController->configureForHVACSystemsSubTab(m_subTabId);
      if (qobject_cast<HVACSystemsTabController*>(m_mainTabController.get())) {
        qobject_cast<HVACSystemsTabController*>(m_mainTabController.get())->clearSceneSelection();
      }
      break;
    case OUTPUT_VARIABLES:
      m_mainRightColumnController->configureForOutputVariablesSubTab(m_subTabId);
      break;
    case SIMULATION_SETTINGS:
      m_mainRightColumnController->configureForSimulationSettingsSubTab(m_subTabId);
      break;
    case RUBY_SCRIPTS:
      // Do special stuff here to hide "My Model" tab
      m_mainRightColumnController->hideMyModelTab(true);
      m_mainRightColumnController->configureForScriptsSubTab(m_subTabId);
      break;
    case RUN_SIMULATION:
      m_mainRightColumnController->configureForRunSimulationSubTab(m_subTabId);
      break;
    case RESULTS_SUMMARY:
      m_mainRightColumnController->configureForResultsSummarySubTab(m_subTabId);
      break;
    default:
      break;
  }

  m_mainWindow->enableFileImportActions(enableFileImportActions);
  m_mainWindow->enablePreferencesActions(enablePreferencesActions);
  m_mainWindow->enableComponentsMeasuresActions(enableComponentsMeasuresActions);

  //m_mainWindow->selectHorizontalTab(LIBRARY);

  //boost::optional<model::ModelObject> mo;

  //m_inspectorController->layoutModelObject(mo);
}

void OSDocument::closeSidebar() {
  m_mainWindow->closeSidebar();
}

void OSDocument::openSidebar() {
  m_mainWindow->openSidebar();
}

void OSDocument::exportIdf() {

  QString defaultDir = savePath().isEmpty() ? mainWindow()->lastPath() : QFileInfo(savePath()).path();

  QString fileName = QFileDialog::getSaveFileName(this->mainWindow(), tr("Export Idf"), defaultDir, tr("(*.idf)"));

  if (!fileName.isEmpty()) {
    model::Model m = this->model();
    energyplus::ForwardTranslator trans;
    Workspace workspace = trans.translateModel(m);
    openstudio::path outDir = toPath(fileName);
    openstudio::filesystem::ofstream ofs(outDir);
    workspace.toIdfFile().print(ofs);
    ofs.close();
  }
}

void OSDocument::exportgbXML() {
  exportFile(GBXML);
}

void OSDocument::exportSDD() {
  exportFile(SDD);
}

void OSDocument::exportFile(fileType type) {

  QString text("Export ");
  if (type == SDD) {
    text.append("SDD");
  } else if (type == GBXML) {
    text.append("gbXML");
  } else {
    // should never get here
    OS_ASSERT(false);
  }

  QString defaultDir = savePath().isEmpty() ? mainWindow()->lastPath() : QFileInfo(savePath()).path();

  QString fileName = QFileDialog::getSaveFileName(this->mainWindow(), tr(text.toStdString().c_str()), defaultDir, tr("(*.xml)"));

  if (!fileName.isEmpty()) {
    model::Model m = this->model();
    openstudio::path outDir = toPath(fileName);

    std::vector<LogMessage> translatorErrors;
    std::vector<LogMessage> translatorWarnings;

    if (type == SDD) {
      sdd::ForwardTranslator trans;
      trans.modelToSDD(m, outDir);
      translatorErrors = trans.errors();
      translatorWarnings = trans.warnings();
    } else if (type == GBXML) {
      gbxml::ForwardTranslator trans;
      trans.modelToGbXML(m, outDir);
      translatorErrors = trans.errors();
      translatorWarnings = trans.warnings();
    }

    bool errorsOrWarnings = false;
    QString log;
    for (const LogMessage& logMessage : translatorErrors) {
      errorsOrWarnings = true;

      log.append(QString::fromStdString(logMessage.logMessage()));
      log.append("\n");
      log.append("\n");
    }

    for (const LogMessage& logMessage : translatorWarnings) {
      errorsOrWarnings = true;

      log.append(QString::fromStdString(logMessage.logMessage()));
      log.append("\n");
      log.append("\n");
    }

    if (errorsOrWarnings) {
      QMessageBox messageBox;
      messageBox.setText("Errors or warnings occurred on export.");
      messageBox.setDetailedText(log);
      messageBox.exec();
    }
  }
}

void OSDocument::addStandardMeasures() {
  disable();

  // needed before we can compute arguments
  OSAppBase::instance()->measureManager().saveTempModel(toPath(m_modelTempDir));

  WorkflowJSON workflow = m_model.workflowJSON();
  std::vector<WorkflowStep> steps;

  // standard report measure
  bool srmAdded = false;
  boost::optional<BCLMeasure> srm = OSAppBase::instance()->measureManager().standardReportMeasure();
  if (srm) {
    try {
      std::pair<bool, std::string> result = OSAppBase::instance()->measureManager().updateMeasure(*srm);
      if (result.first) {
        // have to reload in case measure manager updated
        srm = BCLMeasure::load(srm->directory());
        OS_ASSERT(srm);

        MeasureStep srmStep(result.second);
        // DLM: moved to WorkflowStepResult
        //srmStep.setMeasureId(srm->uid());
        //srmStep.setVersionId(srm->versionId());
        //std::vector<std::string> tags = srm->tags();
        //if (!tags.empty()){
        //  srmStep.setTaxonomy(tags[0]);
        //}
        srmStep.setName(srm->displayName());
        srmStep.setDescription(srm->description());
        srmStep.setModelerDescription(srm->modelerDescription());
        steps.push_back(srmStep);
        srmAdded = true;
      }
    } catch (const std::exception&) {
      QMessageBox::warning(mainWindow(), "Failed to Compute Arguments", "Could not compute arguments for OpenStudio Results Measure.");
      enable();
      return;
    }
  }

  if (!srmAdded) {
    QMessageBox::warning(mainWindow(), "OpenStudio Results Measure Not Found", "Could not find or download OpenStudio Results Measure.");
    enable();
    return;
  }

  workflow.setWorkflowSteps(steps);

  enable();
}

boost::optional<BCLComponent> OSDocument::getLocalComponent(const std::string& uid, const std::string& versionId) const {
  boost::optional<BCLComponent> result;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().getComponent(uid, versionId);
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

boost::optional<BCLMeasure> OSDocument::getLocalMeasure(const std::string& uid, const std::string& versionId) const {
  boost::optional<BCLMeasure> result;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().getMeasure(uid, versionId);
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

std::vector<BCLMeasure> OSDocument::getLocalMeasures() const {
  std::vector<BCLMeasure> result;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().measures();
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

std::vector<BCLComponent> OSDocument::getLocalComponents() const {
  std::vector<BCLComponent> result;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().components();
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

size_t OSDocument::removeOutdatedLocalComponents(const std::string& uid, const std::string& currentVersionId) const {
  size_t result = 0;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().removeOutdatedLocalComponents(uid, currentVersionId);
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

size_t OSDocument::removeOutdatedLocalMeasures(const std::string& uid, const std::string& currentVersionId) const {
  size_t result = 0;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().removeOutdatedLocalMeasures(uid, currentVersionId);
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

std::vector<BCLComponent> OSDocument::componentAttributeSearch(const std::vector<std::pair<std::string, std::string>>& pairs) const {
  std::vector<BCLComponent> result;
  if (m_haveLocalBCL) {
    try {
      result = LocalBCL::instance().componentAttributeSearch(pairs);
    } catch (const std::exception& e) {
      LOG(Error, "Cannot access local BCL: " << e.what());
      m_haveLocalBCL = false;
    }
  }
  return result;
}

bool OSDocument::save() {
  LOG(Debug, "OSDocument::save");
  bool fileSaved = false;

  if (!m_savePath.isEmpty()) {
    // do before saving the osm
    // DLM: should not happen unless user sets an absolute path to epw file in a measure
    fixWeatherFileInTemp(false);

    openstudio::path modelPath = toPath(m_savePath);

    emit modelSaving(modelPath);

    // saves the model to modelTempDir / m_savePath.filename()
    // also copies the temp files to user location
    LOG(Debug, "Saving " << modelPath << " starting");
    bool saved = saveModel(this->model(), modelPath, toPath(m_modelTempDir));
    if (saved) {
      LOG(Debug, "Saving " << modelPath << " complete");
      this->setSavePath(toQString(modelPath));
      this->markAsUnmodified();
      fileSaved = true;
    } else {
      LOG(Debug, "Saving " << modelPath << " failed");
      QMessageBox::warning(this->mainWindow(), tr("Failed to save model"),
                           tr("Failed to save model, make sure that you do not have the location open and that you have correct write access."));
    }

  } else {
    fileSaved = saveAs();
  }

  return fileSaved;
}

void OSDocument::scanForTools() {
  //openstudio::runmanager::RunManager rm;
  //openstudio::runmanager::ConfigOptions co = rm.getConfigOptions();

  //co.findTools(true, false, true, true);
  //rm.setConfigOptions(co);
  //rm.showConfigGui();

  //rm.getConfigOptions().saveQSettings();

  emit toolsUpdated();
}

void OSDocument::showRunManagerPreferences() {
  //openstudio::runmanager::ConfigOptions co(true);

  //openstudio::runmanager::RunManager rm;
  //rm.setConfigOptions(co);
  //rm.showConfigGui();

  //rm.getConfigOptions().saveQSettings();

  emit toolsUpdated();
}

bool OSDocument::saveAs() {
  LOG(Debug, "OSDocument::saveAs");
  bool fileSaved = false;

  // Defaults to the current savePath is there is one (will populate the filename too)
  // Or the lastPath (directory) if not.
  QString defaultDir = savePath().isEmpty() ? mainWindow()->lastPath() : savePath();

  QString filePath = QFileDialog::getSaveFileName(this->mainWindow(), tr("Save"), defaultDir, tr("(*.osm)"));

  if (!filePath.isEmpty()) {
    // remove old model
    if (!m_savePath.isEmpty()) {
      openstudio::path oldModelPath = toPath(m_modelTempDir) / toPath(m_savePath).filename();
      boost::system::error_code ec;
      if (boost::filesystem::exists(oldModelPath, ec)) {
        LOG(Debug, "Removing " << oldModelPath << " starting");
        boost::filesystem::remove(oldModelPath, ec);
        LOG(Debug, "Removing " << oldModelPath << " complete");
      }
    }

    openstudio::path modelPath = toPath(filePath);
    if (getFileExtension(modelPath).empty()) {
      modelPath = setFileExtension(modelPath, modelFileExtension(), false, true);
    }

    emit modelSaving(modelPath);

    // saves the model to modelTempDir / filePath.filename()
    // also copies the temp files to user location
    LOG(Debug, "Saving " << modelPath << " starting");
    bool saved = saveModel(this->model(), modelPath, toPath(m_modelTempDir));
    if (saved) {
      LOG(Debug, "Saving " << modelPath << " complete");
      this->setSavePath(toQString(modelPath));
      this->markAsUnmodified();
      fileSaved = true;
    } else {
      LOG(Debug, "Saving " << modelPath << " failed");
      QMessageBox::warning(this->mainWindow(), tr("Failed to save model"),
                           tr("Failed to save model, make sure that you do not have the location open and that you have correct write access."));
    }
  }

  return fileSaved;
}

openstudio::model::Model OSDocument::componentLibrary() const {
  return m_compLibrary;
}

void OSDocument::setComponentLibrary(const openstudio::model::Model& model) {
  m_compLibrary = model;

  onVerticalTabSelected(m_mainTabId);
}

bool OSDocument::fromModel(const OSItemId& itemId) const {
  return (itemId.sourceId() == modelToSourceId(m_model));
}

bool OSDocument::fromComponentLibrary(const OSItemId& itemId) const {
  return (itemId.sourceId() == modelToSourceId(m_compLibrary));
}

bool OSDocument::fromBCL(const OSItemId& itemId) const {
  return (itemId.sourceId() == OSItemId::BCL_SOURCE_ID);
}

boost::optional<IddObjectType> OSDocument::getIddObjectType(const OSItemId& itemId) const {
  if (fromBCL(itemId)) {
    boost::optional<BCLComponent> component = getLocalComponent(itemId.itemId().toStdString());
    if (component) {
      for (const Attribute& attribute : component->attributes()) {
        if (istringEqual("OpenStudio Type", attribute.name())) {
          try {
            IddObjectType iddObjectType(attribute.valueAsString());
            return iddObjectType;
          } catch (...) {
          }
        }
      }
    }
  }

  boost::optional<model::ModelObject> modelObject = getModelObject(itemId);
  if (modelObject) {
    return modelObject->iddObjectType();
  }

  return boost::none;
}

boost::optional<model::ModelObject> OSDocument::getModelObject(const OSItemId& itemId) const {
  if (fromModel(itemId)) {
    Handle handle(toUUID(itemId.itemId()));
    return m_model.getModelObject<model::ModelObject>(handle);
  }

  else if (fromComponentLibrary(itemId)) {
    if (itemId.sourceId() == modelToSourceId(m_compLibrary)) {
      Handle handle(toUUID(itemId.itemId()));
      return m_compLibrary.getModelObject<model::ModelObject>(handle);
    }
  }
  // TODO: should we handle BCL objects here?

  return boost::none;
}

boost::optional<model::Component> OSDocument::getComponent(const OSItemId& itemId) const {
  boost::optional<model::Component> modelComponent;

  if (itemId.sourceId() == OSItemId::BCL_SOURCE_ID) {
    boost::optional<BCLComponent> component;

    component = getLocalComponent(itemId.itemId().toStdString());

    if (component) {

      std::vector<std::string> oscFiles = component->files("osc");

      if (oscFiles.size() == 1U) {

        openstudio::path oscPath = toPath(oscFiles[0]);

#if (defined(_WIN32) || defined(_WIN64))

        if (oscPath.string().size() > MAX_PATH) {
          if (oscPath.is_complete()) {
            oscPath = toPath("\\\\?\\") / oscPath;
          }
        }

#endif
        //std::error_code ec;
        //OS_ASSERT(openstudio::filesystem::exists(oscPath, ec));

        osversion::VersionTranslator translator;
        //translator.setAllowNewerVersions(false); // DLM: allow to open newer versions?

        modelComponent = translator.loadComponent(oscPath);
      }
    }
  }

  return modelComponent;
}

void OSDocument::toggleUnits(bool displayIP) {}

void OSDocument::openMeasuresDlg() {
  // save model if dirty
  if (this->modified()) {

    auto* messageBox = new QMessageBox(this->mainWindow());
    messageBox->setText("You must save your model before applying a measure.");
    messageBox->setInformativeText("Do you want to save your model now?");
    messageBox->setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    messageBox->setDefaultButton(QMessageBox::Save);
    messageBox->button(QMessageBox::Save)->setShortcut(QKeySequence(Qt::Key_S));
    messageBox->setIcon(QMessageBox::Question);

    int ret = messageBox->exec();

    delete messageBox;

    switch (ret) {
      case QMessageBox::Save:
        // Save was clicked
        if (!this->save()) {
          return;
        }
        break;
      case QMessageBox::Cancel:
        // Cancel was clicked
        return;
      default:
        // should never be reached
        OS_ASSERT(false);
    }
  }

  // save the temp model for the measure manager to use
  OSAppBase::instance()->measureManager().saveTempModel(toPath(m_modelTempDir));

  // open modal dialog
  m_applyMeasureNowDialog = boost::shared_ptr<ApplyMeasureNowDialog>(new ApplyMeasureNowDialog());

  m_applyMeasureNowDialog->exec();

  // DLM: kill the dialog here as there is logic in the destructor that resets application state
  // DLM: this seems overly complicated
  m_applyMeasureNowDialog.reset();
}

void OSDocument::openChangeMeasuresDirDlg() {
  openstudio::path umd = userMeasuresDir();

  QString dirName("");

  if (isNetworkPath(umd) && !isNetworkPathAvailable(umd)) {
    dirName = QFileDialog::getExistingDirectory(this->mainWindow(), tr("Select My Measures Directory"));
  } else {
    dirName = QFileDialog::getExistingDirectory(this->mainWindow(), tr("Select My Measures Directory"), toQString(umd));
  }

  umd = toPath(dirName);

  if (isNetworkPath(umd) && !isNetworkPathAvailable(umd)) {
    QMessageBox::information(this->mainWindow(), "Invalid Directory",
                             "The My Measures Directory you chose appears to be on a network drive that is not currently available.\nYou can change "
                             "your specified My Measures Directory using 'Preferences->Change My Measures Directory'.",
                             QMessageBox::Ok);
    return;
  }

  if (!umd.empty()) {
    if (setUserMeasuresDir(umd)) {
      OSAppBase::instance()->currentDocument()->disable();
      OSAppBase::instance()->measureManager().updateMeasuresLists();
      OSAppBase::instance()->currentDocument()->enable();
    }
  }
}

void OSDocument::changeBclLogin() {
  QMessageBox::information(this->mainWindow(), QString("Change BCL Login Information"), QString("Not yet available.\nMiddleware testing required."));
}

void OSDocument::updateWindowFilePath() {
  if (m_savePath.isEmpty()) {
    // Because we use the setWindowModified, we need to set the WindowTitle with a "[*]" placeholder where the '*' should appear
    // m_mainWindow->setWindowFilePath("Untitled");
    m_mainWindow->setWindowTitle("Untitled[*]");
  } else {
    // m_mainWindow->setWindowTitle();
    m_mainWindow->setWindowFilePath(m_savePath);
    m_mainWindow->setWindowTitle(m_savePath + "[*]");
  }
}

void OSDocument::updateSubTabSelected(int id) {
  m_subTabIds.at(m_verticalId) = id;
}

void OSDocument::openBclDlg() {
  if (!m_haveLocalBCL) {
    QMessageBox::information(this->mainWindow(), "Cannot access LocalBCL",
                             QString("Cannot access LocalBCL. Check that the directory '")
                               .append(toQString(openstudio::LocalBCL::instance().libraryPath()))
                               .append("' exists and has proper permissions. Renaming or deleting the directory may resolve the issue."),
                             QMessageBox::Ok);
    return;
  }

  if (!RemoteBCL::isOnline()) {
    QMessageBox::information(this->mainWindow(), "Offline", "You appear to be offline, please connect to the internet to access the BCL.",
                             QMessageBox::Ok);
    return;
  }

  if (!m_onlineBclDialog) {
    std::string filterType = "components";
    m_onlineBclDialog = new BuildingComponentDialog(filterType, true, m_mainWindow);
    connect(m_onlineBclDialog, &BuildingComponentDialog::rejected, this, &OSDocument::on_closeBclDlg);
  }

  if (m_onlineBclDialog && !m_onlineBclDialog->isVisible()) {
    m_onlineBclDialog->setGeometry(m_mainWindow->geometry());
    m_onlineBclDialog->show();
  }
}

void OSDocument::on_closeBclDlg() {
  if (m_onlineBclDialog->showNewComponents()) {
    onVerticalTabSelected(m_mainTabId);
    m_onlineBclDialog->setShowNewComponents(false);
  }
}

void OSDocument::openLibDlg() {
  if (!m_localLibraryDialog) {
    std::string filterType = "components";
    m_localLibraryDialog = new BuildingComponentDialog(filterType, false, m_mainWindow);
  }
  if (m_localLibraryDialog && !m_localLibraryDialog->isVisible()) {
    m_localLibraryDialog->setGeometry(m_mainWindow->geometry());
    m_localLibraryDialog->show();
  }
}

std::shared_ptr<MainRightColumnController> OSDocument::mainRightColumnController() const {
  return m_mainRightColumnController;
}

void OSDocument::openMeasuresBclDlg() {
  if (!m_haveLocalBCL) {
    QMessageBox::information(this->mainWindow(), "Cannot access LocalBCL",
                             QString("Cannot access LocalBCL. Check that the directory '")
                               .append(toQString(openstudio::LocalBCL::instance().libraryPath()))
                               .append("' exists and has proper permissions. Renaming or deleting the directory may resolve the issue."),
                             QMessageBox::Ok);
    return;
  }

  if (!RemoteBCL::isOnline()) {
    QMessageBox::information(this->mainWindow(), "Offline", "You appear to be offline, please connect to the internet to access the BCL.",
                             QMessageBox::Ok);
    return;
  }

  if (!m_onlineMeasuresBclDialog) {
    std::string filterType = "measures";
    m_onlineMeasuresBclDialog = new BuildingComponentDialog(filterType, true, m_mainWindow);
    connect(m_onlineMeasuresBclDialog, &BuildingComponentDialog::rejected, this, &OSDocument::on_closeMeasuresBclDlg);
  }

  if (m_onlineMeasuresBclDialog && !m_onlineMeasuresBclDialog->isVisible()) {
    m_onlineMeasuresBclDialog->setGeometry(m_mainWindow->geometry());
    m_onlineMeasuresBclDialog->show();
  }
}

void OSDocument::on_closeMeasuresBclDlg() {
  OSAppBase::instance()->checkForRemoteBCLUpdates();
  if (m_onlineMeasuresBclDialog->showNewComponents()) {
    OSAppBase::instance()->currentDocument()->disable();
    OSAppBase::instance()->measureManager().updateMeasuresLists();
    OSAppBase::instance()->currentDocument()->enable();
    m_onlineMeasuresBclDialog->setShowNewComponents(false);
  }
}

}  // namespace openstudio
