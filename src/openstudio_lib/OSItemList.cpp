/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSItemList.hpp"
#include "OSVectorController.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

namespace openstudio {

OSItemList::OSItemList(OSVectorController* vectorController, bool addScrollArea, QWidget* parent)
  : OSItemSelector(parent),
    m_vectorController(vectorController),
    m_vLayout(nullptr),
    m_selectedItem(nullptr),
    m_itemsDraggable(false),
    m_itemsRemoveable(false),
    m_type(OSItemType::ListItem) {
  // for now we will allow this item list to manage memory of
  OS_ASSERT(!m_vectorController->parent());
  m_vectorController->setParent(this);

  this->setObjectName("GrayWidget");

  QString style;

  style.append("QWidget#GrayWidget {");
  style.append(" background: #E6E6E6;");
  style.append(" border-bottom: 1px solid black;");
  style.append("}");

  setStyleSheet(style);

  auto* outerVLayout = new QVBoxLayout();
  outerVLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(outerVLayout);

  auto* outerWidget = new QWidget();

  if (addScrollArea) {
    auto* scrollArea = new QScrollArea();
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    outerVLayout->addWidget(scrollArea);
    scrollArea->setWidget(outerWidget);
    scrollArea->setWidgetResizable(true);
  } else {
    outerVLayout->addWidget(outerWidget);
  }

  m_vLayout = new QVBoxLayout();
  outerWidget->setLayout(m_vLayout);
  m_vLayout->setContentsMargins(0, 0, 0, 0);
  m_vLayout->setSpacing(0);
  m_vLayout->addStretch();

  connect(this, &OSItemList::itemsRequested, vectorController, &OSVectorController::reportItems);

  /* Vector controller does not handle removing items in list from model
  *
  connect(this, &OSItemList::itemRemoveClicked, vectorController, &OSVectorController::removeItem);
  */

  connect(vectorController, &OSVectorController::itemIds, this, &OSItemList::setItemIds);

  connect(vectorController, &OSVectorController::selectedItemId, this, &OSItemList::selectItemId);

  // allow time for OSDocument to finish constructing
  QTimer::singleShot(0, vectorController, &OSVectorController::reportItems);
}

OSItem* OSItemList::selectedItem() const {
  return m_selectedItem;
}

OSItem* OSItemList::firstItem() {
  std::vector<OSItem*> its = this->items();
  if (!its.empty()) {
    return its.front();
  }
  return nullptr;
}

OSItem* OSItemList::lastItem() {
  std::vector<OSItem*> its = this->items();
  if (!its.empty()) {
    return its.back();
  }
  return nullptr;
}

std::vector<OSItem*> OSItemList::items() {
  std::vector<OSItem*> result;

  for (int i = 0; i < m_vLayout->count(); ++i) {
    QLayoutItem* layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();
    auto* item = qobject_cast<OSItem*>(widget);

    if (item) {
      result.push_back(item);
    }
  }

  return result;
}

OSVectorController* OSItemList::vectorController() const {
  return m_vectorController;
}

bool OSItemList::itemsDraggable() const {
  return m_itemsDraggable;
}

void OSItemList::setItemsDraggable(bool itemsDraggable) {
  m_itemsDraggable = itemsDraggable;
  for (OSItem* item : this->items()) {
    item->setDraggable(itemsDraggable);
  }
}

bool OSItemList::itemsRemoveable() const {
  return m_itemsRemoveable;
}

void OSItemList::setItemsRemoveable(bool itemsRemoveable) {
  m_itemsRemoveable = itemsRemoveable;
  for (OSItem* item : this->items()) {
    item->setRemoveable(itemsRemoveable);
  }
}

void OSItemList::setItemIds(const std::vector<OSItemId>& itemIds) {
  /* DLM: ScriptsListView was removed
  if (qobject_cast<ScriptsListView*>(this)) {
    std::vector<OSItem*> myItems = items();
    size_t n = myItems.size();
    if ((n > 0) && (itemIds.size() == n)) {
      bool doNothing = true;
      for (size_t i = 0; i < n; ++i) {
        if (std::find(itemIds.begin(),itemIds.end(),myItems[i]->itemId()) == itemIds.end()) {
          doNothing = false;
          break;
        }
      }
      if (doNothing) {
        return;
      }
    }
  }
  */

  QLayoutItem* child = nullptr;
  while ((child = m_vLayout->takeAt(0)) != nullptr) {
    QWidget* widget = child->widget();
    delete widget;
    delete child;
  }
  m_vLayout->addStretch();

  m_selectedItem = nullptr;

  for (const OSItemId& itemId : itemIds) {
    OSItem* item = OSItem::makeItem(itemId, OSItemType::ListItem);
    if (item) {
      addItem(item, false);
    }
  }
  selectItem(firstItem());
}

void OSItemList::addItem(OSItem* item, bool selectItem) {
  OS_ASSERT(item);

  item->setDraggable(m_itemsDraggable);

  item->setRemoveable(m_itemsRemoveable);

  item->setOSItemType(m_type);

  connect(item, &OSItem::itemClicked, this, &OSItemList::selectItem);

  connect(item, &OSItem::itemRemoveClicked, this, &OSItemList::itemRemoveClicked);

  connect(item, &OSItem::itemReplacementDropped, this, &OSItemList::itemReplacementDropped);

  m_vLayout->insertWidget(0, item);

  if (selectItem) {
    this->selectItem(item);
  }
}

void OSItemList::selectItem(OSItem* selectItem) {
  if (!selectItem) {
    if (m_selectedItem) {
      // deselect
      m_selectedItem->setSelected(false);
    }
    m_selectedItem = nullptr;
    emit selectionCleared();
    return;
  }

  for (int i = 0; i < m_vLayout->count(); ++i) {
    QLayoutItem* layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();
    auto* item = qobject_cast<OSItem*>(widget);

    if (item) {
      if (selectItem->equal(item)) {

        if (m_selectedItem) {
          if (m_selectedItem->equal(item)) {
            // already selected
            return;
          } else {
            // deselect
            m_selectedItem->setSelected(false);
          }
        }

        m_selectedItem = item;
        m_selectedItem->setSelected(true);
        emit itemSelected(m_selectedItem);
        return;
      }
    }
  }

  // selected object was not found
  if (m_selectedItem) {
    // deselect
    m_selectedItem->setSelected(false);
  }
  m_selectedItem = nullptr;
  emit selectionCleared();
}

void OSItemList::selectItemId(const OSItemId& itemId) {
  for (int i = 0; i < m_vLayout->count(); ++i) {
    QLayoutItem* layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();
    auto* item = qobject_cast<OSItem*>(widget);

    if (item) {
      if (item->itemId() == itemId) {
        selectItem(item);
        break;
      }
    }
  }
}

void OSItemList::clearSelection() {
  m_selectedItem = nullptr;

  for (int i = 0; i < m_vLayout->count(); ++i) {
    QLayoutItem* layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();
    auto* item = qobject_cast<OSItem*>(widget);
    if (item) {
      item->setSelected(false);
    }
  }
}

void OSItemList::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

OSItemType OSItemList::itemsType() const {
  return m_type;
}

void OSItemList::setItemsType(OSItemType type) {
  m_type = type;
  for (OSItem* item : this->items()) {
    item->setOSItemType(type);
  }
}

}  // namespace openstudio
