/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelSubTabController.hpp"
#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectListView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "OSItemSelector.hpp"
#include "OSItemSelectorButtons.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/Component.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

ModelSubTabController::ModelSubTabController(ModelSubTabView* subTabView, const model::Model& model) : SubTabController(subTabView), m_model(model) {
  connect(subTabView, &ModelSubTabView::modelObjectSelected, this, &ModelSubTabController::modelObjectSelected);

  connect(subTabView, &ModelSubTabView::dropZoneItemClicked, this, &ModelSubTabController::dropZoneItemClicked);

  connect(subTabView, &ModelSubTabView::dropZoneItemSelected, this, &ModelSubTabController::dropZoneItemSelected);
}

openstudio::model::Model ModelSubTabController::model() const {
  return m_model;
}

bool ModelSubTabController::fromModel(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->fromModel(itemId);
}

bool ModelSubTabController::fromComponentLibrary(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->fromComponentLibrary(itemId);
}

boost::optional<model::ModelObject> ModelSubTabController::getModelObject(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->getModelObject(itemId);
}

boost::optional<model::Component> ModelSubTabController::getComponent(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->getComponent(itemId);
}

void ModelSubTabController::onRemoveItem(OSItem* item) {
  // get selected items
  auto* modelSubTabView = qobject_cast<ModelSubTabView*>(subTabView());
  auto* modelObjectInspectorView = modelSubTabView->modelObjectInspectorView();

  if (modelObjectInspectorView->supportsMultipleObjectSelection()) {
    for (const auto& obj : modelObjectInspectorView->selectedObjects()) {
      onRemoveObject(obj);
    }
  } else {
    auto* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
    OS_ASSERT(modelObjectItem);
    model::ModelObject modelObject = modelObjectItem->modelObject();
    if (!modelObject.handle().isNull()) {
      onRemoveObject(modelObject);
    }
  }
}

void ModelSubTabController::onReplaceItem(OSItem* item, const OSItemId& replacementItemId) {
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
  OS_ASSERT(modelObjectItem);
  model::ModelObject modelObject = modelObjectItem->modelObject();
  if (!modelObject.handle().isNull()) {
    onReplaceObject(modelObject, replacementItemId);
  }
}

void ModelSubTabController::onAddItem() {
  // get selected items
  auto* modelSubTabView = qobject_cast<ModelSubTabView*>(subTabView());
  auto* modelObjectInspectorView = modelSubTabView->modelObjectInspectorView();

  openstudio::IddObjectType currIddObjectType = this->currentIddObjectType();
  if (modelObjectInspectorView->supportsMultipleObjectSelection()) {
    // Always make at least one
    if (modelObjectInspectorView->selectedObjects().empty()) {
      onAddObject(currIddObjectType);
    } else {
      for (const auto& obj : modelObjectInspectorView->selectedObjects()) {
        onAddObject(obj);
      }
    }
  } else {
    onAddObject(currIddObjectType);
  }
}

void ModelSubTabController::onCopyItem() {
  // get selected items
  auto* modelSubTabView = qobject_cast<ModelSubTabView*>(subTabView());
  auto* modelObjectInspectorView = modelSubTabView->modelObjectInspectorView();

  if (modelObjectInspectorView->supportsMultipleObjectSelection()) {
    for (const auto& obj : modelObjectInspectorView->selectedObjects()) {
      if (!obj.handle().isNull()) {
        onCopyObject(obj);
      }
    }
  } else {
    boost::optional<openstudio::model::ModelObject> selectedModelObject = this->selectedModelObject();
    if (selectedModelObject && !selectedModelObject->handle().isNull()) {
      onCopyObject(*selectedModelObject);
    }
  }
}

void ModelSubTabController::onPurgeItems() {
  onPurgeObjects(this->currentIddObjectType());
}

boost::optional<openstudio::model::ModelObject> ModelSubTabController::selectedModelObject() const {
  boost::optional<openstudio::model::ModelObject> result;

  const OSItemSelector* itemSelector = subTabView()->itemSelector();

  const auto* modelObjectListView = qobject_cast<const ModelObjectListView*>(itemSelector);
  if (modelObjectListView) {
    result = modelObjectListView->selectedModelObject();
  }

  const auto* modelObjectTypeListView = qobject_cast<const ModelObjectTypeListView*>(itemSelector);
  if (modelObjectTypeListView) {
    result = modelObjectTypeListView->selectedModelObject();
  }

  return result;
}

openstudio::IddObjectType ModelSubTabController::currentIddObjectType() const {
  openstudio::IddObjectType result;

  const OSItemSelector* itemSelector = subTabView()->itemSelector();

  const auto* modelObjectListView = qobject_cast<const ModelObjectListView*>(itemSelector);
  if (modelObjectListView) {
    result = modelObjectListView->iddObjectType();
  }

  const auto* modelObjectTypeListView = qobject_cast<const ModelObjectTypeListView*>(itemSelector);
  if (modelObjectTypeListView) {
    result = modelObjectTypeListView->currentIddObjectType();
  }

  return result;
}

}  // namespace openstudio
