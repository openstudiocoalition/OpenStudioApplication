/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSDRAGABLEVIEW_HPP
#define SHAREDGUICOMPONENTS_OSDRAGABLEVIEW_HPP

#include <QWidget>

namespace openstudio {

class OSDragPixmapData
{
 public:
  OSDragPixmapData(const QPixmap& t_pixmap, const QPoint& t_hotSpot);

  virtual ~OSDragPixmapData() {}

  QPixmap pixmap;

  QPoint hotSpot;
};

class OSDragableView : public QWidget
{
  Q_OBJECT

 public:
  explicit OSDragableView(QWidget* parent = nullptr);

  virtual ~OSDragableView() {}

 signals:

  void dragRequested(const OSDragPixmapData& dragPixmapData);

  void clicked();

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void leaveEvent(QEvent* event) override;

 private:
  bool m_mouseDown;

  QPoint m_dragStartPosition;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSDRAGABLEVIEW_HPP
