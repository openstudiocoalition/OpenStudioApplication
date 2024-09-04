/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SIMSETTINGSTABCONTROLLER_HPP
#define OPENSTUDIO_SIMSETTINGSTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

namespace model {

class Model;

}

class SimSettingsTabController : public MainTabController
{
  Q_OBJECT

 public:
  SimSettingsTabController(bool isIP, const model::Model& model);

  virtual ~SimSettingsTabController() {}

 public slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SIMSETTINGSTABCONTROLLER_HPP
