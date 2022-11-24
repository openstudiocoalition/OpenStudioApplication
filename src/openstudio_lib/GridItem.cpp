/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "GridItem.hpp"
#include "ServiceWaterGridItems.hpp"
#include "IconLibrary.hpp"
#include "LoopScene.hpp"
#include "SchedulesView.hpp"
#include "OSDocument.hpp"
#include "OSAppBase.hpp"
#include "MainRightColumnController.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>

#include <QPainter>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>

#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/ZoneHVACComponent.hpp>
#include <openstudio/model/ZoneHVACComponent_Impl.hpp>
#include <openstudio/model/WaterUseConnections.hpp>
#include <openstudio/model/WaterUseConnections_Impl.hpp>
#include <openstudio/model/WaterToAirComponent.hpp>
#include <openstudio/model/WaterToAirComponent_Impl.hpp>
#include <openstudio/model/WaterToWaterComponent.hpp>
#include <openstudio/model/WaterToWaterComponent_Impl.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem_Impl.hpp>
#include <openstudio/model/AirLoopHVAC.hpp>
#include <openstudio/model/AirLoopHVAC_Impl.hpp>
#include <openstudio/model/AirLoopHVACSupplyPlenum.hpp>
#include <openstudio/model/AirLoopHVACSupplyPlenum_Impl.hpp>
#include <openstudio/model/AirLoopHVACReturnPlenum.hpp>
#include <openstudio/model/AirLoopHVACReturnPlenum_Impl.hpp>
#include <openstudio/model/AirLoopHVACZoneMixer.hpp>
#include <openstudio/model/AirLoopHVACZoneMixer_Impl.hpp>
#include <openstudio/model/AirLoopHVACZoneSplitter.hpp>
#include <openstudio/model/AirLoopHVACZoneSplitter_Impl.hpp>
#include <openstudio/model/AirToAirComponent.hpp>
#include <openstudio/model/AirToAirComponent_Impl.hpp>
#include <openstudio/model/PlantLoop.hpp>
#include <openstudio/model/PlantLoop_Impl.hpp>
#include <openstudio/model/SetpointManager.hpp>
#include <openstudio/model/SetpointManagerColdest.hpp>
#include <openstudio/model/SetpointManagerFollowGroundTemperature.hpp>
#include <openstudio/model/SetpointManagerFollowOutdoorAirTemperature.hpp>
#include <openstudio/model/SetpointManagerFollowSystemNodeTemperature.hpp>
#include <openstudio/model/SetpointManagerMixedAir.hpp>
#include <openstudio/model/SetpointManagerMultiZoneCoolingAverage.hpp>
#include <openstudio/model/SetpointManagerMultiZoneHeatingAverage.hpp>
#include <openstudio/model/SetpointManagerMultiZoneHumidityMaximum.hpp>
#include <openstudio/model/SetpointManagerMultiZoneHumidityMinimum.hpp>
#include <openstudio/model/SetpointManagerMultiZoneMaximumHumidityAverage.hpp>
#include <openstudio/model/SetpointManagerMultiZoneMinimumHumidityAverage.hpp>
#include <openstudio/model/SetpointManagerOutdoorAirPretreat.hpp>
#include <openstudio/model/SetpointManagerOutdoorAirReset.hpp>
#include <openstudio/model/SetpointManagerScheduled.hpp>
#include <openstudio/model/SetpointManagerScheduledDualSetpoint.hpp>
#include <openstudio/model/SetpointManagerSingleZoneHumidityMaximum.hpp>
#include <openstudio/model/SetpointManagerSingleZoneHumidityMinimum.hpp>
#include <openstudio/model/SetpointManagerSingleZoneOneStageCooling.hpp>
#include <openstudio/model/SetpointManagerSingleZoneOneStageHeating.hpp>
#include <openstudio/model/SetpointManagerSingleZoneReheat.hpp>
#include <openstudio/model/SetpointManagerSingleZoneCooling.hpp>
#include <openstudio/model/SetpointManagerSingleZoneHeating.hpp>
#include <openstudio/model/SetpointManagerWarmest.hpp>
#include <openstudio/model/SetpointManagerWarmestTemperatureFlow.hpp>
#include <openstudio/model/SetpointManagerSystemNodeResetTemperature.hpp>
#include <openstudio/model/SetpointManagerSystemNodeResetHumidity.hpp>
#include <openstudio/model/RenderingColor.hpp>
#include <openstudio/model/RenderingColor_Impl.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Node_Impl.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Splitter_Impl.hpp>
#include <openstudio/model/Mixer.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/ZoneHVACTerminalUnitVariableRefrigerantFlow.hpp>
#include <openstudio/model/ZoneHVACTerminalUnitVariableRefrigerantFlow_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <algorithm>

#include <openstudio/utilities/idd/IddEnums.hxx>
#include <utility>

using namespace openstudio::model;

namespace openstudio {

bool hasSPM(model::Node& node) {
  /* // Previously was only allowing temperature
   *auto spms = node.setpointManagers();
   *for( auto & spm: spms ) {
   *  if ( spm.controlVariable().find( "Temperature" ) != std::string::npos ) {
   *    return true;
   *  }
   *}
   */
  auto spms = node.setpointManagers();
  return !(spms.empty());
}

// Begin move these

ModelObjectGraphicsItem::ModelObjectGraphicsItem(QGraphicsItem* parent)
  : QGraphicsObject(parent), m_deleteAble(false), m_highlight(false), m_removeButtonItem(nullptr), m_enableHighlight(true) {
  setAcceptHoverEvents(true);
  setAcceptDrops(false);
  setFlag(QGraphicsItem::ItemIsFocusable);
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  if (QGraphicsScene* _scene = scene()) {
    auto* gridScene = static_cast<GridScene*>(_scene);

    connect(this, &ModelObjectGraphicsItem::modelObjectSelected, gridScene, &GridScene::modelObjectSelected);

    connect(this, &ModelObjectGraphicsItem::removeModelObjectClicked, gridScene, &GridScene::removeModelObjectClicked);

    connect(this, static_cast<void (ModelObjectGraphicsItem::*)(OSItemId, model::HVACComponent&)>(&ModelObjectGraphicsItem::hvacComponentDropped),
            gridScene, static_cast<void (GridScene::*)(OSItemId, model::HVACComponent&)>(&GridScene::hvacComponentDropped));

    connect(this, static_cast<void (ModelObjectGraphicsItem::*)(OSItemId)>(&ModelObjectGraphicsItem::hvacComponentDropped), gridScene,
            static_cast<void (GridScene::*)(OSItemId)>(&GridScene::hvacComponentDropped));

    connect(this, &ModelObjectGraphicsItem::innerNodeClicked, gridScene, &GridScene::innerNodeClicked);
  }
}

void ModelObjectGraphicsItem::setEnableHighlight(bool highlight) {
  m_enableHighlight = highlight;
}

void ModelObjectGraphicsItem::setDeletable(bool deletable) {
  if (deletable) {
    m_removeButtonItem = new RemoveButtonItem(this);

    m_removeButtonItem->setPos(boundingRect().width() - 30, boundingRect().height() - 30);

    connect(m_removeButtonItem, &RemoveButtonItem::mouseClicked, this, &ModelObjectGraphicsItem::onRemoveButtonClicked);
  } else {
    if (m_removeButtonItem) {
      m_removeButtonItem->deleteLater();

      m_removeButtonItem = nullptr;
    }
  }
}

void ModelObjectGraphicsItem::onRemoveButtonClicked() {
  if (m_modelObject) {
    emit removeModelObjectClicked(m_modelObject.get());
  }
}

void ModelObjectGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  event->accept();
  m_highlight = true;
  update();
}

void ModelObjectGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  event->accept();
  m_highlight = false;
  update();
}

QVariant ModelObjectGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value) {
  if (change == QGraphicsItem::ItemSelectedHasChanged) {
    if (m_modelObject) {
      if (value.toBool()) {
        emit modelObjectSelected(m_modelObject, false);
      } else {
        boost::optional<model::ModelObject> mo;
        emit modelObjectSelected(mo, false);
      }
    }
  }

  return QGraphicsItem::itemChange(change, value);
}

void ModelObjectGraphicsItem::setModelObject(model::OptionalModelObject modelObject) {
  m_modelObject = std::move(modelObject);

  if (!m_modelObject) {
    return;
  }

  if (auto comp_ = m_modelObject->optionalCast<model::HVACComponent>()) {
    setAcceptDrops(true);
    setDeletable(comp_->isRemovable());
  }

  m_modelObject->getImpl<detail::IdfObject_Impl>()
    ->detail::IdfObject_Impl::onNameChange.connect<ModelObjectGraphicsItem, &ModelObjectGraphicsItem::onNameChange>(this);

  setFlag(QGraphicsItem::ItemIsSelectable);

  this->onNameChange();
}

void ModelObjectGraphicsItem::onNameChange() {
  if (m_modelObject) {
    if (boost::optional<std::string> name = m_modelObject->name()) {
      setToolTip(QString::fromStdString(name.get()));
    }
  }
}

model::OptionalModelObject ModelObjectGraphicsItem::modelObject() {
  return m_modelObject;
}

void ModelObjectGraphicsItem::dragEnterEvent(QGraphicsSceneDragDropEvent* /*event*/) {
  m_highlight = true;
  update();
}

void ModelObjectGraphicsItem::dragLeaveEvent(QGraphicsSceneDragDropEvent* /*event*/) {
  m_highlight = false;
  update();
}

void ModelObjectGraphicsItem::dropEvent(QGraphicsSceneDragDropEvent* event) {
  event->accept();

  if (event->proposedAction() == Qt::CopyAction) {
    if (!m_modelObject) {
      emit hvacComponentDropped(OSItemId{event->mimeData()});
    } else {
      try {
        Node node = m_modelObject->cast<Node>();

        event->setDropAction(Qt::CopyAction);

        emit hvacComponentDropped(OSItemId{event->mimeData()}, node);
      } catch (std::bad_cast&) {
        return;
      }
    }
  }
}

// End move these to

GridItem::GridItem(QGraphicsItem* parent) : ModelObjectGraphicsItem(parent), m_hLength(1), m_vLength(1) {}

QRectF GridItem::boundingRect() const {
  return {0.0, 0.0, m_hLength * 100.0, m_vLength * 100.0};
}

void GridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(0, 0, m_hLength * 100, m_vLength * 100);

  if (isSelected()) {
    painter->setBrush(QBrush(QColor(128, 128, 128), Qt::SolidPattern));
    painter->drawRect(0, 0, m_hLength * 100, m_vLength * 100);
  }

  if (m_highlight && m_enableHighlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(0, 0, m_hLength * 100, m_vLength * 100);
  }
}

void GridItem::setGridPos(int x, int y) {
  setPos(x * 100, y * 100);
}

int GridItem::getXGridPos() const {
  return x() / 100;
}

int GridItem::getYGridPos() const {
  return y() / 100;
}

void GridItem::setHGridLength(int l) {
  m_hLength = l;
}

void GridItem::setVGridLength(int l) {
  m_vLength = l;
}

int GridItem::getHGridLength() const {
  return m_hLength;
}

int GridItem::getVGridLength() const {
  return m_vLength;
}

LinkItem::LinkItem(QGraphicsItem* parent) : QGraphicsObject(parent), m_mouseDown(false), m_isHovering(false) {
  this->setToolTip("Go to attached Loop");

  this->setAcceptHoverEvents(true);
}

void LinkItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_mouseDown = true;

  event->accept();
}

void LinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_mouseDown) {
    if (this->shape().contains(event->pos())) {
      event->accept();

      emit mouseClicked();
    }
  }

  m_mouseDown = false;
}

void LinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  m_isHovering = true;

  this->update();

  event->accept();
}

void LinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  m_isHovering = false;

  this->update();

  event->accept();
}

void LinkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  if (m_isHovering) {
    painter->setBrush(QBrush(QColor(44, 50, 51), Qt::SolidPattern));
  } else {
    painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  }
  setZValue(1);

  painter->drawEllipse(3, 3, 14, 14);
}

QRectF LinkItem::boundingRect() const {
  return {0.0, 0.0, 20.0, 20.0};
}

HalfHeightItem::HalfHeightItem(QGraphicsItem* parent) : ModelObjectGraphicsItem(parent) {}

QRectF HalfHeightItem::boundingRect() const {
  return {0.0, 0.0, 100.0, 50.0};
}

HalfHeightOneThreeStraightItem::HalfHeightOneThreeStraightItem(QGraphicsItem* parent) : HalfHeightItem(parent) {}

void HalfHeightOneThreeStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(0, 25, 100, 25);

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(37, 12, 25, 25, *qPixmap);
  }
}

HalfHeightOneThreeNodeItem::HalfHeightOneThreeNodeItem(QGraphicsItem* parent) : HalfHeightItem(parent) {}

void HalfHeightOneThreeNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(0, 25, 100, 25);
  setZValue(1);
  painter->drawEllipse(43, 17, 15, 15);
}

