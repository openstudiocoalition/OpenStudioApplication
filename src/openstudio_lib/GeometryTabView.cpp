/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GeometryTabView.hpp"

namespace openstudio {

GeometryTabView::GeometryTabView(const model::Model& model, const QString& tabLabel, QWidget* parent)
  : MainTabView(tabLabel, MainTabView::SUB_TAB, parent) {}

}  // namespace openstudio
