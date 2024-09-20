/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/StandardOpaqueMaterial.hpp>

class QComboBox;

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class MaterialInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::StandardOpaqueMaterial& standardOpaqueMaterial);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::StandardOpaqueMaterial> m_standardOpaqueMaterial;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_roughness = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_conductivity = nullptr;

  OSQuantityEdit2* m_density = nullptr;

  OSQuantityEdit2* m_specificHeat = nullptr;

  OSQuantityEdit2* m_thermalAbsorptance = nullptr;

  OSQuantityEdit2* m_solarAbsorptance = nullptr;

  OSQuantityEdit2* m_visibleAbsorptance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALINSPECTORVIEW_HPP
