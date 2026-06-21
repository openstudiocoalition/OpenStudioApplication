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

    myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
    myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
    myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

    setMyModelView(myModelList);

    // my library
    auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
    myLibraryList->setItemsDraggable(true);
    myLibraryList->setItemsRemoveable(false);
    myLibraryList->setItemsType(OSItemType::LibraryItem);

    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
    myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
    myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));

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

      myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
      myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
      myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));
      myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedule Sets"));

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

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Schedule Rulesets"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));

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

      myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
      // myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));

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

      myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
      myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
      myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
      myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
      myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
      myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
      myModelList->addModelObjectCategoryPlaceholder(tr("Constructions"));

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Construction Sets"));

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
                                      tr("Refraction Extinction Method Glazing Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice, tr("Daylight Redirection Device Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials"));
      myModelList->addModelObjectCategoryPlaceholder(tr("Window Materials"));

      myModelList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials"));
      myModelList->addModelObjectType(IddObjectType::OS_Material, tr("Materials"));
      myModelList->addModelObjectCategoryPlaceholder(tr("Materials"));

      setMyModelView(myModelList);

      // my library
      auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
      myLibraryList->setItemsDraggable(true);
      myLibraryList->setItemsRemoveable(false);
      myLibraryList->setItemsType(OSItemType::LibraryItem);

      // TODO: commented out until ThermochromicGlazing is properly wrapped
      // myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic, "Glazing Group Thermochromic Window Materials");
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod,
                                        tr("Refraction Extinction Method Glazing Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice,
                                        tr("Daylight Redirection Device Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Window Materials"));

      myLibraryList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material, tr("Materials"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Materials"));

      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions"));

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
                                        tr("Refraction Extinction Method Glazing Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice,
                                        tr("Daylight Redirection Device Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Window Materials"));

      myLibraryList->addModelObjectType(IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_AirGap, tr("Air Gap Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material_NoMass, tr("No Mass Materials"));
      myLibraryList->addModelObjectType(IddObjectType::OS_Material, tr("Materials"));
      myLibraryList->addModelObjectCategoryPlaceholder(tr("Materials"));

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

  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

  setMyModelView(myModelList);

  // my library

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));

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

  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates")); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults"));

  setMyModelView(myModelList);

  // my library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));

  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions"));

  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults"));
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

  myModelList->addModelObjectType(IddObjectType::OS_SubSurface, tr("Sub Surfaces"));
  myModelList->addModelObjectType(IddObjectType::OS_Surface, tr("Surfaces"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Surfaces"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment_Definition, tr("Exterior Water Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment_Definition, tr("Exterior Fuel Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Exterior_Lights_Definition, tr("Exterior Lights Definitions"));
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates")); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myModelList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults"));
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zones"));
  myModelList->addModelObjectType(IddObjectType::OS_BuildingStory, tr("Building Stories"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Building"));

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment_Definition, tr("Exterior Water Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment_Definition, tr("Exterior Fuel Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_Lights_Definition, tr("Exterior Lights Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_WaterEquipment, tr("Exterior Water Equipment"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_FuelEquipment, tr("Exterior Fuel Equipment"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Exterior_Lights, tr("Exterior Lights"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults"));

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

  myModelList->addModelObjectType(IddObjectType::OS_ShadingControl, tr("ShadingControl"));
  myModelList->addModelObjectType(IddObjectType::OS_WindowProperty_FrameAndDivider, tr("Frame And Divider Window Property"));
  myModelList->addModelObjectType(IddObjectType::OS_DaylightingDevice_Shelf, tr("DaylightingDevice Shelf"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Daylighting"));
  myModelList->addModelObjectType(IddObjectType::OS_InteriorPartitionSurface, tr("Interior Partition Surface"));
  myModelList->addModelObjectType(IddObjectType::OS_ShadingSurface, tr("Shading Surface"));
  myModelList->addModelObjectType(IddObjectType::OS_SubSurface, tr("Sub Surfaces"));
  myModelList->addModelObjectType(IddObjectType::OS_Surface, tr("Surfaces"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Surfaces"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  //myModelList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates")); // do not show in my model because these are not shareable
  myModelList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Definitions"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myModelList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myModelList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Defaults"));
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zones"));
  myModelList->addModelObjectType(IddObjectType::OS_BuildingStory, tr("Building Stories"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Building"));

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* myLibraryList = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  myLibraryList->setItemsDraggable(true);
  myLibraryList->setItemsRemoveable(false);
  myLibraryList->setItemsType(OSItemType::LibraryItem);

  myLibraryList->addModelObjectType(IddObjectType::OS_ShadingControl, tr("ShadingControl"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WindowProperty_FrameAndDivider, tr("Frame And Divider Window Property"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DaylightingDevice_Shelf, tr("DaylightingDevice Shelf"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Daylighting"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_WindowDataFile, tr("Window Data File Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_FfactorGroundFloor, tr("F-factor Ground Floor Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_CfactorUndergroundWall, tr("C-factor Underground Wall Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_InternalSource, tr("Internal Source Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction_AirBoundary, tr("Air Boundary Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Construction, tr("Constructions"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Constructions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Schedules"));
  myLibraryList->addModelObjectType(IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump"));
  myLibraryList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser"));
  myLibraryList->addModelObjectType(IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_Lights_Definition, tr("Lights Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_People_Definition, tr("People Definitions"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_DesignFlowRate, tr("Space Infiltration Design Flow Rates"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceInfiltration_EffectiveLeakageArea, tr("Space Infiltration Effective Leakage Areas"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DesignSpecification_OutdoorAir, tr("Design Specification Outdoor Air"));
  myLibraryList->addModelObjectCategoryPlaceholder("Definition");
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultScheduleSet, tr("Schedule Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_DefaultConstructionSet, tr("Construction Sets"));
  myLibraryList->addModelObjectType(IddObjectType::OS_SpaceType, tr("Space Types"));
  myLibraryList->addModelObjectCategoryPlaceholder(tr("Defaults"));

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

  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Heaters"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

  setMyModelView(myModelList);

  // Library
  model::Model lib = doc->componentLibrary();

  auto* libraryWidget = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  libraryWidget->setItemsDraggable(true);
  libraryWidget->setItemsRemoveable(false);
  libraryWidget->setItemsType(OSItemType::LibraryItem);

  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Heaters"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitarySystem, tr("Unitary System"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Unitary Systems"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_EvaporativeCoolerUnit, tr("Evaporative Cooler Unit"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_CoolingPanel_RadiantConvective_Water, tr("Cooling Panel Radiant Convective Water"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_Convective_Electric, tr("Baseboard Convective Electric"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_Convective_Water, tr("Baseboard Convective Water"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_RadiantConvective_Electric, tr("Baseboard Radiant Convective Electric"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Baseboard_RadiantConvective_Water, tr("Baseboard Radiant Convective Water"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_Dehumidifier_DX, tr("Dehumidifier - DX"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_EnergyRecoveryVentilator, tr("ERV"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ZoneExhaust, tr("Fan Zone Exhaust"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_ConstantFlow, tr("Low Temp Radiant Constant Flow"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_VariableFlow, tr("Low Temp Radiant Variable Flow"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_LowTemperatureRadiant_Electric, tr("Low Temp Radiant Electric"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_HighTemperatureRadiant, tr("High Temp Radiant"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Zone HVAC"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneVentilation_DesignFlowRate, tr("Zone Ventilation Design Flow Rate"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneVentilation_WindandStackOpenArea, tr("Zone Ventilation Wind and Stack Open Area"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ventilation"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Schedule Rulesets"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Schedules"));

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
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definition"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterUse_Connections, tr("Water Use Connections"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Uses"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_Mixed, tr("Water Heater Mixed"));
  myModelList->addModelObjectType(IddObjectType::OS_WaterHeater_Stratified, tr("Water Heater Stratified"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Water Heaters"));
  myModelList->addModelObjectType(IddObjectType::OS_AirConditioner_VariableRefrigerantFlow, tr("VRF System"));
  myModelList->addModelObjectCategoryPlaceholder(tr("VRFs"));
  myModelList->addModelObjectType(IddObjectType::OS_ThermalStorage_ChilledWater_Stratified, tr("Thermal Storage - Chilled Water"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Thermal Storage"));

  // Refrigeration
  myModelList->addModelObjectType(IddObjectType::OS_Refrigeration_System, tr("Refrigeration System"));
  myModelList->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_WaterCooled, tr("Refrigeration Condenser Water Cooled"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Refrigeration"));

  // ZoneHVAC Components
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_TerminalUnit_VariableRefrigerantFlow, tr("VRF Terminal"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC"));
  myModelList->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Zone HVAC"));

  // Central components and coils
  myModelList->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Heating, tr("Heat Pump - Water to Water - Heating"));
  myModelList->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Cooling, tr("Heat Pump - Water to Water - Cooling"));
  myModelList->addModelObjectType(IddObjectType::OS_HeatExchanger_FluidToFluid, tr("Heat Exchanger Fluid To Fluid"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Heat Exchangers"));
  myModelList->addModelObjectType(IddObjectType::OS_Coil_Heating_Water, tr("Coil Heating Water"));
  myModelList->addModelObjectType(IddObjectType::OS_Coil_Cooling_Water, tr("Coil Cooling Water"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Coils"));
  myModelList->addModelObjectType(IddObjectType::OS_CentralHeatPumpSystem, tr("Central Heat Pump System"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Heat Pumps"));
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Electric_EIR, tr("Chiller - Electric EIR"));
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Absorption, tr("Chiller - Absorption"));
  myModelList->addModelObjectType(IddObjectType::OS_Chiller_Absorption_Indirect, tr("Chiller - Indirect Absorption"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Chillers"));

  // Zones
  myModelList->addModelObjectType(IddObjectType::OS_ThermalZone, tr("Thermal Zone"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Zones"));

  // Schedules
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_File, tr("Schedule File"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_VariableInterval, tr("Variable Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_FixedInterval, tr("Fixed Interval Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Year, tr("Year Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Constant, tr("Constant Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Compact, tr("Compact Schedules"));
  myModelList->addModelObjectType(IddObjectType::OS_Schedule_Ruleset, tr("Ruleset Schedules"));
  myModelList->addModelObjectCategoryPlaceholder(tr("Schedules"));

  setMyModelView(myModelList);

  // Library

  model::Model lib = doc->componentLibrary();

  auto* libraryWidget = new ModelObjectTypeListView(lib, true, OSItemType::CollapsibleListHeader, true);
  libraryWidget->setItemsDraggable(true);
  libraryWidget->setItemsRemoveable(false);
  libraryWidget->setItemsType(OSItemType::LibraryItem);

  libraryWidget->addModelObjectType(IddObjectType::OS_SwimmingPool_Indoor, tr("Swimming Pool Indoor"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Swimming Pools"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_IntegralCollectorStorage, tr("Solar Collector Integral Collector Storage"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_FlatPlate_Water, tr("Solar Collector Flat Plate Water"));
  //libraryWidget->addModelObjectType(IddObjectType::OS_SolarCollector_FlatPlate_PhotovoltaicThermal, "Solar Collector Flat Plate Photovoltaic Thermal");
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Solar Collectors"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterUse_Equipment, tr("Water Use Equipment"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterUse_Connections, tr("Water Use Connections"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Uses"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump, tr("Water Heater - Heat Pump"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser, tr("Water Heater - Heat Pump - Wrapped Condenser"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_Mixed, tr("Water Heater Mixed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_WaterHeater_Stratified, tr("Water Heater Stratified"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Water Heaters"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirConditioner_VariableRefrigerantFlow, tr("VRF System"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_TerminalUnit_VariableRefrigerantFlow, tr("VRF Terminal"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("VRFs"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ThermalStorage_Ice_Detailed, tr("Thermal Storage - Ice Storage"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ThermalStorage_ChilledWater_Stratified, tr("Thermal Storage - Chilled Water"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Thermal Storage"));
  libraryWidget->addModelObjectType(IddObjectType::OS_TemperingValve, tr("Tempering Valve"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SystemNodeReset_Humidity, tr("Setpoint Manager System Node Reset Humidity"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SystemNodeReset_Temperature,
                                    tr("Setpoint Manager System Node Reset Temperature"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Coldest, tr("Setpoint Manager Coldest"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowGroundTemperature, tr("Setpoint Manager Follow Ground Temperature"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowOutdoorAirTemperature,
                                    tr("Setpoint Manager Follow Outdoor Air Temperature"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_FollowSystemNodeTemperature,
                                    tr("Setpoint Manager Follow System Node Temperature"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MixedAir, tr("Setpoint Manager Mixed Air"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Cooling_Average, tr("Setpoint Manager MultiZone Cooling Average"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Heating_Average, tr("Setpoint Manager MultiZone Heating Average"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Humidity_Maximum, tr("Setpoint Manager MultiZone Humidity Maximum"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_Humidity_Minimum, tr("Setpoint Manager MultiZone Humidity Minimum"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_MaximumHumidity_Average,
                                    tr("Setpoint Manager MultiZone MaximumHumidity Average"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_MultiZone_MinimumHumidity_Average,
                                    tr("Setpoint Manager MultiZone MinimumHumidity Average"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_OutdoorAirPretreat, tr("Setpoint Manager Outdoor Air Pretreat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_OutdoorAirReset, tr("Setpoint Manager Outdoor Air Reset"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Scheduled, tr("Setpoint Manager Scheduled"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Scheduled_DualSetpoint, tr("Setpoint Manager Scheduled Dual Setpoint"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Cooling, tr("Setpoint Manager Single Zone Cooling"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Heating, tr("Setpoint Manager Single Zone Heating"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Humidity_Maximum, tr("Setpoint Manager Humidity Maximum"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Humidity_Minimum, tr("Setpoint Manager Humidity Minimum"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_OneStageCooling, tr("Setpoint Manager One Stage Cooling"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_OneStageHeating, tr("Setpoint Manager One Stage Heating"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_SingleZone_Reheat, tr("Setpoint Manager Single Zone Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_Warmest, tr("Setpoint Manager Warmest"));
  libraryWidget->addModelObjectType(IddObjectType::OS_SetpointManager_WarmestTemperatureFlow, tr("Setpoint Manager Warmest Temp and Flow"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Setpoint Managers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_WalkIn, tr("Refrigeration Walkin"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_System, tr("Refrigeration System"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Subcooler_Mechanical, tr("Refrigeration Subcooler Mechanical"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Subcooler_LiquidSuction, tr("Refrigeration Subcooler Liquid Suction"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Compressor, tr("Refrigeration Compressor"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_Cascade, tr("Refrigeration Condenser Cascade"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_WaterCooled, tr("Refrigeration Condenser Water Cooled"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_EvaporativeCooled, tr("Refrigeration Condenser Evaporative Cooled"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Condenser_AirCooled, tr("Refrigeration Condenser Air Cooled"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Refrigeration_Case, tr("Refrigeration Case"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Refrigeration"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Pump_ConstantSpeed, tr("Pump Constant Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeaderedPumps_ConstantSpeed, tr("Pump Constant Speed Headered"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Pump_VariableSpeed, tr("Pump Variable Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeaderedPumps_VariableSpeed, tr("Pump Variable Speed Headered"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Pumps"));
  libraryWidget->addModelObjectType(IddObjectType::OS_PlantComponent_TemperatureSource, tr("Plant Component - Temp Source"));
  libraryWidget->addModelObjectType(IddObjectType::OS_PlantComponent_UserDefined, tr("Plant Component - User Defined"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Plant Components"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Outdoor, tr("Pipe - Outdoor"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Indoor, tr("Pipe - Indoor"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Pipe_Adiabatic, tr("Pipe - Adiabatic"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Pipes"));
  libraryWidget->addModelObjectType(IddObjectType::OS_LoadProfile_Plant, tr("Load Profile - Plant"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Load Profiles"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Humidifier_Steam_Electric, tr("Humidifier Steam Electric"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Humidifier_Steam_Gas, tr("Humidifier Steam Gas"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Humidifiers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Heating, tr("Heat Pump - Water to Water - Heating"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatPump_WaterToWater_EquationFit_Cooling, tr("Heat Pump - Water to Water - Cooling"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Heat Pumps"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatExchanger_FluidToFluid, tr("Heat Exchanger Fluid To Fluid"));
  libraryWidget->addModelObjectType(IddObjectType::OS_HeatExchanger_AirToAir_SensibleAndLatent, tr("Heat Exchanger Air To Air Sensible and Latent"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Heat Exchangers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Generator_FuelCell_ExhaustGasToWaterHeatExchanger,
                                    tr("Generator FuelCell - Exhaust Gas To Water Heat Exchanger"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Generator_MicroTurbine_HeatRecovery, tr("Generator MicroTurbine - Heat Recovery"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Generators"));
  libraryWidget->addModelObjectType(IddObjectType::OS_GroundHeatExchanger_Vertical, tr("Ground Heat Exchanger - Vertical "));
  libraryWidget->addModelObjectType(IddObjectType::OS_GroundHeatExchanger_HorizontalTrench, tr("Ground Heat Exchanger - Horizontal"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ground Heat Exchangers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_FluidCooler_TwoSpeed, tr("Fluid Cooler Two Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_FluidCooler_SingleSpeed, tr("Fluid Cooler Single Speed"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Fluid Coolers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_FourPipeFanCoil, tr("Four Pipe Fan Coil"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalHeatPump, tr("PTHP"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_WaterToAirHeatPump, tr("Water To Air HP"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_PackagedTerminalAirConditioner, tr("PTAC"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitHeater, tr("Unit Heater"));
  libraryWidget->addModelObjectType(IddObjectType::OS_ZoneHVAC_UnitVentilator, tr("Unit Ventilator"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Zone HVAC"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ComponentModel, tr("Fan Component Model"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_SystemModel, tr("Fan System Model"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_VariableVolume, tr("Fan Variable Volume"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Fan_ConstantVolume, tr("Fan Constant Volume"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Fans"));
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeCooler_Direct_ResearchSpecial, tr("Evaporative Cooler Direct Research Special"));
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeCooler_Indirect_ResearchSpecial, tr("Evaporative Cooler Indirect Research Special"));
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeFluidCooler_TwoSpeed, tr("Evaporative Fluid Cooler Two Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_EvaporativeFluidCooler_SingleSpeed, tr("Evaporative Fluid Cooler Single Speed"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Evaporative Coolers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Duct, tr("Duct"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Ducts"));
  libraryWidget->addModelObjectType(IddObjectType::OS_DistrictCooling, tr("District Cooling"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("District Cooling"));
  libraryWidget->addModelObjectType(IddObjectType::OS_DistrictHeating_Water, tr("District Heating Water"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("District Heating"));
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_TwoSpeed, tr("Cooling Tower Two Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_SingleSpeed, tr("Cooling Tower Single Speed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_CoolingTower_VariableSpeed, tr("Cooling Tower Variable Speed"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Cooling Towers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_CentralHeatPumpSystem, tr("Central Heat Pump System"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Central Heat Pump Systems"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Electric_EIR, tr("Chiller - Electric EIR"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Absorption_Indirect, tr("Chiller - Indirect Absorption"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Chiller_Absorption, tr("Chiller - Absorption"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Chillers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Gas, tr("Coil Heating Gas"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_DX_SingleSpeed, tr("Coil Heating DX SingleSpeed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Electric, tr("Coil Heating Electric"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Heating_Water, tr("Coil Heating Water"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_Water, tr("Coil Cooling Water"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_VariableSpeed, tr("Coil Cooling DX VariableSpeed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_TwoStageWithHumidityControlMode,
                                    tr("Coil Cooling DX TwoStage - Humidity Control"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_TwoSpeed, tr("Coil Cooling DX TwoSpeed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Coil_Cooling_DX_SingleSpeed, tr("Coil Cooling DX SingleSpeed"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Coils"));
  libraryWidget->addModelObjectType(IddObjectType::OS_Boiler_HotWater, tr("Boiler Hot Water"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Boilers"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_FourPipeInduction,
                                    tr("Air Terminal Four Pipe Induction"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_CooledBeam, tr("Air Terminal Chilled Beam"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_FourPipeBeam, tr("Air Terminal Four Pipe Beam"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_Reheat,
                                    tr("AirTerminal Single Duct Constant Volume Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_Reheat, tr("AirTerminal Single Duct VAV Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ParallelPIU_Reheat, tr("AirTerminal Single Duct Parallel PIU Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_SeriesPIU_Reheat, tr("AirTerminal Single Duct Series PIU Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_InletSideMixer, tr("AirTerminal Inlet Side Mixer"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_HeatAndCool_Reheat, tr("AirTerminal Heat and Cool Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_HeatAndCool_NoReheat, tr("AirTerminal Heat and Cool No Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_VAV_NoReheat, tr("AirTerminal Single Duct VAV NoReheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_SingleDuct_ConstantVolume_NoReheat,
                                    tr("AirTerminal Single Duct Constant Volume No Reheat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_ConstantVolume, tr("Air Terminal Dual Duct Constant Volume"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_VAV, tr("Air Terminal Dual Duct VAV"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirTerminal_DualDuct_VAV_OutdoorAir, tr("Air Terminal Dual Duct VAV Outdoor Air"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Air Terminals"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_OutdoorAirSystem, tr("AirLoopHVAC Outdoor Air System"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitaryHeatPump_AirToAir_MultiSpeed,
                                    tr("AirLoopHVAC Unitary Heat Pump AirToAir MultiSpeed"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitarySystem, tr("AirLoopHVAC Unitary System"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AirLoopHVAC_UnitaryHeatCool_VAVChangeoverBypass,
                                    tr("AirLoopHVAC Unitary VAV Changeover Bypass"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Air Loop HVAC"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_Scheduled, tr("Availability Manager Scheduled"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_ScheduledOn, tr("Availability Manager Scheduled On"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_ScheduledOff, tr("Availability Manager Scheduled Off"));

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_LowTemperatureTurnOn, tr("Availability Manager Low Temperature Turn On"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_LowTemperatureTurnOff, tr("Availability Manager Low Temperature Turn Off"));

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HighTemperatureTurnOn, tr("Availability Manager High Temperature Turn On"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HighTemperatureTurnOff,
                                    tr("Availability Manager High Temperature Turn Off"));

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_DifferentialThermostat, tr("Availability Manager Differential Thermostat"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_OptimumStart, tr("Availability Manager Optimum Start"));

  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_NightCycle, tr("Availability Manager Night Cycle"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_NightVentilation, tr("Availability Manager Night Ventilation"));
  libraryWidget->addModelObjectType(IddObjectType::OS_AvailabilityManager_HybridVentilation, tr("Availability Manager Hybrid Ventilation"));
  libraryWidget->addModelObjectCategoryPlaceholder(tr("Availability Managers"));

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
