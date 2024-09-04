/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALGASINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALGASINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/Gas.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialGasInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialGasInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialGasInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::Gas& gas);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::Gas> m_gas;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_gasType = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSQuantityEdit2* m_conductivityCoefficientA = nullptr;

  OSQuantityEdit2* m_conductivityCoefficientB = nullptr;

  OSQuantityEdit2* m_viscosityCoefficientA = nullptr;

  OSQuantityEdit2* m_viscosityCoefficientB = nullptr;

  OSQuantityEdit2* m_specificHeatCoefficientA = nullptr;

  OSQuantityEdit2* m_specificHeatCoefficientB = nullptr;

  OSQuantityEdit2* m_molecularWeight = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALGASINSPECTORVIEW_HPP
