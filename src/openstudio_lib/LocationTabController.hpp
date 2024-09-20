/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOCATIONTABCONTROLLER_HPP
#define OPENSTUDIO_LOCATIONTABCONTROLLER_HPP

#include <openstudio/model/Model.hpp>

#include "MainTabController.hpp"

#include <boost/smart_ptr.hpp>

#include <QDir>
#include <QObject>

class QStackedWidget;

namespace openstudio {

class LocationTabView;

class UtilityBillsController;

class LocationTabController : public MainTabController
{
  Q_OBJECT

 public:
  LocationTabController(bool isIP, const model::Model& model, const QString& modelTempDir);

  virtual ~LocationTabController();

  enum TabID
  {
    WEATHER_FILE,
    LIFE_CYCLE_COSTS,
    UTILITY_BILLS
  };

 private:
  bool showUtilityBills();

  QString m_modelTempDir;

  model::Model m_model;

  bool m_isIP;

  QWidget* m_currentView = nullptr;

  int m_currentIndex = -1;

 private slots:

  virtual void setSubTab(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOCATIONTABCONTROLLER_HPP
