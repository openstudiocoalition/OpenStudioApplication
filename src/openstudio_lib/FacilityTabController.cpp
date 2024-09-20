/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "FacilityTabController.hpp"

#include "BuildingInspectorView.hpp"
#include "FacilityExteriorEquipmentGridView.hpp"
#include "FacilityShadingGridView.hpp"
#include "FacilityStoriesGridView.hpp"
#include "FacilityTabView.hpp"

namespace openstudio {

FacilityTabController::FacilityTabController(bool isIP, bool displayAdditionalProps, const model::Model& model)
  : MainTabController(new FacilityTabView()), m_model(model), m_isIP(isIP), m_displayAdditionalProps(displayAdditionalProps) {
  mainContentWidget()->addSubTab("Building", BUILDING);
  mainContentWidget()->addSubTab("Stories", STORIES);
  mainContentWidget()->addSubTab("Shading", SHADING);
  mainContentWidget()->addSubTab("Exterior Equipment", EXTERIOR_EQUIPMENT);

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &FacilityTabController::setSubTab);

  connect(this, &FacilityTabController::toggleUnitsClicked, this, &FacilityTabController::toggleUnits);
  connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, this, &FacilityTabController::toggleDisplayAdditionalProps);
}

FacilityTabController::~FacilityTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &FacilityTabController::setSubTab);
}

void FacilityTabController::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

void FacilityTabController::toggleDisplayAdditionalProps(bool displayAdditionalProps) {
  m_displayAdditionalProps = displayAdditionalProps;
}

void FacilityTabController::setSubTab(int index) {
  if (m_currentIndex == index) {
    return;
  } else {
    m_currentIndex = index;
  }

  if (m_currentView) {
    m_currentView->disconnect();
    delete m_currentView;
  }

  switch (index) {
    case 0: {
      auto* buildingInspectorView = new BuildingInspectorView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &FacilityTabController::toggleUnitsClicked, buildingInspectorView, &BuildingInspectorView::toggleUnitsClicked);
      connect(buildingInspectorView, &BuildingInspectorView::dropZoneItemClicked, this, &FacilityTabController::dropZoneItemClicked);
      connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, buildingInspectorView,
              &BuildingInspectorView::toggleDisplayAdditionalPropsClicked);

      connect(this, &FacilityTabController::toggleUnitsClicked, buildingInspectorView, &BuildingInspectorView::toggleUnits);
      connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, buildingInspectorView,
              &BuildingInspectorView::toggleDisplayAdditionalProps);
      this->mainContentWidget()->setSubTab(buildingInspectorView);
      m_currentView = buildingInspectorView;
      break;
    }
    case 1: {
      auto* facilityStoriesGridView = new FacilityStoriesGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &FacilityTabController::toggleUnitsClicked, facilityStoriesGridView, &FacilityStoriesGridView::toggleUnitsClicked);
      connect(facilityStoriesGridView, &FacilityStoriesGridView::dropZoneItemSelected, this, &FacilityTabController::dropZoneItemSelected);
      connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, facilityStoriesGridView,
              &FacilityStoriesGridView::toggleDisplayAdditionalPropsClicked);
      this->mainContentWidget()->setSubTab(facilityStoriesGridView);
      m_currentView = facilityStoriesGridView;
      break;
    }
    case 2: {
      auto* facilityShadingGridView = new FacilityShadingGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &FacilityTabController::toggleUnitsClicked, facilityShadingGridView, &FacilityShadingGridView::toggleUnitsClicked);
      connect(facilityShadingGridView, &FacilityShadingGridView::dropZoneItemSelected, this, &FacilityTabController::dropZoneItemSelected);
      connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, facilityShadingGridView,
              &FacilityShadingGridView::toggleDisplayAdditionalPropsClicked);
      this->mainContentWidget()->setSubTab(facilityShadingGridView);
      m_currentView = facilityShadingGridView;
      break;
    }
    case 3: {
      auto* facilityExteriorEquipmentGridView = new FacilityExteriorEquipmentGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &FacilityTabController::toggleUnitsClicked, facilityExteriorEquipmentGridView,
              &FacilityExteriorEquipmentGridView::toggleUnitsClicked);
      connect(facilityExteriorEquipmentGridView, &FacilityExteriorEquipmentGridView::dropZoneItemSelected, this,
              &FacilityTabController::dropZoneItemSelected);
      connect(this, &FacilityTabController::toggleDisplayAdditionalPropsClicked, facilityExteriorEquipmentGridView,
              &FacilityExteriorEquipmentGridView::toggleDisplayAdditionalPropsClicked);
      this->mainContentWidget()->setSubTab(facilityExteriorEquipmentGridView);
      m_currentView = facilityExteriorEquipmentGridView;
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

}  // namespace openstudio
