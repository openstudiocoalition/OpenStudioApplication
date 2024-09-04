/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionsTabView.hpp"

namespace openstudio {

ConstructionsTabView::ConstructionsTabView(const model::Model& model, const QString& tabLabel, QWidget* parent)
  : MainTabView(tabLabel, MainTabView::SUB_TAB, parent) {}

}  // namespace openstudio
