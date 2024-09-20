/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSDIALOG_HPP
#define SHAREDGUICOMPONENTS_OSDIALOG_HPP

#include <QDialog>

class QMargins;

class QPoint;

class QPushButton;

class QSize;

class QVBoxLayout;

namespace openstudio {

class OSDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit OSDialog(bool isIP = false, QWidget* parent = nullptr);

  virtual ~OSDialog() = default;

  QSize sizeHint() const override;

  void setSizeHint(const QSize& sizeHint);

  QMargins layoutContentsMargins() const;

  void setLayoutContentsMargins(const QMargins& layoutContentsMargins);

  QVBoxLayout* upperLayout();

  QPushButton* backButton();

  QPushButton* cancelButton();

  QPushButton* okButton();

 protected:
  void setOkButtonAsDefault(bool isDefault);

  void mouseMoveEvent(QMouseEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

  void resizeEvent(QResizeEvent* event) override;

  void paintEvent(QPaintEvent* event) override;

  bool m_isIP;

  QPushButton* m_backButton;

  QPushButton* m_cancelButton;

  QPushButton* m_okButton;

 private:
  virtual void createLayout();  // TODO: Chances are this doesn't need to be virtual...
  void createLayoutInternal();

  QVBoxLayout* m_upperLayout;

  QSize m_sizeHint;

  QMargins m_layoutContentsMargins;

  QPoint dragPosition;

  bool _move;

 signals:

  void backButtonClicked(bool checked);

  void cancelButtonClicked(bool checked);

  void okButtonClicked(bool checked);

  void toggleUnitsClicked(bool displayIP);

 protected slots:

  virtual void on_backButton(bool checked);

  virtual void on_cancelButton(bool checked);

  virtual void on_okButton(bool checked);

  virtual void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSDIALOG_HPP
