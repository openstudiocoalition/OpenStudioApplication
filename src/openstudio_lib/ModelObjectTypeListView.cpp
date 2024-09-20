/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectTypeListView.hpp"
#include "ModelObjectTypeItem.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectListView.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "OSCategoryPlaceholder.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <iostream>

namespace openstudio {

ModelObjectTypeListView::ModelObjectTypeListView(const model::Model& model, bool addScrollArea, OSItemType headerType, bool isLibrary,
                                                 QWidget* parent)
  : OSCollapsibleItemList(addScrollArea, parent), m_model(model), m_headerType(headerType), m_isLibrary(isLibrary) {}

ModelObjectTypeListView::ModelObjectTypeListView(const std::vector<std::pair<IddObjectType, std::string>>& modelObjectTypesAndNames,
                                                 const model::Model& model, bool addScrollArea, OSItemType headerType, bool isLibrary,
                                                 QWidget* parent)
  : OSCollapsibleItemList(addScrollArea, parent),
    m_modelObjectTypesAndNames(modelObjectTypesAndNames),
    m_model(model),
    m_headerType(headerType),
    m_isLibrary(isLibrary) {
  for (auto modelObjectTypeAndName = m_modelObjectTypesAndNames.rbegin(); modelObjectTypeAndName != m_modelObjectTypesAndNames.rend();
       ++modelObjectTypeAndName) {
    addModelObjectType(modelObjectTypeAndName->first, modelObjectTypeAndName->second);
  }
}

void ModelObjectTypeListView::addModelObjectType(const IddObjectType& iddObjectType, const std::string& name) {
  auto* collapsibleItemHeader = new OSCollapsibleItemHeader(name, OSItemId("", "", false), m_headerType);
  auto* modelObjectListView = new ModelObjectListView(iddObjectType, m_model, false, m_isLibrary);
  auto* modelObjectTypeItem = new ModelObjectTypeItem(collapsibleItemHeader, modelObjectListView);

  addCollapsibleItem(modelObjectTypeItem);
}

void ModelObjectTypeListView::addModelObjectCategoryPlaceholder(const std::string& name) {

  auto* categoryPlaceholderItem = new OSCategoryPlaceholder(name);
  addCategoryPlaceholderItem(categoryPlaceholderItem);
}

IddObjectType ModelObjectTypeListView::currentIddObjectType() const {
  OSCollapsibleItem* selectedCollapsibleItem = this->selectedCollapsibleItem();
  auto* modelObjectTypeItem = qobject_cast<ModelObjectTypeItem*>(selectedCollapsibleItem);
  OS_ASSERT(modelObjectTypeItem);
  return modelObjectTypeItem->iddObjectType();
}

boost::optional<openstudio::model::ModelObject> ModelObjectTypeListView::selectedModelObject() const {
  OSItem* selectedItem = this->selectedItem();
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(selectedItem);
  if (modelObjectItem) {
    return modelObjectItem->modelObject();
  }
  return boost::none;
}

}  // namespace openstudio