std::vector<GridItem*> HorizontalBranchItem::itemFactory(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent) {
  std::vector<GridItem*> result;

  for (const auto& modelObject : modelObjects) {
    if (auto comp = modelObject.optionalCast<model::Node>()) {
      GridItem* gridItem = new OneThreeNodeItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::AirLoopHVACOutdoorAirSystem>()) {
      GridItem* gridItem = new OASystemItem(comp.get(), parent);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::ThermalZone>()) {
      GridItem* gridItem = new OneThreeStraightItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::AirLoopHVACSupplyPlenum>()) {
      GridItem* gridItem = new SupplyPlenumItem(comp.get(), parent);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::AirLoopHVACReturnPlenum>()) {
      GridItem* gridItem = new ReturnPlenumItem(comp.get(), parent);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::WaterUseConnections>()) {
      auto* gridItem = new WaterUseConnectionsItem(parent);
      gridItem->setModelObject(modelObject);
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::StraightComponent>()) {
      GridItem* gridItem = new OneThreeStraightItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::WaterToAirComponent>()) {
      GridItem* gridItem = new OneThreeWaterToAirItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::WaterToWaterComponent>()) {
      GridItem* gridItem = new OneThreeWaterToWaterItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::ZoneHVACComponent>()) {
      GridItem* gridItem = new OneThreeStraightItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::Mixer>()) {
      // Expecting dual duct terminal which is a mixer
      GridItem* gridItem = new OneThreeDualDuctMixerItem(parent);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      result.push_back(gridItem);
    }
  }

  return result;
}

HorizontalBranchItem::HorizontalBranchItem(std::pair<std::vector<model::ModelObject>, std::vector<model::ModelObject>> modelObjectsBeforeTerminal,
                                           const std::vector<model::ModelObject>& modelObjectsAfterTerminal, QGraphicsItem* parent)
  : GridItem(parent), m_isDropZone(false), m_text("Drag From Library"), m_hasDualTwoRightSidePipes(false), m_dualDuct(true) {
  std::vector<GridItem*> beforeTerminalItems;

  auto halfItemFactory = [&](const boost::optional<model::ModelObject>& modelObject, QGraphicsItem* parent) {
    HalfHeightItem* halfHeightItem = nullptr;

    if (modelObject) {
      if (modelObject->iddObjectType() == model::Node::iddObjectType()) {
        halfHeightItem = new HalfHeightOneThreeNodeItem(parent);
        halfHeightItem->setModelObject(modelObject);
      } else {
        halfHeightItem = new HalfHeightOneThreeStraightItem(parent);
        halfHeightItem->setModelObject(modelObject);
      }
    } else {
      halfHeightItem = new HalfHeightOneThreeStraightItem(parent);
    }

    return halfHeightItem;
  };

  bool stop = false;
  unsigned i = 0;
  while (!stop) {
    stop = true;

    boost::optional<model::ModelObject> modelObject1;
    boost::optional<model::ModelObject> modelObject2;

    if (i < modelObjectsBeforeTerminal.first.size()) {
      modelObject1 = modelObjectsBeforeTerminal.first[i];
      stop = false;
    }
    if (i < modelObjectsBeforeTerminal.second.size()) {
      modelObject2 = modelObjectsBeforeTerminal.second[i];
      stop = false;
    }

    if (!stop) {
      // New horizontal item
      auto* item = new GridItem(this);
      beforeTerminalItems.push_back(item);
      auto* halfHeightItem1 = halfItemFactory(modelObject1, item);
      halfHeightItem1->setPos(0, 0);
      auto* halfHeightItem2 = halfItemFactory(modelObject2, item);
      halfHeightItem2->setPos(0, 50);
    }

    ++i;
  }

  m_gridItems = itemFactory(modelObjectsAfterTerminal, this);
  m_gridItems.insert(m_gridItems.end(), beforeTerminalItems.begin(), beforeTerminalItems.end());
  layout();
}

HorizontalBranchItem::HorizontalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent, bool dualDuct)
  : GridItem(parent), m_isDropZone(false), m_text("Drag From Library"), m_hasDualTwoRightSidePipes(false), m_dualDuct(dualDuct) {
  m_gridItems = itemFactory(modelObjects, this);
  layout();
}

void HorizontalBranchItem::setModelObject(model::OptionalModelObject modelObject) {
  m_modelObject = modelObject;
}

void HorizontalBranchItem::dropEvent(QGraphicsSceneDragDropEvent* event) {
  event->accept();
  if (event->proposedAction() == Qt::CopyAction) {
    if (!m_modelObject) {
      emit hvacComponentDropped(OSItemId{event->mimeData()});
    } else if (auto hvacComponent = m_modelObject->optionalCast<HVACComponent>()) {
      event->setDropAction(Qt::CopyAction);
      emit hvacComponentDropped(OSItemId{event->mimeData()}, hvacComponent.get());
    }
  }
}

void HorizontalBranchItem::setIsDropZone(bool isDropZone) {
  m_isDropZone = isDropZone;

  setAcceptDrops(m_isDropZone);

  layout();
}

void HorizontalBranchItem::setPadding(unsigned padding) {
  if (m_paddingItems.size() > padding) {
    for (auto it = m_paddingItems.begin() + padding; it < m_paddingItems.end(); ++it) {
      (*it)->scene()->removeItem(*it);
    }
    m_paddingItems.erase(m_paddingItems.begin() + padding, m_paddingItems.end());
  } else if (m_paddingItems.size() < padding) {
    for (unsigned i = m_paddingItems.size(); i < padding; ++i) {
      auto* straightItem = new OneThreeStraightItem(this, m_dualDuct);
      m_paddingItems.push_back(straightItem);
    }
  }

  layout();
}

unsigned HorizontalBranchItem::padding() {
  return m_paddingItems.size();
}

void HorizontalBranchItem::layout() {
  int i = 0;
  int j = 1;

  for (auto& gridItem : m_gridItems) {
    auto height = gridItem->getVGridLength();
    if (height > j) {
      j = height;
    }
  }

  for (auto& gridItem : m_gridItems) {
    if (m_isDropZone) {
      gridItem->hide();
    } else {
      gridItem->setGridPos(i, (j - gridItem->getVGridLength()));
      i = i + gridItem->getHGridLength();
      gridItem->show();
    }
  }

  for (auto it = m_paddingItems.begin(); it < m_paddingItems.end(); ++it) {
    if (m_isDropZone) {
      i = i + (*it)->getHGridLength();
      (*it)->hide();
    } else {
      (*it)->setGridPos(i, j - 1);
      i = i + (*it)->getHGridLength();
      (*it)->show();
    }
  }

  setHGridLength(i);
  setVGridLength(j);
}

void HorizontalBranchItem::setHasTwoRightSidePipes(bool dualRight) {
  m_hasDualTwoRightSidePipes = dualRight;
}

void HorizontalBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  if (m_isDropZone) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
    painter->drawRect(0, 0, m_hLength * 100, m_vLength * 100);

    GridItem::paint(painter, option, widget);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor(128, 128, 128), 2, Qt::DashLine, Qt::RoundCap));
    painter->drawRoundedRect(7, 7, m_hLength * 100 - 14, m_vLength * 100 - 14, 5.0, 5.0);

    painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::FlatCap));
    painter->drawLine(0, m_vLength * 100 / 2.0, 7, m_vLength * 100 / 2.0);
    if (m_hasDualTwoRightSidePipes) {
      painter->drawLine(m_hLength * 100 - 7, m_vLength * 100 / 3.0, m_hLength * 100, m_vLength * 100 / 3.0);
      painter->drawLine(m_hLength * 100 - 7, m_vLength * 100 * 2.0 / 3.0, m_hLength * 100, m_vLength * 100 * 2.0 / 3.0);
    } else {
      painter->drawLine(m_hLength * 100 - 7, m_vLength * 100 / 2.0, m_hLength * 100, m_vLength * 100 / 2.0);
    }

    QFont font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);
    painter->drawText(9, 9, m_hLength * 100 - 18, m_vLength * 100 - 18, Qt::AlignCenter | Qt::TextWordWrap, m_text);
  }
}

void HorizontalBranchItem::setText(const QString& text) {
  m_text = text;

  update();
}

VerticalBranchItem::VerticalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent) : GridItem(parent) {
  for (const auto& modelObject : modelObjects) {
    if (auto comp = modelObject.optionalCast<model::Node>()) {
      GridItem* gridItem = new TwoFourNodeItem(this);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::StraightComponent>()) {
      GridItem* gridItem = new TwoFourStraightItem(this);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
    }
  }

  layout();
}

void VerticalBranchItem::setPadding(unsigned padding) {
  if (m_paddingItems.size() > padding) {
    for (auto it = m_paddingItems.begin() + padding; it < m_paddingItems.end(); ++it) {
      (*it)->scene()->removeItem(*it);
    }
    m_paddingItems.erase(m_paddingItems.begin() + padding, m_paddingItems.end());
  } else if (m_paddingItems.size() < padding) {
    for (unsigned i = m_paddingItems.size(); i < padding; i++) {
      auto* straightItem = new TwoFourStraightItem();
      straightItem->setParentItem(this);
      m_paddingItems.push_back(straightItem);
    }
  }

  layout();
}

void VerticalBranchItem::layout() {
  int j = 0;
  for (auto it = m_gridItems.begin(); it < m_gridItems.end(); ++it) {
    (*it)->setGridPos(0, j);
    j = j + (*it)->getVGridLength();
  }
  for (auto it = m_paddingItems.begin(); it < m_paddingItems.end(); ++it) {
    (*it)->setGridPos(0, j);
    j = j + (*it)->getVGridLength();
  }
  setVGridLength(j);
}

void VerticalBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

ReverseVerticalBranchItem::ReverseVerticalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent) : GridItem(parent) {
  for (const auto& modelObject : modelObjects) {
    if (auto comp = modelObject.optionalCast<model::Node>()) {
      GridItem* gridItem = new TwoFourNodeItem(this);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
    } else if (auto comp = modelObject.optionalCast<model::StraightComponent>()) {
      GridItem* gridItem = new FourTwoStraightItem(this);
      gridItem->setModelObject(modelObject);
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
    }
  }

  layout();
}

void ReverseVerticalBranchItem::setPadding(unsigned padding) {
  if (m_paddingItems.size() > padding) {
    for (auto it = m_paddingItems.begin() + padding; it < m_paddingItems.end(); ++it) {
      (*it)->scene()->removeItem(*it);
    }
    m_paddingItems.erase(m_paddingItems.begin() + padding, m_paddingItems.end());
  } else if (m_paddingItems.size() < padding) {
    for (unsigned i = m_paddingItems.size(); i < padding; i++) {
      auto* straightItem = new FourTwoStraightItem();
      straightItem->setParentItem(this);
      m_paddingItems.push_back(straightItem);
    }
  }

  layout();
}

void ReverseVerticalBranchItem::layout() {
  int j = 0;
  for (auto it = m_paddingItems.begin(); it < m_paddingItems.end(); ++it) {
    (*it)->setGridPos(0, j);
    j = j + (*it)->getVGridLength();
  }

  for (auto it = m_gridItems.rbegin(); it < m_gridItems.rend(); ++it) {
    (*it)->setGridPos(0, j);
    j = j + (*it)->getVGridLength();
  }
  setVGridLength(j);
}

/* Sort the (parallel) branches by:
 * - ThermalZone name if applicable,
 * - First component's name after the initial Node if more than one component
 * - Only component (=a node) otherwise
 */
bool sortBranches(std::vector<ModelObject> i, std::vector<ModelObject> j) {
  OS_ASSERT(!i.empty());
  OS_ASSERT(!j.empty());

  std::vector<ThermalZone> iZones = subsetCastVector<ThermalZone>(i);
  std::vector<ThermalZone> jZones = subsetCastVector<ThermalZone>(j);

  boost::optional<ModelObject> iModelObject;
  boost::optional<ModelObject> jModelObject;

  if (!iZones.empty()) {
    iModelObject = iZones.front();
  } else if (i.size() > 1u) {
    iModelObject = i[1];
  } else {
    iModelObject = i[0];
  }

  if (!jZones.empty()) {
    jModelObject = jZones.front();
  } else if (j.size() > 1u) {
    jModelObject = j[1];
  } else {
    jModelObject = j[0];
  }

  OS_ASSERT(iModelObject);
  OS_ASSERT(jModelObject);

  return iModelObject->name().get() < jModelObject->name().get();
}

