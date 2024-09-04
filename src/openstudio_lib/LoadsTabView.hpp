/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOADSTABVIEW_HPP
#define OPENSTUDIO_LOADSTABVIEW_HPP

#include "SubTabView.hpp"

namespace openstudio {

class LoadsTabView : public MainTabView
{
  Q_OBJECT

 public:
  LoadsTabView(const model::Model& model, const QString& tabLabel, QWidget* parent = nullptr);
  virtual ~LoadsTabView() {}

 public slots:
  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOADSTABVIEW_HPP
