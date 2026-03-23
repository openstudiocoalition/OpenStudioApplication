/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP
#define OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include <openstudio/model/Model.hpp>

namespace openstudio {

class ElectricLoadCenterDistributionController;

class ElectricLoadCenterDistributionTabController : public MainTabController
{
  Q_OBJECT

 public:
  ElectricLoadCenterDistributionTabController(const model::Model& model);
  virtual ~ElectricLoadCenterDistributionTabController() = default;

 private:
  std::shared_ptr<ElectricLoadCenterDistributionController> m_controller;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP
