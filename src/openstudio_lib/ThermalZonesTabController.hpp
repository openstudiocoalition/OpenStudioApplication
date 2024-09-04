/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_THERMALZONESTABCONTROLLER_HPP
#define OPENSTUDIO_THERMALZONESTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include <openstudio/model/Model.hpp>

namespace openstudio {

class ThermalZonesController;

class ThermalZonesTabController : public MainTabController
{
  Q_OBJECT

 public:
  ThermalZonesTabController(bool isIP, bool displayAdditionalProps, const model::Model& model);

  virtual ~ThermalZonesTabController() {}

 signals:

  void toggleUnitsClicked(bool);

 private:
  std::shared_ptr<ThermalZonesController> m_thermalZonesController;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_THERMALZONESTABCONTROLLER_HPP
