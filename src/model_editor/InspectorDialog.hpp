/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_INSPECTORDIALOG_HPP
#define MODELEDITOR_INSPECTORDIALOG_HPP

#include "ModelEditorAPI.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddFile.hpp>
#include <openstudio/utilities/core/UUID.hpp>
#include <openstudio/utilities/core/Enum.hpp>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <QWidget>
#include <QMainWindow>

class QLabel;
class QListWidget;
class QStackedWidget;
class QTableWidget;
class QPushButton;
class QShowEvent;
class QCloseEvent;
class InspectorGadget;

namespace openstudio {
class WorkspaceObject;

namespace model {
class ModelObject;
}
}  // namespace openstudio

#ifndef Q_MOC_RUN
OPENSTUDIO_ENUM(InspectorDialogClient, ((AllOpenStudio))((SketchUpPlugin)));
#endif

class MODELEDITOR_API InspectorDialog
  : public QMainWindow
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  /// create a new dialog with empty model
  explicit InspectorDialog(InspectorDialogClient client = InspectorDialogClient::AllOpenStudio, QWidget* parent = nullptr);

  /// create a new dialog for existing model
  explicit InspectorDialog(openstudio::model::Model& model, InspectorDialogClient client = InspectorDialogClient::AllOpenStudio,
                           QWidget* parent = nullptr);

  virtual ~InspectorDialog();

  // get the idd object type
  openstudio::IddObjectType iddObjectType() const;

  // set the idd object type, must be an allowable type
  bool setIddObjectType(const openstudio::IddObjectType&, bool force = false);

  // get handles of the selected objects
  std::vector<openstudio::Handle> selectedObjectHandles() const;

  // set the selected object handles, all handles must be found in current model and be of the
  // same allowable idd object type
  bool setSelectedObjectHandles(const std::vector<openstudio::Handle>&, bool force = false);

  // get the current model
  openstudio::model::Model model() const;

  // point the dialog at a new model
  void setModel(const openstudio::model::Model& model, bool force = false);

  // void rebuild inspector gadget
  void rebuildInspectorGadget(bool recursive);

  // save the state
  void saveState();

  // restore the state
  void restoreState();

  void displayIP(const bool displayIP);

 public slots:

  virtual void onIddObjectTypeChanged(const openstudio::IddObjectType&);

  virtual void onSelectedObjectHandlesChanged(const std::vector<openstudio::Handle>&);

  virtual void onModelChanged(openstudio::model::Model&);

  virtual void onPushButtonNew(bool);

  virtual void onPushButtonCopy(bool);

  virtual void onPushButtonDelete(bool);

  virtual void onPushButtonPurge(bool);

 signals:

  // emitted when user selects a new idd object type, will be an allowable type
  void iddObjectTypeChanged(const openstudio::IddObjectType&);

  // emitted when selected objects change, all handles will be in model and of the same allowable type
  void selectedObjectHandlesChanged(const std::vector<openstudio::Handle>&);

  // emitted when inspected model changes
  void modelChanged(openstudio::model::Model&);

  void toggleUnitsClicked(bool);

 protected:
  // handle show event
  virtual void showEvent(QShowEvent* event) override;

  // handle close event
  virtual void closeEvent(QCloseEvent* event) override;

 private slots:

  // for testing
  friend class ModelEditorFixture;

  //void onCheckBox(bool checked);
  void onListWidgetSelectionChanged();
  void onTableWidgetSelectionChanged();
  void onAddWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& type,
                            const openstudio::UUID& uuid);
  void onWorkspaceChange();
  void onTimeout();
  void onRemoveWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& type,
                               const openstudio::UUID& uuid);

  void onNeedsSetFocus();

 private:
  QListWidget* m_listWidget;
  QStackedWidget* m_stackedWidget;
  QLabel* m_selectionLabel;
  QTableWidget* m_tableWidget;
  QPushButton* m_pushButtonNew;
  QPushButton* m_pushButtonCopy;
  QPushButton* m_pushButtonDelete;
  QPushButton* m_pushButtonPurge;
  InspectorGadget* m_inspectorGadget;

  openstudio::IddFile m_iddFile;
  std::set<openstudio::IddObjectType> m_typesToDisplay;
  std::set<openstudio::IddObjectType> m_disableSelectionTypes;
  std::set<openstudio::IddObjectType> m_disableAddTypes;
  std::set<openstudio::IddObjectType> m_disableCopyTypes;
  std::set<openstudio::IddObjectType> m_disableRemoveTypes;
  std::set<openstudio::IddObjectType> m_disablePurgeTypes;

  openstudio::IddObjectType m_iddObjectType;
  std::vector<openstudio::Handle> m_objectHandles;
  std::vector<openstudio::Handle> m_selectedObjectHandles;
  openstudio::model::Model m_model;
  bool m_workspaceChanged;
  bool m_workspaceObjectAdded;
  bool m_workspaceObjectRemoved;

  void init(InspectorDialogClient client);
  void createWidgets();
  void connectSelfSignalsAndSlots();
  void connectModelSignalsAndSlots();
  void hideSelectionWidget(bool hideSelectionWidget);
  void loadStyleSheet();
  void loadListWidgetData();
  void updateListWidgetData();
  void loadTableWidgetData();
  void setTableWidgetHeader();
  void getTableWidgetSelected(std::vector<openstudio::Handle>& selectedHandles);
  void loadModel();
};

#endif  // MODELEDITOR_INSPECTORDIALOG_HPP
