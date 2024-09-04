/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "PlanarSurfaceWidget.hpp"

#include <openstudio/model/PlanarSurface_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>

#define FEET_PER_METER 3.280839895013123

namespace openstudio {

PlanarSurfaceWidget::PlanarSurfaceWidget(bool isIP, QWidget* parent) : QWidget(parent) {
  this->setObjectName("GrayWidget");

  setUnits(isIP);

  auto* layout = new QGridLayout();
  layout->setContentsMargins(7, 7, 7, 7);
  layout->setSpacing(7);
  this->setLayout(layout);
}

void PlanarSurfaceWidget::attach(const openstudio::model::PlanarSurface& planarSurface) {
  detach();

  m_planarSurface = planarSurface;

  m_planarSurface->getImpl<model::detail::ModelObject_Impl>()->onChange.connect<PlanarSurfaceWidget, &PlanarSurfaceWidget::refresh>(this);

  refresh();
}

void PlanarSurfaceWidget::detach() {
  clear();

  if (m_planarSurface) {
    // disconnect(m_planarSurface->getImpl<model::detail::ModelObject_Impl>().get());
    m_planarSurface->getImpl<model::detail::ModelObject_Impl>()->onChange.disconnect<PlanarSurfaceWidget, &PlanarSurfaceWidget::refresh>(this);
    m_planarSurface.reset();
  }
}

void PlanarSurfaceWidget::clear() {
  QLayoutItem* child;
  while ((child = this->layout()->takeAt(0)) != nullptr) {
    QWidget* widget = child->widget();
    if (widget) {
      delete widget;
    }
    delete child;
  }
}

void PlanarSurfaceWidget::refresh() {
  clear();

  if (m_planarSurface) {

    double multiplicationFactor = 1;
    QString units;
    if (m_units == "SI") {
      units = "m";
    } else if (m_units == "IP") {
      units = "ft";
      multiplicationFactor = FEET_PER_METER;
    }

    auto* layout = qobject_cast<QGridLayout*>(this->layout());

    auto* label = new QLabel();
    label->setText("Number");
    label->setStyleSheet("QLabel { font: bold; }");
    layout->addWidget(label, 0, 0);

    QString string;

    string = "x (";
    string += units;
    string += ")";

    label = new QLabel();
    label->setText(string);
    label->setStyleSheet("QLabel { font: bold; }");
    layout->addWidget(label, 0, 1);

    string = "y (";
    string += units;
    string += ")";

    label = new QLabel();
    label->setText(string);
    label->setStyleSheet("QLabel { font: bold; }");
    layout->addWidget(label, 0, 2);

    string = "z (";
    string += units;
    string += ")";

    label = new QLabel();
    label->setText(string);
    label->setStyleSheet("QLabel { font: bold; }");
    layout->addWidget(label, 0, 3);

    std::vector<openstudio::Point3d> vertices = m_planarSurface->vertices();
    for (unsigned i = 0; i < vertices.size(); ++i) {

      int n = i + 1;

      label = new QLabel();
      label->setText(QString::number(n));
      layout->addWidget(label, n, 0);

      label = new QLabel();
      label->setText(QString::number(vertices[i].x() * multiplicationFactor));
      layout->addWidget(label, n, 1);

      label = new QLabel();
      label->setText(QString::number(vertices[i].y() * multiplicationFactor));
      layout->addWidget(label, n, 2);

      label = new QLabel();
      label->setText(QString::number(vertices[i].z() * multiplicationFactor));
      layout->addWidget(label, n, 3);
    }
  }
}

void PlanarSurfaceWidget::setUnits(bool displayIP) {
  if (displayIP) {
    m_units = "IP";
  } else {
    m_units = "SI";
  }
}

void PlanarSurfaceWidget::toggleUnits(bool displayIP) {
  setUnits(displayIP);

  refresh();
}

}  // namespace openstudio
