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
