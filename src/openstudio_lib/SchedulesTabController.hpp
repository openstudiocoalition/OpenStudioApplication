/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULESTABCONTROLLER_HPP
#define OPENSTUDIO_SCHEDULESTABCONTROLLER_HPP

#include "MainTabController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ScheduleRuleset.hpp>
#include <openstudio/model/ScheduleRuleset_Impl.hpp>

#include <openstudio/utilities/core/UUID.hpp>

#include <boost/smart_ptr.hpp>

#include <QObject>

namespace openstudio {

class OSItemId;

namespace model {

class ScheduleCompact;

}
class DayScheduleScene;

class MainTabView;

class ScheduleDialog;

class ScheduleSetsController;

class SchedulesView;

class SchedulesTabController : public MainTabController
{
  Q_OBJECT

 public:
  SchedulesTabController(bool isIP, const model::Model& model);

  virtual ~SchedulesTabController();

  enum TabID
  {
    //YEAR_SETTINGS,
    SCHEDULE_SETS,
    SCHEDULES,
    SCHEDULESOTHER
  };

  static double defaultStartingValue(const model::ScheduleDay& scheduleDay);

 private:
  void showScheduleDialog();

  ScheduleDialog* m_scheduleDialog = nullptr;

  model::Model m_model;

  bool m_isIP;

  QWidget* m_currentView = nullptr;

  QObject* m_currentController = nullptr;

  int m_currentIndex = -1;

 public slots:

  virtual void setSubTab(int index) override;

  void toggleUnits(bool displayIP);

  void displaySelectedScheduleInSchedulesTab(const OSItemId& itemId);

 private slots:

  void addScheduleRuleset();

  void copySelectedSchedule();

  void removeSelectedSchedule();

  void purgeUnusedScheduleRulesets();

  void addRule(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle);

  void addSummerProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle);

  void addWinterProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle);

  void addHolidayProfile(model::ScheduleRuleset& scheduleRuleset, UUID scheduleDayHandle);

  void removeScheduleRule(model::ScheduleRule& scheduleRule);

  void onDayScheduleSceneChanged(DayScheduleScene* scene, double lowerLimitValue, double upperLimitValue);

  void onStartDateTimeChanged(model::ScheduleRule& scheduleRule, const QDateTime& newDate);

  void onEndDateTimeChanged(model::ScheduleRule& scheduleRule, const QDateTime& newDate);

  void onItemDropped(const OSItemId& itemId);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULESTABCONTROLLER_HPP
