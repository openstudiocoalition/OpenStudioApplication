/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_THERMALZONESTABVIEW_HPP
#define OPENSTUDIO_THERMALZONESTABVIEW_HPP

#include "MainTabView.hpp"

namespace openstudio {

class ThermalZonesTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit ThermalZonesTabView(QWidget* parent = nullptr);

  virtual ~ThermalZonesTabView(){};
};

}  // namespace openstudio

#endif  // OPENSTUDIO_THERMALZONESTABVIEW_HPP
