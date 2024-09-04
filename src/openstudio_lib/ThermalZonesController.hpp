/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_THERMALZONESCONTROLLER_HPP
#define OPENSTUDIO_THERMALZONESCONTROLLER_HPP

#include "ModelSubTabController.hpp"

#include "../shared_gui_components/OSQObjectController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Schedule.hpp>

#include <boost/smart_ptr.hpp>

namespace openstudio {

namespace model {
class ThermalZone;

class ZoneHVACComponent;
}  // namespace model

class ThermalZonesView;

class ThermalZonesController : public ModelSubTabController
{
  Q_OBJECT

 public:
  ThermalZonesController(bool isIP, bool displayAdditionalProps, const model::Model& model);

  virtual ~ThermalZonesController() {}

 signals:

  void toggleUnitsClicked(bool);

  void toggleDisplayAdditionalPropsClicked(bool);

 protected:
  virtual void onSelectItem(OSItem* item) override;

  virtual void onAddObject(const openstudio::IddObjectType& iddObjectType) override;

  virtual void onAddObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onCopyObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onRemoveObject(openstudio::model::ModelObject modelObject) override;

  virtual void onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) override;

  virtual void onPurgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  virtual void onDrop(const OSItemId& itemId) override;

  virtual void onInspectItem(OSItem* item) override;

 public slots:

  void setHeatingSchedule(model::ThermalZone&, model::OptionalSchedule&);

  void setCoolingSchedule(model::ThermalZone&, model::OptionalSchedule&);

  void setHumidifyingSchedule(model::ThermalZone&, model::OptionalSchedule&);

  void setDehumidifyingSchedule(model::ThermalZone&, model::OptionalSchedule&);

  void addComponentToZone(model::ThermalZone&, Handle&);

  void removeZoneHVACComponent(model::ZoneHVACComponent&);

 private slots:

  void onZoneHVACComponentClicked(const model::ZoneHVACComponent&);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_THERMALZONESCONTROLLER_HPP