// Given a splitter / mixer pair, return the "center" ModelObject for each branch
// That is the first thing that is either 1) a zone, 2) not a splitter/mixer or node, 3) a node if that is all that is found
// This will be used later to find the components both upstream and downstream of the center ModelObject
// This is unfortunately tricky, but necessary to handle air system demand branches that involve plenums
std::vector<model::HVACComponent> centerHVACComponents(model::Splitter& splitter, model::Mixer& mixer) {
  std::vector<model::HVACComponent> result;

  auto loop = splitter.loop();

  auto removeUnwantedSplitterMixerNodesPred = [](const model::HVACComponent& modelObject) {
    auto iddObjectType = modelObject.iddObjectType();

    return (iddObjectType == openstudio::IddObjectType::OS_AirLoopHVAC_ZoneSplitter)
           || (iddObjectType == openstudio::IddObjectType::OS_AirLoopHVAC_ZoneMixer)
           || (iddObjectType == openstudio::IddObjectType::OS_AirLoopHVAC_SupplyPlenum)
           || (iddObjectType == openstudio::IddObjectType::OS_AirLoopHVAC_ReturnPlenum) || (iddObjectType == openstudio::IddObjectType::OS_Node);
  };

  if (loop) {
    auto outletObjects = subsetCastVector<model::HVACComponent>(splitter.outletModelObjects());
    for (const auto& object : outletObjects) {
      auto branchObjects = subsetCastVector<model::HVACComponent>(loop->demandComponents(object, mixer));

      auto zones = subsetCastVector<model::ThermalZone>(branchObjects);
      if (!zones.empty()) {
        result.insert(result.end(), zones.begin(), zones.end());
        continue;
      }

      // reducedSet is remains with the things from "2" not a plitter/mixer or node
      // We are expecting a terminal. ie a branch with only a terminal and nodes on it
      auto reducedSetIt = std::remove_if(branchObjects.begin(), branchObjects.end(), removeUnwantedSplitterMixerNodesPred);
      if (reducedSetIt != branchObjects.end()) {
        result.push_back(*reducedSetIt);
        continue;
      }

      // If we get here then we should only have nodes
      // ie a branch with only a single node on it.
      auto nodes = subsetCastVector<model::Node>(branchObjects);
      if (!nodes.empty()) {
        result.push_back(nodes.front());
      }
    }
  }

  WorkspaceObjectNameLess sorter;
  std::sort(result.begin(), result.end(), sorter);

  return result;
}

// This is an overload for laying out branch groups for dual duct paths
HorizontalBranchGroupItem::HorizontalBranchGroupItem(model::Splitter& splitter, std::vector<model::Node>& supplyOutletNodes, QGraphicsItem* parent)
  : GridItem(parent), m_splitter(splitter), m_dropZoneBranchItem(nullptr) {
  auto optionalLoop = splitter.loop();
  OS_ASSERT(optionalLoop);
  model::Loop loop = optionalLoop.get();

  auto splitterOutletObjects = splitter.outletModelObjects();

  for (size_t i = 0; i < 2; ++i) {
    auto inlet = splitterOutletObjects[i].cast<model::HVACComponent>();
    auto outlet = supplyOutletNodes[i].cast<model::HVACComponent>();

    auto branchComponents = loop.components(inlet, outlet);
    branchComponents.pop_back();

    m_branchItems.push_back(new HorizontalBranchItem(branchComponents, this));
  }

  layout();
}

HorizontalBranchGroupItem::HorizontalBranchGroupItem(model::Splitter& splitter, model::Mixer& mixer, QGraphicsItem* parent)
  : GridItem(parent), m_splitter(splitter), m_dropZoneBranchItem(nullptr) {
  auto splitterOutletObjects = splitter.outletModelObjects();

  if (!(splitterOutletObjects.front() == mixer)) {
    auto reverseVector = [](const std::vector<model::ModelObject>& modelObjects) {
      std::vector<model::ModelObject> rModelObjects;

      for (auto rit = modelObjects.rbegin(); rit < modelObjects.rend(); ++rit) {
        rModelObjects.push_back(*rit);
      }

      return rModelObjects;
    };

    auto loop = splitter.loop();
    OS_ASSERT(loop);
    auto airLoop = loop->optionalCast<model::AirLoopHVAC>();

    bool isSupplySide = loop->supplyComponent(splitter.handle()).has_value();

    if (airLoop && (!isSupplySide)) {
      // Why is there these extra hoops for air loop demand?
      // The reason is because of plenums. If we go from splitter outlet node,
      // to mixer inlet node (when there are plenums) there may be more than one path,
      // and we will receive all of those extra ModelObject instances from ::demandComponents
      auto centerComps = centerHVACComponents(splitter, mixer);
      auto splitters = airLoop->zoneSplitters();

      std::pair<std::vector<model::ModelObject>, std::vector<model::ModelObject>> allCompsBeforeTerminal;

      for (const auto& centerComp : centerComps) {
        boost::optional<model::HVACComponent> keyComp = centerComp;
        if (auto zone = centerComp.optionalCast<model::ThermalZone>()) {
          auto terminals = zone->airLoopHVACTerminals();
          for (const auto& term : terminals) {
            auto a = term.airLoopHVAC();
            if (a && (a->handle() == airLoop->handle())) {
              keyComp = term;
            }
          }
        }
        OS_ASSERT(keyComp);
        {
          auto compsBeforeTerminal = airLoop->demandComponents(splitters[0], keyComp.get());
          compsBeforeTerminal.erase(compsBeforeTerminal.begin());
          compsBeforeTerminal.pop_back();
          allCompsBeforeTerminal.first = reverseVector(compsBeforeTerminal);
        }

        auto compsAfterTerminal = airLoop->demandComponents(keyComp.get(), mixer);
        // We want the center but not the mixer
        compsAfterTerminal.pop_back();
        auto rCompsAfterTerminal = reverseVector(compsAfterTerminal);

        if (keyComp->optionalCast<model::Mixer>()) {
          // We must have a dual duct for this branch
          OS_ASSERT(splitters.size() == 2u);

          auto compsBeforeTerminal = airLoop->demandComponents(splitters[1], keyComp.get());
          compsBeforeTerminal.erase(compsBeforeTerminal.begin());
          compsBeforeTerminal.pop_back();
          allCompsBeforeTerminal.second = reverseVector(compsBeforeTerminal);

          m_branchItems.push_back(new HorizontalBranchItem(allCompsBeforeTerminal, rCompsAfterTerminal, this));
        } else {
          auto comps = rCompsAfterTerminal;
          comps.insert(comps.end(), allCompsBeforeTerminal.first.begin(), allCompsBeforeTerminal.first.end());
          m_branchItems.push_back(new HorizontalBranchItem(comps, this));
        }
      }
    } else {
      std::vector<std::vector<model::ModelObject>> allBranchComponents;

      for (const auto& splitterOutletObject : splitterOutletObjects) {
        auto comp1 = splitterOutletObject.optionalCast<model::HVACComponent>();
        OS_ASSERT(comp1);
        auto branchComponents = loop->components(comp1.get(), mixer);

        // Can't pop_back if it's empty to begin with...
        if (branchComponents.empty()) {
          std::stringstream ss;
          ss << "Found orphaned component while drawing loop for " << comp1.get().briefDescription() << ".";

          if (comp1->isRemovable()) {
            ss << " Removing it.";
            comp1->remove();
          } else {

            ss << " But this component is not removable. You should use the Ruby bindings to disconnect then remove it";

            //ss << " But this component is not removable. Trying to forcibly disconnect then remove it";
            //// Start by disconnecting
            //comp1->disconnect();
            //// Then remove
            //// TODO: Problem: this will produce a crash when drawing later...
            //std::vector<IdfObject> delComps = comp1->remove();
            //// Check whether it did delete something or not
            //if (delComps.empty()) {
            //ss << ", but it didn't work.";
            //}
          }
          QMessageBox box(QMessageBox::Warning, QString("Orphaned component Found"), toQString(ss.str()), QMessageBox::Ok);
          box.exec();

        } else {
          // Pop the last component (the mixer)
          branchComponents.pop_back();

          if (isSupplySide) {
            allBranchComponents.push_back(branchComponents);
          } else {
            auto rBranchComponents = reverseVector(branchComponents);
            allBranchComponents.push_back(rBranchComponents);
          }
        }
      }

      // Note JM 2019-07-16: If this is the demand side, we order it by the thermal zone names (airLoopHVAC),
      // or the component after the node names,makes it easier to find stuff
      // If this is the supply side (which only affects PlantLoop in effect, since a single duct doesn't have parallel branches,
      // and a dual duct uses vertical branches - but might as well not call a sort function that will do nothing for AirLoopHVAC),
      // we don't want to do it because we want it to display in the same order
      // that will end up in the PlantEquipmentList after Forward Translatation (affects Load Distribution substantially!)
      if (!isSupplySide) {
        std::sort(allBranchComponents.begin(), allBranchComponents.end(), sortBranches);
      }
      for (const auto& allBranchComponent : allBranchComponents) {
        m_branchItems.push_back(new HorizontalBranchItem(allBranchComponent, this));
      }
    }
  }

  layout();
}

unsigned HorizontalBranchGroupItem::numberOfBranches() const {
  return m_branchItems.size();
}

std::vector<int> HorizontalBranchGroupItem::branchBaselineGridPositions() const {
  std::vector<int> result;

  for (const auto& branchItem : m_branchItems) {
    result.emplace_back(branchItem->getYGridPos() + branchItem->getVGridLength() - 1);
  }

  return result;
}

void HorizontalBranchGroupItem::setShowDropZone(bool showDropZone) {
  if (m_dropZoneBranchItem) {
    auto it = std::find(m_branchItems.begin(), m_branchItems.end(), m_dropZoneBranchItem);

    if (it != m_branchItems.end()) {
      m_branchItems.erase(it);

      m_dropZoneBranchItem = nullptr;
    }
  }

  if (showDropZone) {
    m_dropZoneBranchItem = new HorizontalBranchItem(std::vector<model::ModelObject>(), this);
    m_dropZoneBranchItem->setIsDropZone(true);
    m_branchItems.push_back(m_dropZoneBranchItem);

    m_dropZoneBranchItem->setModelObject(m_splitter);
  }

  layout();
}

void HorizontalBranchGroupItem::layout() {
  if (m_branchItems.empty()) {
    auto* branchItem = new HorizontalBranchItem(std::vector<model::ModelObject>(), this);

    branchItem->setPadding(3);

    m_branchItems.push_back(branchItem);
  }

  int longestBranch = 0;
  for (auto it = m_branchItems.begin(); it < m_branchItems.end(); ++it) {
    if ((*it)->getHGridLength() > longestBranch) {
      longestBranch = (*it)->getHGridLength();
    }
  }

  setHGridLength(longestBranch);

  for (auto it = m_branchItems.begin(); it < m_branchItems.end(); ++it) {
    unsigned padding = longestBranch - (*it)->getHGridLength();
    if (padding > 0) {
      (*it)->setPadding(padding);
    }
  }

  int j = 0;
  int lastj = 0;
  for (auto it = m_branchItems.begin(); it < m_branchItems.end(); ++it) {
    (*it)->setGridPos(0, j);
    lastj = j + (*it)->getVGridLength();
    j = lastj + 1;
  }

  if (lastj == 0) {
    lastj = 1;
  }

  setVGridLength(lastj);
}

void HorizontalBranchGroupItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

SystemItem::SystemItem(const model::Loop& loop, LoopScene* loopScene) : m_loop(loop), m_loopScene(loopScene) {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();
  std::shared_ptr<MainRightColumnController> mrc = doc->mainRightColumnController();
  mrc->registerSystemItem(m_loop.handle(), this);

  m_loopScene->addItem(this);

  auto supplyInletNode = m_loop.supplyInletNode();
  auto supplyOutletNodes = m_loop.supplyOutletNodes();

  std::vector<model::AirLoopHVACSupplyPlenum> supplyPlenums =
    subsetCastVector<model::AirLoopHVACSupplyPlenum>(m_loop.demandComponents(openstudio::IddObjectType::OS_AirLoopHVAC_SupplyPlenum));

  std::vector<model::AirLoopHVACReturnPlenum> returnPlenums =
    subsetCastVector<model::AirLoopHVACReturnPlenum>(m_loop.demandComponents(openstudio::IddObjectType::OS_AirLoopHVAC_ReturnPlenum));

  int i = 0;

  for (const auto& supplyPlenum : supplyPlenums) {
    if (boost::optional<model::ThermalZone> tz = supplyPlenum.thermalZone()) {
      if (boost::optional<model::RenderingColor> rc = tz->renderingColor()) {
        QColor color;
        color.setRed(rc->renderingRedValue());
        color.setBlue(rc->renderingBlueValue());
        color.setGreen(rc->renderingGreenValue());
        m_plenumColorMap.insert(std::make_pair(supplyPlenum.handle(), color));
      }
    }
    m_plenumIndexMap.insert(std::make_pair(supplyPlenum.handle(), i));
    i++;
  }

  for (auto& returnPlenum : returnPlenums) {
    if (boost::optional<model::ThermalZone> tz = returnPlenum.thermalZone()) {
      if (boost::optional<model::RenderingColor> rc = tz->renderingColor()) {
        QColor color;
        color.setRed(rc->renderingRedValue());
        color.setBlue(rc->renderingBlueValue());
        color.setGreen(rc->renderingGreenValue());
        m_plenumColorMap.insert(std::make_pair(returnPlenum.handle(), color));
      }
    }
    m_plenumIndexMap.insert(std::make_pair(returnPlenum.handle(), i));
    i++;
  }

  m_supplySideItem = new SupplySideItem(this, supplyInletNode, supplyOutletNodes);

  m_supplySideItem->setGridPos(0, 0);

  auto demandInletNodes = m_loop.demandInletNodes();
  auto demandOutletNode = m_loop.demandOutletNode();

  m_demandSideItem = new DemandSideItem(this, demandInletNodes, demandOutletNode);

  m_demandSideItem->setGridPos(0, m_supplySideItem->getVGridLength() + 1);

  int defaultDemandPadding = 0;
  int defaultSupplyPadding = 0;

  m_supplySideItem->setPadding(defaultSupplyPadding);
  m_demandSideItem->setPadding(defaultDemandPadding);

  int supplyHGridLength = m_supplySideItem->getHGridLength();
  int demandHGridLength = m_demandSideItem->getHGridLength();

  int diff = supplyHGridLength - demandHGridLength;

  if (diff >= 0) {
    m_demandSideItem->setPadding(diff + defaultDemandPadding);
  } else {
    m_supplySideItem->setPadding((diff * -1) + defaultSupplyPadding);
  }

  m_systemCenterItem = new SystemCenterItem(this, m_loop);

  m_systemCenterItem->setHGridLength(m_supplySideItem->getHGridLength());

  m_systemCenterItem->setGridPos(0, m_supplySideItem->getVGridLength());

  setVGridLength(m_supplySideItem->getVGridLength() + m_demandSideItem->getVGridLength() + m_systemCenterItem->getVGridLength());

  setHGridLength(m_supplySideItem->getHGridLength());
}

