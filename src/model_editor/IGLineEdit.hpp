/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_IGLINEEDIT_HPP
#define MODELEDITOR_IGLINEEDIT_HPP

#include "InspectorGadget.hpp"

#include <QLineEdit>

class IGLineEdit : public QLineEdit
{
  Q_OBJECT;

 public:
  IGLineEdit(const QString& val, InspectorGadget* ig, QWidget* parent);
  virtual ~IGLineEdit();

  void setMin(double min);
  void setMax(double max);
  void setDefault(double def);

  bool checkValue(QString& txt);

  QString doubleToQString(double v);
  //QString intToQString( int v );

 public slots:

  void editDone();

  void hardsizeClicked(bool checked);

 signals:

  void newValue(const QString& text);

 private:
  InspectorGadget* m_ig;

  double m_min;
  double m_max;
  double m_default;
};

#endif  // MODELEDITOR_IGLINEEDIT_HPP
