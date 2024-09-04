/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "UtilityBillFuelTypeItem.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "UtilityBillFuelTypeListView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/data/DataEnums.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>

#include <iostream>

namespace openstudio {

UtilityBillFuelTypeItem::UtilityBillFuelTypeItem(OSCollapsibleItemHeader* collapsibleItemHeader,
                                                 UtilityBillFuelTypeListView* utilityBillFuelTypeListView, QWidget* parent)
  : OSCollapsibleItem(collapsibleItemHeader, utilityBillFuelTypeListView, parent) {}

IddObjectType UtilityBillFuelTypeItem::iddObjectType() const {
  OSItemList* itemList = this->itemList();
  auto* utilityBillFuelTypeListView = qobject_cast<UtilityBillFuelTypeListView*>(itemList);
  OS_ASSERT(utilityBillFuelTypeListView);
  OS_ASSERT(utilityBillFuelTypeListView->iddObjectType() == openstudio::IddObjectType::OS_UtilityBill);
  return utilityBillFuelTypeListView->iddObjectType();
}

FuelType UtilityBillFuelTypeItem::fuelType() const {
  OSItemList* itemList = this->itemList();
  auto* utilityBillFuelTypeListView = qobject_cast<UtilityBillFuelTypeListView*>(itemList);
  OS_ASSERT(utilityBillFuelTypeListView);
  FuelType fuelType = utilityBillFuelTypeListView->fuelType();
  return fuelType;
}

}  // namespace openstudio
