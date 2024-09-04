/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACETYPESTABCONTROLLER_HPP
#define OPENSTUDIO_SPACETYPESTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class SpaceTypesController;

class SpaceTypesTabController : public MainTabController
{
  Q_OBJECT

 public:
  SpaceTypesTabController(bool isIP, const openstudio::model::Model& model);

  virtual ~SpaceTypesTabController() {}

 private:
  std::shared_ptr<SpaceTypesController> m_spaceTypesController;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACETYPESTABCONTROLLER_HPP
