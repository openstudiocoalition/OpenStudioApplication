/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GeometryTabController.hpp"

#include "GeometryTabView.hpp"
#include "GeometryPreviewController.hpp"
#include "GeometryPreviewView.hpp"
#include "GeometryEditorController.hpp"
#include "GeometryEditorView.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/core/Assert.hpp>

// DLM: remove once editor is always enabled
#include <QProcessEnvironment>

namespace openstudio {

GeometryTabController::GeometryTabController(bool isIP, const model::Model& model)
  : MainTabController(new GeometryTabView(model, "Geometry")), m_model(model), m_isIP(isIP) {
  this->mainContentWidget()->addSubTab("3D View", VIEW);

  // DLM: remove once editor is always enabled
  //if (QProcessEnvironment::systemEnvironment().value("OPENSTUDIO_GEOMETRY_EDITOR") == QString("1")){
  this->mainContentWidget()->addSubTab("Editor", EDITOR);
  //}

  connect(this->mainContentWidget(), &MainTabView::tabSelected, this, &GeometryTabController::setSubTab);
}

GeometryTabController::~GeometryTabController() {
  disconnect(this->mainContentWidget(), &MainTabView::tabSelected, this, &GeometryTabController::setSubTab);
}

void GeometryTabController::setSubTab(int index) {
  if (m_currentIndex == index) {
    return;
  } else {
    m_currentIndex = index;
  }

  if (m_currentController) {
    m_currentController->disconnect();
    delete m_currentController;
  }

  switch (index) {
    case VIEW: {
      auto* previewController = new GeometryPreviewController(m_isIP, m_model);
      //connect(this, &ConstructionsTabController::toggleUnitsClicked, static_cast<ModelSubTabView*>(defaultConstructionSetsController->subTabView()), &ModelSubTabView::toggleUnitsClicked);
      //connect(defaultConstructionSetsController, &DefaultConstructionSetsController::downloadComponentsClicked, this, &ConstructionsTabController::downloadComponentsClicked);
      //connect(defaultConstructionSetsController, &DefaultConstructionSetsController::openLibDlgClicked, this, &ConstructionsTabController::openLibDlgClicked);
      this->mainContentWidget()->setSubTab(previewController->view());
      m_currentController = previewController;
      break;
    }

    case EDITOR: {
      auto* editorController = new GeometryEditorController(m_isIP, m_model);
      //connect(this, &ConstructionsTabController::toggleUnitsClicked, static_cast<ModelSubTabView*>(constructionsController->subTabView()), &ModelSubTabView::toggleUnitsClicked);
      this->mainContentWidget()->setSubTab(editorController->view());
      m_currentController = editorController;
      break;
    }
    default:
      OS_ASSERT(false);
      break;
  }
}

}  // namespace openstudio
