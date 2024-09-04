/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALROOFVEGETATIONINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALROOFVEGETATIONINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/RoofVegetation.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class MaterialRoofVegetationInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialRoofVegetationInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialRoofVegetationInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::RoofVegetation& roofVegetation);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::RoofVegetation> m_roofVegetation;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_roughness = nullptr;

  OSLineEdit2* m_soilLayerName = nullptr;

  OSLineEdit2* m_moistureDiffusionCalculationMethod = nullptr;

  OSQuantityEdit2* m_heightOfPlants = nullptr;

  OSQuantityEdit2* m_leafAreaIndex = nullptr;

  OSQuantityEdit2* m_leafReflectivity = nullptr;

  OSQuantityEdit2* m_leafEmissivity = nullptr;

  OSQuantityEdit2* m_minimumStomatalResistance = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_conductivityOfDrySoil = nullptr;

  OSQuantityEdit2* m_densityOfDrySoil = nullptr;

  OSQuantityEdit2* m_specificHeatOfDrySoil = nullptr;

  OSQuantityEdit2* m_thermalAbsorptance = nullptr;

  OSQuantityEdit2* m_visibleAbsorptance = nullptr;

  OSQuantityEdit2* m_saturationVolumetricMoistureContentOfTheSoilLayer = nullptr;

  OSQuantityEdit2* m_residualVolumetricMoistureContentOfTheSoilLayer = nullptr;

  OSQuantityEdit2* m_initialVolumetricMoistureContentOfTheSoilLayer = nullptr;

  OSQuantityEdit2* m_solarAbsorptance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALROOFVEGETATIONINSPECTORVIEW_HPP
