/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALSVIEW_HPP
#define OPENSTUDIO_MATERIALSVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

class QStackedWidget;

namespace openstudio {

class MaterialsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  MaterialsView(bool isIP, const openstudio::model::Model& model, const QString& tabLabel, bool hasSubTabs, QWidget* parent = nullptr);

  virtual ~MaterialsView() {}

 private:
  static std::vector<std::pair<IddObjectType, std::string>> modelObjectTypesAndNames();
};

class MaterialsInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialsInspectorView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialsInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void showMaterialAirGapInspectorView(const openstudio::model::ModelObject& modelObject);
  void showMaterialInfraredTransparentInspectorView(const openstudio::model::ModelObject& modelObject);
  void showMaterialInspectorView(const openstudio::model::ModelObject& modelObject);
  void showMaterialNoMassInspectorView(const openstudio::model::ModelObject& modelObject);
  void showMaterialRoofVegetationInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialBlindInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialDaylightRedirectionDeviceInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialGasInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialGasMixtureInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialGlazingGroupThermochromicInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialGlazingInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialGlazingRefractionExtinctionMethodInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialScreenInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialShadeInspectorView(const openstudio::model::ModelObject& modelObject);
  void showWindowMaterialSimpleGlazingSystemInspectorView(const openstudio::model::ModelObject& modelObject);

  void showInspector(QWidget* widget);

  void showDefaultView();

  std::map<openstudio::IddObjectType, int> m_inspectorIndexMap;

  bool m_isIP;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALSVIEW_HPP
