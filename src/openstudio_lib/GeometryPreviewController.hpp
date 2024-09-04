/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYPREVIEWCONTROLLER_HPP
#define OPENSTUDIO_GEOMETRYPREVIEWCONTROLLER_HPP

#include "../shared_gui_components/OSQObjectController.hpp"

namespace openstudio {
namespace model {
class Model;
}

class GeometryPreviewController : public OSQObjectController
{
  Q_OBJECT

 public:
  GeometryPreviewController(bool isIP, const model::Model& model);

  virtual ~GeometryPreviewController();

  QWidget* view() const;

 protected:
  QWidget* m_view;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYPREVIEWCONTROLLER_HPP
