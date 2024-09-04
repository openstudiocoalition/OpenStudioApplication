/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSInspectorView.hpp"
#include "ModelObjectItem.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
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

OSInspectorView::OSInspectorView(bool addScrollArea, QWidget* parent) : QWidget(parent) {
  this->setObjectName("GrayWidget");

  auto* outerVLayout = new QVBoxLayout();
  outerVLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(outerVLayout);

  m_stackedWidget = new QStackedWidget();

  if (addScrollArea) {
    auto* scrollArea = new QScrollArea();
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    outerVLayout->addWidget(scrollArea);
    scrollArea->setWidget(m_stackedWidget);
    scrollArea->setWidgetResizable(true);
  } else {
    outerVLayout->addWidget(m_stackedWidget);
  }
}

void OSInspectorView::clearSelection() {
  onClearSelection();
}

void OSInspectorView::selectItem(OSItem* item) {
  onSelectItem(item);
}

QStackedWidget* OSInspectorView::stackedWidget() const {
  return m_stackedWidget;
}

void OSInspectorView::update() {
  onUpdate();
}

}  // namespace openstudio
