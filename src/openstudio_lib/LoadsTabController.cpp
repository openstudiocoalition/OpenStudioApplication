/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoadsTabController.hpp"
#include "LoadsTabView.hpp"
#include "LoadsController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Construction.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

LoadsTabController::LoadsTabController(bool isIP, const model::Model& model) : MainTabController(new LoadsTabView(model, "Loads")) {
  m_LoadsController = std::shared_ptr<LoadsController>(new LoadsController(isIP, model));
  this->mainContentWidget()->addTabWidget(m_LoadsController->subTabView());

  connect(this, &LoadsTabController::toggleUnitsClicked, m_LoadsController.get(), &LoadsController::toggleUnitsClicked);

  connect(m_LoadsController.get(), &LoadsController::downloadComponentsClicked, this, &LoadsTabController::downloadComponentsClicked);

  connect(m_LoadsController.get(), &LoadsController::openLibDlgClicked, this, &LoadsTabController::openLibDlgClicked);
}

LoadsTabController::~LoadsTabController() = default;

void LoadsTabController::toggleUnits(bool displayIP) {}

}  // namespace openstudio
