/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_COLLAPSIBLEINSPECTOR_HPP
#define OPENSTUDIO_COLLAPSIBLEINSPECTOR_HPP

#include <QAbstractButton>
#include <QWidget>

class QLabel;
class QPaintEvent;

namespace openstudio {

class CollapsibleInspectorHeader;

class CollapsibleInspector : public QWidget
{
  Q_OBJECT

 public:
  CollapsibleInspector(QString text, QWidget* inspector, QWidget* parent = nullptr);

  virtual ~CollapsibleInspector() {}

 private:
  void createLayout();

  CollapsibleInspectorHeader* m_header;

  QWidget* m_inspector;

 public slots:

  void on_headerToggled(bool checked);
};

class CollapsibleInspectorHeader : public QAbstractButton
{
  Q_OBJECT

 public:
  explicit CollapsibleInspectorHeader(QString text, QWidget* parent = nullptr);

  void setChecked(bool isChecked);

 protected:
  void paintEvent(QPaintEvent* event) override;

  QSize sizeHint() const override;

 private:
  void createLayout();

  void setImage(bool isChecked);

  QString m_text;

  QLabel* m_arrowLabel;

 private slots:

  void on_toggled(bool checked);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_COLLAPSIBLEINSPECTOR_HPP
