/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MainRightColumnController.hpp"

#include "ConstructionsTabController.hpp"
#include "GeometryTabController.hpp"
#include "HorizontalTabWidget.hpp"
#include "InspectorController.hpp"
#include "InspectorView.hpp"
#include "LibraryTabWidget.hpp"
#include "LocationTabController.hpp"
#include "ModelObjectTypeListView.hpp"
#include "OSAppBase.hpp"
#include "OSCollapsibleItem.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "OSDocument.hpp"
#include "OSItem.hpp"
#include "OSItemList.hpp"
#include "SchedulesTabController.hpp"
#include "SpaceTypeInspectorView.hpp"
#include "ThermalZonesView.hpp"

#include "../shared_gui_components/EditController.hpp"
#include "../shared_gui_components/EditView.hpp"
#include "../shared_gui_components/LocalLibraryController.hpp"
#include "../shared_gui_components/LocalLibraryView.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/OSViewSwitcher.hpp"

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <openstudio/utilities/core/Assert.hpp>

#include <QStackedWidget>
#include <QLayout>
#include <QTimer>

namespace openstudio {

MainRightColumnController::MainRightColumnController(const model::Model& model, const openstudio::path& resourcesPath)
  : OSQObjectController(),
    m_model(model),
    m_resourcesPath(resourcesPath),
    m_measureLibraryController(new LocalLibraryController(OSAppBase::instance())),
    m_measureEditController(new EditController()),
    m_myModelTabIsHidden(false) {
  m_measureLibraryController->localLibraryView->setStyleSheet("QStackedWidget { border-top: 0px; }");
  OSAppBase::instance()->measureManager().setLibraryController(m_measureLibraryController);
  m_horizontalTabWidget = new HorizontalTabWidget();
  addQObject(m_horizontalTabWidget);

  // My Model

  m_myModelView = new QStackedWidget();
  m_myModelView->setStyleSheet("QStackedWidget { border-top: 1px solid black; }");
  m_horizontalTabWidget->addTab(m_myModelView, MY_MODEL, tr("My Model"));

  // Library

  m_libraryView = new QStackedWidget();
  m_libraryView->setStyleSheet("QStackedWidget { border-top: 1px solid black; }");
  m_horizontalTabWidget->addTab(m_libraryView, LIBRARY, tr("Library"));

  // Editor
  m_editView = new QStackedWidget();
  m_editView->setStyleSheet("QStackedWidget { border-top: 1px solid black; }");
  m_horizontalTabWidget->addTab(m_editView, EDIT, tr("Edit"));

  // Inspector, we're keeping it around to be able to follow the units toggled
  m_inspectorController = std::shared_ptr<InspectorController>(new InspectorController());
  connect(this, &MainRightColumnController::toggleUnitsClicked, m_inspectorController.get(), &InspectorController::toggleUnitsClicked);
  connect(this, &MainRightColumnController::toggleDisplayAdditionalPropsClicked, m_inspectorController.get(),
          &InspectorController::toggleDisplayAdditionalPropsClicked);
  connect(m_inspectorController.get(), &InspectorController::removeButtonClicked, this, &MainRightColumnController::onRemoveButtonClicked);
  connect(m_inspectorController.get(), &InspectorController::workspaceObjectRemoved, this, &MainRightColumnController::onWorkspaceObjectRemoved);
  connect(m_inspectorController.get(), &InspectorController::itemRemoveClicked, this, &MainRightColumnController::onItemRemoveClicked);
}

void MainRightColumnController::onItemRemoveClicked(OSItem*) {
  setEditView(nullptr);
}

void MainRightColumnController::emitItemRemoveClicked(OSItem* item) {
  emit itemRemoveClicked(item);
}

void MainRightColumnController::registerSystemItem(const Handle& systemHandle, SystemItem* systemItem) {
  m_systemItemMap[systemHandle] = systemItem;
}

void MainRightColumnController::unregisterSystemItem(const Handle& systemHandle) {
  auto it = m_systemItemMap.find(systemHandle);
  if (it != m_systemItemMap.end()) {
    m_systemItemMap.erase(it);
  }
}

SystemItem* MainRightColumnController::systemItem(const Handle& systemHandle) const {
  auto it = m_systemItemMap.find(systemHandle);
  if (it != m_systemItemMap.end()) {
    return it->second;
  }

  return nullptr;
}

void MainRightColumnController::inspectModelObject(model::OptionalModelObject& modelObject, bool readOnly) {
  if (modelObject) {
    m_horizontalTabWidget->setCurrentId(EDIT);
    setEditView(m_inspectorController->inspectorView());
    m_inspectorController->layoutModelObject(modelObject, readOnly);
  } else {
    m_inspectorController->layoutModelObject(modelObject, readOnly);
  }
  m_inspectorController->inspectorView()->currentView()->m_libraryTabWidget->hideRemoveButton();
}

void MainRightColumnController::inspectModelObjectByItem(OSItem* item, bool readOnly) {
  m_item = item;
  if (m_item) {
    boost::optional<model::ModelObject> modelObject;
    std::shared_ptr<OSDocument> currentDocument = OSAppBase::instance()->currentDocument();
    if (currentDocument) {
      modelObject = currentDocument->getModelObject(item->itemId());
    }

    if (modelObject) {
      m_horizontalTabWidget->setCurrentId(EDIT);
      setEditView(m_inspectorController->inspectorView());
      m_inspectorController->layoutModelObject(modelObject, readOnly);
    } else {
      m_inspectorController->layoutModelObject(modelObject, readOnly);
    }

    if (m_item->itemId().isDefaulted()) {
      m_inspectorController->inspectorView()->currentView()->m_libraryTabWidget->hideRemoveButton();
    } else {
      m_inspectorController->inspectorView()->currentView()->m_libraryTabWidget->showRemoveButton();
    }
  } else {
    setEditView(nullptr);
  }
}

void MainRightColumnController::onRemoveButtonClicked(bool checked) {
  if (m_item) {
    m_item->onRemoveClicked();
    setEditView(nullptr);
  }
}

void MainRightColumnController::onWorkspaceObjectRemoved() {
  setEditView(nullptr);
}

HorizontalTabWidget* MainRightColumnController::mainRightColumnView() const {
  return m_horizontalTabWidget;
}

QSharedPointer<LocalLibraryController> MainRightColumnController::measureLibraryController() {
  return m_measureLibraryController;
}

void MainRightColumnController::setEditView(QWidget* widget) {
  if (QWidget* oldwidget = m_editView->currentWidget()) {
    LOG(Debug, "Removing old edit widget: " << oldwidget);
    m_editView->removeWidget(oldwidget);

    if (oldwidget != m_inspectorController->inspectorView() && oldwidget != m_measureEditController->editView.data()) {
      LOG(Debug, "Deleting old edit widget: " << oldwidget);
      delete oldwidget;
    } else {
      boost::optional<model::ModelObject> nomodelobject;
      m_inspectorController->layoutModelObject(nomodelobject, false);
    }
  }

  if (widget) {
    LOG(Debug, "Setting new edit widget: " << widget);
    m_editView->addWidget(widget);
  }
}

void MainRightColumnController::setMyModelView(QWidget* widget) {
  if (QWidget* oldwidget = m_myModelView->currentWidget()) {
    m_myModelView->removeWidget(oldwidget);

    delete oldwidget;
  }

  if (widget) {
    m_myModelView->addWidget(widget);
  }
}

void MainRightColumnController::setLibraryView(QWidget* widget) {
  if (QWidget* oldwidget = m_libraryView->currentWidget()) {
    m_libraryView->removeWidget(oldwidget);

    if (oldwidget != m_measureLibraryController->localLibraryView) {
      delete oldwidget;
    }
  }

  if (widget) {
    m_libraryView->addWidget(widget);
  }
}

void MainRightColumnController::configureForSiteSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  if (subTabID == 0) {
    doc->closeSidebar();
  } else if (subTabID == 3) {  // Ground Temperatures
    model::Model lib = doc->componentLibrary();

    // my model
    auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
    myModelList->setItemsType(OSItemType::LibraryItem);
    myModelList->setItemsDraggable(true);
    myModelList->setItemsRemoveable(false);

    myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
    myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

    setMyModelView(myModelList);

    // my library
    auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
    myLibraryList->setItemsDraggable(true);
    myLibraryList->setItemsRemoveable(false);
    myLibraryList->setItemsType(OSItemType::LibraryItem);

    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
    myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

    setLibraryView(myLibraryList);
    doc->openSidebar();

  } else {
    doc->openSidebar();
  }
}

