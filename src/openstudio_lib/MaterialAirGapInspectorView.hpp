/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MATERIALAIRGAPINSPECTORVIEW_HPP
#define OPENSTUDIO_MATERIALAIRGAPINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/AirGap.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class MaterialAirGapInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  MaterialAirGapInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~MaterialAirGapInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::AirGap& airGap);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::AirGap> m_airGap;

  OSLineEdit2* m_nameEdit = nullptr;

  OSQuantityEdit2* m_thermalResistance = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MATERIALAIRGAPINSPECTORVIEW_HPP
