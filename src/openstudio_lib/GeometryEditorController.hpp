/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYEDITORCONTROLLER_HPP
#define OPENSTUDIO_GEOMETRYEDITORCONTROLLER_HPP

#include "../shared_gui_components/OSQObjectController.hpp"

namespace openstudio {
namespace model {
class Model;
}

/**
 * GeometryEditorController manages the Geometry tab, which embeds a web-based 3D geometry editor
 * (FloorspaceJS) via a QWebEngineView. It bridges JSON geometry edits from the web view back to
 * the OpenStudio model and handles save/load of the floorplan JSON.
 */
class GeometryEditorController : public OSQObjectController
{
  Q_OBJECT

 public:
  GeometryEditorController(bool isIP, const model::Model& model);

  virtual ~GeometryEditorController();

  QWidget* view() const;

 protected:
  QWidget* m_view;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYEDITORCONTROLLER_HPP
