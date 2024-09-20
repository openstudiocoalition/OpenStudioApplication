/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALINFRAREDTRANSPARENTINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALINFRAREDTRANSPARENTINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/InfraredTransparentMaterial.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class MaterialInfraredTransparentInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialInfraredTransparentInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialInfraredTransparentInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::InfraredTransparentMaterial& infraredTransparentMaterial);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::InfraredTransparentMaterial> m_infraredTransparentMaterial;

  OSLineEdit2* m_nameEdit = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALINFRAREDTRANSPARENTINSPECTORVIEW_HPP
