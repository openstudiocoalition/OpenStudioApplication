/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "IGSpinBoxes.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QWheelEvent>
#include <string>

using std::string;

IGSpinBox::IGSpinBox(QWidget* parent) : QSpinBox(parent) {}
void IGSpinBox::wheelEvent(QWheelEvent* event) {
  event->ignore();
}

void IGSpinBox::triggered(bool trig) {
  if (trig) {
    emit textChanged(QString("%1").arg(value()));
  } else {
    emit textChanged("AutoSize");
  }
}

IGDSpinBox::IGDSpinBox(InspectorGadget* ig, QWidget* parent) : QDoubleSpinBox(parent), m_ig(ig) {}

void IGDSpinBox::wheelEvent(QWheelEvent* event) {
  event->ignore();
}

void IGDSpinBox::triggered(bool trig) {
  if (trig) {
    emit textChanged(QString("%1").arg(value()));
  } else {
    emit textChanged("AutoSize");
  }
}
