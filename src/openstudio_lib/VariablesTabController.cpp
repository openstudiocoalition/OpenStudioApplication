/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "VariablesTabController.hpp"
#include "VariablesTabView.hpp"

namespace openstudio {

VariablesTabController::VariablesTabController(const model::Model& model) : MainTabController(new VariablesTabView(model)) {}

}  // namespace openstudio