void MainRightColumnController::configureForSchedulesSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  switch (subTabID) {
    //case SchedulesTabController::YEAR_SETTINGS:
    //{
    //}
    case SchedulesTabController::SCHEDULE_SETS: {
      model::Model lib = doc->componentLibrary();

      // my model
      auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
      myModelList->setItemsType(OSItemType::LibraryItem);
      myModelList->setItemsDraggable(true);
      myModelList->setItemsRemoveable(false);

      myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
      myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedule Sets").toStdString());

      setLibraryView(myLibraryList);
      doc->openSidebar();
      //doc->closeSidebar();

      break;
    }
    case SchedulesTabController::SCHEDULES: {
      model::Model lib = doc->componentLibrary();

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Schedule Rulesets").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

      setLibraryView(myLibraryList);
      doc->openSidebar();
      //doc->closeSidebar();

      break;
    }
    case SchedulesTabController::SCHEDULESOTHER: {
      model::Model lib = doc->componentLibrary();

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
      // myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

      setLibraryView(myLibraryList);
      doc->openSidebar();
      //doc->closeSidebar();

      break;
    }
    default:
      break;
  }
}

void MainRightColumnController::configureForConstructionsSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  switch (subTabID) {
    case ConstructionsTabController::DEFAULT_CONSTRUCTIONS: {
      model::Model lib = doc->componentLibrary();

      // my model
      auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
      myModelList->setItemsDraggable(true);
      myModelList->setItemsRemoveable(false);
      myModelList->setItemsType(OSItemType::LibraryItem);

      myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
      myModelList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Construction Sets").toStdString());

      setLibraryView(myLibraryList);

      doc->openSidebar();
      break;
    }
    case ConstructionsTabController::CONSTRUCTIONS: {
      model::Model lib = doc->componentLibrary();

      // my model
      auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
      myModelList->setItemsDraggable(true);
      myModelList->setItemsRemoveable(false);
      myModelList->setItemsType(OSItemType::LibraryItem);

      // TODO: commented out until ThermochromicGlazing is properly wrapped
      // myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic, "Glazing Group Thermochromic Window Materials");

      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod,
                                      tr("Refraction Extinction Method Glazing Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice, tr("Daylight Redirection Device Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials").toStdString());
      myModelList->addModelObjectCategoryPlaceholder(tr("Window Materials").toStdString());

      myModelList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials").toStdString());
      myModelList->addModelObjectType(IddObjectType::OS_Material, tr("Materials").toStdString());
      myModelList->addModelObjectCategoryPlaceholder(tr("Materials").toStdString());

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      // TODO: commented out until ThermochromicGlazing is properly wrapped
      // myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic, "Glazing Group Thermochromic Window Materials");
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod,
                                        tr("Refraction Extinction Method Glazing Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice, tr("Daylight Redirection Device Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Window Materials").toStdString());

      myLibraryList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material, tr("Materials").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Materials").toStdString());

      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());

      setLibraryView(myLibraryList);

      doc->openSidebar();
      break;
    }
    case ConstructionsTabController::MATERIALS: {
      model::Model lib = doc->componentLibrary();

      // my model
      auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
      myModelList->setItemsDraggable(true);
      myModelList->setItemsRemoveable(false);
      myModelList->setItemsType(OSItemType::LibraryItem);

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      // TODO: commented out until ThermochromicGlazing is properly wrapped
      // myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic, "Glazing Group Thermochromic Window Materials");

      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod,
                                        tr("Refraction Extinction Method Glazing Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice, tr("Daylight Redirection Device Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Window Materials").toStdString());

      myLibraryList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials").toStdString());
      myLibraryList->addModelObjectType(IddObjectType::OS_Material, tr("Materials").toStdString());
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Materials").toStdString());

      setLibraryView(myLibraryList);

      doc->openSidebar();
      break;
    }
    default:
      break;
  }
}

void MainRightColumnController::configureForGeometrySubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  switch (subTabID) {
    case GeometryTabController::VIEW: {
      doc->closeSidebar();
      break;
    }
    case GeometryTabController::EDITOR: {
      doc->closeSidebar();
      break;
    }
    default:
      break;
  }
}

