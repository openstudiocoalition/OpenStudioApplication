/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP
#define OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class FacilityExteriorEquipmentGridController;

class FacilityExteriorEquipmentGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  FacilityExteriorEquipmentGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = 0);

  virtual ~FacilityExteriorEquipmentGridView() {}

 private:
  REGISTER_LOGGER("openstudio.FacilityExteriorEquipmentGridView");

  virtual void addObject(const openstudio::IddObjectType& iddObjectType) override;

  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  void clearSelection();

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;
};

class FacilityExteriorEquipmentGridController : public OSGridController
{

  Q_OBJECT

 public:
  FacilityExteriorEquipmentGridController(bool isIP, bool displayAdditionalProps, const QString& settingsText, IddObjectType iddObjectType,
                                          const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~FacilityExteriorEquipmentGridController() = default;

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onCategorySelected(int index) override;

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYEXTERIOREQUIPMENTGRIDVIEW_HPP
