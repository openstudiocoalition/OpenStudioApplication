/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOADSTABCONTROLLER_HPP
#define OPENSTUDIO_LOADSTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class LoadsController;

class LoadsTabController : public MainTabController
{
  Q_OBJECT

 public:
  LoadsTabController(bool isIP, const openstudio::model::Model& model);

  virtual ~LoadsTabController();

 private:
  std::shared_ptr<LoadsController> m_LoadsController;

 public slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOADSTABCONTROLLER_HPP
