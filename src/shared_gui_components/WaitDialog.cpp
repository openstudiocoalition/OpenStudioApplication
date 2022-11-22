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

#include "WaitDialog.hpp"

#include "BusyWidget.hpp"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

namespace openstudio {

WaitDialog::WaitDialog(const QString& windowTitle, const QString& windowMessage, QWidget* parent)
  : OSDialog(false, parent), m_windowMessage(windowMessage) {
  setSizeHint(QSize(770, 540));
  setWindowTitle(windowTitle);
  setWindowModality(Qt::ApplicationModal);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  createWidgets();
}

WaitDialog::~WaitDialog() = default;

void WaitDialog::createWidgets() {

  upperLayout()->addStretch();

  m_firstLine = new QLabel();
  m_firstLine->setObjectName("H1");
  m_firstLine->setVisible(true);
  upperLayout()->addWidget(m_firstLine, 0, Qt::AlignCenter);

  //BusyWidget * busyWidget = new BusyWidget();
  //upperLayout()->addWidget(busyWidget,0,Qt::AlignCenter);

  //QTimer * timer = new QTimer(this);
  //connect(timer,SIGNAL(timeout()),busyWidget,SLOT(rotate()));
  //timer->start(50);

  m_secondLine = new QLabel();
  m_secondLine->setObjectName("H1");
  m_secondLine->setVisible(true);
  upperLayout()->addWidget(m_secondLine, 0, Qt::AlignCenter);

  m_thirdLine = new QLabel();
  m_thirdLine->setObjectName("H2");
  m_thirdLine->setVisible(true);
  upperLayout()->addWidget(m_thirdLine, 0, Qt::AlignCenter);

  m_fourthLine = new QLabel();
  m_fourthLine->setObjectName("H2");
  m_fourthLine->setVisible(true);
  upperLayout()->addWidget(m_fourthLine, 0, Qt::AlignCenter);

  // Will populate label text and set visible state + objectname for styling
  resetLabels();

  upperLayout()->addStretch();

  // BUTTONS

  this->okButton()->hide();
  this->m_cancelButton->hide();

  // OS SETTINGS

#ifdef Q_OS_DARWIN
  setWindowFlags(Qt::FramelessWindowHint);
#elif defined(Q_OS_WIN)
  setWindowFlags(Qt::CustomizeWindowHint);
#endif
}

void WaitDialog::setLine(unsigned lineNumber, const QString& text) {
  if (lineNumber == 1) {
    m_firstLine->setText(text);
  } else if (lineNumber == 2) {
    m_secondLine->setText(text);
  } else if (lineNumber == 3) {
    m_thirdLine->setText(text);
  } else if (lineNumber == 4) {
    m_fourthLine->setText(text);
  }
}

void WaitDialog::resetLabels() {
  m_firstLine->setText(m_windowMessage);
  m_firstLine->setVisible(true);

  m_secondLine->setText("This may take a minute...");
  m_secondLine->setVisible(true);

  // Third and fourth line, disabled by default
  m_thirdLine->setText("");
  m_thirdLine->setVisible(true);

  m_fourthLine->setText("");
  m_fourthLine->setVisible(true);
}

//***** SLOTS *****

void WaitDialog::on_cancelButton(bool checked) {
  OSDialog::on_cancelButton(checked);
}

void WaitDialog::closeEvent(QCloseEvent* event) {
  // DLM: do not accept the event, we don't allow this dialog to close
  // would be better to set window flags to not have the close button
  event->ignore();
}

}  // namespace openstudio