SystemItem::~SystemItem() {
  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();
  std::shared_ptr<MainRightColumnController> mrc = doc->mainRightColumnController();
  mrc->unregisterSystemItem(m_loop.handle());
}

int SystemItem::plenumIndex(const Handle& plenumHandle) {
  auto it = m_plenumIndexMap.find(plenumHandle);
  if (it != m_plenumIndexMap.end()) {
    return it->second;
  } else {
    return -1;
  }
}

QColor SystemItem::plenumColor(const Handle& plenumHandle) {
  QColor color;

  auto it = m_plenumColorMap.find(plenumHandle);
  if (it != m_plenumColorMap.end()) {
    color = it->second;
  } else {
    int index = plenumIndex(plenumHandle);
    if (index < 0) {
      color = SchedulesView::colors[0];
    } else if (index > 12) {
      color = SchedulesView::colors[12];
    } else {
      color = SchedulesView::colors[index];
    }

    // DLM: Create a RenderingColor and associate it with the thermal zone?
  }

  return color;
}

void SystemItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

SystemCenterItem::SystemCenterItem(QGraphicsItem* parent, const model::Loop& loop)
  : GridItem(parent), m_supplyDualDuct(false), m_demandDualDuct(false) {
  this->setModelObject(loop);

  if (loop.supplyOutletNodes().size() == 2u) {
    m_supplyDualDuct = true;
  }

  if (loop.demandInletNodes().size() == 2u) {
    m_demandDualDuct = true;
  }
}

void SystemCenterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  int yOrigin = 0;

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(0, yOrigin, 100, 101);
  painter->drawRect((m_hLength - 1) * 100, yOrigin, 100, 101);
  if (m_supplyDualDuct) {
    painter->drawRect((m_hLength - 3) * 100, yOrigin, 100, 51);
  }
  if (m_demandDualDuct) {
    painter->drawRect((m_hLength - 3) * 100, 50, 100, 51);
  }

  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, yOrigin, 50, yOrigin + 100);
  painter->drawLine((m_hLength - 1) * 100 + 50, yOrigin, (m_hLength - 1) * 100 + 50, yOrigin + 100);
  painter->drawPixmap((m_hLength - 1) * 100 + 37.5, yOrigin + 25, 25, 25, QPixmap(":/images/arrow.png"));

  if (m_supplyDualDuct) {
    painter->drawLine((m_hLength - 3) * 100 + 50, yOrigin, (m_hLength - 3) * 100 + 50, yOrigin + 50);
    painter->drawPixmap((m_hLength - 3) * 100 + 37.5, yOrigin + 25, 25, 25, QPixmap(":/images/arrow.png"));
  }
  if (m_demandDualDuct) {
    painter->drawLine((m_hLength - 3) * 100 + 50, yOrigin + 50, (m_hLength - 3) * 100 + 50, yOrigin + 100);
  }

  painter->setPen(QPen(Qt::black, 1, Qt::DashLine, Qt::RoundCap));
  painter->drawLine(0, yOrigin + 50, (m_hLength)*100, yOrigin + 50);

  painter->rotate(180);
  painter->drawPixmap(-62, -(yOrigin + 75), 25, 25, QPixmap(":/images/arrow.png"));

  painter->rotate(-180);

  if (m_highlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(0, yOrigin, m_hLength * 100, 100);
  }

  QFont font = painter->font();
  font.setPointSize(14);
  painter->setFont(font);
  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
  painter->drawText(QRect(110, 21, 200, 25), Qt::AlignBottom, "Supply Equipment");
  painter->drawText(QRect(110, 52, 200, 25), Qt::AlignTop, "Demand Equipment");
}

SupplyPlenumItem::SupplyPlenumItem(const model::ModelObject& modelObject, QGraphicsItem* parent) : GridItem(parent) {
  setModelObjectInternal(modelObject);

  // HorizontalBranchItem -> BranchGroupItem -> DemandSideItem -> SystemItem
  m_color = static_cast<SystemItem*>(parentItem()->parentItem()->parentItem()->parentItem())->plenumColor(modelObject.handle());
}

void SupplyPlenumItem::setModelObject(model::OptionalModelObject modelObject) {
  setModelObjectInternal(modelObject);
}

void SupplyPlenumItem::setModelObjectInternal(model::OptionalModelObject modelObject) {

  if (modelObject) {
    auto plenum = modelObject->optionalCast<model::AirLoopHVACSupplyPlenum>();
    OS_ASSERT(plenum);

    GridItem::setModelObject(modelObject);

    if (boost::optional<model::ThermalZone> tz = plenum->thermalZone()) {
      setToolTip(QString::fromStdString(tz->name().get()));
    }
  }
}

void SupplyPlenumItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(m_color, Qt::SolidPattern));

  painter->drawLine(0, 50, 25, 50);
  painter->drawLine(75, 50, 100, 50);

  QPointF points[4] = {
    QPointF(25, 25),
    QPointF(75, 40),
    QPointF(75, 60),
    QPointF(25, 75),
  };
  painter->drawPolygon(points, 4);
}

ReturnPlenumItem::ReturnPlenumItem(const model::ModelObject& modelObject, QGraphicsItem* parent) : GridItem(parent) {
  setModelObjectInternal(modelObject);

  // HorizontalBranchItem -> BranchGroupItem -> DemandSideItem -> SystemItem
  m_color = static_cast<SystemItem*>(parentItem()->parentItem()->parentItem()->parentItem())->plenumColor(modelObject.handle());
}

void ReturnPlenumItem::setModelObject(model::OptionalModelObject modelObject) {
  setModelObjectInternal(modelObject);
}

void ReturnPlenumItem::setModelObjectInternal(model::OptionalModelObject modelObject) {
  if (modelObject) {
    boost::optional<model::AirLoopHVACReturnPlenum> plenum = modelObject->optionalCast<model::AirLoopHVACReturnPlenum>();
    OS_ASSERT(plenum);

    GridItem::setModelObject(modelObject);

    if (boost::optional<model::ThermalZone> tz = plenum->thermalZone()) {
      setToolTip(QString::fromStdString(tz->name().get()));
    }
  }
}

void ReturnPlenumItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(m_color, Qt::SolidPattern));

  painter->drawLine(0, 50, 25, 50);
  painter->drawLine(75, 50, 100, 50);

  QPointF points[4] = {
    QPointF(25, 40),
    QPointF(75, 25),
    QPointF(75, 75),
    QPointF(25, 60),
  };
  painter->drawPolygon(points, 4);
}

OneThreeDualDuctMixerItem::OneThreeDualDuctMixerItem(QGraphicsItem* parent, bool /* dualDuct */) : GridItem(parent) {}

void OneThreeDualDuctMixerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(0, 50, 25, 50);
  painter->drawLine(100, 25, 75, 25);
  painter->drawLine(100, 75, 75, 75);

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(0, 0, 100, 100, *qPixmap);
  }
}

OneThreeStraightItem::OneThreeStraightItem(QGraphicsItem* parent, bool dualDuct) : GridItem(parent), m_dualDuct(dualDuct) {}

void OneThreeStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  if (m_dualDuct) {
    painter->drawLine(0, 25, 100, 25);
    painter->drawLine(0, 75, 100, 75);
  } else {
    painter->drawLine(0, 50, 100, 50);
  }

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(70,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

void OneThreeStraightItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  //if(QRect(70,70,25,25).contains(event->pos().toPoint()))
  //{
  //  if(m_modelObject)
  //  {
  //    event->accept();
  //
  //    QApplication::instance()->processEvents();

  //    emit removeModelObjectClicked( m_modelObject.get() );
  //  }
  //}
  //else
  //{
  GridItem::mouseReleaseEvent(event);
  //}
}

OneThreeWaterToAirItem::OneThreeWaterToAirItem(QGraphicsItem* parent) : GridItem(parent), m_showLinks(false) {}

void OneThreeWaterToAirItem::onLinkItemClicked() {
  if (m_modelObject) {
    emit innerNodeClicked(m_modelObject.get());
  }
}

void OneThreeWaterToAirItem::setModelObject(model::OptionalModelObject modelObject) {
  GridItem::setModelObject(modelObject);

  m_showLinks = false;

  if (m_modelObject) {
    if (auto waterToAirComponent = m_modelObject->optionalCast<WaterToAirComponent>()) {
      if (waterToAirComponent->airLoopHVAC() && waterToAirComponent->plantLoop()) {
        auto* linkItem1 = new LinkItem(this);
        linkItem1->setPos(40, 5);
        connect(linkItem1, &LinkItem::mouseClicked, this, &OneThreeWaterToAirItem::onLinkItemClicked);

        auto* linkItem2 = new LinkItem(this);
        linkItem2->setPos(40, 75);
        connect(linkItem2, &LinkItem::mouseClicked, this, &OneThreeWaterToAirItem::onLinkItemClicked);

        m_showLinks = true;
      }
    }
  }
}

void OneThreeWaterToAirItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(0, 50, 100, 50);

  if (m_showLinks) {
    painter->drawLine(50, 15, 50, 85);
  }

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(70,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

void OneThreeWaterToAirItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  //if(QRect(70,70,25,25).contains(event->pos().toPoint()))
  //{
  //  if(m_modelObject)
  //  {
  //    event->accept();
  //
  //    QApplication::instance()->processEvents();

  //    emit removeModelObjectClicked( m_modelObject.get() );
  //  }
  //}
  //else
  //{
  GridItem::mouseReleaseEvent(event);
  //}
}

OneThreeWaterToWaterItem::OneThreeWaterToWaterItem(QGraphicsItem* parent) : GridItem(parent), m_showLinks(false) {}

void OneThreeWaterToWaterItem::onLinkItemClicked() {
  if (m_modelObject) {
    emit innerNodeClicked(m_modelObject.get());
  }
}

void OneThreeWaterToWaterItem::setModelObject(model::OptionalModelObject modelObject) {
  GridItem::setModelObject(modelObject);

  m_showLinks = false;

  if (m_modelObject) {
    if (auto waterToWaterComponent = m_modelObject->optionalCast<WaterToWaterComponent>()) {
      if (waterToWaterComponent->plantLoop() && waterToWaterComponent->secondaryPlantLoop()) {
        auto* linkItem1 = new LinkItem(this);
        linkItem1->setPos(40, 5);
        connect(linkItem1, &LinkItem::mouseClicked, this, &OneThreeWaterToWaterItem::onLinkItemClicked);

        auto* linkItem2 = new LinkItem(this);
        linkItem2->setPos(40, 75);
        connect(linkItem2, &LinkItem::mouseClicked, this, &OneThreeWaterToWaterItem::onLinkItemClicked);

        m_showLinks = true;
      }
    }
  }
}

void OneThreeWaterToWaterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(0, 50, 100, 50);

  if (m_showLinks) {
    painter->drawLine(50, 15, 50, 85);
  }

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(70,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

void OneThreeWaterToWaterItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  //if(QRect(70,70,25,25).contains(event->pos().toPoint()))
  //{
  //  if(m_modelObject)
  //  {
  //    event->accept();
  //
  //    QApplication::instance()->processEvents();

  //    emit removeModelObjectClicked( m_modelObject.get() );
  //  }
  //}
  //else
  //{
  GridItem::mouseReleaseEvent(event);
  //}
}

