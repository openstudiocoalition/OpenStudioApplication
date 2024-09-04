/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LocationTabController.hpp"

#include "LifeCycleCostsTabView.hpp"
#include "LocationTabView.hpp"
#include "UtilityBillsView.hpp"
#include "UtilityBillsController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/RunPeriod.hpp>
#include <openstudio/model/RunPeriod_Impl.hpp>
#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/model/WeatherFile_Impl.hpp>
#include <openstudio/model/YearDescription.hpp>
#include <openstudio/model/YearDescription_Impl.hpp>

#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

LocationTabController::LocationTabController(bool isIP, const model::Model& model, const QString& modelTempDir)
  : MainTabController(new LocationTabView(model, modelTempDir)), m_modelTempDir(modelTempDir), m_model(model), m_isIP(isIP) {
  mainContentWidget()->addSubTab(tr("Weather File && Design Days"), WEATHER_FILE);
  mainContentWidget()->addSubTab(tr("Life Cycle Costs"), LIFE_CYCLE_COSTS);
  mainContentWidget()->addSubTab(tr("Utility Bills"), UTILITY_BILLS);

  // setSubTab(0);
  auto* locationView = new LocationView(m_isIP, m_model, m_modelTempDir);
  connect(this, &LocationTabController::toggleUnitsClicked, locationView, &LocationView::toggleUnitsClicked);
  this->mainContentWidget()->setSubTab(locationView);
  m_currentView = locationView;

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &LocationTabController::setSubTab);
}

LocationTabController::~LocationTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &LocationTabController::setSubTab);
}

bool LocationTabController::showUtilityBills() {
  // Determine if the utility bill sub-tab is shown
  boost::optional<model::YearDescription> yearDescription = m_model.yearDescription();
  if (yearDescription) {
    boost::optional<int> calendarYear = yearDescription.get().calendarYear();
    if (calendarYear) {
      boost::optional<model::WeatherFile> weatherFile = m_model.weatherFile();
      if (weatherFile) {
        boost::optional<model::RunPeriod> runPeriod = m_model.getOptionalUniqueModelObject<model::RunPeriod>();
        return runPeriod.is_initialized();
      }
    }
  }
  // Oops, missing some needed object above, so default to warning
  return false;
}

void LocationTabController::setSubTab(int index) {
  if (m_currentIndex == index) {
    return;
  }

  m_currentIndex = index;

  if (m_currentView != nullptr) {
    m_currentView->disconnect();
    delete m_currentView;
  }

  switch (index) {
    case 0: {
      auto* locationView = new LocationView(m_isIP, m_model, m_modelTempDir);
      connect(this, &LocationTabController::toggleUnitsClicked, locationView, &LocationView::toggleUnitsClicked);
      this->mainContentWidget()->setSubTab(locationView);
      m_currentView = locationView;
      break;
    }
    case 1: {
      auto* lifeCycleCostsView = new LifeCycleCostsView(m_model);
      this->mainContentWidget()->setSubTab(lifeCycleCostsView);
      m_currentView = lifeCycleCostsView;
      break;
    }
    case 2: {
      if (showUtilityBills()) {
        auto* utilityBillsController = new UtilityBillsController(m_model);
        this->mainContentWidget()->setSubTab(utilityBillsController->subTabView());
        m_currentView = utilityBillsController->subTabView()->inspectorView();
      } else {
        auto* label = new QLabel();
        label->setPixmap(QPixmap(":/images/utility_calibration_warning.png"));
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        this->mainContentWidget()->setSubTab(label);
        m_currentView = label;
      }
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

}  // namespace openstudio
