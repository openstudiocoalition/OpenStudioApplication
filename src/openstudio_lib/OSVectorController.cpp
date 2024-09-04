/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSVectorController.hpp"

#include <QMutex>
#include <QTimer>

namespace openstudio {

OSVectorController::OSVectorController() : QObject(), m_reportScheduled(false), m_reportItemsMutex(new QMutex()) {}

OSVectorController::~OSVectorController() {
  delete m_reportItemsMutex;
}

void OSVectorController::reportItems() {
  m_reportItemsMutex->lock();

  if (!m_reportScheduled) {
    m_reportScheduled = true;
    QTimer::singleShot(0, this, &OSVectorController::reportItemsImpl);
  }

  m_reportItemsMutex->unlock();
}

void OSVectorController::reportItemsImpl() {
  m_reportItemsMutex->lock();

  if (m_reportScheduled) {
    m_reportScheduled = false;
    emit itemIds(this->makeVector());
  }

  m_reportItemsMutex->unlock();
}

void OSVectorController::removeItem(OSItem* item) {
  this->onRemoveItem(item);
}

void OSVectorController::replaceItem(OSItem* currentItem, const OSItemId& replacementItemId) {
  this->onReplaceItem(currentItem, replacementItemId);
}

void OSVectorController::drop(const OSItemId& itemId) {
  this->onDrop(itemId);
}

void OSVectorController::makeNewItem() {
  this->onMakeNewItem();
}

void OSVectorController::onRemoveItem(OSItem* item) {}

void OSVectorController::onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) {}

void OSVectorController::onDrop(const OSItemId& itemId) {}

void OSVectorController::onMakeNewItem() {}

}  // namespace openstudio
