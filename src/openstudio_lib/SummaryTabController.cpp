/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SummaryTabController.hpp"
#include "SummaryTabView.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

namespace openstudio {

SummaryTabController::SummaryTabController(const model::Model& model) : MainTabController(new SummaryTabView(model)) {}

}  // namespace openstudio
