/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALSIMPLEGLAZINGSYSTEMINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALSIMPLEGLAZINGSYSTEMINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/SimpleGlazing.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialSimpleGlazingSystemInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialSimpleGlazingSystemInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialSimpleGlazingSystemInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::SimpleGlazing& simpleGlazing);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::SimpleGlazing> m_simpleGlazing;

  OSLineEdit2* m_nameEdit = nullptr;

  OSQuantityEdit2* m_uFactor = nullptr;

  OSQuantityEdit2* m_solarHeatGainCoefficient = nullptr;

  OSQuantityEdit2* m_visibleTransmittance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALSIMPLEGLAZINGSYSTEMINSPECTORVIEW_HPP
