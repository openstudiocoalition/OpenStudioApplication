/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WATERUSEEQUIPMENTINSPECTORVIEW_HPP
#define OPENSTUDIO_WATERUSEEQUIPMENTINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/WaterUseEquipmentDefinition.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSDropZone;

class TargetTemperatureScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~TargetTemperatureScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;

  boost::optional<model::WaterUseEquipmentDefinition> waterUseEquipmentDefinition() const;
};

class SensibleFractionScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SensibleFractionScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;

  boost::optional<model::WaterUseEquipmentDefinition> waterUseEquipmentDefinition() const;
};

class LatentFractionScheduleVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~LatentFractionScheduleVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;

  boost::optional<model::WaterUseEquipmentDefinition> waterUseEquipmentDefinition() const;
};

class WaterUseEquipmentDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WaterUseEquipmentDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WaterUseEquipmentDefinitionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::WaterUseEquipmentDefinition& waterUseEquipmentDefinition);

  void detach();

  void refresh();

  OSLineEdit2* m_nameEdit;

  OSLineEdit2* m_endUseSubcategoryEdit;

  OSQuantityEdit2* m_peakFlowRateEdit;

  TargetTemperatureScheduleVC* m_targetTemperatureScheduleVC;
  SensibleFractionScheduleVC* m_sensibleFractionScheduleVC;
  LatentFractionScheduleVC* m_latentFractionScheduleVC;

  OSDropZone* m_targetTemperatureScheduleDZ;
  OSDropZone* m_sensibleFractionScheduleDZ;
  OSDropZone* m_latentFractionScheduleDZ;

  bool m_isIP;

  boost::optional<model::WaterUseEquipmentDefinition> m_waterUseEquipmentDefinition;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WATERUSEEQUIPMENTINSPECTORVIEW_HPP
