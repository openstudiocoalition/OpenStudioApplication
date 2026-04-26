/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_IGSPINBOXES_HPP
#define MODELEDITOR_IGSPINBOXES_HPP

#include "InspectorGadget.hpp"

#include <QSpinBox>
#include <QDoubleSpinBox>

class QWheelEvent;

class IGSpinBox : public QSpinBox
{
  Q_OBJECT;

 public:
  explicit IGSpinBox(QWidget* parent);

  virtual void wheelEvent(QWheelEvent* event) override;

 public slots:
  void triggered(bool);  //the radio button got triggered and calls this slot
};

class IGDSpinBox : public QDoubleSpinBox
{
  Q_OBJECT;

 public:
  IGDSpinBox(InspectorGadget* ig, QWidget* parent);

  virtual void wheelEvent(QWheelEvent* event) override;

 public slots:
  void triggered(bool);  //the radio button got triggered and calls this slot

 private:
  InspectorGadget* m_ig;
};

#endif  // MODELEDITOR_IGSPINBOXES_HPP
