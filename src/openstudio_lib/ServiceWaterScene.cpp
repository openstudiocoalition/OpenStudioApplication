/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ServiceWaterScene.hpp"
#include "GridItem.hpp"
#include "ServiceWaterGridItems.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "MainRightColumnController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/WaterUseConnections.hpp>
#include <openstudio/model/WaterUseConnections_Impl.hpp>
#include <openstudio/model/WaterUseEquipment.hpp>
#include <openstudio/model/WaterUseEquipment_Impl.hpp>

#include <QTimer>

namespace openstudio {

ServiceWaterScene::ServiceWaterScene(const model::Model& model) : GridScene(), m_dirty(true), m_model(model) {
  //m_model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<ServiceWaterScene, &ServiceWaterScene::onAddedWorkspaceObject>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ServiceWaterScene::onAddedWorkspaceObject, Qt::QueuedConnection);

  m_model.getImpl<model::detail::Model_Impl>()->removeWorkspaceObjectPtr.connect<ServiceWaterScene, &ServiceWaterScene::onRemovedWorkspaceObject>(
    this);

  layout();
}

void ServiceWaterScene::layout() {
  QList<QGraphicsItem*> itemList = items();
  for (QList<QGraphicsItem*>::iterator it = itemList.begin(); it < itemList.end(); ++it) {
    removeItem(*it);
    delete *it;
  }

  auto* backgroundItem = new ServiceWaterItem(this);

  Q_UNUSED(backgroundItem);
}

model::Model ServiceWaterScene::model() const {
  return m_model;
}

void ServiceWaterScene::onAddedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                               const openstudio::UUID& uuid) {
  auto* hvac_impl = dynamic_cast<model::detail::WaterUseConnections_Impl*>(wPtr.get());
  if (hvac_impl) {
    m_dirty = true;

    QTimer::singleShot(0, this, &ServiceWaterScene::layout);
  }
}

void ServiceWaterScene::onRemovedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr,
                                                 const openstudio::IddObjectType& type, const openstudio::UUID& uuid) {
  auto* hvac_impl = dynamic_cast<model::detail::WaterUseConnections_Impl*>(wPtr.get());
  if (hvac_impl) {
    m_dirty = true;

    QTimer::singleShot(0, this, &ServiceWaterScene::layout);
  }
}

WaterUseConnectionsDetailScene::WaterUseConnectionsDetailScene(const model::WaterUseConnections& waterUseConnections)
  : GridScene(), m_dirty(true), m_waterUseConnections(waterUseConnections) {
  model::Model model = m_waterUseConnections.model();

  //model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<WaterUseConnectionsDetailScene, &WaterUseConnectionsDetailScene::onAddedWorkspaceObject>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &WaterUseConnectionsDetailScene::onAddedWorkspaceObject,
          Qt::QueuedConnection);

  model.getImpl<model::detail::Model_Impl>()
    ->removeWorkspaceObjectPtr.connect<WaterUseConnectionsDetailScene, &WaterUseConnectionsDetailScene::onRemovedWorkspaceObject>(this);

  layout();

  // Display the object in MainRightColumnController for editing (name in particular)
  model::OptionalModelObject mo = m_waterUseConnections;
  OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(mo, false);
}

model::WaterUseConnections WaterUseConnectionsDetailScene::waterUseConnections() const {
  return m_waterUseConnections;
}

void WaterUseConnectionsDetailScene::layout() {
  QList<QGraphicsItem*> itemList = items();
  for (QList<QGraphicsItem*>::iterator it = itemList.begin(); it < itemList.end(); ++it) {
    removeItem(*it);
    delete *it;
  }

  auto* backgroundItem = new WaterUseConnectionsDetailItem(this);

  Q_UNUSED(backgroundItem);
}

void WaterUseConnectionsDetailScene::onAddedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr,
                                                            const openstudio::IddObjectType& type, const openstudio::UUID& uuid) {
  auto* hvac_impl = dynamic_cast<model::detail::WaterUseEquipment_Impl*>(wPtr.get());
  if (hvac_impl) {
    m_dirty = true;

    QTimer::singleShot(0, this, &WaterUseConnectionsDetailScene::layout);
  }
}

void WaterUseConnectionsDetailScene::onRemovedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr,
                                                              const openstudio::IddObjectType& type, const openstudio::UUID& uuid) {
  auto* hvac_impl = dynamic_cast<model::detail::WaterUseEquipment_Impl*>(wPtr.get());
  if (hvac_impl) {
    m_dirty = true;

    QTimer::singleShot(0, this, &WaterUseConnectionsDetailScene::layout);
  }
}

}  // namespace openstudio
