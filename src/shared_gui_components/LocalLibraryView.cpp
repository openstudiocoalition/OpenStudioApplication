/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LocalLibraryView.hpp"

#include "Buttons.hpp"
#include "OSViewSwitcher.hpp"

#include "../openstudio_lib/OSItem.hpp"

#include "MeasureBadge.hpp"

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QVBoxLayout>

namespace openstudio {

LocalLibraryView::LocalLibraryView(QWidget* parent) : QWidget(parent) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setSpacing(0);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(mainVLayout);

  auto* buttonGroup = new QButtonGroup();
  buttonGroup->setExclusive(true);

  mainViewSwitcher = new OSViewSwitcher();
  mainVLayout->addWidget(mainViewSwitcher);

  QString style;
  style.append("QWidget#Footer {");
  style.append("border-top: 1px solid black; ");
  style.append("background-color: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop: 0 #B6B5B6, stop: 1 #737172); ");
  style.append("}");

  auto* footer = new QWidget();
  footer->setObjectName("Footer");
  footer->setStyleSheet(style);
  //footer->setFixedHeight(40);
  mainVLayout->addWidget(footer);

  auto* footerVLayout = new QVBoxLayout();
  footerVLayout->setContentsMargins(5, 5, 5, 5);
  footerVLayout->setSpacing(5);
  footer->setLayout(footerVLayout);

  auto* footerHLayout = new QHBoxLayout();
  footerHLayout->setContentsMargins(0, 0, 0, 0);
  footerHLayout->setSpacing(5);
  footerVLayout->addLayout(footerHLayout);

  duplicateMeasureButton = new DuplicateButton();
  duplicateMeasureButton->setToolTip("Copy Selected Measure and Add to My Measures");
  footerHLayout->addWidget(duplicateMeasureButton);
  duplicateMeasureButton->setDisabled(true);

  addMeasureButton = new AddScriptButton();
  addMeasureButton->setToolTip("Create a Measure from Template and add to My Measures");
  footerHLayout->addWidget(addMeasureButton);

  checkForUpdateButton = new CheckForUpdateButton();
  checkForUpdateButton->setToolTip("Look for BCL measure updates online");
  footerHLayout->addWidget(checkForUpdateButton);

  // STRETCH
  footerHLayout->addStretch();

  myMeasuresFolderButton = new MyMeasuresFolderButton();
  myMeasuresFolderButton->setToolTip("Open the My Measures Directory");
  footerHLayout->addWidget(myMeasuresFolderButton);

  auto* footerHLayout2 = new QHBoxLayout();
  footerHLayout2->setContentsMargins(10, 0, 10, 0);
  footerHLayout2->setSpacing(5);
  footerVLayout->addLayout(footerHLayout2);

  addBCLMeasureButton = new GrayButton();
  addBCLMeasureButton->setText("Find Measures on BCL");
  addBCLMeasureButton->setToolTip("Connect to Online BCL to Download New Measures and Update Existing Measures to Library");
  footerHLayout2->addWidget(addBCLMeasureButton);
}

LibraryGroupItemHeader::LibraryGroupItemHeader() : LightGradientHeader() {
  countLabel = new QLabel();

  qobject_cast<QHBoxLayout*>(layout())->addStretch();

  layout()->addWidget(countLabel);
}

void LibraryGroupItemHeader::setCount(int count) {
  QString text = count > 0 ? QString::number(count) : QString();
  countLabel->setText(text);
}

LibrarySubGroupItemHeader::LibrarySubGroupItemHeader() : LightHeader() {
  countLabel = new QLabel();

  qobject_cast<QHBoxLayout*>(layout())->addStretch();

  layout()->addWidget(countLabel);
}

void LibrarySubGroupItemHeader::setCount(int count) {
  QString text = count > 0 ? QString::number(count) : QString();
  countLabel->setText(text);
}

LibraryItemView::LibraryItemView(QWidget* parent) : OSDragableView(parent) {
  setFixedHeight(35);

  setObjectName("Container");
  setHasEmphasis(false);

  auto* mainHBoxLayout = new QHBoxLayout();
  mainHBoxLayout->setContentsMargins(5, 0, 5, 0);

  setLayout(mainHBoxLayout);

  m_measureTypeBadge = new QLabel(this);
  m_measureTypeBadge->setFixedSize(25, 25);
  m_measureTypeBadge->setVisible(false);
  mainHBoxLayout->addWidget(m_measureTypeBadge, Qt::AlignLeft);

  m_measureBadge = new MeasureBadge();
  m_measureBadge->setFixedWidth(25);
  m_measureBadge->setMeasureBadgeType(MeasureBadgeType::MyMeasure);

  mainHBoxLayout->addWidget(m_measureBadge, Qt::AlignLeft);

  label = new QLabel("Measure");
  mainHBoxLayout->addWidget(label, Qt::AlignLeft);

  mainHBoxLayout->addStretch();

  errorLabel = new QLabel();
  //errorLabel->setPixmap(QPixmap(":/images/error-alert.png"));
  errorLabel->setPixmap(QPixmap(":/images/broken_script.png").scaled(24, 24));
  errorLabel->setFixedSize(24, 24);
  errorLabel->setVisible(false);
  mainHBoxLayout->addWidget(errorLabel, Qt::AlignRight);
}

void LibraryItemView::setHasEmphasis(bool hasEmphasis) {
  if (hasEmphasis) {
    setStyleSheet("QWidget#Container { background: #FECD60; border: 2px solid #EE641A; }");
  } else {
    setStyleSheet("QWidget#Container { background: #D5D5D5; border: 2px solid #A9A9A9; }");
  }
}

void LibraryItemView::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
