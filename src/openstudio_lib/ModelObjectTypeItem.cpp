/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectTypeItem.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "ModelObjectListView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <iostream>

namespace openstudio {

ModelObjectTypeItem::ModelObjectTypeItem(OSCollapsibleItemHeader* collapsibleItemHeader, ModelObjectListView* modelObjectListView, QWidget* parent)
  : OSCollapsibleItem(collapsibleItemHeader, modelObjectListView, parent) {}

IddObjectType ModelObjectTypeItem::iddObjectType() const {
  OSItemList* itemList = this->itemList();
  auto* modelObjectListView = qobject_cast<ModelObjectListView*>(itemList);
  OS_ASSERT(modelObjectListView);
  return modelObjectListView->iddObjectType();
}

}  // namespace openstudio
