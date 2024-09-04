/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/MasslessOpaqueMaterial.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class MaterialNoMassInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialNoMassInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialNoMassInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::MasslessOpaqueMaterial& masslessOpaqueMaterial);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::MasslessOpaqueMaterial> m_masslessOpaqueMaterial;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_roughness = nullptr;

  OSQuantityEdit2* m_thermalResistance = nullptr;

  OSQuantityEdit2* m_thermalAbsorptance = nullptr;

  OSQuantityEdit2* m_solarAbsorptance = nullptr;

  OSQuantityEdit2* m_visibleAbsorptance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALNOMASSINSPECTORVIEW_HPP
