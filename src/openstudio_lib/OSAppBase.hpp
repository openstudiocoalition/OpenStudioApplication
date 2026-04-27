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
 * OSAppBase is the abstract base class for the OpenStudio application, implementing the BaseApp
 * interface. It is subclassed by OpenStudioApp (standalone app) and provides common application
 * services: the measure manager, document management, and the current model accessor.
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

  virtual QWidget* mainWidget() override;
  virtual MeasureManager& measureManager() override;
  virtual boost::optional<openstudio::path> tempDir() override;
  virtual boost::optional<openstudio::model::Model> currentModel() override;
  //virtual boost::optional<openstudio::Workspace> currentWorkspace() override;
  virtual void updateSelectedMeasureState() override;
  virtual void addMeasure() override;
  virtual void duplicateSelectedMeasure() override;
  virtual void updateMyMeasures() override;
  virtual void updateBCLMeasures() override;
  virtual void openBclDlg() override;
  virtual void chooseHorizontalEditTab() override;
  virtual void checkForRemoteBCLUpdates() override;
  virtual QSharedPointer<openstudio::EditController> editController() override;
  boost::shared_ptr<WaitDialog> waitDialog() {
    return m_waitDialog;
  }
  virtual bool mouseOverInspectorView() override;

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
