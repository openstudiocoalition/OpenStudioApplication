/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_VARIABLESTABCONTROLLER_HPP
#define OPENSTUDIO_VARIABLESTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class VariablesTabController : public MainTabController
{
  Q_OBJECT

 public:
  explicit VariablesTabController(const model::Model& model);

  virtual ~VariablesTabController() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_VARIABLESTABCONTROLLER_HPP
