/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BASEAPP_HPP
#define SHAREDGUICOMPONENTS_BASEAPP_HPP

#include "BaseDocument.hpp"
#include "LocalLibrary.hpp"
#include "OSItemId.hpp"

#include "../openstudio_qt_utils/QMetaTypes.hpp"

#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/core/Path.hpp>

#include <QApplication>
#include <QWidget>
#include <boost/optional.hpp>
//#include "EditController.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace openstudio {
class MeasureManager;
class EditController;
class OSItem;
class Workspace;

/**
 * BaseApp is the **pure virtual interface** through which all shared GUI components access
 * application services. It lives in openstudio_shared_gui (a lower-level library) and
 * deliberately has no dependency on OSAppBase, OSDocument, or any openstudio_lib symbol.
 *
 * Responsibilities:
 *   - Declare the polymorphic API that shared widgets require (measure manager, BCL
 *     queries, CLI mode flag, inspector focus state, etc.).
 *   - Be reachable process-wide via BaseApp::instance(), which down-casts qApp
 *     (safe because OSAppBase inherits both QApplication and BaseApp).
 *
 * Every method is pure virtual — OSAppBase (in openstudio_lib) provides all implementations,
 * each marked `final`.
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
  virtual bool useClassicCLI() const = 0;

  /// Disable the document UI (e.g. while a drop operation is in progress).
  virtual void disableDocument() = 0;

  /// Re-enable the document UI after a disable call.
  virtual void enableDocument() = 0;

  /// BCL document queries.
  virtual boost::optional<BCLComponent> getLocalComponent(const std::string& uid, const std::string& versionId = "") const = 0;
  virtual boost::optional<BCLMeasure> getLocalMeasure(const std::string& uid, const std::string& versionId = "") const = 0;
  virtual std::vector<BCLMeasure> getLocalMeasures() const = 0;
  virtual std::size_t removeOutdatedLocalComponents(const std::string& uid, const std::string& currentVersionId) const = 0;
  virtual std::size_t removeOutdatedLocalMeasures(const std::string& uid, const std::string& currentVersionId) const = 0;

  /// Returns the current document, or nullptr if no document is open.
  virtual BaseDocument* currentDocument() const = 0;

  /// Factory: create the correct OSItem subclass for a given id.
  virtual OSItem* makeItem(const OSItemId& itemId, OSItemType osItemType) = 0;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BASEAPP_HPP
