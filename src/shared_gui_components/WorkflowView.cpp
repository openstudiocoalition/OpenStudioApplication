/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "WorkflowView.hpp"
#include "OSViewSwitcher.hpp"
#include "OSListView.hpp"
#include "HeaderViews.hpp"
#include "Buttons.hpp"
#include "LocalLibraryController.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QDragEnterEvent>

namespace openstudio {

namespace measuretab {

RectangularDropZone::RectangularDropZone() : nameLabel(new QLabel()) {
  setAcceptDrops(true);

  setFixedHeight(30);

  setStyleSheet("openstudio--measuretab--RectangularDropZone {background: #F4F4F4; border: 2px dashed #A5A5A5;}");

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 5, 5, 5);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  mainHLayout->addWidget(nameLabel);
}

void RectangularDropZone::setAcceptedMimeType(const QString& type) {
  m_acceptedMimeType = type;
}

void RectangularDropZone::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void RectangularDropZone::dropEvent(QDropEvent* event) {
  emit dataDropped(event);
}

void RectangularDropZone::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasFormat(m_acceptedMimeType)) {
    event->accept();
  }
}

NewMeasureDropZone::NewMeasureDropZone() {
  nameLabel->setText("<i>Drop Measure From Library to Create a New Always Run Measure</i>");
  nameLabel->setStyleSheet("QLabel {color: #7D7D7D; }");
}

WorkflowSectionView::WorkflowSectionView(const MeasureType& measureType)
  : header(new LightGradientHeader()), content(new WorkflowSectionContentView()) {

  header->setMeasureType(measureType);
  setHeader(header);

  setContent(content);

  setExpanded(true);
}

WorkflowSectionContentView::WorkflowSectionContentView() : workflowStepsView(new OSListView(false)), newMeasureDropZone(new NewMeasureDropZone()) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 10, 0, 10);
  mainVLayout->setSpacing(0);
  setLayout(mainVLayout);

  // Workflow Steps

  mainVLayout->addWidget(workflowStepsView);

  mainVLayout->addWidget(newMeasureDropZone);
}

WorkflowStepView::WorkflowStepView()
  : workflowStepButton(new WorkflowStepButton()),
    duplicateButton(new SofterDuplicateButton()),
    removeButton(new SofterRemoveButton()),
    upButton(new UpButton()),
    downButton(new DownButton()) {
  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(0, 0, 0, 0);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  mainHLayout->addWidget(workflowStepButton);

  mainHLayout->addWidget(duplicateButton);

  mainHLayout->addWidget(removeButton);

  mainHLayout->addSpacing(50);

  mainHLayout->addWidget(downButton);

  mainHLayout->addWidget(upButton);

  duplicateButton->setVisible(false);
  removeButton->setVisible(true);
}

void WorkflowStepView::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

WorkflowStepButton::WorkflowStepButton()
  : measureTypeBadge(new QLabel()), nameLabel(new QLabel()), cautionLabel(new QLabel()), errorLabel(new QLabel()) {
  setFixedHeight(25);

  setHasEmphasis(false);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 5, 5, 5);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  measureTypeBadge->setVisible(false);

  mainHLayout->addWidget(measureTypeBadge);

  nameLabel->setStyleSheet("font:italic");

  mainHLayout->addWidget(nameLabel);

  mainHLayout->addStretch();

  cautionLabel->setPixmap(QPixmap(":/images/warning_icon.png"));
  mainHLayout->addWidget(cautionLabel);
  cautionLabel->setVisible(false);

  errorLabel->setPixmap(QPixmap(":/images/broken_script.png").scaled(24, 24));
  mainHLayout->addWidget(errorLabel);
  errorLabel->setVisible(false);
}

void WorkflowStepButton::setHasEmphasis(bool hasEmphasis) {
  if (hasEmphasis) {
    setStyleSheet("openstudio--measuretab--WorkflowStepButton { background: #FECD60; border: 2px solid #EE641A; }");
  } else {
    setStyleSheet("openstudio--measuretab--WorkflowStepButton {background: #E0E0E0; }");
  }
}

void WorkflowStepButton::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace measuretab

}  // namespace openstudio
