/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RUNTABCONTROLLER_HPP
#define OPENSTUDIO_RUNTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include <openstudio/utilities/core/Path.hpp>

namespace openstudio {

//namespace runmanager {
//  class RunManager;
//  class JobStatusWidget;
//}

namespace model {
class Model;
}

class RunView;

class RunTabController : public MainTabController
{
  Q_OBJECT

 public:
  RunTabController(const model::Model& model, const openstudio::path& t_modelPath,
                   const openstudio::path& t_tempFolder);  //, openstudio::runmanager::RunManager t_runManager);

  virtual ~RunTabController() {}

  //openstudio::RunView * runView();

  //runmanager::RunManager runManager();

  enum TabID
  {
    OUTPUT,
    TREE
  };

 signals:
  void resultsGenerated(const openstudio::path& t_sqlFile, const openstudio::path& t_radianceOutputFile);
  void toolsUpdated();
  //void useRadianceStateChanged(bool);

 public slots:
  void updateToolsWarnings();

 private:
  //RunView * m_runView;

  //openstudio::runmanager::JobStatusWidget * m_status;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RUNTABCONTROLLER_HPP
