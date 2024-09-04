/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesTabController.hpp"

#include "SpacesDaylightingGridView.hpp"
#include "SpacesInteriorPartitionsGridView.hpp"
#include "SpacesLoadsGridView.hpp"
#include "SpacesShadingGridView.hpp"
#include "SpacesSpacesGridView.hpp"
#include "SpacesSubsurfacesGridView.hpp"
#include "SpacesSurfacesGridView.hpp"
#include "SpacesTabView.hpp"

namespace openstudio {

SpacesTabController::SpacesTabController(bool isIP, bool displayAdditionalProps, const model::Model& model)
  : MainTabController(new SpacesTabView()), m_model(model), m_isIP(isIP), m_displayAdditionalProps(displayAdditionalProps) {
  mainContentWidget()->addSubTab("Properties", SPACES);
  mainContentWidget()->addSubTab("Loads", LOADS);
  mainContentWidget()->addSubTab("Surfaces", SURFACES);
  mainContentWidget()->addSubTab("Subsurfaces", SUBSURFACES);
  mainContentWidget()->addSubTab("Interior Partitions", INTERIOR_PARTITIONS);
  mainContentWidget()->addSubTab("Shading", SHADING);

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &SpacesTabController::setSubTab);

  // Evan note: TODO SpacesDaylightingGridView may be a dead class, given the latest Radiance plans
  //this->mainContentWidget()->addSubTab("Daylighting", new SpacesDaylightingGridView(m_isIP, m_model), DAYLIGHTING);
}

SpacesTabController::~SpacesTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &SpacesTabController::setSubTab);
}

void SpacesTabController::toggleUnits(bool displayIP) {}

void SpacesTabController::setSubTab(int index) {
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
      auto* spacesSpacesGridView = new SpacesSpacesGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesSpacesGridView, &SpacesSpacesGridView::toggleUnitsClicked);
      connect(spacesSpacesGridView, &SpacesSpacesGridView::dropZoneItemSelected, this, &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesSpacesGridView,
              &SpacesSpacesGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesSpacesGridView);
      m_currentView = spacesSpacesGridView;
      break;
    }
    case 1: {
      auto* spacesLoadsGridView = new SpacesLoadsGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesLoadsGridView, &SpacesLoadsGridView::toggleUnitsClicked);
      connect(spacesLoadsGridView, &SpacesLoadsGridView::dropZoneItemSelected, this, &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesLoadsGridView,
              &SpacesLoadsGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesLoadsGridView);
      m_currentView = spacesLoadsGridView;
      break;
    }
    case 2: {
      auto* spacesSurfacesGridView = new SpacesSurfacesGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesSurfacesGridView, &SpacesSurfacesGridView::toggleUnitsClicked);
      connect(spacesSurfacesGridView, &SpacesSurfacesGridView::dropZoneItemSelected, this, &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesSurfacesGridView,
              &SpacesSurfacesGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesSurfacesGridView);
      m_currentView = spacesSurfacesGridView;
      break;
    }
    case 3: {
      auto* spacesSubsurfacesGridView = new SpacesSubsurfacesGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesSubsurfacesGridView, &SpacesSubsurfacesGridView::toggleUnitsClicked);
      connect(spacesSubsurfacesGridView, &SpacesSubsurfacesGridView::dropZoneItemSelected, this, &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesSubsurfacesGridView,
              &SpacesSubsurfacesGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesSubsurfacesGridView);
      m_currentView = spacesSubsurfacesGridView;
      break;
    }
    case 4: {
      auto* spacesInteriorPartitionsGridView = new SpacesInteriorPartitionsGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesInteriorPartitionsGridView,
              &SpacesInteriorPartitionsGridView::toggleUnitsClicked);
      connect(spacesInteriorPartitionsGridView, &SpacesInteriorPartitionsGridView::dropZoneItemSelected, this,
              &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesInteriorPartitionsGridView,
              &SpacesInteriorPartitionsGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesInteriorPartitionsGridView);
      m_currentView = spacesInteriorPartitionsGridView;
      break;
    }
    case 5: {
      auto* spacesShadingGridView = new SpacesShadingGridView(m_isIP, m_displayAdditionalProps, m_model);
      connect(this, &SpacesTabController::toggleUnitsClicked, spacesShadingGridView, &SpacesShadingGridView::toggleUnitsClicked);
      connect(spacesShadingGridView, &SpacesShadingGridView::dropZoneItemSelected, this, &SpacesTabController::dropZoneItemSelected);
      connect(this, &SpacesTabController::toggleDisplayAdditionalPropsClicked, spacesShadingGridView,
              &SpacesShadingGridView::toggleDisplayAdditionalPropsClicked);
      mainContentWidget()->setSubTab(spacesShadingGridView);
      m_currentView = spacesShadingGridView;
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

}  // namespace openstudio
