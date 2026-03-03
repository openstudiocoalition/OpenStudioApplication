/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SITEGROUNDTEMPERATURECONTROLLER_HPP
#define OPENSTUDIO_SITEGROUNDTEMPERATURECONTROLLER_HPP

#include "ModelSubTabController.hpp"

namespace openstudio {

/** Controller for the "Ground Temperatures" sub-tab of the Location tab.
 *  Manages all Site:GroundTemperature:* object types and delegates to the
 *  appropriate sub-tab view (e.g. SiteGroundTemperatureMonthlyView for objects
 *  with 12 monthly values). */
class SiteGroundTemperatureController : public ModelSubTabController
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureController(bool isIP, const model::Model& model);

  virtual ~SiteGroundTemperatureController() = default;

 protected:
  virtual void onAddObject(const openstudio::IddObjectType& iddObjectType) override;
  virtual void onCopyObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onRemoveObject(openstudio::model::ModelObject modelObject) override;
  virtual void onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) override;
  virtual void onPurgeObjects(const openstudio::IddObjectType& iddObjectType) override;
  virtual void onDrop(const OSItemId& itemId) override;
  virtual void onInspectItem(OSItem* item) override;

 private:
  bool m_isIP;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SITEGROUNDTEMPERATURECONTROLLER_HPP
