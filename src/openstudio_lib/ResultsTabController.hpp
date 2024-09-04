/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RESULTSTABCONTROLLER_HPP
#define OPENSTUDIO_RESULTSTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/core/Logger.hpp>

namespace openstudio {

namespace model {

class Model;

}

class ResultsTabView;

class ResultsTabController : public MainTabController
{
  Q_OBJECT

 public:
  ResultsTabController();

  virtual ~ResultsTabController() {}

 public slots:

  void onUnitSystemChange(bool t_isIP);

 private:
  ResultsTabView* m_resultsTabView;

  REGISTER_LOGGER("openstudio::ResultsTabController");
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RESULTSTABCONTROLLER_HPP
