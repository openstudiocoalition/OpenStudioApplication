/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSCollapsibleItemList.hpp"
#include "OSItem.hpp"
#include "OSItemList.hpp"
#include "OSCollapsibleItem.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "OSCategoryPlaceholder.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStringBuilder>
#include <QStyleOption>
#include <QVBoxLayout>

namespace openstudio {

OSCollapsibleItemList::OSCollapsibleItemList(bool addScrollArea, QWidget* parent)
  : OSItemSelector(parent),
    m_vLayout(nullptr),
    m_contentLayout(nullptr),
    m_selectedCollapsibleItem(nullptr),
    m_searchActive(false),
    m_itemsDraggable(false),
    m_itemsRemoveable(false),
    m_itemsType(OSItemType::ListItem) {
  this->setObjectName("GrayWidget");

  auto* outerVLayout = new QVBoxLayout();
  outerVLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(outerVLayout);

  m_searchBox = new QLineEdit();
  m_searchBox->setClearButtonEnabled(true);
  outerVLayout->addWidget(m_searchBox);
  connect(m_searchBox, &QLineEdit::textEdited, this, &OSCollapsibleItemList::onSearchTextEdited);

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
  m_vLayout->addStretch(10);
}

OSCollapsibleItem* OSCollapsibleItemList::selectedCollapsibleItem() const {
  return m_selectedCollapsibleItem;
}

OSItem* OSCollapsibleItemList::selectedItem() const {
  OSItem* result = nullptr;
  if (m_selectedCollapsibleItem) {
    result = m_selectedCollapsibleItem->itemList()->selectedItem();
  }
  return result;
}

bool OSCollapsibleItemList::itemsDraggable() const {
  return m_itemsDraggable;
}

void OSCollapsibleItemList::setItemsDraggable(bool itemsDraggable) {
  m_itemsDraggable = itemsDraggable;
  for (OSCollapsibleItem* collapsibleItem : m_collapsibleItems) {
    collapsibleItem->setItemsDraggable(itemsDraggable);
  }
}

bool OSCollapsibleItemList::itemsRemoveable() const {
  return m_itemsRemoveable;
}

void OSCollapsibleItemList::setItemsRemoveable(bool itemsRemoveable) {
  m_itemsRemoveable = itemsRemoveable;
  for (OSCollapsibleItem* collapsibleItem : m_collapsibleItems) {
    collapsibleItem->setItemsRemoveable(itemsRemoveable);
  }
}

OSItemType OSCollapsibleItemList::itemsType() const {
  return m_itemsType;
}

void OSCollapsibleItemList::setItemsType(OSItemType type) {
  m_itemsType = type;
  for (OSCollapsibleItem* collapsibleItem : m_collapsibleItems) {
    collapsibleItem->setItemsType(type);
  }
}

void OSCollapsibleItemList::addCollapsibleItem(OSCollapsibleItem* collapsibleItem) {
  OS_ASSERT(collapsibleItem);

  collapsibleItem->setItemsDraggable(m_itemsDraggable);

  collapsibleItem->setItemsRemoveable(m_itemsRemoveable);

  collapsibleItem->setItemsType(m_itemsType);

  m_vLayout->insertWidget(0, collapsibleItem);

  QString style;
  style.append("QWidget#OSCollapsibleItemList {");
  style.append("background: #F2F2F2; ");
  style.append("border-bottom: 1px solid black; ");
  style.append("}");
  style.append("QWidget#SideBar {background: #EEDEDE;}");

  collapsibleItem->setStyleSheet(style);

  connect(collapsibleItem, &OSCollapsibleItem::collapsableItemSelected, this, &OSCollapsibleItemList::onCollapsableItemSelected);

  connect(collapsibleItem, &OSCollapsibleItem::itemSelected, this, &OSCollapsibleItemList::onItemSelected);

  connect(collapsibleItem, &OSCollapsibleItem::itemSelected, this, &OSCollapsibleItemList::itemSelected);

  connect(collapsibleItem, &OSCollapsibleItem::itemReplacementDropped, this, &OSCollapsibleItemList::itemReplacementDropped);

  connect(collapsibleItem, &OSCollapsibleItem::itemRemoveClicked, this, &OSCollapsibleItemList::itemRemoveClicked);

  connect(collapsibleItem, &OSCollapsibleItem::selectionCleared, this, &OSCollapsibleItemList::selectionCleared);

  connect(collapsibleItem, &OSCollapsibleItem::openLibDlgClicked, this, &OSCollapsibleItemList::openLibDlgClicked);
}

void OSCollapsibleItemList::addCategoryPlaceholderItem(OSCategoryPlaceholder* categoryPlaceholderItem) {

  m_placeholderItems.push_back(categoryPlaceholderItem);
  m_vLayout->insertWidget(0, categoryPlaceholderItem);
}

void OSCollapsibleItemList::onCollapsableItemSelected(OSCollapsibleItem* selectedItem) {
  QLayoutItem* layoutItem = nullptr;
  OSCollapsibleItem* collapsibleItem = nullptr;

  for (int i = 0; i < m_vLayout->count(); ++i) {

    layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();

    collapsibleItem = qobject_cast<OSCollapsibleItem*>(widget);
    if (collapsibleItem) {
      if (collapsibleItem == selectedItem) {

        if (m_selectedCollapsibleItem != collapsibleItem) {
          // no need to select collapsibleItem since it is already selected
          // expand collapsable item and select first item inside
          m_selectedCollapsibleItem = collapsibleItem;
          m_selectedCollapsibleItem->setExpanded(true);
          OSItem* newSelectedItem = m_selectedCollapsibleItem->itemList()->firstItem();
          m_selectedCollapsibleItem->itemList()->selectItem(newSelectedItem);
        }

      } else {
        // deselect other collapsable items
        collapsibleItem->setSelected(false);
        if (!m_searchActive) {
          collapsibleItem->setExpanded(false);
        }
        collapsibleItem->itemList()->clearSelection();
      }
    }
  }
}

void OSCollapsibleItemList::onItemSelected(OSItem* item) {
  QLayoutItem* layoutItem = nullptr;
  OSCollapsibleItem* collapsibleItem = nullptr;

  for (int i = 0; i < m_vLayout->count(); ++i) {

    layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();

    collapsibleItem = qobject_cast<OSCollapsibleItem*>(widget);
    if (collapsibleItem) {
      std::vector<OSItem*> items = collapsibleItem->itemList()->items();
      if (std::find(items.begin(), items.end(), item) != items.end()) {

        if (m_selectedCollapsibleItem != collapsibleItem) {
          // no need to select item since it is already selected
          // expand collapsable item and select first item inside
          m_selectedCollapsibleItem = collapsibleItem;
          m_selectedCollapsibleItem->setSelected(true);
          m_selectedCollapsibleItem->setExpanded(true);
        }

      } else {
        // deselect other collapsable items
        collapsibleItem->setSelected(false);
        if (!m_searchActive) {
          collapsibleItem->setExpanded(false);
        }
        //collapsibleItem->itemList()->clearSelection();
      }
    }
  }
}

void OSCollapsibleItemList::onSearchTextEdited(const QString& text) {
  m_searchActive = !text.isEmpty();

  OSItem* newSelectedItem = nullptr;
  OSCategoryPlaceholder* placeholderItem = nullptr;
  QString searchText, categoryText, placeholderText;
  for (int i = 0; i < m_vLayout->count(); ++i) {

    QLayoutItem* layoutItem = m_vLayout->itemAt(i);
    QWidget* widget = layoutItem->widget();

    // filter collapsible items
    OSCollapsibleItem* collapsibleItem = qobject_cast<OSCollapsibleItem*>(widget);
    if (collapsibleItem) {
      categoryText = collapsibleItem->collapsibleItemHeader()->text();
      std::vector<OSItem*> items = collapsibleItem->itemList()->items();
      unsigned numVisible = 0;
      for (const auto& item : items) {
        if (m_searchActive) {
          searchText = item->text() % " " % categoryText % " " % placeholderText;
          if (searchText.contains(text, Qt::CaseInsensitive)) {
            item->setVisible(true);
            if (!newSelectedItem) {
              newSelectedItem = item;
              collapsibleItem->itemList()->selectItem(newSelectedItem);
            }
            // show the last category placeholder item
            if (placeholderItem) {
              placeholderItem->setVisible(true);
            }
            ++numVisible;
          } else {
            item->setVisible(false);
          }
        } else {
          item->setVisible(true);
          ++numVisible;
        }
      }

      if (!m_searchActive) {
        collapsibleItem->setExpanded(collapsibleItem->isSelected());
        collapsibleItem->setVisible(true);
      } else {
        collapsibleItem->setExpanded(numVisible > 0);
        collapsibleItem->setVisible(numVisible > 0);
      }
    } else {

      // filter category placeholder items
      placeholderItem = qobject_cast<OSCategoryPlaceholder*>(widget);
      if (placeholderItem) {
        placeholderText = placeholderItem->text();
        if (m_searchActive) {
          if (placeholderText.contains(text, Qt::CaseInsensitive)) {
            placeholderItem->setVisible(true);
          } else {
            placeholderItem->setVisible(false);
          }
        } else {
          placeholderItem->setVisible(true);
        }
      }
    }
  }
}

void OSCollapsibleItemList::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
