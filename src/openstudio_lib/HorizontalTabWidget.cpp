/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HorizontalTabWidget.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyleOption>

#include <vector>

#include <algorithm>

namespace openstudio {

HorizontalTabWidget::HorizontalTabWidget(QWidget* parent) : QWidget(parent) {
  //setObjectName("GrayWidget");

  auto* mainLayout = new QVBoxLayout();

  mainLayout->setSpacing(0);

  mainLayout->setContentsMargins(0, 0, 0, 0);

  setLayout(mainLayout);

  // Tab bar

  m_tabBar = new QWidget(this);

  m_tabBar->setFixedHeight(31);

  m_tabBar->setContentsMargins(0, 0, 0, 0);

  layout()->addWidget(m_tabBar);

  // Tab bar line

  m_tabBarLine = new QWidget(m_tabBar);

  m_tabBarLine->setFixedHeight(5);

  m_tabBarLine->move(0, m_tabBar->height() - 5);

  m_tabBarLine->setStyleSheet("QWidget { border-top: none; background-color: #95B3DE; }");

  // Page stack

  m_pageStack = new QStackedWidget();

  m_pageStack->setObjectName("GrayWidget");

  m_pageStack->setContentsMargins(0, 0, 0, 0);

  layout()->addWidget(m_pageStack);
}

void HorizontalTabWidget::addTab(QWidget* widget, int id, const QString& label) {
  auto* button = new QPushButton(m_tabBar);

  button->setText(label);

  button->setFixedHeight(27);

  m_tabButtons.push_back(button);

  connect(button, &QPushButton::clicked, this, &HorizontalTabWidget::select);

  m_pageStack->addWidget(widget);

  m_ids.push_back(id);

  setCurrentIndex(0);
}

void HorizontalTabWidget::select() {
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

void HorizontalTabWidget::setCurrentId(int id) {
  std::vector<int>::iterator it;

  it = std::find(m_ids.begin(), m_ids.end(), id);

  if (it != m_ids.end()) {
    setCurrentIndex(*it);
  }
}

void HorizontalTabWidget::setCurrentIndex(int index) {
  int xPos = 0;

  for (unsigned i = 0; i < m_tabButtons.size(); i++) {
    QPushButton* button = m_tabButtons[i];

    if (button->isHidden()) {
      button->move(0, 0);
      continue;
    }

    QString style;

    style.append("QPushButton { ");
    if (i == index) {
      style.append("            border-bottom: none;");
      style.append("            background-color: #95B3DE;");
    } else {
      style.append("            border-bottom: 1px solid black;");
      style.append("            background-color: #808080;");
    }
    if (i == m_tabButtons.size() - 1) {
      style.append("            border-right: none;");
    } else {
      style.append("            border-right: 1px solid black;");
    }
    style.append("              border-top: 1px solid black;");
    style.append("              border-left: 1px solid black;");
    style.append("              border-top-left-radius: 5px;");
    style.append("              border-top-right-radius: 5px;");
    style.append("              border-bottom-left-radius: none;");
    style.append("              border-bottom-right-radius: none;");
    style.append("              padding-left: 10px;");
    style.append("              padding-right: 10px;");
    style.append("              padding-top: 5px;");
    style.append("              color: white;");
    style.append("              font-size: 12px;");
    style.append("}");

    button->setStyleSheet(style);

    button->adjustSize();

    button->move(xPos, 0);

    xPos = xPos + button->width();
  }

  m_tabBarLine->setFixedWidth(xPos);
  m_tabBarLine->raise();

  QPushButton* button = m_tabButtons[index];
  button->raise();

  m_pageStack->setCurrentIndex(index);
}

void HorizontalTabWidget::setCurrentWidget(QWidget* widget) {
  int i = m_pageStack->indexOf(widget);

  setCurrentIndex(i);
}

void HorizontalTabWidget::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void HorizontalTabWidget::hideTab(QWidget* widget, bool hide) {
  int index = m_pageStack->indexOf(widget);
  OS_ASSERT(index >= 0);

  int currentIndex = m_pageStack->currentIndex();
  if (currentIndex == index) {
    if (currentIndex + 1 < m_pageStack->count()) {
      currentIndex++;
    } else if (currentIndex != 0) {
      currentIndex = 0;
    } else {
      // index and currentIndex are both 0
      // can't hide both the tab and the page
      return;
    }
  }

  QPushButton* button = nullptr;
  button = m_tabButtons.at(index);
  OS_ASSERT(button);
  if (hide) {
    button->hide();
  } else {
    button->show();
  }

  setCurrentIndex(currentIndex);
}

}  // namespace openstudio
