/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SchedulesTabController.hpp"

#include "MainTabView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItem.hpp"
#include "ScheduleDialog.hpp"
#include "ScheduleSetsController.hpp"
#include "ScheduleSetsView.hpp"
#include "SchedulesTabView.hpp"
#include "SchedulesView.hpp"
#include "ScheduleDayView.hpp"
#include "ScheduleOthersController.hpp"
#include "ScheduleOthersView.hpp"
#include "SubTabView.hpp"

#include <model/ScheduleCompact.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ScheduleRule.hpp>
#include <openstudio/model/ScheduleRuleset.hpp>
#include <openstudio/model/ScheduleRuleset_Impl.hpp>
#include <openstudio/model/ScheduleRule_Impl.hpp>
#include <openstudio/model/ScheduleTypeLimits.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idf/IdfFile.hpp>
#include <openstudio/utilities/time/Date.hpp>
#include <openstudio/utilities/time/Time.hpp>
#include <openstudio/utilities/units/Unit.hpp>
#include <openstudio/utilities/units/OSOptionalQuantity.hpp>
#include <openstudio/utilities/units/Quantity.hpp>
#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <openstudio/energyplus/ReverseTranslator.hpp>

#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QString>

namespace openstudio {

SchedulesTabController::SchedulesTabController(bool isIP, const model::Model& model)
  : MainTabController(new SchedulesTabView(model)), m_model(model), m_isIP(isIP) {
  mainContentWidget()->addSubTab("Schedule Sets", SCHEDULE_SETS);
  mainContentWidget()->addSubTab("Schedules", SCHEDULES);
  mainContentWidget()->addSubTab("Other Schedules", SCHEDULESOTHER);

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &SchedulesTabController::setSubTab);
}

SchedulesTabController::~SchedulesTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &SchedulesTabController::setSubTab);
}

void SchedulesTabController::addScheduleRuleset() {
  showScheduleDialog();
}

void SchedulesTabController::showScheduleDialog() {
  if (!m_scheduleDialog) {
    m_scheduleDialog = new ScheduleDialog(m_isIP, m_model);

    connect(this, &SchedulesTabController::toggleUnitsClicked, m_scheduleDialog, &ScheduleDialog::toggleUnitsClicked);
  }
  m_scheduleDialog->show();
}

void SchedulesTabController::copySelectedSchedule() {
  if (qobject_cast<SchedulesView*>(m_currentView)) {
    if (boost::optional<model::ScheduleRuleset> schedule = (qobject_cast<SchedulesView*>(m_currentView))->currentSchedule()) {
      schedule->clone();
    }
  }
}

void SchedulesTabController::removeSelectedSchedule() {
  if (qobject_cast<SchedulesView*>(m_currentView)) {
    if (boost::optional<model::ScheduleRuleset> schedule = (qobject_cast<SchedulesView*>(m_currentView))->currentSchedule()) {
      schedule->remove();
    }
  }
}

void SchedulesTabController::purgeUnusedScheduleRulesets() {
  std::vector<model::ScheduleRuleset> schedules = m_model.getConcreteModelObjects<model::ScheduleRuleset>();

  for (auto it = schedules.begin(); it != schedules.end(); ++it) {
    if (it->directUseCount(true) == 0) {
      it->remove();
    }
  }
}

void SchedulesTabController::addRule(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle) {
  boost::optional<model::ScheduleRule> rule;
  if (!scheduleDayHandle.isNull()) {
    boost::optional<model::ScheduleDay> scheduleDayToCopy = scheduleRuleset.model().getModelObject<model::ScheduleDay>(scheduleDayHandle);
    if (scheduleDayToCopy) {
      rule = model::ScheduleRule(scheduleRuleset, scheduleDayToCopy->clone().cast<model::ScheduleDay>());
    }
  }
  if (!rule) {
    rule = model::ScheduleRule(scheduleRuleset);
    rule->daySchedule().addValue(Time(1, 0), defaultStartingValue(rule->daySchedule()));
  }
  OS_ASSERT(rule);
}

