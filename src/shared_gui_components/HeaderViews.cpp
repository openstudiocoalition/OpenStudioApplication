/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HeaderViews.hpp"
#include <QString>
#include <QLabel>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

namespace openstudio {

LightHeaderToggleButton::LightHeaderToggleButton(QWidget* parent) : QPushButton(parent) {
  setCheckable(true);

  setFixedSize(11, 11);

  QString style;
  style.append("QPushButton { border: none; background: transparent;");
  style.append("background-image: url(\":/images/toggle_arrow_closed.png\"); ");
  style.append(" } ");
  style.append("QPushButton:checked {");
  style.append("background-image: url(\":/images/toggle_arrow.png\"); ");
  style.append(" } ");

  setStyleSheet(style);
}

HeaderToggleButton::HeaderToggleButton(QWidget* parent) : QPushButton(parent) {
  setCheckable(true);

  setFixedSize(11, 11);

  QString style;
  style.append("QPushButton { border: none; background: transparent;");
  style.append("background-image: url(\":/images/toggle_arrow_closed2.png\"); ");
  style.append(" } ");
  style.append("QPushButton:checked { border: none; ");
  style.append("background-image: url(\":/images/toggle_arrow.png\"); ");
  style.append(" } ");

  setStyleSheet(style);
}

DarkGradientHeader::DarkGradientHeader(QWidget* parent) : OSHeader(new LightHeaderToggleButton()) {
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
  setFixedHeight(30);

  QString style;
  style.append("openstudio--DarkGradientHeader { ");
  style.append("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, ");
  style.append("stop: 0 #575757, stop: 0.2 #6C6C6C, stop: 1 #393939); ");
  style.append("} ");
  setStyleSheet(style);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 0, 5, 0);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  mainHLayout->addWidget(toggleButton);

  label = new QLabel();
  style.clear();
  style.append("QLabel { ");
  style.append("font: bold; ");
  style.append("color: white; ");
  style.append("} ");
  label->setStyleSheet(style);
  mainHLayout->addWidget(label);
}

LightGradientHeader::LightGradientHeader(QWidget* parent) : OSHeader(new HeaderToggleButton()) {
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
  setFixedHeight(30);

  QString style;
  style.append("openstudio--LightGradientHeader { ");
  style.append("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, ");
  style.append("stop: 0 #E5E5E5, stop: 1 #807E7E); ");
  style.append("} ");
  setStyleSheet(style);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 0, 5, 0);
  mainHLayout->setSpacing(5);
  setLayout(mainHLayout);

  mainHLayout->addWidget(toggleButton);

  m_measureTypeBadge = new QLabel();
  m_measureTypeBadge->setFixedSize(25, 25);
  m_measureTypeBadge->setVisible(false);
  mainHLayout->addWidget(m_measureTypeBadge);

  label = new QLabel();
  style.clear();
  style.append("QLabel { ");
  style.append("font: bold; ");
  style.append("} ");
  label->setStyleSheet(style);
  mainHLayout->addWidget(label);
}

void LightGradientHeader::setMeasureType(const MeasureType& measureType) {
  bool found = false;

  if (measureType == MeasureType::ModelMeasure) {
    m_measureTypeBadge->setPixmap(QPixmap(":/images/openstudio_measure_icon.png").scaled(25, 25, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    found = true;
  } else if (measureType == MeasureType::EnergyPlusMeasure) {
    m_measureTypeBadge->setPixmap(QPixmap(":/images/energyplus_measure_icon.png").scaled(25, 25, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    found = true;
  } else if (measureType == MeasureType::ReportingMeasure) {
    m_measureTypeBadge->setPixmap(QPixmap(":/images/report_measure_icon.png").scaled(25, 25, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    found = true;
  }

  m_measureTypeBadge->setVisible(found);
}

LightHeader::LightHeader(QWidget* parent) : OSHeader(new HeaderToggleButton()) {
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
  setFixedHeight(30);

  QString style;
  style.append("openstudio--LightHeader { ");
  style.append("background: #E0E0E0; ");
  style.append("} ");
  setStyleSheet(style);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(5, 0, 5, 0);
  setLayout(mainHLayout);

  mainHLayout->addSpacing(15);

  mainHLayout->addWidget(toggleButton);

  label = new QLabel();
  style.clear();
  style.append("QLabel { ");
  style.append("font: bold; ");
  style.append("} ");
  label->setStyleSheet(style);
  mainHLayout->addWidget(label);
}

}  // namespace openstudio
