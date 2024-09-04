/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "CollapsibleInspector.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

namespace openstudio {

CollapsibleInspector::CollapsibleInspector(QString text, QWidget* inspector, QWidget* parent)
  : QWidget(parent), m_header(new CollapsibleInspectorHeader(std::move(text))), m_inspector(inspector) {
  createLayout();
}

void CollapsibleInspector::createLayout() {
  setContentsMargins(0, 0, 0, 0);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);

  mainLayout->addWidget(m_header);
  mainLayout->addWidget(m_inspector);

  connect(m_header, &CollapsibleInspectorHeader::toggled, this, &CollapsibleInspector::on_headerToggled);
}

///! SLOTS

void CollapsibleInspector::on_headerToggled(bool checked) {
  if (checked) {
    m_inspector->show();
  } else {
    m_inspector->hide();
  }
}

////////////////////////////////////////////////////////////////////////////////

CollapsibleInspectorHeader::CollapsibleInspectorHeader(QString text, QWidget* parent) : QAbstractButton(parent), m_text(std::move(text)) {
  createLayout();
}

void CollapsibleInspectorHeader::createLayout() {
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(50);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setObjectName("CollapsibleInspectorHeader");
  setCheckable(true);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(10, 0, 10, 0);
  mainHLayout->setSpacing(7);
  setLayout(mainHLayout);

  // Arrow
  m_arrowLabel = new QLabel();
  mainHLayout->addWidget(m_arrowLabel, 0, Qt::AlignLeft);

  // Name
  auto* textLabel = new QLabel(m_text);
  textLabel->setWordWrap(false);
  textLabel->setObjectName("H2");
  mainHLayout->addWidget(textLabel, 0, Qt::AlignLeft);

  // Stretch
  mainHLayout->addStretch();

  connect(this, &CollapsibleInspectorHeader::toggled, this, &CollapsibleInspectorHeader::on_toggled);

  setChecked(false);
}

QSize CollapsibleInspectorHeader::sizeHint() const {
  return {150, 50};
}

void CollapsibleInspectorHeader::setImage(bool isChecked) {
  if (isChecked) {
    m_arrowLabel->setPixmap(QPixmap(":/images/toggle_arrow.png"));
  } else {
    m_arrowLabel->setPixmap(QPixmap(":/images/toggle_arrow_closed2.png"));
  }
}

void CollapsibleInspectorHeader::setChecked(bool isChecked) {
  QAbstractButton::setChecked(isChecked);
  setImage(isChecked);
  if (isChecked) {
    QString style;
    style.append("QWidget#CollapsibleInspectorHeader {");
    style.append(" background: qlineargradient(x1:0,y1:0,x2:0,y2:1,");
    style.append(" stop: 0.0 #636161,");
    style.append(" stop: 0.10 #636161,");
    style.append(" stop: 0.15 #A3A3A3,");
    style.append(" stop: 1.0 #A3A3A3);");
    style.append(" border-bottom: 1px solid black;");
    style.append(" }");
    setStyleSheet(style);
  } else {
    setStyleSheet("QWidget#CollapsibleInspectorHeader { background: #CECECE; border-bottom: 1px solid black;}");
  }
}

void CollapsibleInspectorHeader::paintEvent(QPaintEvent* /* event */) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter painter(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

///! SLOTS

void CollapsibleInspectorHeader::on_toggled(bool checked) {
  setChecked(checked);
}

}  // namespace openstudio
