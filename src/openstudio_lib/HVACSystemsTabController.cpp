/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HVACSystemsTabController.hpp"
#include "HVACSystemsController.hpp"
#include "HVACSystemsView.hpp"
#include "HVACSystemsTabView.hpp"

namespace openstudio {

HVACSystemsTabController::HVACSystemsTabController(bool isIP, const model::Model& model)
  : MainTabController(new HVACSystemsTabView()), m_hvacSystemsController(std::make_shared<HVACSystemsController>(isIP, model)), m_isIP(isIP) {

  connect(this, &HVACSystemsTabController::toggleUnitsClicked, m_hvacSystemsController.get(), &HVACSystemsController::toggleUnitsClicked);

  connect(this, &HVACSystemsTabController::toggleUnitsClicked, this, &HVACSystemsTabController::toggleUnits);

  this->mainContentWidget()->addTabWidget(m_hvacSystemsController->hvacSystemsView());
}

void HVACSystemsTabController::clearSceneSelection() {
  m_hvacSystemsController->clearSceneSelection();
}

void HVACSystemsTabController::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
