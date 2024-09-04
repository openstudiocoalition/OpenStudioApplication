/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ResultsTabController.hpp"
#include "ResultsTabView.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

namespace openstudio {

ResultsTabController::ResultsTabController() : MainTabController(new ResultsTabView("Results Summary", MainTabView::MAIN_TAB)) {
  m_resultsTabView = static_cast<ResultsTabView*>(mainContentWidget());
}

void ResultsTabController::onUnitSystemChange(bool t_isIP) {
  LOG(Debug, "onUnitSystemChange isIP " << t_isIP);
  m_resultsTabView->onUnitSystemChange(t_isIP);
}

}  // namespace openstudio
