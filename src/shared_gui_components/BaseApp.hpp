/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BASEAPP_HPP
#define SHAREDGUICOMPONENTS_BASEAPP_HPP

#include <openstudio/utilities/core/Path.hpp>
#include "../model_editor/QMetaTypes.hpp"

#include <QWidget>
#include <boost/optional.hpp>
//#include "EditController.hpp"

namespace openstudio {
class MeasureManager;
class EditController;
class Workspace;

//namespace analysisdriver {
//  class SimpleProject;
//}

namespace model {
class Model;
}

class BaseApp
{
 public:
  virtual ~BaseApp() {}

  //virtual boost::optional<analysisdriver::SimpleProject> project() = 0;
  virtual QWidget* mainWidget() = 0;
  virtual openstudio::MeasureManager& measureManager() = 0;
  virtual void updateSelectedMeasureState() = 0;

  virtual void addMeasure() = 0;
  virtual void duplicateSelectedMeasure() = 0;
  virtual void updateMyMeasures() = 0;
  virtual void updateBCLMeasures() = 0;
  virtual void openBclDlg() = 0;
  virtual void checkForRemoteBCLUpdates() = 0;

  virtual void chooseHorizontalEditTab() = 0;
  virtual QSharedPointer<openstudio::EditController> editController() = 0;
  virtual boost::optional<openstudio::path> tempDir() = 0;
  virtual boost::optional<openstudio::model::Model> currentModel() = 0;
  //virtual boost::optional<openstudio::Workspace> currentWorkspace() = 0;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BASEAPP_HPP
