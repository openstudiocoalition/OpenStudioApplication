/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoadsTabView.hpp"

namespace openstudio {

LoadsTabView::LoadsTabView(const model::Model& model, const QString& tabLabel, QWidget* parent)
  : MainTabView(tabLabel, MainTabView::MAIN_TAB, parent) {}

void LoadsTabView::toggleUnits(bool displayIP) {}

}  // namespace openstudio
