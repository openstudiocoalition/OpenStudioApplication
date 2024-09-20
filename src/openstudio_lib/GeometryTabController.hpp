/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYTABCONTROLLER_HPP
#define OPENSTUDIO_GEOMETRYTABCONTROLLER_HPP

#include "MainTabController.hpp"

namespace openstudio {

class GeometryPreviewController;
class GeometryEditorController;

class GeometryTabController : public MainTabController
{
  Q_OBJECT

 public:
  GeometryTabController(bool isIP, const openstudio::model::Model& model);

  virtual ~GeometryTabController();

  enum TabID
  {
    VIEW,
    EDITOR
  };

 private:
  model::Model m_model;

  bool m_isIP;

  QObject* m_currentController = nullptr;

  int m_currentIndex = -1;

 public slots:

  virtual void setSubTab(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYTABCONTROLLER_HPP
