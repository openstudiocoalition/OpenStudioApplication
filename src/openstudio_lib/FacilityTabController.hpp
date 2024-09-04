/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FACILITYTABCONTROLLER_HPP
#define OPENSTUDIO_FACILITYTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class FacilityTabController : public MainTabController
{
  Q_OBJECT

 public:
  FacilityTabController(bool isIP, bool displayAdditionalProps, const model::Model& model);

  virtual ~FacilityTabController();

 private:
  enum TabID
  {
    BUILDING,
    STORIES,
    SHADING,
    EXTERIOR_EQUIPMENT
  };

  model::Model m_model;

  bool m_isIP;

  bool m_displayAdditionalProps;

  QWidget* m_currentView = nullptr;

  int m_currentIndex = -1;

 public slots:

  void toggleUnits(bool displayIP);

  void toggleDisplayAdditionalProps(bool displayAdditionalProps);

  virtual void setSubTab(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYTABCONTROLLER_HPP
