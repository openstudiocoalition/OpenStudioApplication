/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSListView.hpp"
#include "OSListController.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QStyleOption>
#include <QPainter>
#include <QGraphicsObject>

namespace openstudio {

OSListView::OSListView(bool scrollable, QWidget* parent)
  : QWidget(parent),
    m_delegate(QSharedPointer<OSItemDelegate>::create()),
    m_widgetItemPairs(std::map<QObject*, QSharedPointer<OSListItem>>()),
    m_scrollable(scrollable),
    m_scrollArea(nullptr) {
  m_mainVLayout = new QVBoxLayout();
  m_mainVLayout->setSizeConstraint(QLayout::SetMinimumSize);
  m_mainVLayout->setAlignment(Qt::AlignTop);

  if (scrollable) {
    auto* scrollWidget = new QWidget();
    scrollWidget->setObjectName("ScrollWidget");
    scrollWidget->setStyleSheet("QWidget#ScrollWidget { background: transparent; }");
    scrollWidget->setLayout(m_mainVLayout);

    m_scrollArea = new QScrollArea();
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setBackgroundRole(QPalette::NoRole);

    auto* scrollLayout = new QVBoxLayout();
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addWidget(m_scrollArea);

    setLayout(scrollLayout);
  } else {
    setLayout(m_mainVLayout);
  }

  setContentsMargins(5, 5, 5, 5);
  setSpacing(5);
}

OSListView::~OSListView() {
  for (const auto& widgetItemPair : m_widgetItemPairs) {
    disconnect(widgetItemPair.first, &QWidget::destroyed, this, &OSListView::removePair);
  }
}

void OSListView::setHorizontalScrollBarAlwaysOn(bool alwaysOn) {
  if (!m_scrollable && !m_scrollArea) {
    return;
  }

  if (alwaysOn) {
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  } else {
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
}

void OSListView::setVerticalScrollBarAlwaysOn(bool alwaysOn) {
  if (!m_scrollable && !m_scrollArea) {
    return;
  }

  if (alwaysOn) {
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  } else {
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
}

void OSListView::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OSListView::setDelegate(QSharedPointer<OSItemDelegate> delegate) {
  if (delegate) {
    m_delegate = delegate;

    refreshAllViews();
  }
}

void OSListView::setListController(QSharedPointer<OSListController> listController) {
  if (m_listController) {
    m_listController->disconnect(this);
  }

  m_listController = listController;

  connect(m_listController.data(), &OSListController::itemInserted, this, &OSListView::insertItemView);
  connect(m_listController.data(), &OSListController::itemRemoved, this, &OSListView::removeItemView);
  connect(m_listController.data(), &OSListController::itemChanged, this, &OSListView::refreshItemView);
  connect(m_listController.data(), &OSListController::modelReset, this, &OSListView::refreshAllViews);
  refreshAllViews();
}

QSharedPointer<OSListController> OSListView::listController() const {
  return m_listController;
}

void OSListView::setSpacing(int spacing) {
  m_mainVLayout->setSpacing(spacing);
}

void OSListView::setContentsMargins(int left, int top, int right, int bottom) {
  m_mainVLayout->setContentsMargins(left, top, right, bottom);
}

void OSListView::refreshAllViews() {
  QLayoutItem* child;
  while ((child = m_mainVLayout->takeAt(0)) != nullptr) {
    QWidget* widget = child->widget();

    OS_ASSERT(widget);

    delete widget;

    delete child;
  }

  if (m_listController) {
    for (int i = 0, n = m_listController->count(); i < n; i++) {
      insertItemView(i);
    }
  }
}

void OSListView::insertItemView(int i) {
  OS_ASSERT(m_listController);

  QSharedPointer<OSListItem> itemData = m_listController->itemAt(i);

  OS_ASSERT(itemData);

  QWidget* itemView = m_delegate->view(itemData);

  itemView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

  m_mainVLayout->insertWidget(i, itemView);

  m_widgetItemPairs.insert(std::make_pair(itemView, itemData));

  // For some reason, this needs to use the old-style connect on mac (exiting OS app crash)
  connect(itemView, &QWidget::destroyed, this, &OSListView::removePair);
}

void OSListView::removeItemView(int i) {
  QLayoutItem* item = m_mainVLayout->takeAt(i);

  OS_ASSERT(item);

  QWidget* widget = item->widget();

  OS_ASSERT(widget);

  delete widget;

  delete item;
}

void OSListView::removePair(QObject* object) {
  auto it = m_widgetItemPairs.find(object);
  if (it != m_widgetItemPairs.end()) {
    m_widgetItemPairs.erase(it);
  }
}

void OSListView::refreshItemView(int i) {
  if (i < int(m_widgetItemPairs.size())) {
    removeItemView(i);
  } else {
    LOG(Trace, "Not calling removeItemView(" << i << "), because the list is not that long.");
  }

  insertItemView(i);
}

}  // namespace openstudio
