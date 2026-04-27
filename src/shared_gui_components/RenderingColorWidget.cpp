/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "RenderingColorWidget.hpp"

#include <openstudio/model/RenderingColor_Impl.hpp>

#include <QBoxLayout>
#include <QColor>
#include <QColorDialog>
#include <QLabel>
#include <QPushButton>

namespace openstudio {

RenderingColorWidget::RenderingColorWidget(QWidget* parent) : QWidget(parent) {
  this->setObjectName("GrayWidget");

  auto* hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(10);
  this->setLayout(hLayout);

  auto* renderingColorLabel = new QLabel();
  renderingColorLabel->setText("Rendering Color: ");
  renderingColorLabel->setStyleSheet("QLabel { font: bold; }");
  hLayout->addWidget(renderingColorLabel);

  m_renderColorWidget = new QWidget();
  m_renderColorWidget->setFixedHeight(30);
  m_renderColorWidget->setFixedWidth(30);
  hLayout->addWidget(m_renderColorWidget);

  m_renderColorButton = new QPushButton();
  m_renderColorButton->setFlat(true);
  m_renderColorButton->setText("Select Color");
  m_renderColorButton->setObjectName("StandardGrayButton");
  hLayout->addWidget(m_renderColorButton);
  hLayout->addStretch();

  connect(m_renderColorButton, &QPushButton::clicked, this, &RenderingColorWidget::renderColorButtonClicked);
}

void RenderingColorWidget::attach(const openstudio::model::RenderingColor& renderingColor) {
  detach();

  m_renderingColor = renderingColor;

  m_renderingColor->getImpl<model::detail::ModelObject_Impl>()->onChange.connect<RenderingColorWidget, &RenderingColorWidget::refresh>(this);

  refresh();
}

void RenderingColorWidget::detach() {
  clear();

  if (m_renderingColor) {
    m_renderingColor->getImpl<model::detail::ModelObject_Impl>()->onChange.disconnect<RenderingColorWidget, &RenderingColorWidget::refresh>(this);
    m_renderingColor.reset();
  }
}

void RenderingColorWidget::clear() {
  QString style = "QWidget { background-color : rgba(255,255,255,255);}";
  m_renderColorWidget->setStyleSheet(style);
  m_renderColorButton->setEnabled(false);
}

void RenderingColorWidget::refresh() {
  clear();

  if (m_renderingColor) {

    int r = m_renderingColor->renderingRedValue();
    int g = m_renderingColor->renderingGreenValue();
    int b = m_renderingColor->renderingBlueValue();
    int a = m_renderingColor->renderingAlphaValue();
    QString style = "QWidget { background-color : rgba(" + QString::number(r) + "," + QString::number(g) + ", " + QString::number(b) + ", "
                    + QString::number(a) + ");}";
    m_renderColorWidget->setStyleSheet(style);
    m_renderColorButton->setEnabled(true);
  }
}

void RenderingColorWidget::renderColorButtonClicked() {
  if (m_renderingColor) {
    int r = m_renderingColor->renderingRedValue();
    int g = m_renderingColor->renderingGreenValue();
    int b = m_renderingColor->renderingBlueValue();
    int a = m_renderingColor->renderingAlphaValue();
    QColor initialColor = QColor(r, g, b, a);

    QColor color = QColorDialog::getColor(initialColor, this, "Choose Rendering Color", QColorDialog::ShowAlphaChannel);

    if (color.isValid()) {
      m_renderingColor->setRenderingRedValue(color.red());
      m_renderingColor->setRenderingGreenValue(color.green());
      m_renderingColor->setRenderingBlueValue(color.blue());
      m_renderingColor->setRenderingAlphaValue(color.alpha());

      refresh();
    }
  }
}

}  // namespace openstudio
