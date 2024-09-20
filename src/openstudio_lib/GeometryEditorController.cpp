/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GeometryEditorController.hpp"
#include "GeometryEditorView.hpp"

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

GeometryEditorController::GeometryEditorController(bool isIP, const model::Model& model) : OSQObjectController() {
  m_view = new GeometryEditorView(isIP, model);
  addQObject(m_view);
}

GeometryEditorController::~GeometryEditorController() {
  // will delete m_view
}

QWidget* GeometryEditorController::view() const {
  return m_view;
}

}  // namespace openstudio