TwoFourStraightItem::TwoFourStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void TwoFourStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 0, 50, 100);

  if (modelObject()) {
    painter->translate(100, 0);
    painter->rotate(90);

    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    painter->rotate(-90);
    painter->translate(-100, 0);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(70,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

OASupplyStraightItem::OASupplyStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void OASupplyStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(5, 0, 90, 100);

  if (isSelected()) {
    painter->setBrush(QBrush(QColor(128, 128, 128), Qt::SolidPattern));
    painter->drawRect(5, 0, 90, 100);
  }

  if (m_highlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(5, 0, 90, 100);
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 0, 50, 100);

  if (modelObject()) {
    painter->translate(100, 0);
    painter->rotate(90);

    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    painter->rotate(-90);
    painter->translate(-100, 0);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(68,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

OAReliefStraightItem::OAReliefStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void OASupplyStraightItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  //if(QRect(68,70,25,25).contains(event->pos().toPoint()))
  //{
  //  if(m_modelObject)
  //  {
  //    event->accept();
  //
  //    QApplication::instance()->processEvents();

  //    emit removeModelObjectClicked( m_modelObject.get() );
  //  }
  //}
  //else
  //{
  GridItem::mouseReleaseEvent(event);
  //}
}

void OAReliefStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(5, 0, 90, 100);

  if (m_highlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(5, 0, 90, 100);
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 0, 50, 100);

  if (modelObject()) {
    painter->translate(0, 100);
    painter->rotate(-90);

    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    painter->rotate(90);
    painter->translate(0, -100);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(68,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

void OAReliefStraightItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  //if(QRect(68,70,25,25).contains(event->pos().toPoint()))
  //{
  //  if(m_modelObject)
  //  {
  //    event->accept();
  //
  //    QApplication::instance()->processEvents();

  //    emit removeModelObjectClicked( m_modelObject.get() );
  //  }
  //}
  //else
  //{
  GridItem::mouseReleaseEvent(event);
  //}
}

OAAirToAirItem::OAAirToAirItem(QGraphicsItem* parent) : GridItem(parent) {
  setHGridLength(2);
  setVGridLength(1);
}

void OAAirToAirItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(5, 0, m_hLength * 100 - 10, m_vLength * 100);

  if (isSelected()) {
    painter->setBrush(QBrush(QColor(128, 128, 128), Qt::SolidPattern));
    painter->drawRect(5, 0, m_hLength * 100 - 10, m_vLength * 100);
  }

  if (m_highlight && m_enableHighlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(5, 0, m_hLength * 100 - 10, m_vLength * 100);
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 0, 50, 100);
  painter->drawLine(150, 0, 150, 100);

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(0, 0, 200, 100, *qPixmap);
  }
}

OASupplyBranchItem::OASupplyBranchItem(const std::vector<model::ModelObject>& supplyModelObjects,
                                       const std::vector<model::ModelObject>& reliefModelObjects, QGraphicsItem* parent)
  : GridItem(parent) {
  setAcceptHoverEvents(false);

  // reliefIt = components from return to outside
  auto reliefIt = reliefModelObjects.begin();
  // supplyIt = components from mixed air node to outside (= oaSystem.oaComponents.reverse)
  auto supplyIt = supplyModelObjects.begin();

  while (supplyIt < supplyModelObjects.end()) {
    // If this is an AirToAirComponent (an ERV basically...)
    if (supplyIt->optionalCast<model::AirToAirComponent>()) {
      // We fake draw the relief side until we get to the ERV so ERV is ligned up in both cases
      while ((reliefIt < reliefModelObjects.end()) && (!reliefIt->optionalCast<model::AirToAirComponent>())) {
        GridItem* gridItem = new OASupplyStraightItem(this);
        m_gridItems.push_back(gridItem);
        ++reliefIt;
      }
      ++reliefIt;
      m_gridItems.push_back(nullptr);
    } else if (boost::optional<model::Node> comp = supplyIt->optionalCast<model::Node>()) {
      GridItem* gridItem = new OAStraightNodeItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);

      if ((reliefIt < reliefModelObjects.end()) && (!reliefIt->optionalCast<model::AirToAirComponent>())) {
        ++reliefIt;
      }
    } else if (boost::optional<model::StraightComponent> comp = supplyIt->optionalCast<model::StraightComponent>()) {
      GridItem* gridItem = new OASupplyStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);

      if ((reliefIt < reliefModelObjects.end()) && (!reliefIt->optionalCast<model::AirToAirComponent>())) {
        ++reliefIt;
      }
    } else if (boost::optional<model::WaterToAirComponent> comp = supplyIt->optionalCast<model::WaterToAirComponent>()) {
      GridItem* gridItem = new OAReliefStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);

      if ((reliefIt < reliefModelObjects.end()) && (!reliefIt->optionalCast<model::AirToAirComponent>())) {
        ++reliefIt;
      }
    } else if (auto comp = supplyIt->optionalCast<model::ZoneHVACTerminalUnitVariableRefrigerantFlow>()) {
      // VRF TU
      GridItem* gridItem = new OASupplyStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);

      if ((reliefIt < reliefModelObjects.end()) && (!reliefIt->optionalCast<model::AirToAirComponent>())) {
        ++reliefIt;
      }
    }

    ++supplyIt;
  }

  while (reliefIt < reliefModelObjects.end()) {
    GridItem* gridItem = new OASupplyStraightItem(this);
    m_gridItems.push_back(gridItem);
    ++reliefIt;
  }

  layout();
}

void OASupplyBranchItem::layout() {
  int j = 0;
  for (auto it = m_gridItems.rbegin(); it < m_gridItems.rend(); ++it) {
    if (*it) {
      (*it)->setGridPos(0, j);
      j = j + (*it)->getVGridLength();
    } else {
      j = j + 1;
    }
  }
  setVGridLength(j);
}

void OASupplyBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

OAReliefBranchItem::OAReliefBranchItem(const std::vector<model::ModelObject>& reliefModelObjects,
                                       const std::vector<model::ModelObject>& supplyModelObjects, QGraphicsItem* parent)
  : GridItem(parent) {
  setAcceptHoverEvents(false);

  auto reliefIt = reliefModelObjects.begin();
  auto supplyIt = supplyModelObjects.begin();

  while (reliefIt < reliefModelObjects.end()) {
    if (boost::optional<model::AirToAirComponent> comp = reliefIt->optionalCast<model::AirToAirComponent>()) {
      while ((supplyIt < supplyModelObjects.end()) && (!supplyIt->optionalCast<model::AirToAirComponent>())) {
        GridItem* gridItem = new OAReliefStraightItem(this);
        m_gridItems.push_back(gridItem);
        ++supplyIt;
      }
      GridItem* gridItem = new OAAirToAirItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      m_gridItems.push_back(gridItem);
      ++supplyIt;
    } else if (boost::optional<model::Node> comp = reliefIt->optionalCast<model::Node>()) {
      GridItem* gridItem = new OAStraightNodeItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
      if ((supplyIt < supplyModelObjects.end()) && (!supplyIt->optionalCast<model::AirToAirComponent>())) {
        ++supplyIt;
      }
    } else if (boost::optional<model::StraightComponent> comp = reliefIt->optionalCast<model::StraightComponent>()) {
      GridItem* gridItem = new OAReliefStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
      if ((supplyIt < supplyModelObjects.end()) && (!supplyIt->optionalCast<model::AirToAirComponent>())) {
        ++supplyIt;
      }
    } else if (boost::optional<model::WaterToAirComponent> comp = reliefIt->optionalCast<model::WaterToAirComponent>()) {
      GridItem* gridItem = new OAReliefStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
      if ((supplyIt < supplyModelObjects.end()) && (!supplyIt->optionalCast<model::AirToAirComponent>())) {
        ++supplyIt;
      }
    } else if (auto comp = reliefIt->optionalCast<model::ZoneHVACTerminalUnitVariableRefrigerantFlow>()) {
      // VRF TU
      GridItem* gridItem = new OAReliefStraightItem(this);
      gridItem->setModelObject(comp->optionalCast<model::ModelObject>());
      if (comp->isRemovable()) {
        gridItem->setDeletable(true);
      }
      m_gridItems.push_back(gridItem);
      if ((supplyIt < supplyModelObjects.end()) && (!supplyIt->optionalCast<model::AirToAirComponent>())) {
        ++supplyIt;
      }
    }

    ++reliefIt;
  }

  while (supplyIt < supplyModelObjects.end()) {
    GridItem* gridItem = new OAReliefStraightItem(this);
    m_gridItems.push_back(gridItem);
    ++supplyIt;
  }

  layout();
}

void OAReliefBranchItem::layout() {
  int j = 0;
  for (auto it = m_gridItems.rbegin(); it < m_gridItems.rend(); ++it) {
    if (*it) {
      (*it)->setGridPos(0, j);
      j = j + (*it)->getVGridLength();
    } else {
      j = j + 1;
    }
  }
  setVGridLength(j);
}

void OAReliefBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

FourTwoStraightItem::FourTwoStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void FourTwoStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 0, 50, 100);

  if (modelObject()) {
    painter->translate(0, 100);
    painter->rotate(-90);

    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, 12, 75, 75, *qPixmap);

    painter->rotate(90);
    painter->translate(0, -100);

    //if(m_deleteAble)
    //{
    //  painter->drawPixmap(70,70,25,25,QPixmap(":/images/delete-icon.png"));
    //}
  }
}

OneFourStraightItem::OneFourStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void OneFourStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 50, 100, 50);
  painter->drawLine(50, 50, 50, 100);
}

ThreeFourStraightItem::ThreeFourStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void ThreeFourStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 50, 0, 50);
  painter->drawLine(50, 50, 50, 100);
}

TwoThreeStraightItem::TwoThreeStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void TwoThreeStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 50, 0, 50);
  painter->drawLine(50, 50, 50, 0);
}

OneTwoStraightItem::OneTwoStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void OneTwoStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(50, 50, 100, 50);
  painter->drawLine(50, 50, 50, 0);
}

OneThreeNodeItem::OneThreeNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void OneThreeNodeItem::setModelObject(model::OptionalModelObject modelObject) {
  GridItem::setModelObject(modelObject);

  delete m_contextButton;

  if (modelObject) {
    if (boost::optional<model::Node> node = modelObject->optionalCast<model::Node>()) {
      if (hasSPM(node.get())) {
        m_contextButton = new NodeContextButtonItem(this);

        m_contextButton->setPos(70, 15);
      }
    }
  }
}

void OneThreeNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(0, 50, 100, 50);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);

  if (m_modelObject) {
    if (model::OptionalNode node = m_modelObject->optionalCast<model::Node>()) {
      for (auto& spm : node->setpointManagers()) {
        if (spm.controlVariable().find("Temperature") != std::string::npos) {
          if (spm.iddObjectType() == SetpointManagerMixedAir::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_mixed.png"));
          } else if ((spm.iddObjectType() == SetpointManagerSingleZoneReheat::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneCooling::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneHeating::iddObjectType())) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_singlezone.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduled::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_scheduled.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduledDualSetpoint::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_dual.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmest::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_warmest.png"));
          } else if (spm.iddObjectType() == SetpointManagerColdest::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_coldest.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirReset::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_outdoorair.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowGroundTemperature::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_follow_ground_temp.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowOutdoorAirTemperature::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_follow_outdoorair.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowSystemNodeTemperature::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_follow_system_node.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneCoolingAverage::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_cooling.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHeatingAverage::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_heating.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirPretreat::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_pretreat.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageCooling::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_onestage_cooling.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageHeating::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_onestage_heating.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmestTemperatureFlow::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_warmest_tempflow.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetTemperature::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_systemnodereset_temperature.png"));
          }
          break;
        } else {
          // These are the Humidty SPMs
          if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_humidity_max.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_humidity_min.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMaximumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_maxhumidity_avg.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMinimumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_multizone_minhumidity_avg.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_max.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_min.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetHumidity::iddObjectType()) {
            painter->drawPixmap(37, 13, 25, 25, QPixmap(":images/setpoint_systemnodereset_humidity.png"));
          }
          break;
        }
      }
    }
  }
}

OneThreeDualDuctItem::OneThreeDualDuctItem(QGraphicsItem* parent) : GridItem(parent) {}

void OneThreeDualDuctItem::setModelObject(model::OptionalModelObject modelObject) {
  m_modelObject = modelObject;
}

void OneThreeDualDuctItem::setModelObject2(model::OptionalModelObject modelObject) {
  m_modelObject2 = std::move(modelObject);
}

void OneThreeDualDuctItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

FourFiveNodeItem::FourFiveNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void FourFiveNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(50, 50, 50, 100);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);
}

