/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACELOADINSTANCESWIDGET_HPP
#define OPENSTUDIO_SPACELOADINSTANCESWIDGET_HPP

#include "ModelObjectVectorController.hpp"

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/Building.hpp>
#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/SpaceLoadInstance.hpp>
#include <openstudio/model/SpaceType.hpp>

#include <openstudio/utilities/idf/WorkspaceObject_Impl.hpp>

#include <QWidget>

class QGridLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace openstudio {

class OSDoubleEdit2;
class OSDropZone;
class OSIntegerEdit;
class OSLineEdit2;

class SpaceLoadInstanceDefinitionVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SpaceLoadInstanceDefinitionVectorController() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
};

class SpaceLoadInstanceScheduleVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SpaceLoadInstanceScheduleVectorController() {}
  virtual void attach(const model::ModelObject& modelObject) override;

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;

 private:
  void attachOtherModelObjects(const model::SpaceLoadInstance& spaceLoadInstance);
  void attachOtherModelObjects(const model::Space& space);
  void attachOtherModelObjects(const model::SpaceType& spaceType);
  void attachOtherModelObjects(const model::BuildingStory& buildingStory);
  void attachOtherModelObjects(const model::Building& building);
  void attachOtherModelObjects(const model::DefaultScheduleSet& defaultScheduleSet);
};

class SpaceLoadInstanceActivityScheduleVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SpaceLoadInstanceActivityScheduleVectorController() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SpaceLoadInstanceMiniView : public QWidget
{
  Q_OBJECT

 public:
  SpaceLoadInstanceMiniView(const model::SpaceLoadInstance& spaceLoadInstance, bool isDefault);
  virtual ~SpaceLoadInstanceMiniView() {}
  model::SpaceLoadInstance spaceLoadInstance() const;
  bool isDefault() const;

 signals:
  void removeClicked(SpaceLoadInstanceMiniView* spaceLoadInstanceMiniView);

 private slots:
  void onRemoveClicked();

 private:
  OSDoubleEdit2* m_multiplierEdit;
  OSLineEdit2* m_nameEdit;
  QLabel* m_activityScheduleLabel;
  QPushButton* m_removeButton;

  SpaceLoadInstanceDefinitionVectorController* m_definitionVectorController;
  OSDropZone* m_definitionDropZone;

  SpaceLoadInstanceScheduleVectorController* m_scheduleVectorController;
  OSDropZone* m_scheduleDropZone;

  SpaceLoadInstanceActivityScheduleVectorController* m_activityScheduleVectorController;
  OSDropZone* m_activityScheduleDropZone;

  model::SpaceLoadInstance m_spaceLoadInstance;
  bool m_isDefault;

  boost::optional<model::SpaceLoadInstance> opt_spaceLoadInstance;

 private:
  void createLayout(bool isDefault);
};

class NewSpaceLoadVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~NewSpaceLoadVectorController() {}

 protected:
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SpaceLoadInstancesWidget
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit SpaceLoadInstancesWidget(QWidget* parent = nullptr);
  virtual ~SpaceLoadInstancesWidget() {}
  void attach(const model::Space& space);
  void attach(const model::SpaceType& spaceType);
  void detach();

 private slots:

  void remove(SpaceLoadInstanceMiniView* spaceLoadInstanceMiniView);

  void onBuildingRelationshipChange(int index, Handle, Handle);

  void onSpaceTypeRelationshipChange(int index, Handle, Handle);

  void onSpaceRelationshipChange(int index, Handle, Handle);

  void objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);

  void objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);

  void refresh();

 private:
  void addSpaceLoads(const model::Space& space);

  void addSpaceTypeLoads(const model::SpaceType& spaceType, bool addAsDefaultLoads);

  void addSpaceLoadInstance(const model::SpaceLoadInstance& spaceLoadInstance, bool isDefault);

  QVBoxLayout* m_mainVLayout;

  NewSpaceLoadVectorController* m_newSpaceLoadVectorController;
  OSDropZone* m_newSpaceLoadDropZone;

  boost::optional<model::Space> m_space;
  boost::optional<model::SpaceType> m_spaceType;
  boost::optional<model::Model> m_model;

  bool m_dirty;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACELOADINSTANCESWIDGET_HPP
