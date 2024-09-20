/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoopScene.hpp"
#include "OSAppBase.hpp"
#include "GridItem.hpp"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QGraphicsItem>
#include <QMimeData>
#include <cmath>
#include <openstudio/model/Loop.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Mixer.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/StraightComponent.hpp>
#include <openstudio/model/StraightComponent_Impl.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Node_Impl.hpp>
#include <QTimer>

using namespace openstudio::model;

namespace openstudio {

LoopScene::LoopScene(model::Loop loop, QObject* parent) : GridScene(parent), m_loop(loop), m_dirty(true) {
  // loop.model().getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<LoopScene, &LoopScene::addedWorkspaceObject>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &LoopScene::addedWorkspaceObject, Qt::QueuedConnection);

  loop.model().getImpl<model::detail::Model_Impl>()->removeWorkspaceObjectPtr.connect<LoopScene, &LoopScene::removedWorkspaceObject>(this);

  layout();
}

void LoopScene::initDefault() {}

void LoopScene::layout() {
  if (m_dirty && !m_loop.handle().isNull()) {
    QList<QGraphicsItem*> itemList = items();
    for (QList<QGraphicsItem*>::iterator it = itemList.begin(); it < itemList.end(); ++it) {
      removeItem(*it);
      delete *it;
    }

    auto* systemItem = new SystemItem(m_loop, this);

    systemItem->setPos(50, 50);

    this->setSceneRect(0, 0, (systemItem->getHGridLength() * 100) + 100, ((systemItem->getVGridLength()) * 100) + 100);

    update();

    m_dirty = false;
  }
}

DemandSideItem* LoopScene::createDemandSide() {
  auto demandInletNodes = m_loop.demandInletNodes();
  auto demandOutletNode = m_loop.demandOutletNode();

  auto* demandSideItem = new DemandSideItem(nullptr, demandInletNodes, demandOutletNode);

  return demandSideItem;
}

SupplySideItem* LoopScene::createSupplySide() {
  auto supplyInletNode = m_loop.supplyInletNode();
  auto supplyOutletNodes = m_loop.supplyOutletNodes();

  auto* supplySideItem = new SupplySideItem(nullptr, supplyInletNode, supplyOutletNodes);

  return supplySideItem;
}

model::Loop LoopScene::loop() {
  return m_loop;
}

void LoopScene::addedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                     const openstudio::UUID& uuid) {
  auto* hvac_impl = dynamic_cast<model::detail::HVACComponent_Impl*>(wPtr.get());
  if (hvac_impl) {
    m_dirty = true;

    QTimer::singleShot(0, this, &LoopScene::layout);
  }
}

void LoopScene::removedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                       const openstudio::UUID& uuid) {
  m_dirty = true;

  QTimer::singleShot(0, this, &LoopScene::layout);
}

}  // namespace openstudio
