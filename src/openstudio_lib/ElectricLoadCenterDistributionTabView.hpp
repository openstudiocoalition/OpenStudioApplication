/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABVIEW_HPP
#define OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABVIEW_HPP

#include "MainTabView.hpp"

namespace openstudio {

class ELCDTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit ELCDTabView(QWidget* parent = nullptr);

  virtual ~ELCDTabView() = default;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABVIEW_HPP
