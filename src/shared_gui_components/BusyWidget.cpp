/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BusyWidget.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QSize>

/********************************************************************************/
// TODO this class exists in PAT.  Once PAT is in the OpenStudio project,
// the PAT implementation of BusyWidget should be used.
// Note that QPixmap pixmap = QPixmap(":/images/rotating_arrow.png").scaled(25,25)
// should not scale-down the image for the OS App application
/********************************************************************************/

#include <QStyleOption>

namespace openstudio {

BusyWidget::BusyWidget(QWidget* parent) : QWidget(parent) {
  //QPixmap pixmap = QPixmap(":/images/rotating_arrow.png").scaled(25,25);
  QPixmap pixmap = QPixmap(":/images/rotating_arrow.png");

  m_pixmap = std::shared_ptr<QPixmap>(new QPixmap(pixmap));

  m_rotation = 0.0;

  setMinimumSize(m_pixmap->width(), m_pixmap->height());
}

void BusyWidget::rotate() {
  m_rotation = m_rotation + 5.0;

  update();
}

void BusyWidget::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setRenderHint(QPainter::Antialiasing, true);

  int transX = m_pixmap->width() / 2;
  int transY = m_pixmap->height() / 2;

  painter.translate(transX, transY);

  painter.rotate(m_rotation);

  painter.drawPixmap(-transX, -transY, *m_pixmap);
}

const QSize BusyWidget::sizeHint() {
  return QSize(m_pixmap->width(), m_pixmap->height());
}

}  // namespace openstudio
