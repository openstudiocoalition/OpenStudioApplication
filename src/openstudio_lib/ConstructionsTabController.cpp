/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionsTabController.hpp"

#include "ConstructionsController.hpp"
#include "ConstructionsTabView.hpp"
#include "DefaultConstructionSetsController.hpp"
#include "MaterialsController.hpp"

#include <openstudio/model/Construction.hpp>
#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

ConstructionsTabController::ConstructionsTabController(bool isIP, const model::Model& model)
  : MainTabController(new ConstructionsTabView(model, "Constructions")), m_model(model), m_isIP(isIP) {
  this->mainContentWidget()->addSubTab("Construction Sets", DEFAULT_CONSTRUCTIONS);
  this->mainContentWidget()->addSubTab("Constructions", CONSTRUCTIONS);
  this->mainContentWidget()->addSubTab("Materials", MATERIALS);

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &ConstructionsTabController::setSubTab);
  connect(this, &ConstructionsTabController::toggleUnitsClicked, this, &ConstructionsTabController::toggleUnits);
}

ConstructionsTabController::~ConstructionsTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &ConstructionsTabController::setSubTab);
}

void ConstructionsTabController::setSubTab(int index) {
  if (m_currentIndex == index) {
    return;
  } else {
    m_currentIndex = index;
  }

  if (m_currentController) {
    m_currentController->disconnect();
    delete m_currentController;
  }

  switch (index) {
    case 0: {
      auto* defaultConstructionSetsController = new DefaultConstructionSetsController(m_model);
      connect(this, &ConstructionsTabController::toggleUnitsClicked, static_cast<ModelSubTabView*>(defaultConstructionSetsController->subTabView()),
              &ModelSubTabView::toggleUnitsClicked);
      connect(defaultConstructionSetsController, &DefaultConstructionSetsController::downloadComponentsClicked, this,
              &ConstructionsTabController::downloadComponentsClicked);
      connect(defaultConstructionSetsController, &DefaultConstructionSetsController::openLibDlgClicked, this,
              &ConstructionsTabController::openLibDlgClicked);
      this->mainContentWidget()->setSubTab(defaultConstructionSetsController->subTabView());
      m_currentController = defaultConstructionSetsController;
      break;
    }

    case 1: {
      auto* constructionsController = new ConstructionsController(m_isIP, m_model);
      connect(this, &ConstructionsTabController::toggleUnitsClicked, static_cast<ModelSubTabView*>(constructionsController->subTabView()),
              &ModelSubTabView::toggleUnitsClicked);
      this->mainContentWidget()->setSubTab(constructionsController->subTabView());
      m_currentController = constructionsController;
      break;
    }

    case 2: {
      auto* materialsController = new MaterialsController(m_isIP, m_model);
      connect(this, &ConstructionsTabController::toggleUnitsClicked, static_cast<ModelSubTabView*>(materialsController->subTabView()),
              &ModelSubTabView::toggleUnitsClicked);
      this->mainContentWidget()->setSubTab(materialsController->subTabView());
      m_currentController = materialsController;
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

/****************** SLOTS ******************/

void ConstructionsTabController::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
