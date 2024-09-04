/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_HVACSYSTEMSTABCONTROLLER_HPP
#define OPENSTUDIO_HVACSYSTEMSTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include <openstudio/model/Model.hpp>

namespace openstudio {

class HVACSystemsController;

class HVACSystemsTabController : public MainTabController
{
  Q_OBJECT

 public:
  HVACSystemsTabController(bool isIP, const model::Model& model);

  virtual ~HVACSystemsTabController() {}

  void clearSceneSelection();

 private:
  std::shared_ptr<HVACSystemsController> m_hvacSystemsController;

  bool m_isIP;

 signals:

  void toggleUnitsClicked(bool displayIP);

 private slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_HVACSYSTEMSTABCONTROLLER_HPP
