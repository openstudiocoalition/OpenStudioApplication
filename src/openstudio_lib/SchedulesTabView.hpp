/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULESTABVIEW_HPP
#define OPENSTUDIO_SCHEDULESTABVIEW_HPP

#include "SubTabView.hpp"

namespace openstudio {

class SchedulesTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit SchedulesTabView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~SchedulesTabView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULESTABVIEW_HPP
