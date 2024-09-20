/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSCollapsibleItem.hpp"

#include "OSCollapsibleItemHeader.hpp"
#include "OSItemList.hpp"
#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QComboBox>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QResizeEvent>
#include <QStyleOption>

#define FILTERS_ON "Filters On "
#define FILTERS_OFF "Filters Off "

namespace openstudio {

OSCollapsibleItem::OSCollapsibleItem(OSCollapsibleItemHeader* collapsibleItemHeader, OSItemList* itemList, QWidget* parent)
  : QWidget(parent), m_collapsibleItemHeader(collapsibleItemHeader), m_itemList(itemList), m_mainLayout(nullptr) {
  OS_ASSERT(m_collapsibleItemHeader);
  OS_ASSERT(m_itemList);

  //m_collapsibleItemHeader->setDraggable(false);
  //m_collapsibleItemHeader->setRemoveable(false);

  setObjectName("OSCollapsibleItem");

  m_mainLayout = new QVBoxLayout();
  m_mainLayout->setSpacing(0);
  m_mainLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(m_mainLayout);

  // collapsible header
  m_mainLayout->addWidget(m_collapsibleItemHeader);

  connect(collapsibleItemHeader, &OSCollapsibleItemHeader::clicked, this, &OSCollapsibleItem::onHeaderClicked);

  // item list
  m_mainLayout->addWidget(m_itemList);

  connect(itemList, &OSItemList::itemSelected, this, &OSCollapsibleItem::itemSelected);

  connect(itemList, &OSItemList::itemRemoveClicked, this, &OSCollapsibleItem::itemRemoveClicked);

  connect(itemList, &OSItemList::itemReplacementDropped, this, &OSCollapsibleItem::itemReplacementDropped);

  connect(itemList, &OSItemList::selectionCleared, this, &OSCollapsibleItem::selectionCleared);

  m_mainLayout->addStretch();

  setExpanded(false);
}

OSCollapsibleItemHeader* OSCollapsibleItem::collapsibleItemHeader() const {
  return m_collapsibleItemHeader;
}

OSItemList* OSCollapsibleItem::itemList() const {
  return m_itemList;
}

bool OSCollapsibleItem::itemsDraggable() const {
  return m_itemList->itemsDraggable();
}

void OSCollapsibleItem::setItemsDraggable(bool itemsDraggable) {
  m_itemList->setItemsDraggable(itemsDraggable);
}

bool OSCollapsibleItem::itemsRemoveable() const {
  return m_itemList->itemsRemoveable();
}

void OSCollapsibleItem::setItemsRemoveable(bool itemsRemoveable) {
  m_itemList->setItemsRemoveable(itemsRemoveable);
}

OSItemType OSCollapsibleItem::itemsType() const {
  return m_itemList->itemsType();
}

void OSCollapsibleItem::setItemsType(OSItemType type) {
  m_itemList->setItemsType(type);
}

bool OSCollapsibleItem::isSelected() const {
  return this->m_collapsibleItemHeader->selected();
}

void OSCollapsibleItem::setSelected(bool selected) {
  this->m_collapsibleItemHeader->setSelected(selected);
  if (selected) {
    emit collapsableItemSelected(this);
  }
}

bool OSCollapsibleItem::expanded() const {
  return this->m_collapsibleItemHeader->expanded();
}

void OSCollapsibleItem::setExpanded(bool expanded) {
  this->m_collapsibleItemHeader->setExpanded(expanded);
  this->m_itemList->setVisible(expanded);
}

void OSCollapsibleItem::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OSCollapsibleItem::onHeaderClicked(OSCollapsibleItemHeader* header) {
  setExpanded(!expanded());
  setSelected(true);
}

}  // namespace openstudio
