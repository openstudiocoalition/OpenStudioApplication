/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_INSPECTORCONTROLLER_HPP
#define OPENSTUDIO_INSPECTORCONTROLLER_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/HVACComponent.hpp>

#include <QObject>

namespace openstudio {

class OSItem;

namespace model {

class ThermalZone;
class Loop;
class WaterToAirComponent;

}  // namespace model

class InspectorView;

class InspectorController : public QObject
{
  Q_OBJECT

 public:
  InspectorController();

  virtual ~InspectorController();

  InspectorView* inspectorView();

 signals:

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void itemRemoveClicked(OSItem*);

  void removeButtonClicked(bool);

  void workspaceObjectRemoved();

 public slots:

  void layoutModelObject(model::OptionalModelObject&, bool readOnly);

  void addBranchForZone(model::ThermalZone&);
  void removeBranchForZone(model::ThermalZone&);
  void moveBranchForZoneSupply(model::ThermalZone& zone, const Handle& newPlenumHandle);
  void moveBranchForZoneReturn(model::ThermalZone& zone, const Handle& newPlenumHandle);

  void addToLoop(model::Loop&, boost::optional<model::HVACComponent>&);
  void removeFromLoop(model::Loop&, boost::optional<model::HVACComponent>&);

  void toggleUnits(bool displayIP);

 private slots:

  void onViewDestroyed(QObject* object = nullptr);

 private:
  InspectorView* m_inspectorView;

  boost::optional<model::ModelObject> m_modelObject;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_INSPECTORCONTROLLER_HPP
