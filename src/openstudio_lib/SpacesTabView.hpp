/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESTABVIEW_HPP
#define OPENSTUDIO_SPACESTABVIEW_HPP

#include <openstudio/model/Model.hpp>

#include "MainTabView.hpp"

class QWidget;

namespace openstudio {

namespace model {
class Model;
}

class SpacesTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit SpacesTabView(QWidget* parent = nullptr);

  virtual ~SpacesTabView() {}

 public slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACESTABVIEW_HPP
