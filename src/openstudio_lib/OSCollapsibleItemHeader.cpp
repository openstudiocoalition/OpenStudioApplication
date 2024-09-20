/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSCollapsibleItemHeader.hpp"

#include "OSItem.hpp"
#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

#include <bitset>

namespace openstudio {

OSCollapsibleItemHeader::OSCollapsibleItemHeader(const std::string& text, const OSItemId& itemId, OSItemType type, QWidget* parent)
  : QWidget(parent), m_mouseDown(false) {
  setFixedHeight(50);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setObjectName("OSCollapsibleItemHeader");

  this->setProperty("style", "0");
  this->setStyleSheet("QWidget#OSCollapsibleItemHeader[style=\"0\"] { background: #CECECE; border-bottom: 1px solid black;}"  // Selected=0
                      "QWidget#OSCollapsibleItemHeader[style=\"1\"] { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"       // Selected=1
                      "  stop: 0.0 #636161, stop: 0.10 #636161, stop: 0.15 #A3A3A3, stop: 1.0 #A3A3A3);  border-bottom: 1px solid black; }");

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(9, 0, 9, 0);
  setLayout(mainHLayout);

  // Label

  m_textLabel = new QLabel(QString::fromStdString(text));
  m_textLabel->setWordWrap(true);
  m_textLabel->setObjectName("H2");
  mainHLayout->addWidget(m_textLabel, 10);

  mainHLayout->addStretch();

  // Arrow

  m_arrowLabel = new QLabel();
  mainHLayout->addWidget(m_arrowLabel);

  setSelected(false);

  setExpanded(false);
}

QSize OSCollapsibleItemHeader::sizeHint() const {
  return QSize(150, 50);
}

QString OSCollapsibleItemHeader::text() const {
  return m_textLabel->text();
}

bool OSCollapsibleItemHeader::expanded() const {
  return m_expanded;
}

void OSCollapsibleItemHeader::setExpanded(bool isExpanded) {
  m_expanded = isExpanded;
  setImage(m_expanded);
}

void OSCollapsibleItemHeader::setImage(bool expanded) {
  if (m_expanded) {
    m_arrowLabel->setPixmap(QPixmap(":/images/toggle_arrow.png"));
  } else {
    m_arrowLabel->setPixmap(QPixmap(":/images/toggle_arrow_closed.png"));
  }
}

bool OSCollapsibleItemHeader::selected() const {
  return m_selected;
}

void OSCollapsibleItemHeader::setSelected(bool isSelected) {
  m_selected = isSelected;

  // Selected
  std::bitset<1> style;
  style[0] = isSelected;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

void OSCollapsibleItemHeader::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OSCollapsibleItemHeader::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (m_mouseDown) {
      emit clicked(this);
      setSelected(true);
      m_mouseDown = false;
    }
  }
}

void OSCollapsibleItemHeader::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_mouseDown = true;
  }
}

void OSCollapsibleItemHeader::leaveEvent(QEvent* event) {
  m_mouseDown = false;
}

}  // namespace openstudio
