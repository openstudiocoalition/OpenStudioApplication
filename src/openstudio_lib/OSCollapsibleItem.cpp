/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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
