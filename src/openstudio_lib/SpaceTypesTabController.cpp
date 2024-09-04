/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpaceTypesTabController.hpp"

#include "OSItem.hpp"
#include "SpaceTypeInspectorView.hpp"
#include "SpaceTypesController.hpp"
#include "SpaceTypesTabView.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

SpaceTypesTabController::SpaceTypesTabController(bool isIP, const model::Model& model) : MainTabController(new SpaceTypesTabView()) {
  m_spaceTypesController = std::shared_ptr<SpaceTypesController>(new SpaceTypesController(isIP, model));

  connect(m_spaceTypesController.get(), &SpaceTypesController::modelObjectSelected, this, &SpaceTypesTabController::modelObjectSelected);

  connect(m_spaceTypesController.get(), &SpaceTypesController::dropZoneItemSelected, this, &SpaceTypesTabController::dropZoneItemSelected);

  connect(m_spaceTypesController.get(), &SpaceTypesController::downloadComponentsClicked, this, &SpaceTypesTabController::downloadComponentsClicked);

  connect(m_spaceTypesController.get(), &SpaceTypesController::openLibDlgClicked, this, &SpaceTypesTabController::openLibDlgClicked);

  auto* spaceTypeInspectorView = qobject_cast<SpaceTypeInspectorView*>(m_spaceTypesController->subTabView()->inspectorView());
  OS_ASSERT(spaceTypeInspectorView);

  connect(this, &SpaceTypesTabController::itemRemoveClicked, m_spaceTypesController.get(), &SpaceTypesController::removeItem);

  mainContentWidget()->addTabWidget(m_spaceTypesController->subTabView());
}

}  // namespace openstudio
