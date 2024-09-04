/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SUMMARYTABVIEW_HPP
#define OPENSTUDIO_SUMMARYTABVIEW_HPP

#include <openstudio/model/Model.hpp>
#include "MainTabView.hpp"

namespace openstudio {

class SummaryTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit SummaryTabView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~SummaryTabView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SUMMARYTABVIEW_HPP
