/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectItem.hpp"

#include "OSItem.hpp"

#include "../shared_gui_components/MeasureBadge.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/ComponentData_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <QLabel>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

OSItemId modelObjectToItemId(const openstudio::model::ModelObject& modelObject, bool isDefaulted) {
  std::stringstream ss;
  ss << modelObject;
  return OSItemId(toQString(modelObject.handle()), modelToSourceId(modelObject.model()), isDefaulted, toQString(ss.str()));
}

QString modelToSourceId(const openstudio::model::Model& model) {
  std::stringstream ss;
  ss << model.getImpl<model::detail::Model_Impl>().get();
  std::string result = ss.str();
  return toQString(result);
}

ModelObjectItem::ModelObjectItem(const openstudio::model::ModelObject& modelObject, bool isDefaulted, OSItemType type, QWidget* parent)
  : OSItem(modelObjectToItemId(modelObject, isDefaulted), type, parent), m_handle(modelObject.handle()), m_modelObject(modelObject) {
  this->setText(QString::fromStdString(m_modelObject.name().get()));

  m_modelObject.getImpl<model::detail::ModelObject_Impl>()->onChange.connect<ModelObjectItem, &ModelObjectItem::onObjectChanged>(this);

  if (!modelObject.getModelObjectSources<model::ComponentData>().empty()) {
    m_measureBadge->setMeasureBadgeType(MeasureBadgeType::BCLMeasure);
  }
}

openstudio::Handle ModelObjectItem::handle() const {
  return m_handle;
}

openstudio::model::ModelObject ModelObjectItem::modelObject() const {
  return m_modelObject;
}

bool ModelObjectItem::equal(const openstudio::OSItem* otherItem) const {
  if (m_modelObject.handle().isNull()) {
    return false;
  }

  const auto* modelObjectItem = qobject_cast<const ModelObjectItem*>(otherItem);
  if (modelObjectItem) {
    model::ModelObject otherModelObject = modelObjectItem->modelObject();
    if (otherModelObject.handle().isNull()) {
      return false;
    }
    return (m_modelObject.handle() == otherModelObject.handle());
  }
  return false;
}

void ModelObjectItem::onObjectChanged() {
  this->setText(QString::fromStdString(m_modelObject.name().get()));
}

}  // namespace openstudio
