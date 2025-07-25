/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOCATIONTABVIEW_HPP
#define OPENSTUDIO_LOCATIONTABVIEW_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Site.hpp>
#include <openstudio/model/YearDescription.hpp>
#include <openstudio/model/DesignDay.hpp>
#include "MainTabView.hpp"
#include "YearSettingsWidget.hpp"

#include <QWidget>

class QComboBox;
class QDir;
class QLineEdit;

namespace openstudio {

class EpwFile;
class DesignDayGridView;
class OSComboBox2;
class OSItemSelectorButtons;
class OSQuantityEdit2;
class OSSwitch2;

namespace model {
class Model;
class Site;
class YearDescription;
}  // namespace model

/// <summary>
/// SortableDesignDay is a helper class for sorting DesignDays
/// </summary>
class SortableDesignDay
{
 public:
  explicit SortableDesignDay(const openstudio::model::DesignDay& designDay);

  static int qstringToPermil(const QString& str);

  static QString permilToQString(int permil);

  static QString key(const QString& type, int sortablePermil);

  QString type() const;

  int permil() const;

  int sortablePermil() const;

 private:
  QString m_type;
  int m_permil{0};  // per thousand, e.g. 0.4% -> 4, 99.6% -> 996
  openstudio::model::DesignDay m_designDay;
};

class LocationView : public QWidget
{

  Q_OBJECT

 public:
  LocationView(bool isIP, const model::Model& model, const QString& modelTempDir);

  virtual ~LocationView();

  virtual bool supportsMultipleObjectSelection() const {
    return true;
  }

  virtual std::set<model::ModelObject> selectedObjects() const;

  bool calendarYearChecked();

 protected:
  void onSelectModelObject(const openstudio::model::ModelObject& modelObject);

  void onUpdate();

 private:
  void update();

  void loadQSettings();

  void saveQSettings() const;

  void setSiteInfo();

  void clearSiteInfo();

  model::Model m_model;
  boost::optional<model::Site> m_site;
  boost::optional<model::YearDescription> m_yearDescription;
  YearSettingsWidget* m_yearSettingsWidget = nullptr;
  DesignDayGridView* m_designDaysGridView = nullptr;
  OSItemSelectorButtons* m_itemSelectorButtons = nullptr;
  QString m_modelTempDir;
  QString m_lastEpwPathOpened;
  QString m_lastDdyPathOpened;

  QPushButton* m_weatherFileBtn = nullptr;
  QLineEdit* m_siteName = nullptr;
  QLabel* m_latitudeLbl = nullptr;
  QLabel* m_longitudeLbl = nullptr;
  QLabel* m_timeZoneLbl = nullptr;

  OSComboBox2* m_terrain = nullptr;
  OSSwitch2* m_keepSiteLocationInfo = nullptr;
  OSQuantityEdit2* m_elevation = nullptr;
  double m_weatherFileElevation = 0.0;
  QComboBox* m_ashraeClimateZone = nullptr;
  QComboBox* m_cecClimateZone = nullptr;

  bool m_isIP;

 signals:

  void calendarYearSelectionChanged();

  void modelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly);

  void toggleUnitsClicked(bool displayIP);

 public slots:

  void refresh();

 private slots:

  void onSelectItem();

  void onClearSelection();

  void toggleUnits(bool);

  void onSiteNameChanged(const QString& text);

  void checkNumDesignDays();

  void setCalendarYear(int year);

  void setFirstDayofYear(const QString& firstDayofYear);

  void setDaylightSavingsTime(bool enabled);

  void setDstStartDayOfWeekAndMonth(int newWeek, int newDay, int newMonth);

  void setDstStartDate(const QDate& newdate);

  void setDstEndDayOfWeekAndMonth(int newWeek, int newDay, int newMonth);

  void setDstEndDate(const QDate& newdate);

  void onWeatherFileBtnClicked();

  std::vector<openstudio::model::DesignDay> showDesignDaySelectionDialog(const std::vector<openstudio::model::DesignDay>& allDesignDays);

  void onDesignDayBtnClicked();

  void onASHRAEClimateZoneChanged(const QString& climateZone);

  void onCECClimateZoneChanged(const QString& climateZone);
};

class LocationTabView : public MainTabView
{
  Q_OBJECT

 public:
  LocationTabView(const model::Model& model, const QString& modelTempDir, QWidget* parent = nullptr);

  virtual ~LocationTabView() = default;

  //private:
  //LocationView* m_locationView;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOCATIONTABVIEW_HPP
