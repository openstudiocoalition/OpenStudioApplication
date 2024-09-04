/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SummaryTabView.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QLabel>

namespace openstudio {

SummaryTabView::SummaryTabView(const model::Model& model, QWidget* parent) : MainTabView("Building Summary", MainTabView::MAIN_TAB, parent) {
  // Hack code to remove when tab active
  auto* underConstructionLabel = new QLabel();
  underConstructionLabel->setPixmap(QPixmap(":/images/coming_soon_building_summary.png"));
  underConstructionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  bool success = this->addTabWidget(underConstructionLabel);
  OS_ASSERT(success);
}

}  // namespace openstudio
