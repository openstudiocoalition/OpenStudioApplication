/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "UtilityBillAllFuelTypesListView.hpp"

#include "ModelObjectTypeItem.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectListView.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "OSItem.hpp"
#include "UtilityBillFuelTypeItem.hpp"
#include "UtilityBillFuelTypeListView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/UtilityBill.hpp>
#include <openstudio/model/UtilityBill_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <iostream>

namespace openstudio {

UtilityBillAllFuelTypesListView::UtilityBillAllFuelTypesListView(const model::Model& model, bool addScrollArea, OSItemType headerType,
                                                                 QWidget* parent)
  : OSCollapsibleItemList(addScrollArea, parent), m_model(model), m_headerType(headerType) {}

UtilityBillAllFuelTypesListView::UtilityBillAllFuelTypesListView(const std::vector<std::pair<FuelType, std::string>>& utilityBillFuelTypesAndNames,
                                                                 const model::Model& model, bool addScrollArea, OSItemType headerType,
                                                                 QWidget* parent)
  : OSCollapsibleItemList(addScrollArea, parent),
    m_utilityBillFuelTypesAndNames(utilityBillFuelTypesAndNames),
    m_model(model),
    m_headerType(headerType) {
  for (auto utilityBillFuelTypesAndName = m_utilityBillFuelTypesAndNames.rbegin();
       utilityBillFuelTypesAndName != m_utilityBillFuelTypesAndNames.rend(); ++utilityBillFuelTypesAndName) {
    addUtilityBillFuelType(utilityBillFuelTypesAndName->first, utilityBillFuelTypesAndName->second);
  }
}

void UtilityBillAllFuelTypesListView::addModelObjectType(const IddObjectType& iddObjectType, const std::string& name) {
  auto* collapsibleItemHeader = new OSCollapsibleItemHeader(name, OSItemId("", "", false), m_headerType);
  auto* modelObjectListView = new ModelObjectListView(iddObjectType, m_model, false, false);
  auto* modelObjectTypeItem = new ModelObjectTypeItem(collapsibleItemHeader, modelObjectListView);

  addCollapsibleItem(modelObjectTypeItem);
}

void UtilityBillAllFuelTypesListView::addUtilityBillFuelType(const FuelType& fuelType, const std::string& name) {
  auto* collapsibleItemHeader = new OSCollapsibleItemHeader(name, OSItemId("", "", false), m_headerType);
  auto* utilityBillFuelTypeListView = new UtilityBillFuelTypeListView(m_model, fuelType, false);
  auto* utilityBillFuelTypeItem = new UtilityBillFuelTypeItem(collapsibleItemHeader, utilityBillFuelTypeListView);

  addCollapsibleItem(utilityBillFuelTypeItem);
}

FuelType UtilityBillAllFuelTypesListView::currentFuelType() const {
  OSCollapsibleItem* selectedCollapsibleItem = this->selectedCollapsibleItem();
  auto* utilityBillFuelTypeItem = qobject_cast<UtilityBillFuelTypeItem*>(selectedCollapsibleItem);
  OS_ASSERT(utilityBillFuelTypeItem);
  return utilityBillFuelTypeItem->fuelType();
}

boost::optional<openstudio::model::ModelObject> UtilityBillAllFuelTypesListView::selectedModelObject() const {
  OSItem* selectedItem = this->selectedItem();
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(selectedItem);
  if (modelObjectItem) {
    return modelObjectItem->modelObject();
  }
  return boost::none;
}

boost::optional<openstudio::FuelType> UtilityBillAllFuelTypesListView::selectedFuelType() const {
  boost::optional<openstudio::model::ModelObject> modelObject = selectedModelObject();
  if (modelObject) {
    if (boost::optional<model::UtilityBill> utilityBill = modelObject.get().optionalCast<model::UtilityBill>()) {
      return utilityBill.get().fuelType();
    }
    // Opps, its not a UtilityBill!
    OS_ASSERT(false);
  }
  return boost::none;
}

}  // namespace openstudio