void SchedulesTabController::addSummerProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle) {
  boost::optional<model::ScheduleDay> scheduleDay;
  if (!scheduleDayHandle.isNull()) {
    boost::optional<model::ScheduleDay> scheduleDayToCopy = scheduleRuleset.model().getModelObject<model::ScheduleDay>(scheduleDayHandle);
    if (scheduleDayToCopy) {
      scheduleDay = scheduleDayToCopy->clone().cast<model::ScheduleDay>();
    }
  }
  if (!scheduleDay) {
    scheduleDay = model::ScheduleDay(scheduleRuleset.model());
    boost::optional<model::ScheduleTypeLimits> limits = scheduleRuleset.scheduleTypeLimits();
    if (limits) {
      scheduleDay->setScheduleTypeLimits(*limits);
    }
    scheduleDay->addValue(Time(1, 0), defaultStartingValue(*scheduleDay));
  }
  OS_ASSERT(scheduleDay);

  scheduleRuleset.setSummerDesignDaySchedule(*scheduleDay);

  scheduleDay->remove();

  if (qobject_cast<SchedulesView*>(m_currentView)) {
    (qobject_cast<SchedulesView*>(m_currentView))->showSummerScheduleDay(scheduleRuleset);
  }
}

void SchedulesTabController::addWinterProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle) {
  boost::optional<model::ScheduleDay> scheduleDay;
  if (!scheduleDayHandle.isNull()) {
    boost::optional<model::ScheduleDay> scheduleDayToCopy = scheduleRuleset.model().getModelObject<model::ScheduleDay>(scheduleDayHandle);
    if (scheduleDayToCopy) {
      scheduleDay = scheduleDayToCopy->clone().cast<model::ScheduleDay>();
    }
  }
  if (!scheduleDay) {
    scheduleDay = model::ScheduleDay(scheduleRuleset.model());
    boost::optional<model::ScheduleTypeLimits> limits = scheduleRuleset.scheduleTypeLimits();
    if (limits) {
      scheduleDay->setScheduleTypeLimits(*limits);
    }
    scheduleDay->addValue(Time(1, 0), defaultStartingValue(*scheduleDay));
  }
  OS_ASSERT(scheduleDay);

  scheduleRuleset.setWinterDesignDaySchedule(*scheduleDay);

  scheduleDay->remove();

  if (qobject_cast<SchedulesView*>(m_currentView)) {
    (qobject_cast<SchedulesView*>(m_currentView))->showWinterScheduleDay(scheduleRuleset);
  }
}

void SchedulesTabController::addHolidayProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle) {
  boost::optional<model::ScheduleDay> scheduleDay;
  if (!scheduleDayHandle.isNull()) {
    boost::optional<model::ScheduleDay> scheduleDayToCopy = scheduleRuleset.model().getModelObject<model::ScheduleDay>(scheduleDayHandle);
    if (scheduleDayToCopy) {
      scheduleDay = scheduleDayToCopy->clone().cast<model::ScheduleDay>();
    }
  }
  if (!scheduleDay) {
    scheduleDay = model::ScheduleDay(scheduleRuleset.model());
    boost::optional<model::ScheduleTypeLimits> limits = scheduleRuleset.scheduleTypeLimits();
    if (limits) {
      scheduleDay->setScheduleTypeLimits(*limits);
    }
    scheduleDay->addValue(Time(1, 0), defaultStartingValue(*scheduleDay));
  }
  OS_ASSERT(scheduleDay);

  scheduleRuleset.setHolidaySchedule(*scheduleDay);

  scheduleDay->remove();

  if (qobject_cast<SchedulesView*>(m_currentView)) {
    (qobject_cast<SchedulesView*>(m_currentView))->showHolidayScheduleDay(scheduleRuleset);
  }
}

void SchedulesTabController::onDayScheduleSceneChanged(DayScheduleScene* scene, double lowerLimitValue, double upperLimitValue) {
  std::vector<CalendarSegmentItem*> segments = scene->segments();

  model::ScheduleDay scheduleDay = scene->scheduleDay();

  OptionalUnit units = scene->scheduleDayView()->units();

  scheduleDay.clearValues();

  for (auto it = segments.begin(); it < segments.end(); ++it) {
    openstudio::Time time(0, 0, 0, (*it)->endTime());

    double scaledvalue = (*it)->value();

    double value = lowerLimitValue + scaledvalue * (upperLimitValue - lowerLimitValue);

    if (units) {
      // Now, if we need and can convert, we do it
      if (boost::optional<model::ScheduleTypeLimits> _scheduleTypeLimits = scheduleDay.scheduleTypeLimits()) {

        boost::optional<Unit> _siUnits = _scheduleTypeLimits->units(false);
        if (units.get() != _siUnits.get()) {
          Quantity q = Quantity(value, units.get());
          OptionalQuantity result = openstudio::convert(q, _siUnits.get());
          OS_ASSERT(result);
          value = result.get().value();
        }
      }
    }

    scheduleDay.addValue(time, value);
  }
}

