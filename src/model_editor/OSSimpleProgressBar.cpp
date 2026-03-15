/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSSimpleProgressBar.hpp"

/// constructor
OSSimpleProgressBar::OSSimpleProgressBar(bool visible) : m_visible(visible) {}

/// virtual destructor
OSSimpleProgressBar::~OSSimpleProgressBar() {}

/// get min
int OSSimpleProgressBar::minimum() const {
  return m_minimum;
}

/// set min
void OSSimpleProgressBar::setMinimum(int min) {
  m_minimum = min;
  updatePercentage();
}

/// get max
int OSSimpleProgressBar::maximum() const {
  return m_maximum;
}

/// set max
void OSSimpleProgressBar::setMaximum(int max) {
  m_maximum = max;
  updatePercentage();
}

/// get value
int OSSimpleProgressBar::value() const {
  return m_value;
}

/// get the window title
std::string OSSimpleProgressBar::windowTitle() const {
  return m_windowTitle;
}

/// set the window title
void OSSimpleProgressBar::setWindowTitle(const std::string& title) {
  m_windowTitle = title;
}

/// get the text
std::string OSSimpleProgressBar::text() const {
  return "";
}

/// get if visible
bool OSSimpleProgressBar::isVisible() const {
  return m_visible;
}

/// set if visible
void OSSimpleProgressBar::setVisible(bool visible) {
  m_visible = visible;
}

/// virtual method called every time progressUpdated fires
void OSSimpleProgressBar::onPercentageUpdated(double percentage) {}

