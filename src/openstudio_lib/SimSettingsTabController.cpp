/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SimSettingsTabController.hpp"

#include "SimSettingsTabView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

SimSettingsTabController::SimSettingsTabController(bool isIP, const model::Model& model)
  : MainTabController(new SimSettingsTabView(isIP, model, "Simulation Settings", MainTabView::MAIN_TAB)) {
  connect(this, &SimSettingsTabController::toggleUnitsClicked, mainContentWidget(), &MainTabView::toggleUnitsClicked);
}

void SimSettingsTabController::toggleUnits(bool displayIP) {}

}  // namespace openstudio