void SchedulesTabController::onStartDateTimeChanged(model::ScheduleRule& scheduleRule, const QDateTime& newDate) {
  openstudio::Date date(newDate.date().month(), newDate.date().day(), newDate.date().year());

  if (scheduleRule.startDate()) {
    if (date != scheduleRule.startDate().get()) {
      scheduleRule.setStartDate(date);
    }
  } else {
    scheduleRule.setStartDate(date);
  }
}

void SchedulesTabController::onEndDateTimeChanged(model::ScheduleRule& scheduleRule, const QDateTime& newDate) {
  openstudio::Date date(newDate.date().month(), newDate.date().day(), newDate.date().year());

  if (scheduleRule.endDate()) {
    if (date != scheduleRule.endDate().get()) {
      scheduleRule.setEndDate(date);
    }
  } else {
    scheduleRule.setEndDate(date);
  }
}

void SchedulesTabController::removeScheduleRule(model::ScheduleRule& scheduleRule) {
  // TODO: why is this calling Impl and not doing anything with it?
  scheduleRule.getImpl<openstudio::model::detail::ScheduleRule_Impl>();

  // Store current ruleIndex
  int ruleIndex = scheduleRule.ruleIndex();
  model::ScheduleRuleset scheduleRuleset = scheduleRule.scheduleRuleset();

  scheduleRule.remove();

  std::vector<model::ScheduleRule> rules = scheduleRuleset.scheduleRules();
  int n_rules = static_cast<int>(rules.size());

  if (n_rules == 0) {
    // Set the new displayed to the defaultDaySchedule
    qobject_cast<SchedulesView*>(m_currentView)->showDefaultScheduleDay(scheduleRuleset);
  } else if (ruleIndex <= (n_rules - 1)) {
    // Set the new displayed ScheduleRule to the same index (will display the priority right below the one we just deleted)
    if (qobject_cast<SchedulesView*>(m_currentView)) {
      qobject_cast<SchedulesView*>(m_currentView)->showScheduleRule(rules[ruleIndex]);
    }
  } else {
    // Set the new displayed ScheduleRule to the last one
    qobject_cast<SchedulesView*>(m_currentView)->showScheduleRule(rules[n_rules - 1]);
  }
}

void SchedulesTabController::onItemDropped(const OSItemId& itemId) {
  if (OSAppBase::instance()->currentDocument()->fromComponentLibrary(itemId)) {
    if (boost::optional<model::ModelObject> mo = OSAppBase::instance()->currentDocument()->getModelObject(itemId)) {
      if (boost::optional<model::ScheduleRuleset> s = mo->optionalCast<model::ScheduleRuleset>()) {
        model::Model m = OSAppBase::instance()->currentDocument()->model();

        s->clone(m);
      }
    }
  }
}

void SchedulesTabController::displaySelectedScheduleInSchedulesTab(const OSItemId& itemId) {
  if (auto mo_ = OSAppBase::instance()->currentDocument()->getModelObject(itemId)) {
    const bool isRuleset = mo_->iddObjectType() == openstudio::IddObjectType("OS_Schedule_Ruleset");
    setSubTab(isRuleset ? SCHEDULES : SCHEDULESOTHER);
    if (isRuleset) {
      qobject_cast<SchedulesView*>(m_currentView)->setCurrentSchedule(mo_->cast<model::ScheduleRuleset>());
    } else {
      qobject_cast<ScheduleOthersView*>(m_currentView)->setCurrentSchedule(*mo_);
    }
  }
}

void SchedulesTabController::toggleUnits(bool displayIP) {
  m_isIP = displayIP;

  if (m_scheduleDialog) {
    m_scheduleDialog->setIsIP(displayIP);
  }
}

