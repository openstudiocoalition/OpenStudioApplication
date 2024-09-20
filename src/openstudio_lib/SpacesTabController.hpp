/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESTABCONTROLLER_HPP
#define OPENSTUDIO_SPACESTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class SpacesTabController : public MainTabController
{
  Q_OBJECT

 public:
  SpacesTabController(bool isIP, bool displayAdditionalProps, const model::Model& model);

  virtual ~SpacesTabController();

  enum TabID
  {
    SPACES,  // "Properties"
    LOADS,
    SURFACES,
    SUBSURFACES,
    INTERIOR_PARTITIONS,
    SHADING,
    DAYLIGHTING
  };

 private:
  model::Model m_model;

  bool m_isIP;

  bool m_displayAdditionalProps;

  QWidget* m_currentView = nullptr;

  int m_currentIndex = -1;

 public slots:

  virtual void setSubTab(int index) override;

 private slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACESTABCONTROLLER_HPP
