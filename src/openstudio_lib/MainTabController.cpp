/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MainTabController.hpp"

#include "MainTabView.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

MainTabController::MainTabController(MainTabView* mainContentWidget) : OSQObjectController() {
  m_mainContentWidget = mainContentWidget;

  addQObject(mainContentWidget);
}

MainTabController::~MainTabController() = default;

MainTabView* MainTabController::mainContentWidget() const {
  return m_mainContentWidget;
}

}  // namespace openstudio
