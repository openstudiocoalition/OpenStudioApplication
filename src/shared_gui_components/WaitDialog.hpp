/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_WAITDIALOG_HPP
#define SHAREDGUICOMPONENTS_WAITDIALOG_HPP

#include "OSDialog.hpp"

class QLabel;

namespace openstudio {

class WaitDialog : public OSDialog
{
  Q_OBJECT

 public:
  WaitDialog(const QString& windowTitle, const QString& windowMessage, QWidget* parent = nullptr);

  virtual ~WaitDialog();

 public slots:

  void setLine(unsigned lineNumber, const QString& text);

  // Reset waitDialog labels to default value/visible state/objectName
  // It's also for the initial creation in createWidgets
  void resetLabels();

 protected slots:

  virtual void on_cancelButton(bool checked) override;

 protected:
  void closeEvent(QCloseEvent* event) override;

 private:
  void createWidgets();

  // Convenience to avoid having to do QLabel * descriptionLabel1 = qobject_cast<QLabel*>(waitDialog()->upperLayout()->itemAt(1)->widget());
  QLabel* m_firstLine;
  QLabel* m_secondLine;
  QLabel* m_thirdLine;
  QLabel* m_fourthLine;

  QString m_windowMessage;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_WAITDIALOG_HPP
