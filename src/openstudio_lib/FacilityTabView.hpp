/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FACILITYTABVIEW_HPP
#define OPENSTUDIO_FACILITYTABVIEW_HPP

#include <openstudio/model/Model.hpp>

#include "MainTabView.hpp"

class QWidget;

namespace openstudio {

namespace model {
class Model;
}

class FacilityTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit FacilityTabView(QWidget* parent = nullptr);

  virtual ~FacilityTabView() {}

 public slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYTABVIEW_HPP
