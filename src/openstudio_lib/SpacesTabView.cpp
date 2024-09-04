/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesTabView.hpp"

namespace openstudio {

SpacesTabView::SpacesTabView(QWidget* parent) : MainTabView("Spaces", MainTabView::GRIDVIEW_SUB_TAB, parent) {}

void SpacesTabView::toggleUnits(bool displayIP) {}

}  // namespace openstudio
