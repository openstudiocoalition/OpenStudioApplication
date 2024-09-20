/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULESETINSPECTORVIEW_HPP
#define OPENSTUDIO_SCHEDULESETINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"

#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/Model.hpp>

namespace openstudio {

class OSDropZone;
class OSLineEdit2;

class HoursOfOperationVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~HoursOfOperationVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class NumberOfPeopleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~NumberOfPeopleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class PeopleActivityScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~PeopleActivityScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class LightingScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~LightingScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class ElectricEquipmentScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~ElectricEquipmentScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class GasEquipmentScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~GasEquipmentScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class HotWaterEquipmentScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~HotWaterEquipmentScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class InfiltrationScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~InfiltrationScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SteamEquipmentScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SteamEquipmentScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class OtherEquipmentScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~OtherEquipmentScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class ScheduleSetInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleSetInspectorView(const model::Model& model, QWidget* parent = nullptr);
  virtual ~ScheduleSetInspectorView() {}

 protected:
  virtual void onClearSelection() override;
  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::DefaultScheduleSet& defaultScheduleSet);
  void detach();
  void configDropZones();

  OSLineEdit2* m_nameEdit;

  OSDropZone* m_hoursOfOperationDZ;
  OSDropZone* m_numberOfPeopleDZ;
  OSDropZone* m_peopleActivityScheduleDZ;
  OSDropZone* m_lightingScheduleDZ;
  OSDropZone* m_electricEquipmentScheduleDZ;
  OSDropZone* m_gasEquipmentScheduleDZ;
  OSDropZone* m_hotWaterEquipmentScheduleDZ;
  OSDropZone* m_infiltrationScheduleDZ;
  OSDropZone* m_ventilationPerPersonScheduleDZ;
  OSDropZone* m_ventilationPerFloorAreaScheduleDZ;

  HoursOfOperationVC* m_hoursOfOperationVC;
  NumberOfPeopleVC* m_numberOfPeopleVC;
  PeopleActivityScheduleVC* m_peopleActivityScheduleVC;
  LightingScheduleVC* m_lightingScheduleVC;
  ElectricEquipmentScheduleVC* m_electricEquipmentScheduleVC;
  GasEquipmentScheduleVC* m_gasEquipmentScheduleVC;
  HotWaterEquipmentScheduleVC* m_hotWaterEquipmentScheduleVC;
  InfiltrationScheduleVC* m_infiltrationScheduleVC;
  SteamEquipmentScheduleVC* m_steamEquipmentScheduleVC;
  OtherEquipmentScheduleVC* m_otherEquipmentScheduleVC;

  std::vector<ModelObjectVectorController*> m_vectorControllers;
  std::vector<OSDropZone*> m_dropZones;

  boost::optional<model::DefaultScheduleSet> m_defaultScheduleSet;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULESETINSPECTORVIEW_HPP
