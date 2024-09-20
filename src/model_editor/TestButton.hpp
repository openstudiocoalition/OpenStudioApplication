/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_TESTBUTTON_HPP
#define MODELEDITOR_TESTBUTTON_HPP

#include <QObject>

#include "ModelEditorAPI.hpp"

class MODELEDITOR_API TestButton : public QObject
{
  Q_OBJECT

 public:
  explicit TestButton(bool clickValue = true);
  void doClick();
  void doClick(double msecDelay);
 public slots:
  void onTimeout();
 signals:
  void clicked(bool);

 private:
  bool m_clickValue;
};

#endif  // MODELEDITOR_TESTBUTTON_HPP
