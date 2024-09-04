/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALGASMIXTUREINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALGASMIXTUREINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/GasMixture.hpp>

namespace openstudio {

class OSComboBox2;

class OSIntegerEdit2;

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialGasMixtureInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialGasMixtureInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialGasMixtureInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::GasMixture& gasMixture);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::GasMixture> m_gasMixture;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_gas1Type = nullptr;
  OSComboBox2* m_gas2Type = nullptr;
  OSComboBox2* m_gas3Type = nullptr;
  OSComboBox2* m_gas4Type = nullptr;

  OSLineEdit2* m_2AEdit = nullptr;

  OSLineEdit2* m_3AEdit = nullptr;

  OSLineEdit2* m_4AEdit = nullptr;

  OSQuantityEdit2* m_thickness = nullptr;

  OSIntegerEdit2* m_numberOfGasesInMixture = nullptr;

  OSQuantityEdit2* m_gas1Fraction = nullptr;

  OSQuantityEdit2* m_gas2Fraction = nullptr;

  OSQuantityEdit2* m_gas3Fraction = nullptr;

  OSQuantityEdit2* m_gas4Fraction = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALGASMIXTUREINSPECTORVIEW_HPP