double SchedulesTabController::defaultStartingValue(const model::ScheduleDay& scheduleDay) {
  double result(0.3);
  if (model::OptionalScheduleTypeLimits typeLimits = scheduleDay.scheduleTypeLimits()) {
    OptionalUnit siU = model::ScheduleTypeLimits::units(typeLimits->unitType(), false);
    if (siU) {
      std::string unitType = typeLimits->unitType();
      boost::to_lower(unitType);
      char firstLetter = unitType[0];
      OptionalQuantity siQ;
      switch (firstLetter) {
        case 'a': {
          if (unitType == "activitylevel") {
            siQ = Quantity(100.0, *siU);
          } else if (unitType == "angle") {
            siQ = Quantity(0.0, *siU);
          } else if (unitType == "availability") {
            siQ = Quantity(1.0, *siU);
          }
          break;
        }
        case 'c': {
          if (unitType == "capacity") {
            siQ = Quantity(100.0, *siU);
          } else if (unitType == "clothinginsulation") {
            siQ = Quantity(0.5, *siU);
          } else if (unitType == "controlmode") {
            siQ = Quantity(0.0, *siU);
          } else if (unitType == "convectioncoefficient") {
            siQ = Quantity(10.0, *siU);
          }
          break;
        }
        case 'd': {
          if (unitType == "deltatemperature") {
            siQ = Quantity(3.0, *siU);
          }
          break;
        }
        case 'm': {
          if (unitType == "massflowrate") {
            siQ = Quantity(1.0, *siU);
          }
          break;
        }
        case 'p': {
          if (unitType == "percent") {
            siQ = Quantity(50.0, *siU);
          } else if (unitType == "power") {
            siQ = Quantity(100.0, *siU);
          } else if (unitType == "precipitationrate") {
            siQ = Quantity(0.01, *siU);
          } else if (unitType == "pressure") {
            siQ = Quantity(100.0, *siU);
          }
          break;
        }
        case 'r': {
          siQ = Quantity(1500.0, *siU);
          break;
        }
        case 's': {
          if (unitType == "solarenergy") {
            siQ = Quantity(1000.0, *siU);
          }
          break;
        }
        case 't': {
          if (unitType == "temperature") {
            siQ = Quantity(23.5, *siU);
          }
          break;
        }
        case 'v': {
          if (unitType == "velocity") {
            siQ = Quantity(1.0, *siU);
          }
          if (unitType == "volumetricflowrate") {
            siQ = Quantity(1.0E-4, *siU);
          }
          break;
        }
        default:
          break;
      }  // switch
      if (siQ) {
        result = siQ->value();
      }
    }  // if siU
  }
  return result;
}

