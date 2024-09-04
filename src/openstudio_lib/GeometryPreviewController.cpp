/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GeometryPreviewController.hpp"
#include "GeometryPreviewView.hpp"

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

GeometryPreviewController::GeometryPreviewController(bool isIP, const model::Model& model) : OSQObjectController() {
  m_view = new GeometryPreviewView(isIP, model);
  addQObject(m_view);
}

GeometryPreviewController::~GeometryPreviewController() {
  // will delete m_view
}

QWidget* GeometryPreviewController::view() const {
  return m_view;
}

}  // namespace openstudio
