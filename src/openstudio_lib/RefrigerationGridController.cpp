/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "RefrigerationGridController.hpp"

#include "RefrigerationGridView.hpp"

namespace openstudio {

RefrigerationGridController::RefrigerationGridController(bool isIP, const model::Model& model)
  : QObject(), m_refrigerationGridView(new RefrigerationGridView(isIP, model)) {}

RefrigerationGridController::~RefrigerationGridController() = default;

RefrigerationGridView* RefrigerationGridController::refrigerationGridView() const {
  return m_refrigerationGridView;
}

}  // namespace openstudio
