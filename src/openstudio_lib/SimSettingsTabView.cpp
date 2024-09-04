/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SimSettingsTabView.hpp"

#include "SimSettingsView.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QScrollArea>

namespace openstudio {

SimSettingsTabView::SimSettingsTabView(bool isIP, const model::Model& model, const QString& tabLabel, TabType tabType, QWidget* parent)
  : MainTabView(tabLabel, tabType, parent), m_simSettingsView(new SimSettingsView(isIP, model, parent)) {
  auto* scrollarea = new QScrollArea();
  scrollarea->setWidget(m_simSettingsView);
  scrollarea->setWidgetResizable(true);
  addTabWidget(scrollarea);
  m_simSettingsView->setAutoFillBackground(false);

  connect(this, &SimSettingsTabView::toggleUnitsClicked, m_simSettingsView, &SimSettingsView::toggleUnitsClicked);
}

void SimSettingsTabView::toggleUnits(bool displayIP) {}

}  // namespace openstudio
