/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MainTabView.hpp"

#include "../shared_gui_components/OSViewSwitcher.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QStyleOption>

#include <openstudio/utilities/core/Assert.hpp>

#include <vector>

static const int TAB_SEPARATION = 10;

namespace openstudio {

MainTabView::MainTabView(const QString& tabLabel, TabType tabType, QWidget* parent)
  : QWidget(parent), m_editView(new OSViewSwitcher()), m_tabType(tabType) {
  this->setObjectName("BlueGradientWidget");

  m_tabLabel = new QLabel(tabLabel, this);
  m_tabLabel->setFixedHeight(20);
  m_tabLabel->setStyleSheet("QLabel { color: white; font-size: 14px; }");
  m_tabLabel->adjustSize();
  m_tabLabel->setFixedWidth(m_tabLabel->width());
  m_tabLabel->move(7, 5);

  auto* label = new QLabel();
  label->setObjectName("H2");

  m_editView->setView(label);

  m_innerLayout = new QVBoxLayout();
  m_innerLayout->setSpacing(0);
  m_innerLayout->addWidget(m_editView);

  m_mainWidget = new QWidget(this);
  m_mainWidget->setObjectName("MainTabView");
  m_mainWidget->move(7, 25);
  m_mainWidget->setLayout(m_innerLayout);

  setTabType(tabType);
}

MainTabView::~MainTabView() {
  if (m_editView) {
    delete m_editView;
  }
}

void MainTabView::setTabType(MainTabView::TabType tabType) {
  QString style;

  switch (tabType) {
    case MAIN_TAB:
      style.append("QWidget#MainTabView { ");
      style.append("  background: #E6E6E6; ");
      style.append("  border-left: 1px solid black; ");
      style.append("  border-top: 1px solid black; ");
      style.append("}");
      m_mainWidget->layout()->setContentsMargins(0, 0, 0, 0);
      break;
    case SUB_TAB:
      style.append("QWidget#MainTabView { ");
      style.append("  background: #E6E6E6; ");
      style.append("  border-left: 1px solid black; ");
      style.append("  border-top: 1px solid black; ");
      style.append("  border-top-left-radius: 5px; ");
      style.append("}");
      m_mainWidget->layout()->setContentsMargins(7, 10, 0, 0);
      break;
    case GRIDVIEW_SUB_TAB:
      style.append("QWidget#MainTabView { ");
      style.append("  background: #E6E6E6; ");
      style.append("  border-left: 1px solid black; ");
      style.append("  border-top: 1px solid black; ");
      style.append("  border-top-left-radius: 5px; ");
      style.append("}");
      m_mainWidget->layout()->setContentsMargins(1, 2, 0, 0);
      break;
    default:
      OS_ASSERT(false);
  }

  m_mainWidget->setStyleSheet(style);
}

bool MainTabView::addTabWidget(QWidget* widget) {
  // This method should only be called in cases where the tab will not have sub tabs
  OS_ASSERT(m_tabType == MAIN_TAB);
  if (m_tabType != MAIN_TAB) {
    return false;
  }

  m_editView->setView(widget);

  return true;
}

bool MainTabView::addSubTab(const QString& subTabLabel, int id) {
  // This method should only be called in cases where the tab will have sub tabs
  OS_ASSERT(m_tabType != MAIN_TAB);
  if (m_tabType == MAIN_TAB) {
    return false;
  }

  auto* button = new QPushButton(this);
  button->setText(subTabLabel);
  button->setFixedHeight(21);
  m_tabButtons.push_back(button);
  connect(button, &QPushButton::clicked, this, &MainTabView::select);

  m_ids.push_back(id);

  setCurrentIndex(0);
  return true;
}

void MainTabView::setSubTab(QWidget* widget) {
  m_editView->setView(widget);
}

void MainTabView::select() {
  auto* button = qobject_cast<QPushButton*>(sender());

  int index = 0;

  for (auto it = m_tabButtons.begin(); it < m_tabButtons.end(); ++it) {
    if (*it == button) {
      break;
    } else {
      index++;
    }
  }

  setCurrentIndex(index);
}

void MainTabView::setCurrentIndex(int index) {
  int xPos = m_tabLabel->width() + TAB_SEPARATION;

  for (unsigned i = 0; i < m_tabButtons.size(); i++) {
    QPushButton* button = m_tabButtons[i];

    if (i == index) {
      button->setObjectName("SubTabButtonSelected");
      button->raise();
    } else {
      button->setObjectName("SubTabButton");
      button->stackUnder(m_mainWidget);
    }

    button->adjustSize();
    button->move(xPos, 5);
    button->style()->unpolish(button);
    button->style()->polish(button);

    xPos += TAB_SEPARATION + button->width();
  }

  emit tabSelected(m_ids[index]);
}

void MainTabView::setCurrentWidget(QWidget* widget) {
  OS_ASSERT(false);
}

void MainTabView::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainTabView::resizeEvent(QResizeEvent* event) {
  QSize size = event->size();

  QSize newSize(size.width() - 7, size.height() - 25);

  m_mainWidget->resize(newSize);
}

//int MainTabView::subTabId() const
//{
//  OS_ASSERT(false);
//  if (m_tabType != MAIN_TAB)
//  {
//    return m_ids[subTabIndex()];
//  }
//  else
//  {
//    return -1;
//  }
//}

//int MainTabView::subTabIndex() const
//{
//  OS_ASSERT(false);
//  return -1;
//}

bool MainTabView::selectSubTabByIndex(int index) {
  if (m_tabType != MAIN_TAB) {
    if (index < 0) {
      return false;
    } else {
      setCurrentIndex(index);
      return true;
    }
  }
  return false;
}

}  // namespace openstudio
