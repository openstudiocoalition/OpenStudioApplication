/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
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

WorkflowSectionView::WorkflowSectionView(MeasureType measureType) : header(new LightGradientHeader()), content(new WorkflowSectionContentView()) {

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

WorkflowStepButton::WorkflowStepButton() : nameLabel(new QLabel()), cautionLabel(new QLabel()) {
  setFixedHeight(25);

  setHasEmphasis(false);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 5, 5, 5);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  nameLabel->setStyleSheet("font:italic");

  mainHLayout->addWidget(nameLabel);

  mainHLayout->addStretch();

  cautionLabel->setPixmap(QPixmap(":/shared_gui_components/images/warning_icon.png"));
  mainHLayout->addWidget(cautionLabel);
  cautionLabel->setVisible(false);
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