OAEndNodeItem::OAEndNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void OAEndNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(5, 0, 90, 100);

  if (isSelected()) {
    painter->setBrush(QBrush(QColor(128, 128, 128), Qt::SolidPattern));
    painter->drawRect(5, 0, 90, 100);
  }

  if (m_highlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(5, 0, 90, 100);
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(50, 50, 50, 100);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);
}

TwoFiveNodeItem::TwoFiveNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void TwoFiveNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(50, 0, 50, 50);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);
}

TwoFourNodeItem::TwoFourNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void TwoFourNodeItem::setModelObject(model::OptionalModelObject modelObject) {
  GridItem::setModelObject(modelObject);

  delete m_contextButton;

  if (modelObject) {
    if (boost::optional<model::Node> node = modelObject->optionalCast<model::Node>()) {
      if (hasSPM(node.get())) {
        m_contextButton = new NodeContextButtonItem(this);

        m_contextButton->setPos(72, 65);
      }
    }
  }
}

void TwoFourNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(50, 0, 50, 100);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);

  if (m_modelObject) {
    if (model::OptionalNode node = m_modelObject->optionalCast<model::Node>()) {
      for (auto& spm : node->setpointManagers()) {
        if (spm.controlVariable().find("Temperature") != std::string::npos) {
          if (spm.iddObjectType() == SetpointManagerMixedAir::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_mixed_right.png"));
          } else if ((spm.iddObjectType() == SetpointManagerSingleZoneReheat::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneCooling::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneHeating::iddObjectType())) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduled::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_scheduled_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduledDualSetpoint::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_dual_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmest::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_warmest_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerColdest::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_coldest_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirReset::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_outdoorair_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowGroundTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_ground_temp_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowOutdoorAirTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_outdoorair_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowSystemNodeTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_system_node_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneCoolingAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_cooling_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHeatingAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_heating_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirPretreat::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_pretreat_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageCooling::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_onestage_cooling_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageHeating::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_onestage_heating_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmestTemperatureFlow::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_warmest_tempflow_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_systemnodereset_temperature_right.png"));
          }

          break;
        } else {
          // These are the humidity ones
          if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_humidity_max_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_humidity_min_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMaximumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_maxhumidity_avg_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMinimumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_minhumidity_avg_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_max_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_min_right.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetHumidity::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_systemnodereset_humidity_right.png"));
          }
          break;
        }
      }
    }
  }
}

OAStraightNodeItem::OAStraightNodeItem(QGraphicsItem* parent) : GridItem(parent) {}

void OAStraightNodeItem::setModelObject(model::OptionalModelObject modelObject) {
  GridItem::setModelObject(modelObject);

  delete m_contextButton;

  if (modelObject) {
    if (boost::optional<model::Node> node = modelObject->optionalCast<model::Node>()) {
      if (hasSPM(node.get())) {
        m_contextButton = new NodeContextButtonItem(this);

        m_contextButton->setPos(70, 65);
      }
    }
  }
}

void OAStraightNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawRect(5, 0, 90, 100);

  if (isSelected()) {
    painter->setBrush(QBrush(QColor(128, 128, 128), Qt::SolidPattern));
    painter->drawRect(5, 0, 90, 100);
  }

  if (m_highlight) {
    painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
    painter->setBrush(QBrush(QColor(127, 127, 127, 127)));
    painter->drawRect(5, 0, 90, 100);
  }

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->drawLine(50, 0, 50, 100);
  setZValue(1);
  painter->drawEllipse(43, 43, 15, 15);

  if (m_modelObject) {
    if (model::OptionalNode node = m_modelObject->optionalCast<model::Node>()) {
      for (auto& spm : node->setpointManagers()) {
        if (spm.controlVariable().find("Temperature") != std::string::npos) {
          if (spm.iddObjectType() == SetpointManagerMixedAir::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_mixed.png"));
          } else if ((spm.iddObjectType() == SetpointManagerSingleZoneReheat::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneCooling::iddObjectType())
                     || (spm.iddObjectType() == SetpointManagerSingleZoneHeating::iddObjectType())) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduled::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_scheduled.png"));
          } else if (spm.iddObjectType() == SetpointManagerScheduledDualSetpoint::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_dual.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmest::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_warmest.png"));
          } else if (spm.iddObjectType() == SetpointManagerColdest::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_coldest.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirReset::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_outdoorair.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowGroundTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_ground_temp.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowOutdoorAirTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_outdoorair.png"));
          } else if (spm.iddObjectType() == SetpointManagerFollowSystemNodeTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_follow_system_node.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneCoolingAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_cooling.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHeatingAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_heating.png"));
          } else if (spm.iddObjectType() == SetpointManagerOutdoorAirPretreat::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_pretreat.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageCooling::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_onestage_cooling.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneOneStageHeating::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_onestage_heating.png"));
          } else if (spm.iddObjectType() == SetpointManagerWarmestTemperatureFlow::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_warmest_tempflow.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetTemperature::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_systemnodereset_temperature.png"));
          }
          break;
        } else {
          // These are the humidity ones
          if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_humidity_max.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_humidity_min.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMaximumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_maxhumidity_avg.png"));
          } else if (spm.iddObjectType() == SetpointManagerMultiZoneMinimumHumidityAverage::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_multizone_minhumidity_avg.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMaximum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_max.png"));
          } else if (spm.iddObjectType() == SetpointManagerSingleZoneHumidityMinimum::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_singlezone_humidity_min.png"));
          } else if (spm.iddObjectType() == SetpointManagerSystemNodeResetHumidity::iddObjectType()) {
            painter->drawPixmap(62, 37, 25, 25, QPixmap(":images/setpoint_systemnodereset_humidity.png"));
          }
          break;
        }
      }
    }
  }
}

OAMixerItem::OAMixerItem(QGraphicsItem* parent) : GridItem(parent) {
  setHGridLength(2);
}

void OAMixerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::NoPen, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(0, 50, 200, 50);
  painter->drawLine(50, 50, 50, 0);
  setZValue(1);
  painter->drawLine(150, 50, 150, 0);

  if (modelObject()) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(0, 0, 200, 100, *qPixmap);
  }
}

void OAMixerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  GridItem::mouseReleaseEvent(event);
}

OASystemItem::OASystemItem(model::AirLoopHVACOutdoorAirSystem& oaSystem, QGraphicsItem* parent)
  : GridItem(parent),
    m_oaMixerItem(new OAMixerItem(this)),
    m_oaBranch(nullptr),
    m_reliefBranch(nullptr),
    m_oaNodeItem(new OAEndNodeItem(this)),
    m_reliefNodeItem(new OAEndNodeItem(this)) {
  m_oaMixerItem->setModelObject(oaSystem);

  std::vector<model::ModelObject> oaComponents = oaSystem.oaComponents();
  std::vector<model::ModelObject> oaBranchComponents(oaComponents.begin() + 1, oaComponents.end());

  std::vector<model::ModelObject> reliefComponents = oaSystem.reliefComponents();
  std::vector<model::ModelObject> reliefBranchComponents(reliefComponents.begin(), reliefComponents.end() - 1);
  std::reverse(oaBranchComponents.begin(), oaBranchComponents.end());

  m_reliefBranch = new OAReliefBranchItem(reliefBranchComponents, oaBranchComponents, this);
  m_oaBranch = new OASupplyBranchItem(oaBranchComponents, reliefBranchComponents, this);

  m_oaNodeItem->setModelObject(oaSystem.outboardOANode().get());

  m_reliefNodeItem->setModelObject(oaSystem.outboardReliefNode().get());

  layout();
}

void OASystemItem::layout() {
  setHGridLength(2);

  m_reliefNodeItem->setGridPos(0, 0);
  m_reliefBranch->setGridPos(0, 1);
  m_oaNodeItem->setGridPos(1, 0);
  m_oaBranch->setGridPos(1, 1);

  unsigned oaVLength = m_oaBranch->getVGridLength();
  setVGridLength(oaVLength + 2);
  m_oaMixerItem->setGridPos(0, oaVLength + 1);
}

void OASystemItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

SplitterItem::SplitterItem(QGraphicsItem* parent) : GridItem(parent), m_numberBranches(1), m_firstDuct1Index(0), m_firstDuct2Index(0) {}

void SplitterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));

  int midpointIndex = m_numberBranches - 1;
  if (m_numberBranches == 1) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, (midpointIndex * 100) + 12, 75, 75, *qPixmap);
  }

  if (m_terminalTypes.empty()) {
    painter->drawLine(50, 50, 50, (((m_numberBranches * 2) - 1) * 100) - 50);
    painter->drawLine(50, (midpointIndex * 100) + 50, 100, (midpointIndex * 100) + 50);

    int j = 50;
    for (int branchIndex = 1; branchIndex <= m_numberBranches; branchIndex++) {
      painter->drawLine(0, j, 50, j);
      j = j + 200;
    }
  } else {
    // if m_terminalTypes is not empty then we probably have a dual duct and things get more complicated
    // because we have to draw two splitters in one

    // Draw the two longest vertical lines
    painter->drawLine(75, m_firstDuct1Index * 100 + 25, 75, (((m_numberBranches * 2) - 1) * 100) - 75);
    painter->drawLine(25, m_firstDuct2Index * 100 + 75, 25, (((m_numberBranches * 2) - 1) * 100) - 25);

    // Draw the leader into the vertical pipe for duct 2
    painter->drawLine(75, (midpointIndex * 100) + 25, 100, (midpointIndex * 100) + 25);

    // Draw the leader into the vertical pipe for duct 1
    painter->drawLine(25, (midpointIndex * 100) + 75, 65, (midpointIndex * 100) + 75);
    painter->drawLine(85, (midpointIndex * 100) + 75, 100, (midpointIndex * 100) + 75);
    painter->drawArc(65, (midpointIndex * 100) + 65, 20, 20, 0, 2880);

    // Draw the horizontal hops from duct 2 to the terminal
    for (unsigned j = m_firstDuct2Index; j < m_terminalTypes.size() + 1; ++j) {
      if (j < m_terminalTypes.size()) {
        if (((m_terminalTypes[j] == TerminalType::DualDuct) || (m_terminalTypes[j] == TerminalType::SingleDuct2))) {
          // Check to make sure we need to hop
          if (j > static_cast<unsigned>(m_firstDuct1Index)) {
            painter->drawLine(0, j * 200 + 25, 15, j * 200 + 25);
            painter->drawLine(35, j * 200 + 25, 75, j * 200 + 25);
            painter->drawArc(15, j * 200 + 15, 20, 20, 0, 2880);
          } else {
            painter->drawLine(0, j * 200 + 25, 75, j * 200 + 25);
          }
        }
      } else {
        // This last one is for the drop zone
        painter->drawLine(0, j * 200 + 25, 15, j * 200 + 25);
        painter->drawLine(35, j * 200 + 25, 75, j * 200 + 25);
        painter->drawArc(15, j * 200 + 15, 20, 20, 0, 2880);
      }
    }

    // Draw the horizontal lines from duct 1 to terminal
    for (unsigned j = m_firstDuct1Index; j < m_terminalTypes.size() + 1; ++j) {
      if (j < m_terminalTypes.size()) {
        auto type = m_terminalTypes[j];
        if (type == TerminalType::DualDuct) {
          painter->drawLine(0, j * 200 + 75, 25, j * 200 + 75);
        } else {
          painter->drawLine(0, j * 200 + 50, 25, j * 200 + 50);
        }
      } else {
        painter->drawLine(0, j * 200 + 75, 25, j * 200 + 75);
      }
    }
  }
}

void SplitterItem::setTerminalTypes(const std::vector<SplitterItem::TerminalType>& types) {
  // We want the number of branches to be in sync with the number of terminal/terminal types
  // Add one for the drop zone
  setNumberBranches(types.size() + 1);
  m_terminalTypes = types;

  if (!m_terminalTypes.empty()) {

    {
      auto terminalIt = std::find_if(m_terminalTypes.begin(), m_terminalTypes.end(), [](const TerminalType& t_type) {
        return ((t_type == SplitterItem::SingleDuct1) || (t_type == TerminalType::DualDuct));
      });

      if (terminalIt != m_terminalTypes.end()) {
        m_firstDuct1Index = std::distance(m_terminalTypes.begin(), terminalIt);
      }
    }

    {
      auto terminalIt = std::find_if(m_terminalTypes.begin(), m_terminalTypes.end(), [](const TerminalType& t_type) {
        return ((t_type == SplitterItem::SingleDuct2) || (t_type == TerminalType::DualDuct));
      });
      if (terminalIt != m_terminalTypes.end()) {
        m_firstDuct2Index = std::distance(m_terminalTypes.begin(), terminalIt);
      }
    }
  }
}

void SplitterItem::setNumberBranches(int branches) {
  m_numberBranches = branches;
  m_vLength = (branches * 2) - 1;
}

int SplitterItem::numberBranches() {
  return m_numberBranches;
}

SupplySplitterItem::SupplySplitterItem(QGraphicsItem* parent) : GridItem(parent) {
  setNumberBranches(1);
}

void SupplySplitterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));

  int midpointIndex = 0;
  if (m_baselineBranchPositions.size() > 1) {
    midpointIndex = (m_baselineBranchPositions.back() - m_baselineBranchPositions.front()) / 2;
    for (const auto& pos : m_baselineBranchPositions) {
      painter->drawLine(50, pos * 100 + 50, 100, pos * 100 + 50);
    }
    painter->drawLine(50, m_baselineBranchPositions.front() * 100 + 50, 50, m_baselineBranchPositions.back() * 100 + 50);
  } else {
    QPixmap qPixmap(":images/supply_splitter.png");
    painter->drawPixmap(12, 12, 75, 75, qPixmap);
  }
  painter->drawLine(0, (midpointIndex * 100) + 50, 50, (midpointIndex * 100) + 50);
}

void SupplySplitterItem::setBranchGridPositions(const std::vector<int>& branchGridPositions) {
  m_baselineBranchPositions = branchGridPositions;

  if (!branchGridPositions.empty()) {
    m_vLength = branchGridPositions.back() + 1;
  } else {
    m_vLength = 1;
  }
}

void SupplySplitterItem::setNumberBranches(int branches) {
  std::vector<int> branchGridPositions;

  int jPos = 0;
  // If client uses this method we assume 1 unit height for each branch with 1 unit separating each one
  for (int i = 0; i < branches; ++i) {
    branchGridPositions.push_back(jPos);
    jPos = jPos + 2;
  }

  setBranchGridPositions(branchGridPositions);
}

int SupplySplitterItem::numberBranches() {
  return m_baselineBranchPositions.size();
}

MixerItem::MixerItem(QGraphicsItem* parent) : GridItem(parent), m_numberBranches(1) {}

void MixerItem::setNumberBranches(int branches) {
  m_numberBranches = branches;
  m_vLength = (branches * 2) - 1;
}

int MixerItem::numberBranches() {
  return m_numberBranches;
}

void MixerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));

  painter->drawLine(50, 50, 50, (((m_numberBranches * 2) - 1) * 100) - 50);

  int midpointIndex = m_numberBranches - 1;
  if (m_numberBranches == 1) {
    const QPixmap* qPixmap = IconLibrary::Instance().findIcon(modelObject()->iddObject().type().value());
    painter->drawPixmap(12, (midpointIndex * 100) + 12, 75, 75, *qPixmap);
  }
  painter->drawLine(0, (midpointIndex * 100) + 50, 50, (midpointIndex * 100) + 50);

  int j = 50;
  for (int branchIndex = 1; branchIndex <= m_numberBranches; branchIndex++) {
    painter->drawLine(50, j, 100, j);
    j = j + 200;
  }
}

SupplyMixerItem::SupplyMixerItem(QGraphicsItem* parent) : GridItem(parent), m_numberBranches(1) {}

void SupplyMixerItem::setNumberBranches(int branches) {
  m_numberBranches = branches;
  m_vLength = (branches * 2) - 1;
}

int SupplyMixerItem::numberBranches() {
  return m_numberBranches;
}

void SupplyMixerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));

  painter->drawLine(50, 50, 50, (((m_numberBranches * 2) - 1) * 100) - 50);

  int midpointIndex = m_numberBranches - 1;
  if (m_numberBranches == 1) {
    QPixmap qPixmap(":/images/supply_mixer.png");
    painter->drawPixmap(12, (midpointIndex * 100) + 12, 75, 75, qPixmap);
  }
  painter->drawLine(50, (midpointIndex * 100) + 50, 100, (midpointIndex * 100) + 50);

  int j = 50;
  for (int branchIndex = 1; branchIndex <= m_numberBranches; branchIndex++) {
    painter->drawLine(0, j, 50, j);
    j = j + 200;
  }
}

TwoThreeStraightItem2::TwoThreeStraightItem2(QGraphicsItem* parent) : GridItem(parent) {}

void TwoThreeStraightItem2::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));

  painter->drawLine(0, 75, 50, 75);
  painter->drawLine(50, 0, 50, 75);
}

DualDuctTee::DualDuctTee(QGraphicsItem* parent) : GridItem(parent) {
  setHGridLength(2);
}

void DualDuctTee::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));

  painter->drawLine(0, 25, 50, 25);
  painter->drawLine(50, 0, 50, 25);
  painter->drawLine(0, 75, m_hLength * 100, 75);
}

DemandSideItem::DemandSideItem(QGraphicsItem* parent, const std::vector<model::Node>& demandInletNodes, const model::Node& demandOutletNode)
  : GridItem(parent),
    m_demandInletNodes(demandInletNodes),
    m_demandOutletNode(demandOutletNode),
    m_inletBranch(nullptr),
    m_outletBranch(nullptr),
    m_zoneBranches(nullptr),
    m_splitterItem(nullptr),
    m_mixerItem(nullptr),
    m_rightElbow(nullptr),
    m_rightElbow2(nullptr),
    m_leftElbow(nullptr),
    m_leftVertical(nullptr),
    m_rightVertical(nullptr),
    m_rightVertical2(nullptr),
    m_inletNode(nullptr),
    m_inletNode2(nullptr),
    m_outletNode(nullptr),
    m_padding(0),
    m_inletSpacer(nullptr),
    m_outletSpacer(nullptr) {
  model::Loop loop = m_demandInletNodes[0].loop().get();
  model::Mixer mixer = loop.demandMixer();
  model::Splitter splitter = loop.demandSplitter();

  // Do we have a dual duct system
  auto dualDuct = false;
  std::vector<SplitterItem::TerminalType> terminalTypes;
  if (m_demandInletNodes.size() == 2u) {
    dualDuct = true;
    if (auto airLoop = loop.optionalCast<model::AirLoopHVAC>()) {
      std::vector<model::AirLoopHVACZoneSplitter> splitters = airLoop->zoneSplitters();
      OS_ASSERT(splitters.size() == 2u);

      //auto zones = airLoop->thermalZones();
      auto comps = centerHVACComponents(splitter, mixer);
      for (const auto& comp : comps) {
        // What terminal types do we have
        // Could be a mix of single and dual duct terminals
        // See if zone is on the m_demandInletNodes[0] path
        bool singleDuct1Terminal = !airLoop->demandComponents(splitters[0], comp).empty();
        bool singleDuct2Terminal = !airLoop->demandComponents(splitters[1], comp).empty();

        auto terminalType = SplitterItem::None;
        if (singleDuct1Terminal && singleDuct2Terminal) {
          terminalType = SplitterItem::DualDuct;
        } else if (singleDuct1Terminal) {
          terminalType = SplitterItem::SingleDuct1;
        } else if (singleDuct2Terminal) {
          terminalType = SplitterItem::SingleDuct2;
        }
        terminalTypes.push_back(terminalType);
      }
    }
  }

  m_zoneBranches = new HorizontalBranchGroupItem(splitter, mixer, this);
  m_zoneBranches->setShowDropZone(true);

  // We only show one splitter
  // even though a dual duct will have two
  // Instead of two splitters we make a "double" line splitter
  m_splitterItem = new SplitterItem(this);
  m_splitterItem->setModelObject(splitter);
  m_splitterItem->setNumberBranches(m_zoneBranches->numberOfBranches());
  if (dualDuct) {
    m_splitterItem->setTerminalTypes(terminalTypes);
  }

  m_mixerItem = new MixerItem(this);
  m_mixerItem->setModelObject(mixer);
  m_mixerItem->setNumberBranches(m_zoneBranches->numberOfBranches());

  auto inletComponents = loop.demandComponents(m_demandInletNodes[0], splitter);
  inletComponents.erase(inletComponents.begin());
  inletComponents.pop_back();
  std::vector<model::ModelObject> rInletComponents;
  for (auto rit = inletComponents.rbegin(); rit < inletComponents.rend(); ++rit) {
    rInletComponents.push_back(*rit);
  }
  m_inletBranch = new HorizontalBranchItem(rInletComponents, this, dualDuct);

  auto outletComponents = loop.demandComponents(mixer, m_demandOutletNode);
  outletComponents.erase(outletComponents.begin());
  outletComponents.pop_back();
  std::vector<model::ModelObject> rOutletComponents;
  for (auto rit = outletComponents.rbegin(); rit < outletComponents.rend(); ++rit) {
    rOutletComponents.push_back(*rit);
  }
  m_outletBranch = new HorizontalBranchItem(rOutletComponents, this);

  m_leftElbow = new OneTwoStraightItem(this);
  std::vector<model::ModelObject> vertComps;
  m_leftVertical = new VerticalBranchItem(vertComps, this);
  m_rightVertical = new VerticalBranchItem(vertComps, this);
  m_outletNode = new TwoFourNodeItem(this);
  m_outletNode->setModelObject(demandOutletNode);
  m_inletNode = new TwoFourNodeItem(this);
  m_inletNode->setModelObject(demandInletNodes[0]);

  if (dualDuct) {
    m_rightElbow2 = new DualDuctTee(this);
    m_rightVertical2 = new VerticalBranchItem(vertComps, this);
    m_inletNode2 = new TwoFourNodeItem(this);
    m_inletNode2->setModelObject(demandInletNodes[1]);
    m_rightElbow = new TwoThreeStraightItem2(this);
    dualDuct = true;
  } else {
    m_rightElbow = new TwoThreeStraightItem(this);
  }

  if (rInletComponents.empty()) {
    m_inletSpacer = new OneThreeStraightItem(this, dualDuct);
  }

  if (rOutletComponents.empty()) {
    m_outletSpacer = new OneThreeStraightItem(this);
  }

  layout();
}

void DemandSideItem::layout() {
  int midpoint = 0;

  if (m_zoneBranches) {
    midpoint = (m_zoneBranches->getVGridLength() / 2);
  }

  m_outletNode->setGridPos(0, 0);

  m_leftVertical->setPadding(midpoint);
  m_leftVertical->setGridPos(0, 1);
  int i = 0;

  m_leftElbow->setGridPos(i, midpoint + 1);
  i = i + 1;

  if (m_outletSpacer) {
    m_outletSpacer->setGridPos(i, midpoint + 1);
    i = i + m_outletSpacer->getHGridLength();
  }

  if (m_padding % 2 == 0) {
    m_outletBranch->setPadding(m_padding / 2);
  } else {
    m_outletBranch->setPadding((m_padding / 2) + 1);
  }

  m_outletBranch->setGridPos(i, midpoint + 1);
  i = i + m_outletBranch->getHGridLength();

  m_mixerItem->setGridPos(i, 1);
  i = i + 1;

  m_zoneBranches->show();
  m_zoneBranches->setGridPos(i, 1);
  i = i + m_zoneBranches->getHGridLength();

  m_splitterItem->setGridPos(i, 1);
  i = i + 1;

  m_inletBranch->setPadding(m_padding / 2);
  m_inletBranch->setGridPos(i, midpoint + 1);
  i = i + m_inletBranch->getHGridLength();

  if (m_inletSpacer) {
    m_inletSpacer->setGridPos(i, midpoint + 1);
    i = i + m_inletSpacer->getHGridLength();
  }

  // dual duct requires extra stuff
  if (m_inletNode2) {
    m_rightElbow2->setGridPos(i, midpoint + 1);
    m_rightVertical2->setPadding(midpoint);
    m_rightVertical2->setGridPos(i, 1);
    m_inletNode2->setGridPos(i, 0);

    i = i + 2;
    m_rightElbow->setGridPos(i, midpoint + 1);
    m_rightVertical->setPadding(midpoint);
    m_rightVertical->setGridPos(i, 1);
    m_inletNode->setGridPos(i, 0);
  } else {
    m_rightElbow->setGridPos(i, midpoint + 1);
    m_rightVertical->setPadding(midpoint);
    m_rightVertical->setGridPos(i, 1);
    m_inletNode->setGridPos(i, 0);
  }

  setHGridLength(i + 1);
  setVGridLength(m_zoneBranches->getVGridLength() + 1);
}

int DemandSideItem::numberBranches() {
  return m_mixerItem->numberBranches();
}

void DemandSideItem::setPadding(unsigned padding) {
  m_padding = padding;
  layout();
}

unsigned DemandSideItem::padding() {
  return m_padding;
}

void DemandSideItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

