/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_YEARSETTINGSWIDGET_HPP
#define OPENSTUDIO_YEARSETTINGSWIDGET_HPP

#include "../model_editor/QMetaTypes.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/YearDescription.hpp>
#include <openstudio/model/YearDescription_Impl.hpp>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <QRadioButton>
#include <QWidget>

class QDate;

class QDateEdit;

namespace openstudio {

class OSComboBox2;

class OSSwitch2;

class YearSettingsWidget
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  static const int FIRSTYEAR;

  static const int LASTYEAR;

  // A list of the weeks in the month.  1st, 2nd, 3rd, 4th, Last
  static QStringList weeksInMonth();

  // A list of the days in the week.  Monday, Tuesday...
  static QStringList daysOfWeek();

  // A list of the months.
  static QStringList months();

  static QStringList validDayofWeekforStartDay();

  explicit YearSettingsWidget(const model::Model& model, QWidget* parent = nullptr);

  bool calendarYearChecked();

  virtual ~YearSettingsWidget() {}

 signals:

  void calendarYearSelected(int year);

  void firstDayofYearSelected(const QString& firstDayofYear);

  void daylightSavingTimeClicked(bool enabled);

  void dstStartDayOfWeekAndMonthChanged(int newWeek, int intDay, int newMonth);

  void dstStartDateChanged(const QDate& newdate);

  void dstEndDayOfWeekAndMonthChanged(int newWeek, int newDay, int newMonth);

  void dstEndDateChanged(const QDate& newdate);

 public slots:

  void scheduleRefresh();

 private slots:

  void refresh();

  void onCalendarYearChanged(int index);

  void onCalendarYearButtonClicked();

  void onFirstDayofYearClicked();

  void onWorkspaceObjectAdd(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wo, const openstudio::IddObjectType& type,
                            const openstudio::UUID& uuid);

  void onWorkspaceObjectRemove(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wo, const openstudio::IddObjectType& type,
                               const openstudio::UUID& uuid);

  void onDstStartDayWeekMonthChanged();

  //void onDstStartDateChanged();

  void onDstEndDayWeekMonthChanged();

  //void onDstEndDateChanged();

  void onDefineStartByDayWeekMonthClicked();

  void onDefineEndByDayWeekMonthClicked();

  void onDefineStartByDateClicked();

  void onDefineEndByDateClicked();

  //void emitNewDSTStartDate();

  //void emitNewDSTEndDate();

 private:
  // year selection section
  QRadioButton* m_calendarYearButton = nullptr;

  OSComboBox2* m_calendarYearEdit = nullptr;

  QRadioButton* m_firstDayOfYearButton = nullptr;

  OSComboBox2* m_firstDayOfYearEdit = nullptr;

  // daylight savings section

  OSSwitch2* m_dstOnOffButton = nullptr;

  QRadioButton* m_dayOfWeekAndMonthStartButton = nullptr;

  OSComboBox2* m_startWeekBox = nullptr;

  OSComboBox2* m_startDayBox = nullptr;

  OSComboBox2* m_startMonthBox = nullptr;

  QRadioButton* m_dateStartButton = nullptr;

  QDateEdit* m_startDateEdit = nullptr;

  QRadioButton* m_dayOfWeekAndMonthEndButton = nullptr;

  OSComboBox2* m_endWeekBox = nullptr;

  OSComboBox2* m_endDayBox = nullptr;

  OSComboBox2* m_endMonthBox = nullptr;

  QRadioButton* m_dateEndButton = nullptr;

  QDateEdit* m_endDateEdit = nullptr;

  // other

  boost::optional<model::YearDescription> m_yearDescription;

  model::Model m_model;

  bool m_dirty;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_YEARSETTINGSWIDGET_HPP
