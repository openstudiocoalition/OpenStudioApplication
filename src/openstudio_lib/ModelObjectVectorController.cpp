/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectVectorController.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

void ModelObjectVectorController::attach(const model::ModelObject& modelObject) {
  detach();

  m_modelObject = modelObject;

  attachModel(modelObject.model());

  // m_model->getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<ModelObjectVectorController, &ModelObjectVectorController::objectAdded>(this);
  //connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectVectorController::objectAdded, Qt::QueuedConnection);

  //m_model->getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.connect<ModelObjectVectorController, &ModelObjectVectorController::objectRemoved>(this);
  //connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectVectorController::objectRemoved, Qt::QueuedConnection);

  m_modelObject->getImpl<model::detail::ModelObject_Impl>()
    ->onRelationshipChange.connect<ModelObjectVectorController, &ModelObjectVectorController::changeRelationship>(this);

  m_modelObject->getImpl<model::detail::ModelObject_Impl>()
    ->onDataChange.connect<ModelObjectVectorController, &ModelObjectVectorController::dataChange>(this);

  m_modelObject->getImpl<model::detail::ModelObject_Impl>()->onChange.connect<ModelObjectVectorController, &ModelObjectVectorController::change>(
    this);
}

void ModelObjectVectorController::attachModel(const model::Model& model) {
  if (m_model) {

    // m_model->getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectAdded>(this);
    disconnect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectVectorController::objectAdded);

    //m_model->getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectRemoved>(this);
    disconnect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectVectorController::objectRemoved);

    m_model.reset();
  }

  m_model = model;

  // m_model->getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<ModelObjectVectorController, &ModelObjectVectorController::objectAdded>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectVectorController::objectAdded, Qt::QueuedConnection);

  //m_model->getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.connect<ModelObjectVectorController, &ModelObjectVectorController::objectRemoved>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectVectorController::objectRemoved, Qt::QueuedConnection);
}

void ModelObjectVectorController::attachOtherModelObject(const model::ModelObject& modelObject) {
  // check not already connected
  for (const model::ModelObject& currentModelObject : m_otherModelObjects) {
    if (modelObject.handle() == currentModelObject.handle()) {
      return;
    }
  }

  m_otherModelObjects.push_back(modelObject);

  modelObject.getImpl<model::detail::ModelObject_Impl>()
    ->onRelationshipChange.connect<ModelObjectVectorController, &ModelObjectVectorController::changeRelationship>(this);
}

void ModelObjectVectorController::detach() {
  if (m_modelObject) {
    m_modelObject->getImpl<model::detail::ModelObject_Impl>()
      ->onRelationshipChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::changeRelationship>(this);

    m_modelObject->getImpl<model::detail::ModelObject_Impl>()
      ->onDataChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::dataChange>(this);

    m_modelObject->getImpl<model::detail::ModelObject_Impl>()->onChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::change>(
      this);

    m_modelObject.reset();
  }

  if (m_model) {
    // m_model->getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectAdded>(this);
    disconnect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectVectorController::objectAdded);

    //m_model->getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectRemoved>(this);
    disconnect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectVectorController::objectRemoved);

    m_model.reset();
  }

  detachOtherModelObjects();
}

void ModelObjectVectorController::detachOtherModelObject(const model::ModelObject& modelObject) {
  auto it = m_otherModelObjects.begin();
  auto itend = m_otherModelObjects.end();
  std::vector<model::ModelObject> newVector;
  for (; it != itend; ++it) {
    if (it->handle() == modelObject.handle()) {
      m_modelObject->getImpl<model::detail::ModelObject_Impl>()
        ->onRelationshipChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::changeRelationship>(this);

      //m_modelObject->getImpl<model::detail::ModelObject_Impl>().get()->onDataChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::dataChange>(this);

      //m_modelObject->getImpl<model::detail::ModelObject_Impl>().get()->onChange.disconnect<ModelObjectVectorController, &ModelObjectVectorController::change>(this);

    } else {
      newVector.push_back(*it);
    }
  }
  m_otherModelObjects.swap(newVector);
}

void ModelObjectVectorController::detachOtherModelObjects() {
  for (size_t i = 0; i < m_otherModelObjects.size(); i++) {
    // m_model->getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectAdded>(this);
    disconnect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectVectorController::objectAdded);

    // m_model->getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.disconnect<ModelObjectVectorController, &ModelObjectVectorController::objectRemoved>(this);
    //connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectVectorController::objectRemoved, Qt::QueuedConnection);
  }
  m_otherModelObjects.clear();
}

void ModelObjectVectorController::changeRelationship(int index, Handle newHandle, Handle oldHandle) {
  if (m_modelObject) {
    onChangeRelationship(m_modelObject.get(), index, newHandle, oldHandle);
  }
}

void ModelObjectVectorController::dataChange() {
  if (m_modelObject) {
    onDataChange(m_modelObject.get());
  }
}

void ModelObjectVectorController::change() {
  if (m_modelObject) {
    onChange(m_modelObject.get());
  }
}

void ModelObjectVectorController::objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                              const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  onObjectAdded(impl->getObject<model::ModelObject>(), iddObjectType, handle);
}

void ModelObjectVectorController::objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                                const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  onObjectRemoved(impl->getObject<model::ModelObject>(), iddObjectType, handle);
}

void ModelObjectVectorController::onChangeRelationship(const openstudio::model::ModelObject& modelObject, int index, Handle newHandle,
                                                       Handle oldHandle) {}

void ModelObjectVectorController::onDataChange(const openstudio::model::ModelObject& modelObject) {}

void ModelObjectVectorController::onChange(const openstudio::model::ModelObject& modelObject) {}

void ModelObjectVectorController::onObjectAdded(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType,
                                                const openstudio::UUID& handle) {}

void ModelObjectVectorController::onObjectRemoved(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType,
                                                  const openstudio::UUID& handle) {
  detachOtherModelObject(modelObject);
}

bool ModelObjectVectorController::fromModel(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->fromModel(itemId);
}

bool ModelObjectVectorController::fromComponentLibrary(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->fromComponentLibrary(itemId);
}

boost::optional<model::ModelObject> ModelObjectVectorController::getModelObject(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->getModelObject(itemId);
}

boost::optional<model::Component> ModelObjectVectorController::getComponent(const OSItemId& itemId) const {
  return OSAppBase::instance()->currentDocument()->getComponent(itemId);
}

}  // namespace openstudio
