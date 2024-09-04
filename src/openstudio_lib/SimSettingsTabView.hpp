/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SIMSETTINGSTABVIEW_HPP
#define OPENSTUDIO_SIMSETTINGSTABVIEW_HPP

#include "MainTabView.hpp"

//#include <model/Model.hpp> TODO delete

namespace openstudio {

class SimSettingsView;

namespace model {

class Model;

}

class SimSettingsTabView : public MainTabView
{
  Q_OBJECT

 public:
  SimSettingsTabView(bool isIP, const model::Model& model, const QString& tabLabel, TabType tabType, QWidget* parent = nullptr);

  virtual ~SimSettingsTabView() {}

 public slots:

  void toggleUnits(bool displayIP);

 private:
  SimSettingsView* m_simSettingsView;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SIMSETTINGSTABVIEW_HPP
