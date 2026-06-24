/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIOQTUTILS_OSPROGRESSBAR_HPP
#define OPENSTUDIOQTUTILS_OSPROGRESSBAR_HPP

#include <openstudio/utilities/plot/ProgressBar.hpp>
#include <openstudio/utilities/core/Macro.hpp>
#include <openstudio/utilities/core/String.hpp>

#include <QProgressBar>
#include <memory>

class QWidget;

/** OSProgressBar wraps a QProgressBar and provides virtual methods setRange, setValue, and setWindowTitle(QString)
  *  which may be overridden.
  *
  *  OSProgressBar an atypical QObject because it is designed to be stack allocated.  In many cases it
  *  would be preferred to connect your own heap allocated QObject to the signals directly rather
  *  than using this convenience class.
  **/
class OSProgressBar : public openstudio::ProgressBar
{

 public:
  /// constructor
  explicit OSProgressBar(QWidget* parent = nullptr);

  /// constructor
  explicit OSProgressBar(bool visible, QWidget* parent = nullptr);

  /// virtual destructor
  virtual ~OSProgressBar();

  /// get min
  int minimum() const;

  /// set min
  void setMinimum(int min);

  /// get max
  int maximum() const;

  /// set max
  void setMaximum(int max);

  /// get value
  int value() const;

  /// get the window title
  std::string windowTitle() const;

  /// set the window title
  void setWindowTitle(const std::string& title);

  /// get the text
  std::string text() const;

  /// get if visible
  bool isVisible() const;

  /// set if visible
  void setVisible(bool visible);

  /// virtual method called every time percentageUpdated fires
  virtual void onPercentageUpdated(double percentage);

  // public slots:

  /// set range
  void setRange(int min, int max);

  /// set value
  void setValue(int value);

  /// set window title
  void setWindowTitle(const QString& windowTitle);

 private:
  /// impl
  std::shared_ptr<QProgressBar> m_impl;
};

#endif  // OPENSTUDIOQTUTILS_OSPROGRESSBAR_HPP
