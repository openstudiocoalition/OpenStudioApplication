/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACETYPESTABVIEW_HPP
#define OPENSTUDIO_SPACETYPESTABVIEW_HPP

#include "SubTabView.hpp"

namespace openstudio {

class SpaceTypesTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit SpaceTypesTabView(QWidget* parent = nullptr);
  virtual ~SpaceTypesTabView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACETYPESTABVIEW_HPP
