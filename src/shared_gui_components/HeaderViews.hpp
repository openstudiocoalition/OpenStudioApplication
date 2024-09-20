/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_HEADERVIEWS_HPP
#define SHAREDGUICOMPONENTS_HEADERVIEWS_HPP

#include "OSCollapsibleView.hpp"
#include <QPushButton>

class QLabel;

namespace openstudio {

class MeasureType;

class LightHeaderToggleButton : public QPushButton
{
 public:
  explicit LightHeaderToggleButton(QWidget* parent = nullptr);
  virtual ~LightHeaderToggleButton() {}
};

class HeaderToggleButton : public QPushButton
{
 public:
  explicit HeaderToggleButton(QWidget* parent = nullptr);
  virtual ~HeaderToggleButton() {}
};

class DarkGradientHeader : public OSHeader
{
  Q_OBJECT

 public:
  QLabel* label;

  explicit DarkGradientHeader(QWidget* parent = nullptr);
  virtual ~DarkGradientHeader() {}
};

class LightGradientHeader : public OSHeader
{
  Q_OBJECT

 public:
  QLabel* label;

  explicit LightGradientHeader(QWidget* parent = nullptr);
  void setMeasureType(const MeasureType& measureType);
  virtual ~LightGradientHeader() {}

 private:
  QLabel* m_measureTypeBadge;
};

class LightHeader : public OSHeader
{
  Q_OBJECT

 public:
  QLabel* label;

  explicit LightHeader(QWidget* parent = nullptr);
  virtual ~LightHeader() {}
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_HEADERVIEWS_HPP
