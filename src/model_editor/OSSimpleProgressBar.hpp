/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_OSSimpleProgressBar_HPP
#define MODELEDITOR_OSSimpleProgressBar_HPP

#include <openstudio/utilities/plot/ProgressBar.hpp>
#include <openstudio/utilities/core/Macro.hpp>
#include <openstudio/utilities/core/String.hpp>

/** OSProgressBar provides virtual methods setRange, setValue, and setWindowTitle.
  *  This class does not provide any implementation of those methods, they must be implemented by the derived class.
  *
  **/
class OSSimpleProgressBar : public openstudio::ProgressBar
{

 public:
  /// constructor
  explicit OSSimpleProgressBar(bool visible);

  /// virtual destructor
  virtual ~OSSimpleProgressBar();

  /// get min
  int minimum() const override;

  /// set min
  void setMinimum(int min) override;

  /// get max
  int maximum() const override;

  /// set max
  void setMaximum(int max) override;

  /// get value
  int value() const override;

  /// get the window title
  std::string windowTitle() const override;

  /// set the window title
  void setWindowTitle(const std::string& title) override;

  /// get the text
  std::string text() const override;

  /// get if visible
  bool isVisible() const override;

  /// set if visible
  void setVisible(bool visible) override;

  /// virtual method called every time percentageUpdated fires
  void onPercentageUpdated(double percentage) override;

  /// set range
  void setRange(int min, int max) override;

  /// set value
  void setValue(int value) override;

 private:
  int m_minimum{0};
  int m_maximum{1};
  int m_value{0};
  std::string m_windowTitle;
  bool m_visible{false};
};

#endif  //MODELEDITOR_OSSimpleProgressBar_HPP
