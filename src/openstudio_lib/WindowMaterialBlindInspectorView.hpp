/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALBLINDINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALBLINDINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/Blind.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialBlindInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialBlindInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialBlindInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  // cppcheck-suppress constParameter
  void attach(openstudio::model::Blind& blind);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::Blind> m_material;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_slatOrientation = nullptr;

  OSQuantityEdit2* m_slatWidth = nullptr;

  OSQuantityEdit2* m_slatSeparation = nullptr;

  OSQuantityEdit2* m_slatThickness = nullptr;

  OSQuantityEdit2* m_slatAngle = nullptr;

  OSQuantityEdit2* m_slatConductivity = nullptr;

  OSQuantityEdit2* m_slatBeamSolarTransmittance = nullptr;

  OSQuantityEdit2* m_frontSideSlatBeamSolarReflectance = nullptr;

  OSQuantityEdit2* m_backSideSlatBeamSolarReflectance = nullptr;

  OSQuantityEdit2* m_slatDiffuseSolarTransmittance = nullptr;

  OSQuantityEdit2* m_frontSideSlatDiffuseSolarReflectance = nullptr;

  OSQuantityEdit2* m_backSideSlatDiffuseSolarReflectance = nullptr;

  OSQuantityEdit2* m_slatBeamVisibleTransmittance = nullptr;

  OSQuantityEdit2* m_frontSideSlatBeamVisibleReflectance = nullptr;

  OSQuantityEdit2* m_backSideSlatBeamVisibleReflectance = nullptr;

  OSQuantityEdit2* m_slatDiffuseVisibleTransmittance = nullptr;

  OSQuantityEdit2* m_frontSideSlatDiffuseVisibleReflectance = nullptr;

  OSQuantityEdit2* m_backSideSlatDiffuseVisibleReflectance = nullptr;

  OSQuantityEdit2* m_slatInfraredHemisphericalTransmittance = nullptr;

  OSQuantityEdit2* m_frontSideSlatInfraredHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_backSideSlatInfraredHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_blindToGlassDistance = nullptr;

  OSQuantityEdit2* m_blindTopOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_blindBottomOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_blindLeftSideOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_blindRightSideOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_minimumSlatAngle = nullptr;

  OSQuantityEdit2* m_maximumSlatAngle = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALBLINDINSPECTORVIEW_HPP
