/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "TextEditDialog.hpp"

#include <QBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTextEdit>

namespace openstudio {

TextEditDialog::TextEditDialog(const QString& windowTitle, const QString& windowMessage, QWidget* parent)
  : OSDialog(false, parent), m_windowMessage(windowMessage) {
  setWindowTitle(windowTitle);
  setWindowModality(Qt::ApplicationModal);
  setSizeGripEnabled(true);
  createWidgets();
}

TextEditDialog::~TextEditDialog() = default;

QString TextEditDialog::text() {
  return m_textEdit->toPlainText();
}

void TextEditDialog::setText(const QString& text) {
  m_textEdit->setText(text);
}

void TextEditDialog::createWidgets() {
  m_textEdit = new QTextEdit(m_windowMessage);
  m_textEdit->setReadOnly(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  upperLayout()->addWidget(m_textEdit);

  // BUTTONS

  this->m_cancelButton->hide();

  // OS SETTINGS

#ifdef Q_OS_DARWIN
  setWindowFlags(Qt::FramelessWindowHint);
#elif defined(Q_OS_WIN)
  setWindowFlags(Qt::WindowCloseButtonHint);
#endif
}

}  // namespace openstudio
