/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LibraryTabWidget.hpp"

#include <QBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>

#include <openstudio/utilities/core/Assert.hpp>

#include <vector>

namespace openstudio {

LibraryTabWidget::LibraryTabWidget(QWidget* parent) : QWidget(parent) {
  auto* mainLayout = new QVBoxLayout();

  mainLayout->setSpacing(0);

  mainLayout->setContentsMargins(0, 0, 0, 0);

  setLayout(mainLayout);

  m_tabBar = new QWidget(this);

  m_tabBar->setFixedHeight(29);

  m_tabBar->setObjectName("VBlueGradientWidget");

  m_tabBar->setContentsMargins(0, 0, 5, 0);

  m_removeButton = new QPushButton(this);

  QString str;
  str.append("QWidget { ");
  str.append("border: none;");
  str.append(" background-image: url(\":/images/delete.png\")");
  str.append("}");

  m_removeButton->setFlat(true);
  m_removeButton->setStyleSheet(str);
  m_removeButton->setFixedSize(20, 20);
  m_removeButton->setToolTip("Remove object");

  // Default to hide
  m_removeButton->hide();

  connect(m_removeButton, &QPushButton::clicked, this, &LibraryTabWidget::removeButtonClicked);

  auto* hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->addStretch();
  hLayout->addWidget(m_removeButton, 0, Qt::AlignVCenter);

  m_tabBar->setLayout(hLayout);

  layout()->addWidget(m_tabBar);

  m_pageStack = new QStackedWidget();

  m_pageStack->setStyleSheet("QStackedWidget { border: none;}");

  m_pageStack->setContentsMargins(0, 0, 0, 0);

  layout()->addWidget(m_pageStack);
}

void LibraryTabWidget::showRemoveButton() {
  m_removeButton->show();
}

void LibraryTabWidget::hideRemoveButton() {
  m_removeButton->hide();
}

void LibraryTabWidget::addTab(QWidget* widget, const QString& selectedImagePath, const QString& unSelectedImagePath) {
  auto* button = new QPushButton(m_tabBar);

  button->setFixedSize(QSize(29, 29));

  m_tabButtons.push_back(button);

  connect(button, &QPushButton::clicked, this, &LibraryTabWidget::select);

  m_pageStack->addWidget(widget);

  m_selectedPixmaps.push_back(selectedImagePath);

  m_unSelectedPixmaps.push_back(unSelectedImagePath);

  setCurrentIndex(0);
}

void LibraryTabWidget::hideTab(QWidget* widget, bool hide) {
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

void LibraryTabWidget::select() {
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

void LibraryTabWidget::setCurrentIndex(int index) {
  int xPos = 0;

  for (unsigned i = 0; i < m_tabButtons.size(); i++) {
    QPushButton* button = m_tabButtons[i];

    button->move(xPos, 0);

    xPos = xPos + button->width();

    QString imagePath = m_unSelectedPixmaps[i];

    QString style;

    style.append("QPushButton { background: none; background-image: url(\"");
    style.append(imagePath);
    style.append("\"); border: none; }");

    button->setStyleSheet(style);
  }

  QPushButton* button = m_tabButtons[index];

  QString imagePath = m_selectedPixmaps[index];

  QString style;

  style.append("QPushButton { background: none; background-image: url(\"");
  style.append(imagePath);
  style.append("\"); border: none; background-repeat: 0; }");

  button->setStyleSheet(style);

  m_pageStack->setCurrentIndex(index);
}

void LibraryTabWidget::setCurrentWidget(QWidget* widget) {
  int i = m_pageStack->indexOf(widget);

  setCurrentIndex(i);
}

}  // namespace openstudio
