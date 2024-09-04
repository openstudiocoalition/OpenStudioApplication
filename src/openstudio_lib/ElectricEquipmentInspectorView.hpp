/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICEQUIPMENTINSPECTORVIEW_HPP
#define OPENSTUDIO_ELECTRICEQUIPMENTINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ElectricEquipmentDefinition.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSDropZone;

class ElectricEquipmentDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ElectricEquipmentDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ElectricEquipmentDefinitionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::ElectricEquipmentDefinition& electricEquipmentDefinition);

  void detach();

  void refresh();

  OSLineEdit2* m_nameEdit;

  OSQuantityEdit2* m_designLevelEdit;

  OSQuantityEdit2* m_wattsPerSpaceFloorAreaEdit;

  OSQuantityEdit2* m_wattsPerPersonEdit;

  OSQuantityEdit2* m_fractionLatentEdit;

  OSQuantityEdit2* m_fractionRadiantEdit;

  OSQuantityEdit2* m_fractionLostEdit;

  bool m_isIP;

  boost::optional<model::ElectricEquipmentDefinition> m_electricEquipmentDefinition;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICEQUIPMENTINSPECTORVIEW_HPP