void MainRightColumnController::configureForLoadsSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  model::Model lib = doc->componentLibrary();

  setEditView(nullptr);

  // my model

  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  setMyModelView(myModelList);

  // my library

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  setLibraryView(myLibraryList);

  doc->openSidebar();
}

void MainRightColumnController::configureForSpaceTypesSubTab(int subTabID) {
  // no sub tabs
  OS_ASSERT(subTabID == 0);

  setEditView(nullptr);

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  // my model
  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString()); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());

  setMyModelView(myModelList);

  // my library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());

  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());
  setLibraryView(myLibraryList);

  doc->openSidebar();
}

void MainRightColumnController::configureForFacilitySubTab(int subTabID) {
  setEditView(nullptr);

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  // my model
  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  myModelList->addModelObjectType(IddObjectType::OS_SubSurface, tr("Sub Surfaces").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Surface, tr("Surfaces").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Surfaces").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment_Definition, tr("Exterior Water Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment_Definition, tr("Exterior Fuel Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_Lights_Definition, tr("Exterior Lights Definitions").toStdString());
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString()); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zones").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_BuildingStory, tr("Building Stories").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Building").toStdString());

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment_Definition, tr("Exterior Water Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment_Definition, tr("Exterior Fuel Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_Lights_Definition, tr("Exterior Lights Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment, tr("Exterior Water Equipment").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment, tr("Exterior Fuel Equipment").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_Lights, tr("Exterior Lights").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());

  setLibraryView(myLibraryList);

  doc->openSidebar();
  //doc->closeSidebar();
}

void MainRightColumnController::configureForSpacesSubTab(int subTabID) {
  setEditView(nullptr);

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  // my model
  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  myModelList->addModelObjectType(IddObjectType::OS_ShadingControl, tr("ShadingControl").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WindowProperty_FrameAndDivider, tr("Frame And Divider Window Property").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DaylightingDevice_Shelf, tr("DaylightingDevice Shelf").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Daylighting").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_InteriorPartitionSurface, tr("Interior Partition Surface").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ShadingSurface, tr("Shading Surface").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SubSurface, tr("Sub Surfaces").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Surface, tr("Surfaces").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Surfaces").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString()); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zones").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_BuildingStory, tr("Building Stories").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Building").toStdString());

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_ShadingControl, tr("ShadingControl").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WindowProperty_FrameAndDivider, tr("Frame And Divider Window Property").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DaylightingDevice_Shelf, tr("DaylightingDevice Shelf").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Daylighting").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder("Definition");
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets").toStdString());
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types").toStdString());
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults").toStdString());

  setLibraryView(myLibraryList);

  doc->openSidebar();
  //doc->closeSidebar();
}

