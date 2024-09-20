/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BUSYWIDGET_HPP
#define SHAREDGUICOMPONENTS_BUSYWIDGET_HPP

#include <QWidget>
#include <boost/smart_ptr.hpp>

class QPixmap;

class QPaintEvent;

namespace openstudio {

class BusyWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit BusyWidget(QWidget* parent = nullptr);

  virtual ~BusyWidget() {}

  const QSize sizeHint();

 public slots:

  void rotate();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  std::shared_ptr<QPixmap> m_pixmap;

  float m_rotation;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BUSYWIDGET_HPP
