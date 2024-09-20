/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "RunTabController.hpp"
#include "RunTabView.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
//#include "../runmanager/lib/JobStatusWidget.hpp"

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

RunTabController::RunTabController(const model::Model& model, const openstudio::path& t_modelPath,
                                   const openstudio::path& t_tempFolder)  //, openstudio::runmanager::RunManager t_runManager)
  : MainTabController(new RunTabView(model))                              //,
                                                                          //m_runView(new RunView(model, t_modelPath, t_tempFolder, t_runManager)),
                                                                          //m_status(new openstudio::runmanager::JobStatusWidget(t_runManager))
{
  //mainContentWidget()->addSubTab("Output", m_runView,OUTPUT);
  //mainContentWidget()->addSubTab("Tree", m_status,TREE);

  //connect(m_runView, &RunView::resultsGenerated, this, &RunTabController::resultsGenerated);

  //connect(m_runView, &RunView::toolsUpdated, this, &RunTabController::toolsUpdated);

  //connect(m_runView, &RunView::useRadianceStateChanged, this, &RunTabController::useRadianceStateChanged);
}

//openstudio::RunView * RunTabController::runView(){
//
//  // make sure non-null pointer
//  OS_ASSERT(m_runView);
//
//  return m_runView;
//}

//runmanager::RunManager RunTabController::runManager() {
//  return m_runView->runManager();
//}

void RunTabController::updateToolsWarnings() {
  //m_runView->updateToolsWarnings();
}

}  // namespace openstudio
