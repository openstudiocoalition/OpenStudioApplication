/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALSCREENINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALSCREENINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/Screen.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialScreenInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialScreenInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialScreenInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  // cppcheck-suppress constParameter
  void attach(openstudio::model::Screen& screen);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::Screen> m_screen;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_reflectedBeamTransmittanceAccountingMethod = nullptr;

  OSComboBox2* m_angleOfResolutionForScreenTransmittanceOutputMap = nullptr;

  OSQuantityEdit2* m_diffuseSolarReflectance = nullptr;

  OSQuantityEdit2* m_diffuseVisibleReflectance = nullptr;

  OSQuantityEdit2* m_thermalHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_conductivity = nullptr;

  OSQuantityEdit2* m_screenMaterialSpacing = nullptr;

  OSQuantityEdit2* m_screenMaterialDiameter = nullptr;

  OSQuantityEdit2* m_screenToGlassDistance = nullptr;

  OSQuantityEdit2* m_topOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_bottomOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_leftSideOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_rightSideOpeningMultiplier = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALSCREENINSPECTORVIEW_HPP
