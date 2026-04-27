/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BASEAPP_HPP
#define SHAREDGUICOMPONENTS_BASEAPP_HPP

#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include "../openstudio_qt_utils/QMetaTypes.hpp"

#include <QWidget>
#include <QApplication>
#include <boost/optional.hpp>
//#include "EditController.hpp"

#include <string>
#include <vector>
#include <cstddef>

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

/**
 * BaseApp is the pure abstract interface through which all shared GUI components access
 * application services. It lives in openstudio_shared_gui (a lower-level library) and
 * deliberately has no dependency on OSAppBase, OSDocument, or any openstudio_lib symbol.
 *
 * Responsibilities:
 *   - Declare the polymorphic API that shared widgets require (measure manager, BCL
 *     queries, CLI mode flag, inspector focus state, etc.).
 *   - Be reachable process-wide via BaseApp::instance(), which down-casts qApp
 *     (safe because OSAppBase inherits both QApplication and BaseApp).
 *   - Provide safe default implementations for optional capabilities (BCL queries return
 *     empty/none; useClassicCLI returns false) so that lightweight hosts such as test
 *     fixtures do not need to implement every method.
 *
 * Dependency rule:
 *   shared_gui_components  →  BaseApp  (this file, no openstudio_lib symbols)
 *   openstudio_lib         →  OSAppBase  (implements BaseApp, owns QApplication)
 *
 * Never add includes from openstudio_lib to this header.
 */
class BaseApp
{
 public:
  virtual ~BaseApp() {}

  /// Returns the BaseApp singleton. Works because OSAppBase inherits both QApplication
  /// and BaseApp, so qApp can be down-cast at runtime.
  static BaseApp* instance() {
    return dynamic_cast<BaseApp*>(qApp);
  }

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

  virtual bool mouseOverInspectorView() = 0;

  /// Whether the application is using the classic (legacy) CLI rather than the Labs CLI.
  virtual bool useClassicCLI() const {
    return false;
  }

  /// Disable the document UI (e.g. while a drop operation is in progress).
  virtual void disableDocument() {}

  /// Re-enable the document UI after a disable call.
  virtual void enableDocument() {}

  /// BCL document queries — default implementations return empty/none for contexts without a document.
  virtual boost::optional<BCLComponent> getLocalComponent(const std::string& uid, const std::string& versionId = "") const {
    return boost::none;
  }
  virtual boost::optional<BCLMeasure> getLocalMeasure(const std::string& uid, const std::string& versionId = "") const {
    return boost::none;
  }
  virtual std::vector<BCLMeasure> getLocalMeasures() const {
    return {};
  }
  virtual std::size_t removeOutdatedLocalComponents(const std::string& uid, const std::string& currentVersionId) const {
    return 0;
  }
  virtual std::size_t removeOutdatedLocalMeasures(const std::string& uid, const std::string& currentVersionId) const {
    return 0;
  }
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BASEAPP_HPP
