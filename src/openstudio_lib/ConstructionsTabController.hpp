/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_HPP
#define OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class DefaultConstructionSetsController;
class ConstructionsController;
class MaterialsController;

class ConstructionsTabController : public MainTabController
{
  Q_OBJECT

 public:
  ConstructionsTabController(bool isIP, const openstudio::model::Model& model);

  virtual ~ConstructionsTabController();

  enum TabID
  {
    DEFAULT_CONSTRUCTIONS,
    CONSTRUCTIONS,
    MATERIALS
  };

 private:
  model::Model m_model;

  bool m_isIP;

  QObject* m_currentController = nullptr;

  int m_currentIndex = -1;

 public slots:

  virtual void setSubTab(int index) override;

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_HPP
