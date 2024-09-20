/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
