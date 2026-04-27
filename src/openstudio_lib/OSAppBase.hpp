/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSAPPBASE_HPP
#define OPENSTUDIO_OSAPPBASE_HPP

#include "../shared_gui_components/BaseApp.hpp"

#include <openstudio/measure/OSMeasureInfoGetter.hpp>
#include "../openstudio_qt_utils/QMetaTypes.hpp"

#include <openstudio/utilities/core/Logger.hpp>

#include <QApplication>

#include <boost/smart_ptr.hpp>

class QEvent;

namespace openstudio {

class OSDocument;

class WaitDialog;

/**
 * OSAppBase is the abstract base class for the full OpenStudio application. It:
 *
 *   1. Owns the Qt event loop by inheriting QApplication — only one instance may exist
 *      per process, and it must outlive all widgets.
 *
 *   2. Implements the BaseApp interface — all virtual methods declared in BaseApp are
 *      overridden here as `final`, forwarding to the current OSDocument and MainWindow.
 *      Shared components reach this implementation via static_cast<BaseApp*>(qApp).
 *
 *   3. Leaves currentDocument() pure — concrete subclasses (e.g. OpenStudioApp) supply
 *      the document ownership strategy.
 *
 * Separation rationale:
 *   BaseApp lives in openstudio_shared_gui (lower-level static lib) and knows nothing
 *   about OSDocument or MainWindow. OSAppBase lives in openstudio_lib and bridges the
 *   two: it receives calls through the BaseApp interface and dispatches them to the
 *   real application objects. This one-way dependency prevents the linker errors that
 *   arise on Linux when a lower-level archive references symbols defined in a
 *   higher-level archive.
 */
class OSAppBase
  : public QApplication
  , public BaseApp
{

  Q_OBJECT;

 public:
  OSAppBase(int& argc, char** argv, const QSharedPointer<MeasureManager>& t_measureManager);

  virtual ~OSAppBase();

  virtual std::shared_ptr<OSDocument> currentDocument() const = 0;

  static OSAppBase* instance();

  QWidget* mainWidget() final;
  MeasureManager& measureManager() final;
  boost::optional<openstudio::path> tempDir() final;
  boost::optional<openstudio::model::Model> currentModel() final;
  //boost::optional<openstudio::Workspace> currentWorkspace() final;
  void updateSelectedMeasureState() final;
  void addMeasure() final;
  void duplicateSelectedMeasure() final;
  void updateMyMeasures() final;
  void updateBCLMeasures() final;
  void openBclDlg() final;
  void chooseHorizontalEditTab() final;
  void checkForRemoteBCLUpdates() final;
  QSharedPointer<openstudio::EditController> editController() final;
  boost::shared_ptr<WaitDialog> waitDialog() {
    return m_waitDialog;
  }
  bool mouseOverInspectorView() final;

  bool useClassicCLI() const final;
  void disableDocument() final;
  void enableDocument() final;
  boost::optional<BCLComponent> getLocalComponent(const std::string& uid, const std::string& versionId = "") const final;
  boost::optional<BCLMeasure> getLocalMeasure(const std::string& uid, const std::string& versionId = "") const final;
  std::vector<BCLMeasure> getLocalMeasures() const final;
  std::size_t removeOutdatedLocalComponents(const std::string& uid, const std::string& currentVersionId) const final;
  std::size_t removeOutdatedLocalMeasures(const std::string& uid, const std::string& currentVersionId) const final;

  virtual openstudio::path dviewPath() const;
  virtual bool notify(QObject* receiver, QEvent* e) override;

  // Slots
  void addWorkspaceObject(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type, const openstudio::UUID& uuid);
  void addWorkspaceObjectPtr(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                             const openstudio::UUID& uuid);

  void removeWorkspaceObject(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type, const openstudio::UUID& uuid);
  void removeWorkspaceObjectPtr(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                const openstudio::UUID& uuid);

 signals:
  void workspaceObjectAdded(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type, const openstudio::UUID& uuid);
  void workspaceObjectAddedPtr(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                               const openstudio::UUID& uuid);

  void workspaceObjectRemoved(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type, const openstudio::UUID& uuid);
  void workspaceObjectRemovedPtr(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                 const openstudio::UUID& uuid);

 protected:
  virtual bool event(QEvent* e) override;

  virtual void childEvent(QChildEvent* e) override;

  REGISTER_LOGGER("openstudio.OSAppBase");

 private:
  QSharedPointer<openstudio::MeasureManager> m_measureManager;

  boost::shared_ptr<WaitDialog> m_waitDialog;

 public slots:

  virtual void reloadFile(const QString& osmPath, bool modified, bool saveCurrentTabs) = 0;

  void showMeasureUpdateDlg();

  virtual void configureExternalTools();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSAPPBASE_HPP
