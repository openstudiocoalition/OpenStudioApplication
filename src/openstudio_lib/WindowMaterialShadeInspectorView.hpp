/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALSHADEINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALSHADEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/Shade.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialShadeInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialShadeInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialShadeInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::Shade& shade);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::Shade> m_shade;

  OSLineEdit2* m_nameEdit = nullptr;

  OSQuantityEdit2* m_solarTransmittance = nullptr;

  OSQuantityEdit2* m_solarReflectance = nullptr;

  OSQuantityEdit2* m_visibleTransmittance = nullptr;

  OSQuantityEdit2* m_visibleReflectance = nullptr;

  OSQuantityEdit2* m_thermalHemisphericalEmissivity = nullptr;

  OSQuantityEdit2* m_thermalTransmittance = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_conductivity = nullptr;

  OSQuantityEdit2* m_shadeToGlassDistance = nullptr;

  OSQuantityEdit2* m_topOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_bottomOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_leftSideOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_rightSideOpeningMultiplier = nullptr;

  OSQuantityEdit2* m_airflowPermeability = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALSHADEINSPECTORVIEW_HPP
