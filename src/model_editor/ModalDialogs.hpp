/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_MODALDIALOGS_HPP
#define MODELEDITOR_MODALDIALOGS_HPP

#include "ModelEditorAPI.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/core/UUID.hpp>

#include <QDialog>

class QLabel;
class QComboBox;
class QPushButton;

namespace openstudio {

class WorkspaceObject;

namespace model {

class ModelObject;
class Space;
class SpaceLoadInstance;

}  // namespace model
}  // namespace openstudio

class MODELEDITOR_API ModelObjectSelectorDialog
  : public QDialog
  , public Nano::Observer
{
  Q_OBJECT

 public:
  ModelObjectSelectorDialog(const openstudio::IddObjectType& typeToDisplay, const openstudio::model::Model& model, QWidget* parent = nullptr);

  ModelObjectSelectorDialog(const std::vector<openstudio::IddObjectType>& typesToDisplay, const openstudio::model::Model& model,
                            QWidget* parent = nullptr);

  virtual ~ModelObjectSelectorDialog();

  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  void setWindowTitle(const std::string& text);

  void setUserText(const std::string& text);

  void setOKButtonText(const std::string& text);

  void setCancelButtonText(const std::string& text);

  void onPushButtonOK(bool);

  void onPushButtonCancel(bool);

 signals:

  // emitted when the dialog is closed
  void closed(const boost::optional<openstudio::model::ModelObject>&);

 private slots:

  void onAddWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& type,
                            const openstudio::UUID& uuid);
  void onRemoveWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& type,
                               const openstudio::UUID& uuid);

 private:
  // for testing
  friend class ModelEditorFixture;

  QLabel* m_userTextLabel;
  QComboBox* m_comboBox;
  QPushButton* m_okButton;
  QPushButton* m_cancelButton;

  std::vector<openstudio::IddObjectType> m_typesToDisplay;
  openstudio::model::Model m_model;

  void init();
  void createWidgets();
  void connectSignalsAndSlots();
  void loadStyleSheet();
  void loadComboBoxData();
};

class MODELEDITOR_API ModelObjectSelectorDialogWatcher
  : public QObject
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit ModelObjectSelectorDialogWatcher(std::shared_ptr<ModelObjectSelectorDialog> modelObjectSelectorDialog);

  /// get the selected object
  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  /// true if has the model object selector dialog closed
  bool isSelectionFinal() const;

 private slots:

  virtual void onClose(const boost::optional<openstudio::model::ModelObject>&);

 private:
  std::shared_ptr<ModelObjectSelectorDialog> m_modelObjectSelectorDialog;
  mutable boost::optional<openstudio::model::ModelObject> m_selectedModelObject;
};

MODELEDITOR_API void ensureThermalZone(openstudio::model::Space& space);

MODELEDITOR_API void ensureSpaceLoadDefinition(openstudio::model::SpaceLoadInstance& instance);

#endif  //MODELEDITOR_MODALDIALOGS_HPP
