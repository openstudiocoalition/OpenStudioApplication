/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALGLAZINGGROUPTHERMOCHROMICINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALGLAZINGGROUPTHERMOCHROMICINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ThermochromicGlazing.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSQuantityEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialGlazingGroupThermochromicInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialGlazingGroupThermochromicInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialGlazingGroupThermochromicInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::ThermochromicGlazing& thermochromicGlazing);

  void detach();

  void refresh();

  bool m_isIP;

  boost::optional<model::ThermochromicGlazing> m_thermochromicGlazing;

  OSLineEdit2* m_nameEdit = nullptr;

  OSQuantityEdit2* m_opticalDataTemperature = nullptr;

  OSLineEdit2* m_windowMaterialGlazingName = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALGLAZINGGROUPTHERMOCHROMICINSPECTORVIEW_HPP
