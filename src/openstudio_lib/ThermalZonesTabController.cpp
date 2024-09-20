/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ThermalZonesTabController.hpp"

#include "OSItem.hpp"
#include "ThermalZonesController.hpp"
#include "ThermalZonesTabView.hpp"
#include "ThermalZonesView.hpp"
#include <memory>

namespace openstudio {

ThermalZonesTabController::ThermalZonesTabController(bool isIP, bool displayAdditionalProps, const model::Model& model)
  : MainTabController(new ThermalZonesTabView()),
    m_thermalZonesController(std::make_shared<ThermalZonesController>(isIP, displayAdditionalProps, model)) {
  this->mainContentWidget()->addTabWidget(m_thermalZonesController->subTabView());

  connect(this, &ThermalZonesTabController::itemRemoveClicked, m_thermalZonesController.get(), &ThermalZonesController::removeItem);

  connect(m_thermalZonesController.get(), &ThermalZonesController::modelObjectSelected, this, &ThermalZonesTabController::modelObjectSelected);

  connect(m_thermalZonesController.get(), &ThermalZonesController::dropZoneItemSelected, this, &ThermalZonesTabController::dropZoneItemSelected);

  connect(this, &ThermalZonesTabController::toggleUnitsClicked, m_thermalZonesController.get(), &ThermalZonesController::toggleUnitsClicked);

  connect(this, &ThermalZonesTabController::toggleDisplayAdditionalPropsClicked, m_thermalZonesController.get(),
          &ThermalZonesController::toggleDisplayAdditionalPropsClicked);
}

}  // namespace openstudio
