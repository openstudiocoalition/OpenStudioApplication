/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "FacilityTabView.hpp"

namespace openstudio {

FacilityTabView::FacilityTabView(QWidget* parent) : MainTabView("Facility", MainTabView::GRIDVIEW_SUB_TAB, parent) {}

void FacilityTabView::toggleUnits(bool displayIP) {}

}  // namespace openstudio