SupplySideItem::SupplySideItem(QGraphicsItem* parent, model::Node& supplyInletNode, std::vector<model::Node>& supplyOutletNodes)
  : GridItem(parent),
    m_supplyInletNode(supplyInletNode),
    m_supplyOutletNodes(supplyOutletNodes),
    m_outletBranchItem(nullptr),
    m_inletBranchItem(nullptr),
    m_mainBranchGroupItem(nullptr),
    m_inletNodeItem(nullptr),
    m_outletNodeItem(nullptr),
    m_outletNodeItem2(nullptr),
    m_leftElbowItem(nullptr),
    m_leftVerticalItem(nullptr),
    m_rightElbowItem(nullptr),
    m_rightElbowItem2(nullptr),
    m_rightVerticalItem(nullptr),
    m_rightVerticalItem2(nullptr),
    m_splitterItem(nullptr),
    m_mixerItem(nullptr),
    m_padding(0),
    m_inletSpacer(nullptr),
    m_outletSpacer(nullptr),
    m_dualDuctHorizontalSpace(nullptr) {
  auto _supplyOutletNode = m_supplyOutletNodes.front();
  auto loop = m_supplyInletNode.loop().get();

  auto plantLoop = loop.optionalCast<model::PlantLoop>();
  auto airLoop = loop.optionalCast<model::AirLoopHVAC>();

  // boost::optional<model::AirLoopHVACOutdoorAirSystem> oaSystem;
  boost::optional<model::Splitter> splitter;
  boost::optional<model::Mixer> mixer;

  if (airLoop) {
    // oaSystem = airLoop->airLoopHVACOutdoorAirSystem();
    splitter = airLoop->supplySplitter();
  }

  if (plantLoop) {
    mixer = plantLoop->supplyMixer();
    splitter = plantLoop->supplySplitter();
  }

  m_inletNodeItem = new TwoFourNodeItem(this);
  m_inletNodeItem->setModelObject(m_supplyInletNode);

  m_outletNodeItem = new TwoFourNodeItem(this);
  m_outletNodeItem->setModelObject(m_supplyOutletNodes[0]);

  if (splitter && (!mixer)) {
    OS_ASSERT(m_supplyOutletNodes.size() == 2u);

    {
      auto comps = airLoop->supplyComponents(m_supplyInletNode, splitter.get());
      comps.pop_back();
      m_inletBranchItem = new HorizontalBranchItem(comps, this);
    }

    m_splitterItem = new SupplySplitterItem(this);
    m_splitterItem->setModelObject(splitter.get());

    {
      m_mainBranchGroupItem = new HorizontalBranchGroupItem(splitter.get(), m_supplyOutletNodes, this);
      auto branchBaselineGridPositions = m_mainBranchGroupItem->branchBaselineGridPositions();
      OS_ASSERT(branchBaselineGridPositions.size() == 2u);

      std::vector<int> splitterPositions;
      splitterPositions.reserve(branchBaselineGridPositions.size());
      auto firstBranchBaselineGridPosition = branchBaselineGridPositions.front();
      for (const auto& branchPos : branchBaselineGridPositions) {
        splitterPositions.emplace_back(branchPos - firstBranchBaselineGridPosition);
      }

      m_splitterItem->setBranchGridPositions(splitterPositions);
    }

    {
      std::vector<model::ModelObject> comps;
      m_rightElbowItem2 = new ThreeFourStraightItem(this);
      m_rightVerticalItem2 = new VerticalBranchItem(comps, this);
      m_dualDuctHorizontalSpace = new HorizontalBranchItem(comps, this);
      m_outletNodeItem2 = new TwoFourNodeItem(this);
      m_outletNodeItem2->setModelObject(m_supplyOutletNodes[1]);
    }
  } else if (splitter && mixer) {
    m_splitterItem = new SupplySplitterItem(this);
    m_splitterItem->setModelObject(splitter.get());

    m_mixerItem = new SupplyMixerItem(this);
    m_mixerItem->setModelObject(mixer.get());

    m_mainBranchGroupItem = new HorizontalBranchGroupItem(splitter.get(), mixer.get(), this);
    m_mainBranchGroupItem->setShowDropZone(true);

    m_mixerItem->setNumberBranches(m_mainBranchGroupItem->numberOfBranches());
    m_splitterItem->setNumberBranches(m_mainBranchGroupItem->numberOfBranches());

    auto inletComponents = loop.supplyComponents(m_supplyInletNode, splitter.get());
    inletComponents.erase(inletComponents.begin());
    inletComponents.pop_back();
    m_inletBranchItem = new HorizontalBranchItem(inletComponents, this);

    auto outletComponents = loop.supplyComponents(mixer.get(), _supplyOutletNode);
    outletComponents.erase(outletComponents.begin());
    outletComponents.pop_back();
    m_outletBranchItem = new HorizontalBranchItem(outletComponents, this);

    if (inletComponents.empty()) {
      m_inletSpacer = new OneThreeStraightItem(this);
    }

    if (outletComponents.empty()) {
      m_outletSpacer = new OneThreeStraightItem(this);
    }
  } else {
    auto inletComponents = loop.supplyComponents(m_supplyInletNode, _supplyOutletNode);
    // If there isn't at least two components (the inlet and the outlet node we passed as argument),
    // then something went CLEARLY wrong!
    OS_ASSERT(inletComponents.size() >= 2u);
    inletComponents.erase(inletComponents.begin());
    inletComponents.pop_back();
    m_inletBranchItem = new HorizontalBranchItem(inletComponents, this);

    std::vector<model::ModelObject> comps;
    m_outletBranchItem = new HorizontalBranchItem(comps, this);
  }

  m_rightElbowItem = new ThreeFourStraightItem(this);
  m_leftElbowItem = new OneFourStraightItem(this);

  std::vector<model::ModelObject> vertComps;
  m_leftVerticalItem = new VerticalBranchItem(vertComps, this);
  m_rightVerticalItem = new VerticalBranchItem(vertComps, this);

  layout();
}

int SupplySideItem::numberBranches() {
  if (m_splitterItem) {
    return m_mixerItem->numberBranches();
  } else {
    return 1;
  }
}

void SupplySideItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}

void SupplySideItem::layout() {
  int i = 0;
  int j = 0;  // Represents the centerline

  // Figure out the maximum vertical height of the flow path items
  int halfBranchGroupHeight = 0;

  if (m_mainBranchGroupItem) {
    halfBranchGroupHeight = m_mainBranchGroupItem->getVGridLength() / 2;
    auto branchPositions = m_mainBranchGroupItem->branchBaselineGridPositions();
    if (branchPositions.size() > 1) {
      // If we have explicit branch positions, then find the midpoint of those
      // instead of the simple vGridLength / 2
      halfBranchGroupHeight = (branchPositions.back() - branchPositions.front() + 1) / 2.0;
    }
    j = m_mainBranchGroupItem->getVGridLength() - halfBranchGroupHeight - 1;
  }

  if (m_inletBranchItem) {
    auto height = m_inletBranchItem->getVGridLength();
    if (height > j) {
      j = height - 1;
    }
  }

  if (m_outletBranchItem) {
    auto height = m_outletBranchItem->getVGridLength();
    if (height > j) {
      j = height - 1;
    }
  }

  // Now put things in position
  m_inletNodeItem->setGridPos(i, j + halfBranchGroupHeight + 1);

  if (halfBranchGroupHeight > 0) {
    m_leftVerticalItem->setGridPos(i, j + 1);
    m_leftVerticalItem->setPadding(halfBranchGroupHeight);
    m_leftVerticalItem->show();
  } else {
    m_leftVerticalItem->hide();
  }

  m_leftElbowItem->setGridPos(i, j);
  i++;

  if (m_inletBranchItem) {
    m_inletBranchItem->setGridPos(i, (j - m_inletBranchItem->getVGridLength() + 1));

    // if dual duct put all of the padding on the inlet branch
    if (m_outletNodeItem2) {
      m_inletBranchItem->setPadding(m_padding);
    } else {
      if (m_padding % 2 == 0) {
        m_inletBranchItem->setPadding(m_padding / 2);
      } else {
        m_inletBranchItem->setPadding((m_padding / 2) + 1);
      }
    }

    i = i + m_inletBranchItem->getHGridLength();
  }

  if (m_inletSpacer) {
    m_inletSpacer->setGridPos(i, j);
    i = i + m_inletSpacer->getHGridLength();
  }

  if (m_splitterItem) {
    m_splitterItem->setGridPos(i, j - (m_splitterItem->getVGridLength() - 1) / 2);
    i++;
  }

  if (m_mainBranchGroupItem) {
    m_mainBranchGroupItem->setGridPos(i, j + 1 - (m_mainBranchGroupItem->getVGridLength() - halfBranchGroupHeight));
    i = i + m_mainBranchGroupItem->getHGridLength();
  }

  if (m_mixerItem) {
    m_mixerItem->setGridPos(i, j - halfBranchGroupHeight);
    i++;
  }

  if (m_outletSpacer) {
    m_outletSpacer->setGridPos(i, j);
    i = i + m_outletSpacer->getHGridLength();
  }

  if (m_outletBranchItem) {
    m_outletBranchItem->setGridPos(i, (j - m_outletBranchItem->getVGridLength() + 1));

    if (m_inletBranchItem) {
      m_outletBranchItem->setPadding(m_padding / 2);
    } else {
      m_outletBranchItem->setPadding(m_padding);
    }

    i = i + m_outletBranchItem->getHGridLength();
  }

  // dual ducts have extra have an extra elbow and vertical item
  if (m_outletNodeItem2) {
    OS_ASSERT(m_mainBranchGroupItem);

    int topedge = m_mainBranchGroupItem->getYGridPos() + m_mainBranchGroupItem->branchBaselineGridPositions().front();
    m_dualDuctHorizontalSpace->setPadding(2);
    m_dualDuctHorizontalSpace->setGridPos(i, topedge);
    m_rightElbowItem->setGridPos(i + 2, topedge);
    m_rightVerticalItem->setGridPos(i + 2, topedge + 1);
    m_rightVerticalItem->setPadding(m_mainBranchGroupItem->getVGridLength());
    m_outletNodeItem->setGridPos(i + 2, j + halfBranchGroupHeight + 1);

    m_outletNodeItem2->setGridPos(i, j + halfBranchGroupHeight + 1);
    m_rightElbowItem2->setGridPos(i, j + halfBranchGroupHeight);

    i = i + 2;
  } else {
    m_rightElbowItem->setGridPos(i, j);
    if (halfBranchGroupHeight > 0) {
      m_rightVerticalItem->setGridPos(i, j + 1);
      m_rightVerticalItem->setPadding(halfBranchGroupHeight);
      m_rightVerticalItem->show();
    } else {
      m_rightVerticalItem->hide();
    }
    m_outletNodeItem->setGridPos(i, j + halfBranchGroupHeight + 1);
  }

  setHGridLength(i + 1);
  setVGridLength(j + halfBranchGroupHeight + 2);
}

void SupplySideItem::setPadding(unsigned padding) {
  m_padding = padding;
  layout();
}

unsigned SupplySideItem::padding() {
  return m_padding;
}

TwoThreeFourStraightItem::TwoThreeFourStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void TwoThreeFourStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));

  painter->drawLine(50, 0, 50, 100);
  painter->drawLine(0, 50, 50, 50);
}

OneTwoFourStraightItem::OneTwoFourStraightItem(QGraphicsItem* parent) : GridItem(parent) {}

void OneTwoFourStraightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  GridItem::paint(painter, option, widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
  painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));

  painter->drawLine(50, 0, 50, 100);
  painter->drawLine(50, 50, 100, 50);
}

NodeContextButtonItem::NodeContextButtonItem(GridItem* parent)
  : ButtonItem(QPixmap(":/images/contextual_arrow.png"), QPixmap(":/images/contextual_arrow.png"), QPixmap(":/images/contextual_arrow.png"), parent) {
  connect(this, &NodeContextButtonItem::mouseClicked, this, &NodeContextButtonItem::showContextMenu);

  connect(this, &NodeContextButtonItem::removeModelObjectClicked, parent, &GridItem::removeModelObjectClicked);
}

void NodeContextButtonItem::showContextMenu() {
  if (scene() != nullptr && !scene()->views().empty() && scene()->views().first() != nullptr && scene()->views().first()->viewport() != nullptr) {

    QGraphicsView* v = scene()->views().first();
    QPointF sceneP = mapToScene(boundingRect().bottomRight());
    QPoint viewP = v->mapFromScene(sceneP);
    QPoint menuPos = v->viewport()->mapToGlobal(viewP);

    QMenu menu;
    QAction removeSPMAction(QIcon(":/images/delete-icon.png"), "Delete Setpoint Manager", &menu);
    menu.addAction(&removeSPMAction);
    connect(&removeSPMAction, &QAction::triggered, this, &NodeContextButtonItem::onRemoveSPMActionTriggered, Qt::QueuedConnection);

    menu.exec(menuPos);
  }
}

void NodeContextButtonItem::onRemoveSPMActionTriggered() {
  auto* gridItem = qobject_cast<GridItem*>(parentObject());
  if (gridItem != nullptr && gridItem->modelObject() && gridItem->modelObject()->optionalCast<model::Node>()) {
    auto node = gridItem->modelObject()->cast<model::Node>();

    for (auto& _setpointManager : node.setpointManagers()) {
      // TODO: this duplicateBranch condition is strange
      // if (istringEqual("Temperature", it->controlVariable())) {
      emit removeModelObjectClicked(_setpointManager);
      break;
      // } else {
      //  emit removeModelObjectClicked(*it);
      //  break;
      // }
    }
  }
}

}  // namespace openstudio
