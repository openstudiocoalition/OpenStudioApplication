/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_HVACSYSTEMSTABVIEW_HPP
#define OPENSTUDIO_HVACSYSTEMSTABVIEW_HPP

#include "MainTabView.hpp"

namespace openstudio {

class HVACSystemsTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit HVACSystemsTabView(QWidget* parent = nullptr);

  virtual ~HVACSystemsTabView(){};
};

}  // namespace openstudio

#endif  // OPENSTUDIO_HVACSYSTEMSTABVIEW_HPP
