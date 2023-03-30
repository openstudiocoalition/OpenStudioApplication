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

#include "OSDialog.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QLinearGradient>
#include <QBrush>

namespace openstudio {

// Note: Window flags are not able to present the user
//       with buttons on the upper right corner

OSDialog::OSDialog(bool isIP, QWidget* parent)
  : QDialog(parent),
    m_isIP(isIP),
    m_backButton(nullptr),
    m_cancelButton(nullptr),
    m_okButton(nullptr),
    m_upperLayout(nullptr),
    m_sizeHint(QSize(800, 500)),
    m_layoutContentsMargins(QMargins(20, 70, 20, 20)),
    _move(false) {
  setStyleSheet("openstudio--OSDialog { background: #E6E6E6; }");

#ifdef Q_OS_DARWIN
  setWindowFlags(Qt::FramelessWindowHint);
#else
  setWindowFlags(Qt::CustomizeWindowHint);
#endif

  createLayoutInternal();
}

void OSDialog::createLayout() {
  createLayoutInternal();
}

void OSDialog::createLayoutInternal() {
  m_upperLayout = new QVBoxLayout();

  auto* lowerLayout = new QHBoxLayout();

  lowerLayout->addStretch();

  m_backButton = new QPushButton("Back", this);
  connect(m_backButton, &QPushButton::clicked, this, &OSDialog::on_backButton);
  connect(m_backButton, &QPushButton::clicked, this, &OSDialog::backButtonClicked);
  lowerLayout->addWidget(m_backButton);
  m_backButton->hide();

  m_okButton = new QPushButton("OK", this);
  m_okButton->setDefault(true);
  connect(m_okButton, &QPushButton::clicked, this, &OSDialog::on_okButton);
  connect(m_okButton, &QPushButton::clicked, this, &OSDialog::okButtonClicked);
  lowerLayout->addWidget(m_okButton);

  m_cancelButton = new QPushButton("Cancel", this);
  connect(m_cancelButton, &QPushButton::clicked, this, &OSDialog::on_cancelButton);
  connect(m_cancelButton, &QPushButton::clicked, this, &OSDialog::cancelButtonClicked);
  lowerLayout->addWidget(m_cancelButton);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(m_layoutContentsMargins);
  mainLayout->addLayout(m_upperLayout);
  mainLayout->addLayout(lowerLayout);

  setLayout(mainLayout);
}

void OSDialog::setOkButtonAsDefault(bool isDefault) {
  if (isDefault) {
    setTabOrder(m_okButton, m_cancelButton);
  } else {
    setTabOrder(m_cancelButton, m_okButton);
  }
}

QVBoxLayout* OSDialog::upperLayout() {
  return m_upperLayout;
}

QPushButton* OSDialog::backButton() {
  return m_backButton;
}

QPushButton* OSDialog::cancelButton() {
  return m_cancelButton;
}

QPushButton* OSDialog::okButton() {
  return m_okButton;
}

void OSDialog::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (event->position().toPoint().y() < 50) {
      dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
      event->accept();
      _move = true;
    } else {
      _move = false;
    }
  }
}

void OSDialog::mouseReleaseEvent(QMouseEvent* event) {
  _move = false;
}

void OSDialog::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    if (_move) {
      move(event->globalPosition().toPoint() - dragPosition);
      event->accept();
    }
  }
}

void OSDialog::resizeEvent(QResizeEvent* event) {
#ifdef Q_OS_DARWIN
  QPainterPath path;
  path.addRoundedRect(rect(), 9.0, 9.0);
  QPolygon p = path.toFillPolygon().toPolygon();
  QRegion region(p);
  setMask(region);
#endif
}

void OSDialog::paintEvent(QPaintEvent* event) {
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  // CSS equivalent
  // background: rgb(130,130,130);
  // background: linear-gradient(180deg, rgba(130,130,130,0) 0%,
  //             rgba(130,130,130,1) 1%, rgba(130,130,130,1) 2%, rgba(149,179,222,1) 2%, rgba(71,122,174,1) 100%);
  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 50));
  linearGrad.setColorAt(0.00, QColor(130, 130, 130, 0));
  linearGrad.setColorAt(0.01, QColor(130, 130, 130, 255));
  linearGrad.setColorAt(0.02, QColor(130, 130, 130, 255));
  linearGrad.setColorAt(0.03, QColor(149, 179, 222, 255));
  linearGrad.setColorAt(1.00, QColor(71, 122, 174, 255));

  QBrush brush(linearGrad);
  painter.setBrush(brush);
  painter.setPen(Qt::NoPen);
  painter.drawRect(0, 0, rect().width(), 50 - 1);

  QImage icon(":/images/os_48.png");
  painter.drawImage(1, 1, icon);

  painter.setPen(Qt::white);
  painter.setFont(QFont("Muli", 22));
  painter.drawText(QRect(55, 0, width() - 60, 50), Qt::AlignLeft | Qt::AlignVCenter, "OpenStudio Application");
}

void OSDialog::setSizeHint(const QSize& sizeHint) {
  m_sizeHint = sizeHint;
}

QSize OSDialog::sizeHint() const {
  return m_sizeHint;
}

QMargins OSDialog::layoutContentsMargins() const {
  return m_layoutContentsMargins;
}

void OSDialog::setLayoutContentsMargins(const QMargins& layoutContentsMargins) {
  m_layoutContentsMargins = layoutContentsMargins;
  this->layout()->setContentsMargins(m_layoutContentsMargins);
}

// ***** SLOTS *****

void OSDialog::on_backButton(bool checked) {}

void OSDialog::on_cancelButton(bool checked) {
  hide();
  setResult(0);
}

void OSDialog::on_okButton(bool checked) {
  hide();
  setResult(1);
}

void OSDialog::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