void MainRightColumnController::configureForThermalZonesSubTab(int subTabID) {

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  // My Model

  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Heaters").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* libraryWidget = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  libraryWidget->setItemsDraggable(true);
  libraryWidget->setItemsRemoveable(false);
  libraryWidget->setItemsType(OSItemType::LibraryItem);

  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Heaters").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitarySystem, tr("Unitary System").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Unitary Systems").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_EvaporativeCoolerUnit, tr("Evaporative Cooler Unit").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_CoolingPanel_RadiantConvective_Water, tr("Cooling Panel Radiant Convective Water").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_Convective_Electric, tr("Baseboard Convective Electric").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_Convective_Water, tr("Baseboard Convective Water").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_RadiantConvective_Electric, tr("Baseboard Radiant Convective Electric").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_RadiantConvective_Water, tr("Baseboard Radiant Convective Water").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Dehumidifier_DX, tr("Dehumidifier - DX").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_EnergyRecoveryVentilator, tr("ERV").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ZoneExhaust, tr("Fan Zone Exhaust").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_ConstantFlow, tr("Low Temp Radiant Constant Flow").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_VariableFlow, tr("Low Temp Radiant Variable Flow").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_Electric, tr("Low Temp Radiant Electric").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_HighTemperatureRadiant, tr("High Temp Radiant").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Zone HVAC").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneVentilation_DesignFlowRate, tr("Zone Ventilation Design Flow Rate").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneVentilation_WindandStackOpenArea, tr("Zone Ventilation Wind and Stack Open Area").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ventilation").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Schedule Rulesets").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  setLibraryView(libraryWidget);

  m_horizontalTabWidget->setCurrentId(LIBRARY);

  doc->openSidebar();
}

