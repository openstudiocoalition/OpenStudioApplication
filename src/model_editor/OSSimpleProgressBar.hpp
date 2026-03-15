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
  virtual int minimum() const;

  /// set min
  virtual void setMinimum(int min);

  /// get max
  virtual int maximum() const;

  /// set max
  virtual void setMaximum(int max);

  /// get value
  virtual int value() const;

  /// get the window title
  virtual std::string windowTitle() const;

  /// set the window title
  virtual void setWindowTitle(const std::string& title);

  /// get the text
  virtual std::string text() const;

  /// get if visible
  virtual bool isVisible() const;

  /// set if visible
  virtual void setVisible(bool visible);

  /// virtual method called every time percentageUpdated fires
  virtual void onPercentageUpdated(double percentage);

 private:
  int m_minimum{0};
  int m_maximum{1};
  int m_value{0};
  std::string m_windowTitle;
  bool m_visible{false};
};

#endif  //MODELEDITOR_OSSimpleProgressBar_HPP
