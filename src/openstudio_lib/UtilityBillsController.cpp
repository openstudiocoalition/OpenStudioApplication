/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "UtilityBillsController.hpp"

#include "UtilityBillAllFuelTypesListView.hpp"
#include "UtilityBillsView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/UtilityBill.hpp>
#include <openstudio/model/UtilityBill_Impl.hpp>

#include "OSInspectorView.hpp"
#include "OSItemSelectorButtons.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/data/DataEnums.hpp>

namespace openstudio {

UtilityBillsController::UtilityBillsController(const model::Model& model) : ModelSubTabController(new UtilityBillsView(model), model) {
  subTabView()->itemSelectorButtons()->disableCopyButton();
  subTabView()->itemSelectorButtons()->disablePurgeButton();
  subTabView()->itemSelectorButtons()->hideDropZone();

  auto* utilityBillsInspectorView = static_cast<UtilityBillsInspectorView*>(subTabView()->inspectorView());

  connect(this, &UtilityBillsController::toggleUnitsClicked, utilityBillsInspectorView, &UtilityBillsInspectorView::toggleUnitsClicked);

  connect(utilityBillsInspectorView, &UtilityBillsInspectorView::enableAddNewObjectButton, this, &UtilityBillsController::enableAddNewObjectButton);
}

void UtilityBillsController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  OSItemSelector* itemSelector = this->subTabView()->itemSelector();
  auto* utilityBillAllFuelTypesListView = qobject_cast<UtilityBillAllFuelTypesListView*>(itemSelector);
  OS_ASSERT(utilityBillAllFuelTypesListView);
  FuelType fuelType = utilityBillAllFuelTypesListView->currentFuelType();
  model::Model model = this->model();
  openstudio::model::UtilityBill(fuelType, model);
}

void UtilityBillsController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  // not desired
}

void UtilityBillsController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void UtilityBillsController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void UtilityBillsController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  // DLM: we don't want the purge button enabled
}

void UtilityBillsController::onDrop(const OSItemId& itemId) {
  // DLM: we don't want the drop zone enabled
}

void UtilityBillsController::onInspectItem(OSItem* item) {
  subTabView()->inspectorView()->selectItem(item);
}

void UtilityBillsController::onSelectItem(OSItem* item) {
  m_subTabView->inspectorView()->selectItem(item);
  m_subTabView->itemSelectorButtons()->disableAddButton();
  m_subTabView->itemSelectorButtons()->enableRemoveButton();
}

void UtilityBillsController::onClearSelection() {
  m_subTabView->inspectorView()->clearSelection();
  m_subTabView->itemSelectorButtons()->disableRemoveButton();

  openstudio::OSInspectorView* inspectorView = subTabView()->inspectorView();
  auto* utilityBillsInspectorView = qobject_cast<UtilityBillsInspectorView*>(inspectorView);
  OS_ASSERT(utilityBillsInspectorView);

  enableAddNewObjectButton(utilityBillsInspectorView->runPeriodDates().has_value());
}

///// SLOTS

void UtilityBillsController::enableAddNewObjectButton(bool enable) {
  if (enable) {
    m_subTabView->itemSelectorButtons()->enableAddButton();
  } else {
    m_subTabView->itemSelectorButtons()->disableAddButton();
  };
}

}  // namespace openstudio