void MainRightColumnController::configureForHVACSystemsSubTab(int subTabID) {

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  // my model
  auto* myModelList = new ModelObjectTypeListView(m_model, true, OSItemType::CollapsibleListHeader, false);
  myModelList->setItemsDraggable(true);
  myModelList->setItemsRemoveable(false);
  myModelList->setItemsType(OSItemType::LibraryItem);

  // Water and Tanks
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definition").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Connections, tr("Water Use Connections").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Uses").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_Mixed, tr("Water Heater Mixed").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_Stratified, tr("Water Heater Stratified").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Heaters").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_AirConditioner_VariableRefrigerantFlow, tr("VRF System").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("VRFs").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ThermalStorage_ChilledWater_Stratified, tr("Thermal Storage - Chilled Water").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Thermal Storage").toStdString());

  // Refrigeration
  myModelList->addModelObjectType(IddObjectType::OS_Refrigeration_System, tr("Refrigeration System").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_WaterCooled, tr("Refrigeration Condenser Water Cooled").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Refrigeration").toStdString());

  // ZoneHVAC Components
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_TerminalUnit_VariableRefrigerantFlow, tr("VRF Terminal").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Zone HVAC").toStdString());

  // Central components and coils
  myModelList->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Heating, tr("Heat Pump - Water to Water - Heating").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Cooling, tr("Heat Pump - Water to Water - Cooling").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_HeatExchanger_FluidToFluid, tr("Heat Exchanger Fluid To Fluid").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Heat Exchangers").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Coil_Heating_Water, tr("Coil Heating Water").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Coil_Cooling_Water, tr("Coil Cooling Water").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Coils").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_CentralHeatPumpSystem, tr("Central Heat Pump System").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Heat Pumps").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Electric_EIR, tr("Chiller - Electric EIR").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Absorption, tr("Chiller - Absorption").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Absorption_Indirect, tr("Chiller - Indirect Absorption").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Chillers").toStdString());

  // Zones
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zone").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Zones").toStdString());

  // Schedules
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules").toStdString());
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules").toStdString());
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules").toStdString());

  setMyModelView(myModelList);

  // Library

  model::Model lib = doc->componentLibrary();

  auto* libraryWidget = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  libraryWidget->setItemsDraggable(true);
  libraryWidget->setItemsRemoveable(false);
  libraryWidget->setItemsType(OSItemType::LibraryItem);

  libraryWidget->addModelObjectType(IddObjectType::OS_SwimmingPool_Indoor, tr("Swimming Pool Indoor").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Swimming Pools").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_IntegralCollectorStorage, tr("Solar Collector Integral Collector Storage").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_FlatPlate_Water, tr("Solar Collector Flat Plate Water").toStdString());
  //libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_FlatPlate_PhotovoltaicThermal, "Solar Collector Flat Plate Photovoltaic Thermal");
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Solar Collectors").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterUse_Equipment, tr("Water Use Equipment").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterUse_Connections, tr("Water Use Connections").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Uses").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_Mixed, tr("Water Heater Mixed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_Stratified, tr("Water Heater Stratified").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Heaters").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirConditioner_VariableRefrigerantFlow, tr("VRF System").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_TerminalUnit_VariableRefrigerantFlow, tr("VRF Terminal").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("VRFs").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ThermalStorage_Ice_Detailed, tr("Thermal Storage - Ice Storage").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ThermalStorage_ChilledWater_Stratified, tr("Thermal Storage - Chilled Water").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Thermal Storage").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_TemperingValve, tr("Tempering Valve").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SystemNodeReset_Humidity, tr("Setpoint Manager System Node Reset Humidity").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SystemNodeReset_Temperature, tr("Setpoint Manager System Node Reset Temperature").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Coldest, tr("Setpoint Manager Coldest").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowGroundTemperature, tr("Setpoint Manager Follow Ground Temperature").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowOutdoorAirTemperature, tr("Setpoint Manager Follow Outdoor Air Temperature").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowSystemNodeTemperature, tr("Setpoint Manager Follow System Node Temperature").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MixedAir, tr("Setpoint Manager Mixed Air").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Cooling_Average, tr("Setpoint Manager MultiZone Cooling Average").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Heating_Average, tr("Setpoint Manager MultiZone Heating Average").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Humidity_Maximum, tr("Setpoint Manager MultiZone Humidity Maximum").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Humidity_Minimum, tr("Setpoint Manager MultiZone Humidity Minimum").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_MaximumHumidity_Average,
                                    tr("Setpoint Manager MultiZone MaximumHumidity Average").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_MinimumHumidity_Average,
                                    tr("Setpoint Manager MultiZone MinimumHumidity Average").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_OutdoorAirPretreat, tr("Setpoint Manager Outdoor Air Pretreat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_OutdoorAirReset, tr("Setpoint Manager Outdoor Air Reset").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Scheduled, tr("Setpoint Manager Scheduled").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Scheduled_DualSetpoint, tr("Setpoint Manager Scheduled Dual Setpoint").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Cooling, tr("Setpoint Manager Single Zone Cooling").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Heating, tr("Setpoint Manager Single Zone Heating").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Humidity_Maximum, tr("Setpoint Manager Humidity Maximum").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Humidity_Minimum, tr("Setpoint Manager Humidity Minimum").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_OneStageCooling, tr("Setpoint Manager One Stage Cooling").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_OneStageHeating, tr("Setpoint Manager One Stage Heating").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Reheat, tr("Setpoint Manager Single Zone Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Warmest, tr("Setpoint Manager Warmest").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_WarmestTemperatureFlow, tr("Setpoint Manager Warmest Temp and Flow").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Setpoint Managers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_WalkIn, tr("Refrigeration Walkin").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_System, tr("Refrigeration System").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Subcooler_Mechanical, tr("Refrigeration Subcooler Mechanical").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Subcooler_LiquidSuction, tr("Refrigeration Subcooler Liquid Suction").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Compressor, tr("Refrigeration Compressor").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_Cascade, tr("Refrigeration Condenser Cascade").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_WaterCooled, tr("Refrigeration Condenser Water Cooled").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_EvaporativeCooled, tr("Refrigeration Condenser Evaporative Cooled").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_AirCooled, tr("Refrigeration Condenser Air Cooled").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Case, tr("Refrigeration Case").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Refrigeration").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Pump_ConstantSpeed, tr("Pump Constant Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeaderedPumps_ConstantSpeed, tr("Pump Constant Speed Headered").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Pump_VariableSpeed, tr("Pump Variable Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeaderedPumps_VariableSpeed, tr("Pump Variable Speed Headered").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Pumps").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_PlantComponent_TemperatureSource, tr("Plant Component - Temp Source").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_PlantComponent_UserDefined, tr("Plant Component - User Defined").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Plant Components").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Outdoor, tr("Pipe - Outdoor").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Indoor, tr("Pipe - Indoor").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Adiabatic, tr("Pipe - Adiabatic").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Pipes").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_LoadProfile_Plant, tr("Load Profile - Plant").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Load Profiles").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Humidifier_Steam_Electric, tr("Humidifier Steam Electric").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Humidifier_Steam_Gas, tr("Humidifier Steam Gas").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Humidifiers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Heating, tr("Heat Pump - Water to Water - Heating").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Cooling, tr("Heat Pump - Water to Water - Cooling").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Heat Pumps").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatExchanger_FluidToFluid, tr("Heat Exchanger Fluid To Fluid").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatExchanger_AirToAir_SensibleAndLatent, tr("Heat Exchanger Air To Air Sensible and Latent").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Heat Exchangers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Generator_FuelCell_ExhaustGasToWaterHeatExchanger,
                                    tr("Generator FuelCell - Exhaust Gas To Water Heat Exchanger").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Generator_MicroTurbine_HeatRecovery, tr("Generator MicroTurbine - Heat Recovery").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Generators").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_GroundHeatExchanger_Vertical, tr("Ground Heat Exchanger - Vertical ").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_GroundHeatExchanger_HorizontalTrench, tr("Ground Heat Exchanger - Horizontal").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ground Heat Exchangers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_FluidCooler_TwoSpeed, tr("Fluid Cooler Two Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_FluidCooler_SingleSpeed, tr("Fluid Cooler Single Speed").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Fluid Coolers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Zone HVAC").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ComponentModel, tr("Fan Component Model").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_SystemModel, tr("Fan System Model").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_VariableVolume, tr("Fan Variable Volume").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ConstantVolume, tr("Fan Constant Volume").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Fans").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeCooler_Direct_ResearchSpecial, tr("Evaporative Cooler Direct Research Special").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeCooler_Indirect_ResearchSpecial, tr("Evaporative Cooler Indirect Research Special").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeFluidCooler_TwoSpeed, tr("Evaporative Fluid Cooler Two Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeFluidCooler_SingleSpeed, tr("Evaporative Fluid Cooler Single Speed").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Evaporative Coolers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Duct, tr("Duct").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ducts").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_DistrictCooling, tr("District Cooling").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("District Cooling").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_DistrictHeating_Water, tr("District Heating Water").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("District Heating").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_TwoSpeed, tr("Cooling Tower Two Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_SingleSpeed, tr("Cooling Tower Single Speed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_VariableSpeed, tr("Cooling Tower Variable Speed").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Cooling Towers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_CentralHeatPumpSystem, tr("Central Heat Pump System").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Central Heat Pump Systems").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Electric_EIR, tr("Chiller - Electric EIR").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Absorption_Indirect, tr("Chiller - Indirect Absorption").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Absorption, tr("Chiller - Absorption").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Chillers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Gas, tr("Coil Heating Gas").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_DX_SingleSpeed, tr("Coil Heating DX SingleSpeed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Electric, tr("Coil Heating Electric").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Water, tr("Coil Heating Water").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_Water, tr("Coil Cooling Water").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_VariableSpeed, tr("Coil Cooling DX VariableSpeed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_TwoStageWithHumidityControlMode, tr("Coil Cooling DX TwoStage - Humidity Control").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_TwoSpeed, tr("Coil Cooling DX TwoSpeed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_SingleSpeed, tr("Coil Cooling DX SingleSpeed").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Coils").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_Boiler_HotWater, tr("Boiler Hot Water").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Boilers").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_FourPipeInduction, tr("Air Terminal Four Pipe Induction").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_CooledBeam, tr("Air Terminal Chilled Beam").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_FourPipeBeam, tr("Air Terminal Four Pipe Beam").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_Reheat, tr("AirTerminal Single Duct Constant Volume Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_Reheat, tr("AirTerminal Single Duct VAV Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ParallelPIU_Reheat, tr("AirTerminal Single Duct Parallel PIU Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_SeriesPIU_Reheat, tr("AirTerminal Single Duct Series PIU Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_InletSideMixer, tr("AirTerminal Inlet Side Mixer").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_HeatAndCool_Reheat, tr("AirTerminal Heat and Cool Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_HeatAndCool_NoReheat, tr("AirTerminal Heat and Cool No Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_NoReheat, tr("AirTerminal Single Duct VAV NoReheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_NoReheat,
                                    tr("AirTerminal Single Duct Constant Volume No Reheat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_ConstantVolume, tr("Air Terminal Dual Duct Constant Volume").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_VAV, tr("Air Terminal Dual Duct VAV").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_VAV_OutdoorAir, tr("Air Terminal Dual Duct VAV Outdoor Air").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Air Terminals").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_OutdoorAirSystem, tr("AirLoopHVAC Outdoor Air System").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitaryHeatPump_AirToAir_MultiSpeed,
                                    tr("AirLoopHVAC Unitary Heat Pump AirToAir MultiSpeed").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitarySystem, tr("AirLoopHVAC Unitary System").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitaryHeatCool_VAVChangeoverBypass, tr("AirLoopHVAC Unitary VAV Changeover Bypass").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Air Loop HVAC").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_Scheduled, tr("Availability Manager Scheduled").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_ScheduledOn, tr("Availability Manager Scheduled On").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_ScheduledOff, tr("Availability Manager Scheduled Off").toStdString());

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_LowTemperatureTurnOn, tr("Availability Manager Low Temperature Turn On").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_LowTemperatureTurnOff, tr("Availability Manager Low Temperature Turn Off").toStdString());

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HighTemperatureTurnOn, tr("Availability Manager High Temperature Turn On").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HighTemperatureTurnOff, tr("Availability Manager High Temperature Turn Off").toStdString());

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_DifferentialThermostat, tr("Availability Manager Differential Thermostat").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_OptimumStart, tr("Availability Manager Optimum Start").toStdString());

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_NightCycle, tr("Availability Manager Night Cycle").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_NightVentilation, tr("Availability Manager Night Ventilation").toStdString());
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HybridVentilation, tr("Availability Manager Hybrid Ventilation").toStdString());
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Availability Managers").toStdString());

  setLibraryView(libraryWidget);

  m_horizontalTabWidget->setCurrentId(LIBRARY);

  doc->openSidebar();
}

