/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SubTabView.hpp"
#include "OSItemSelector.hpp"
#include "OSItemSelectorButtons.hpp"
#include "ModelObjectListView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QButtonGroup>
#include <QResizeEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QRadioButton>

#include <iostream>

namespace openstudio {

SubTabView::SubTabView(OSItemSelector* itemSelector, OSInspectorView* inspectorView, bool showGridViewLayout, QWidget* parent)
  : QSplitter(parent), m_itemSelector(itemSelector), m_inspectorView(inspectorView) {
  this->setObjectName("GrayWidgetWithLeftTopBorders");
  connectItemSelector();
  connectInspectorView();
  connectItemSelectorButtons();
  if (showGridViewLayout) {
    createGridViewLayout();
  } else {
    createLayout();
  }
}

void SubTabView::connectItemSelector() {
  // Item Selector
  connect(m_itemSelector, &OSItemSelector::itemSelected, this, &SubTabView::itemSelected);

  connect(m_itemSelector, &OSItemSelector::itemRemoveClicked, this, &SubTabView::itemRemoveClicked);

  connect(m_itemSelector, &OSItemSelector::itemReplacementDropped, this, &SubTabView::itemReplacementDropped);

  connect(m_itemSelector, &OSItemSelector::selectionCleared, this, &SubTabView::selectionCleared);
}

void SubTabView::connectInspectorView() {
  // Inspector View
  connect(m_inspectorView, &OSInspectorView::dropZoneItemClicked, this, &SubTabView::dropZoneItemClicked);

  connect(this, &SubTabView::dropZoneItemClicked, this, &SubTabView::onDropZoneItemClicked);
}

void SubTabView::connectItemSelectorButtons() {
  // Item Selector Buttons
  m_itemSelectorButtons = new OSItemSelectorButtons();

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::itemDropped, this, &SubTabView::itemDropped);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::addClicked, this, &SubTabView::addClicked);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::copyClicked, this, &SubTabView::copyClicked);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::removeClicked, this, &SubTabView::removeClicked);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::purgeClicked, this, &SubTabView::purgeClicked);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::downloadComponentsClicked, this, &SubTabView::downloadComponentsClicked);
}

void SubTabView::createLayout() {
  auto* leftWidget = new QWidget();
  addWidget(leftWidget);

  auto* outerLeftVLayout = new QVBoxLayout();
  outerLeftVLayout->setContentsMargins(0, 0, 0, 0);
  outerLeftVLayout->setSpacing(0);
  leftWidget->setLayout(outerLeftVLayout);

  outerLeftVLayout->addWidget(m_itemSelector, 10);

  outerLeftVLayout->addWidget(m_itemSelectorButtons);

  addWidget(m_inspectorView);
  setStretchFactor(1, 100000);
}

void SubTabView::createGridViewLayout() {
  auto* widget = new QWidget();
  addWidget(widget);

  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  widget->setLayout(layout);

  layout->addWidget(m_inspectorView);

  m_itemSelectorButtons->hideDropZone();
  layout->addWidget(m_itemSelectorButtons);

  //setStretchFactor(1, 100000);
}

OSItemSelector* SubTabView::itemSelector() {
  return m_itemSelector;
}

OSItemSelectorButtons* SubTabView::itemSelectorButtons() {
  return m_itemSelectorButtons;
}

OSInspectorView* SubTabView::inspectorView() {
  return m_inspectorView;
}

const OSItemSelector* SubTabView::itemSelector() const {
  return m_itemSelector;
}

const OSItemSelectorButtons* SubTabView::itemSelectorButtons() const {
  return m_itemSelectorButtons;
}

const OSInspectorView* SubTabView::inspectorView() const {
  return m_inspectorView;
}

void SubTabView::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SubTabView::onDropZoneItemClicked(OSItem* item) {}

}  // namespace openstudio
