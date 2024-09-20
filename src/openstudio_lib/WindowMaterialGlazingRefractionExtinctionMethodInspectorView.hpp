/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALGLAZINGREFRACTIONEXTINCTIONMETHODINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALGLAZINGREFRACTIONEXTINCTIONMETHODINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/RefractionExtinctionGlazing.hpp>

namespace openstudio {

class OSLineEdit2;

class OSSwitch2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialGlazingRefractionExtinctionMethodInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialGlazingRefractionExtinctionMethodInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialGlazingRefractionExtinctionMethodInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::RefractionExtinctionGlazing& refractionExtinctionGlazing);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::RefractionExtinctionGlazing> m_refractionExtinctionGlazing;

  OSLineEdit2* m_nameEdit = nullptr;

  OSSwitch2* m_solarDiffusing = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_solarIndexOfRefraction = nullptr;

  OSQuantityEdit2* m_solarExtinctionCoefficient = nullptr;

  OSQuantityEdit2* m_visibleIndexOfRefraction = nullptr;

  OSQuantityEdit2* m_visibleExtinctionCoefficient = nullptr;

  OSQuantityEdit2* m_infraredTransmittanceAtNormalIncidence = nullptr;

  OSQuantityEdit2* m_infraredHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_conductivity = nullptr;

  OSQuantityEdit2* m_dirtCorrectionFactorForSolarAndVisibleTransmittance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALGLAZINGREFRACTIONEXTINCTIONMETHODINSPECTORVIEW_HPP
