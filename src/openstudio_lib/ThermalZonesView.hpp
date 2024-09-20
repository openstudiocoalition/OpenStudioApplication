/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_THERMALZONESVIEW_HPP
#define OPENSTUDIO_THERMALZONESVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"

#include <openstudio/model/ThermalZone.hpp>

namespace openstudio {

class ThermalZonesGridView;

class ThermalZonesView : public ModelSubTabView
{
  Q_OBJECT

 public:
  ThermalZonesView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~ThermalZonesView() {}
};

class ThermalZoneView : public ModelObjectInspectorView
{

  Q_OBJECT

 public:
  ThermalZoneView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~ThermalZoneView() {}

  virtual bool supportsMultipleObjectSelection() const override {
    return true;
  }
  virtual std::set<model::ModelObject> selectedObjects() const override;

 public slots:

  void refresh();

 signals:

  void modelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly);

  void dropZoneItemSelected(OSItem* item, bool readOnly);

 protected:
  void onClearSelection() override;

  void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  void onUpdate() override;

 private slots:

  void toggleUnits(bool) override;

  void toggleDisplayAdditionalProps(bool) override;

 private:
  ThermalZonesGridView* m_thermalZonesGridView = nullptr;

  bool m_isIP;

  bool m_displayAdditionalProps;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_THERMALZONESVIEW_HPP
