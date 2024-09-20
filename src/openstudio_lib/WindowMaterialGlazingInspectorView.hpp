/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALGLAZINGINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALGLAZINGINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/StandardGlazing.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class OSSwitch2;

class StandardsInformationMaterialWidget;

class WindowMaterialGlazingInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialGlazingInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialGlazingInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::StandardGlazing& glazing);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::StandardGlazing> m_glazing;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_opticalDataType = nullptr;

  OSSwitch2* m_solarDiffusing = nullptr;

  // OSLineEdit2 * m_windowGlassSpectralDataSetName = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_solarTransmittanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_frontSideSolarReflectanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_backSideSolarReflectanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_visibleTransmittanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_frontSideVisibleReflectanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_backSideVisibleReflectanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_infraredTransmittanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_frontSideInfraredHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_backSideInfraredHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_conductivity = nullptr;

  OSQuantityEdit2* m_dirtCorrectionFactorForSolarAndVisibleTransmittance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALGLAZINGINSPECTORVIEW_HPP
