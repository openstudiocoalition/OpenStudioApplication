/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SUMMARYTABCONTROLLER_HPP
#define OPENSTUDIO_SUMMARYTABCONTROLLER_HPP

#include <openstudio/model/Model.hpp>
#include <boost/smart_ptr.hpp>
#include "MainTabController.hpp"

namespace openstudio {

class SummaryTabController : public MainTabController
{
  Q_OBJECT

 public:
  explicit SummaryTabController(const model::Model& model);

  virtual ~SummaryTabController() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SUMMARYTABCONTROLLER_HPP
