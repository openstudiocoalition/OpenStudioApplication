/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSDragableView.hpp"
#include <QMouseEvent>
#include <QApplication>

namespace openstudio {

OSDragPixmapData::OSDragPixmapData(const QPixmap& t_pixmap, const QPoint& t_hotSpot) : pixmap(t_pixmap), hotSpot(t_hotSpot) {}

OSDragableView::OSDragableView(QWidget* parent) : QWidget(parent), m_mouseDown(false) {}

void OSDragableView::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (m_mouseDown) {
      emit clicked();
      m_mouseDown = false;
    }
  }
}

void OSDragableView::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_mouseDown = true;
    m_dragStartPosition = event->pos();
  }
}

void OSDragableView::mouseMoveEvent(QMouseEvent* event) {
  if (!m_mouseDown) {
    return;
  }

  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
    return;
  }

  m_mouseDown = false;

  QPixmap pixmap(size());
  pixmap.fill(Qt::transparent);

  render(&pixmap, QPoint(), QRegion(), QWidget::RenderFlags(QWidget::DrawChildren));

  OSDragPixmapData dragPixmapData(pixmap, event->pos());

  emit dragRequested(dragPixmapData);
}

void OSDragableView::leaveEvent(QEvent* event) {
  m_mouseDown = false;
}

}  // namespace openstudio
