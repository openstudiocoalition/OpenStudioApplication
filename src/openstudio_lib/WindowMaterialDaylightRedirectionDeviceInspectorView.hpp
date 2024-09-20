/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_WINDOWMATERIALDAYLIGHTREDIRECTIONDEVICEINSPECTORVIEW_HPP
#define OPENSTUDIO_WINDOWMATERIALDAYLIGHTREDIRECTIONDEVICEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/DaylightRedirectionDevice.hpp>

namespace openstudio {

class OSComboBox2;

class OSLineEdit2;

class StandardsInformationMaterialWidget;

class WindowMaterialDaylightRedirectionDeviceInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  WindowMaterialDaylightRedirectionDeviceInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~WindowMaterialDaylightRedirectionDeviceInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::DaylightRedirectionDevice& daylightRedirectionDevice);

  void detach();

  void refresh();

  bool m_isIP;

  OSLineEdit2* m_nameEdit = nullptr;

  OSComboBox2* m_daylightRedirectionDeviceType = nullptr;

  StandardsInformationMaterialWidget* m_standardsInformationWidget = nullptr;

  boost::optional<model::DaylightRedirectionDevice> m_material;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_WINDOWMATERIALDAYLIGHTREDIRECTIONDEVICEINSPECTORVIEW_HPP
