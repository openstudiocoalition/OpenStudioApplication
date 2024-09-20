/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_TEXTEDITDIALOG_HPP
#define SHAREDGUICOMPONENTS_TEXTEDITDIALOG_HPP

#include "OSDialog.hpp"

class QTextEdit;

namespace openstudio {

class TextEditDialog : public OSDialog
{
  Q_OBJECT

 public:
  explicit TextEditDialog(const QString& windowTitle, const QString& windowMessage = QString(), QWidget* parent = nullptr);

  virtual ~TextEditDialog();

  QString text();

  void setText(const QString& text);

 private:
  void createWidgets();

  QTextEdit* m_textEdit;

  QString m_windowMessage;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_TEXTEDITDIALOG_HPP
