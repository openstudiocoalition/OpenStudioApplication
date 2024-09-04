/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSCategoryPlaceholder.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

#include <iostream>

namespace openstudio {

OSCategoryPlaceholder::OSCategoryPlaceholder(const std::string& text, QWidget* parent) : QWidget(parent) {
  setFixedHeight(40);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setObjectName("OSCategoryPlaceholder");

  this->setProperty("style", "0");
  this->setStyleSheet("QWidget#OSCategoryPlaceholder[style=\"0\"] { background-color: #95B3DE; border-bottom: 1px solid black; }");

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(9, 0, 9, 0);
  setLayout(mainHLayout);

  // Label

  m_textLabel = new QLabel(QString::fromStdString(text));
  m_textLabel->setWordWrap(true);
  m_textLabel->setObjectName("OSCategoryPlaceholderText");
  m_textLabel->setStyleSheet("QLabel#OSCategoryPlaceholderText { font-size: 14px; color: white; }");
  mainHLayout->addWidget(m_textLabel, 10);

  mainHLayout->addStretch();
}

QString OSCategoryPlaceholder::text() const {
  return m_textLabel->text();
}

void OSCategoryPlaceholder::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
