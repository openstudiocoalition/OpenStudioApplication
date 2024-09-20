/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScriptsTabController.hpp"

#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "ScriptsTabView.hpp"

#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/WorkflowController.hpp"
#include "../shared_gui_components/WorkflowView.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

ScriptsTabController::ScriptsTabController()
  : MainTabController(new ScriptsTabView(nullptr)), scriptsTabView(dynamic_cast<ScriptsTabView*>(mainContentWidget())) {
  auto* app = OSAppBase::instance();
  boost::optional<path> tempDir = app->tempDir();
  OS_ASSERT(tempDir);

  // save the current osm to a temp location
  app->measureManager().saveTempModel(*tempDir);

  // update measures
  app->currentDocument()->disable();
  app->measureManager().updateMeasuresLists();
  app->currentDocument()->enable();

  m_workflowController =
    QSharedPointer<openstudio::measuretab::WorkflowController>(new openstudio::measuretab::WorkflowController(OSAppBase::instance()));
  m_workflowSectionItemDelegate =
    QSharedPointer<openstudio::measuretab::WorkflowSectionItemDelegate>(new openstudio::measuretab::WorkflowSectionItemDelegate());

  scriptsTabView->workflowView->setListController(m_workflowController);
  scriptsTabView->workflowView->setDelegate(m_workflowSectionItemDelegate);
}

ScriptsTabController::~ScriptsTabController() {
  if (scriptsTabView) {
    delete scriptsTabView;
  }
}

}  // namespace openstudio