void SchedulesTabController::setSubTab(int index) {
  if (m_currentIndex == index) {
    return;
  } else {
    m_currentIndex = index;
  }

  if (qobject_cast<ScheduleSetsView*>(m_currentView) && qobject_cast<ScheduleSetsController*>(m_currentController)) {
    disconnect(qobject_cast<ScheduleSetsController*>(m_currentController), &ScheduleSetsController::downloadComponentsClicked, this,
               &SchedulesTabController::downloadComponentsClicked);
    disconnect(qobject_cast<ScheduleSetsController*>(m_currentController), &ScheduleSetsController::openLibDlgClicked, this,
               &SchedulesTabController::openLibDlgClicked);
    disconnect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
    m_currentController = nullptr;
  } else if (qobject_cast<SchedulesView*>(m_currentView)) {
    disconnect(this, &SchedulesTabController::toggleUnitsClicked, qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::toggleUnitsClicked);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::addScheduleClicked, this, &SchedulesTabController::addScheduleRuleset);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::copySelectedScheduleClicked, this,
               &SchedulesTabController::copySelectedSchedule);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::removeSelectedScheduleClicked, this,
               &SchedulesTabController::removeSelectedSchedule);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::purgeUnusedScheduleRulesetsClicked, this,
               &SchedulesTabController::purgeUnusedScheduleRulesets);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::addRuleClicked, this, &SchedulesTabController::addRule);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::addSummerProfileClicked, this, &SchedulesTabController::addSummerProfile);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::addWinterProfileClicked, this, &SchedulesTabController::addWinterProfile);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::addHolidayProfileClicked, this,
               &SchedulesTabController::addHolidayProfile);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::dayScheduleSceneChanged, this,
               &SchedulesTabController::onDayScheduleSceneChanged);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::startDateTimeChanged, this,
               &SchedulesTabController::onStartDateTimeChanged);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::endDateTimeChanged, this, &SchedulesTabController::onEndDateTimeChanged);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::removeScheduleRuleClicked, this,
               &SchedulesTabController::removeScheduleRule);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::itemDropped, this, &SchedulesTabController::onItemDropped);
    disconnect(qobject_cast<SchedulesView*>(m_currentView), &SchedulesView::modelObjectSelected, this, &SchedulesTabController::modelObjectSelected);
    disconnect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
  } else if (qobject_cast<ScheduleOthersView*>(m_currentView) && qobject_cast<ScheduleOthersController*>(m_currentController)) {
    disconnect(qobject_cast<ScheduleOthersController*>(m_currentController), &ScheduleOthersController::downloadComponentsClicked, this,
               &SchedulesTabController::downloadComponentsClicked);
    disconnect(qobject_cast<ScheduleOthersController*>(m_currentController), &ScheduleOthersController::openLibDlgClicked, this,
               &SchedulesTabController::openLibDlgClicked);
    disconnect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
    m_currentController = nullptr;
  } else if (m_currentView) {
    // Oops! Should never get here
    OS_ASSERT(false);
  }

  switch (index) {
    case TabID::SCHEDULE_SETS: {
      auto* scheduleSetsController = new ScheduleSetsController(m_model);
      connect(scheduleSetsController, &ScheduleSetsController::downloadComponentsClicked, this, &SchedulesTabController::downloadComponentsClicked);
      connect(scheduleSetsController, &ScheduleSetsController::openLibDlgClicked, this, &SchedulesTabController::openLibDlgClicked);
      connect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
      this->mainContentWidget()->setSubTab(scheduleSetsController->subTabView());
      m_currentView = scheduleSetsController->subTabView();
      m_currentController = scheduleSetsController;
      break;
    }
    case TabID::SCHEDULES: {
      auto* schedulesView = new SchedulesView(m_isIP, m_model);
      addQObject(schedulesView);
      connect(this, &SchedulesTabController::toggleUnitsClicked, schedulesView, &SchedulesView::toggleUnitsClicked);
      connect(schedulesView, &SchedulesView::addScheduleClicked, this, &SchedulesTabController::addScheduleRuleset);
      connect(schedulesView, &SchedulesView::copySelectedScheduleClicked, this, &SchedulesTabController::copySelectedSchedule);
      connect(schedulesView, &SchedulesView::removeSelectedScheduleClicked, this, &SchedulesTabController::removeSelectedSchedule);
      connect(schedulesView, &SchedulesView::purgeUnusedScheduleRulesetsClicked, this, &SchedulesTabController::purgeUnusedScheduleRulesets);
      connect(schedulesView, &SchedulesView::addRuleClicked, this, &SchedulesTabController::addRule);
      connect(schedulesView, &SchedulesView::addSummerProfileClicked, this, &SchedulesTabController::addSummerProfile);
      connect(schedulesView, &SchedulesView::addWinterProfileClicked, this, &SchedulesTabController::addWinterProfile);
      connect(schedulesView, &SchedulesView::addHolidayProfileClicked, this, &SchedulesTabController::addHolidayProfile);
      connect(schedulesView, &SchedulesView::dayScheduleSceneChanged, this, &SchedulesTabController::onDayScheduleSceneChanged);
      connect(schedulesView, &SchedulesView::startDateTimeChanged, this, &SchedulesTabController::onStartDateTimeChanged);
      connect(schedulesView, &SchedulesView::endDateTimeChanged, this, &SchedulesTabController::onEndDateTimeChanged);
      connect(schedulesView, &SchedulesView::removeScheduleRuleClicked, this, &SchedulesTabController::removeScheduleRule);
      connect(schedulesView, &SchedulesView::itemDropped, this, &SchedulesTabController::onItemDropped);
      connect(schedulesView, &SchedulesView::modelObjectSelected, this, &SchedulesTabController::modelObjectSelected);
      connect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
      this->mainContentWidget()->setSubTab(schedulesView);
      m_currentView = schedulesView;
      break;
    }
    case TabID::SCHEDULESOTHER: {
      auto* scheduleOthersController = new ScheduleOthersController(m_model);
      connect(scheduleOthersController, &ScheduleOthersController::downloadComponentsClicked, this,
              &SchedulesTabController::downloadComponentsClicked);
      connect(scheduleOthersController, &ScheduleOthersController::openLibDlgClicked, this, &SchedulesTabController::openLibDlgClicked);
      connect(this, &SchedulesTabController::toggleUnitsClicked, this, &SchedulesTabController::toggleUnits);
      this->mainContentWidget()->setSubTab(scheduleOthersController->subTabView());
      m_currentView = scheduleOthersController->subTabView();
      m_currentController = scheduleOthersController;
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

}  // namespace openstudio