void MainRightColumnController::configureForOutputVariablesSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  //doc->openSidebar();
  doc->closeSidebar();
}

void MainRightColumnController::configureForSimulationSettingsSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  //doc->openSidebar();
  doc->closeSidebar();
}

void MainRightColumnController::configureForScriptsSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(m_measureLibraryController->localLibraryView.data());
  setMyModelView(nullptr);
  m_measureEditController->reset();
  setEditView(m_measureEditController->editView.data());

  doc->openSidebar();
}

void MainRightColumnController::configureForRunSimulationSubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  //doc->openSidebar();
  doc->closeSidebar();
}

void MainRightColumnController::configureForResultsSummarySubTab(int subTabID) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  setLibraryView(nullptr);
  setMyModelView(nullptr);
  setEditView(nullptr);

  //doc->openSidebar();
  doc->closeSidebar();
}

void MainRightColumnController::toggleUnits(bool displayIP) {}

QSharedPointer<LocalLibraryController> MainRightColumnController::measuresLibraryController() {
  return m_measureLibraryController;
}

QSharedPointer<EditController> MainRightColumnController::measuresEditController() {
  return m_measureEditController;
}

void MainRightColumnController::chooseEditTab() {
  m_horizontalTabWidget->setCurrentId(EDIT);

  OSAppBase::instance()->currentDocument()->openSidebar();
}

void MainRightColumnController::hideMyModelTab(bool hide) {
  m_myModelTabIsHidden = hide;

  m_horizontalTabWidget->hideTab(m_myModelView, hide);
}

}  // namespace openstudio
