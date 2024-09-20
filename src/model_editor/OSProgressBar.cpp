/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSProgressBar.hpp"

#include "Application.hpp"
#include "Utilities.hpp"

#include <cmath>
#include <openstudio/nano/nano_signal_slot.hpp>

/// constructor
OSProgressBar::OSProgressBar(QWidget* parent) {
  // make sure a QApplication exists
  openstudio::Application::instance().application(true);

  // create a QProgressBar
  m_impl = std::shared_ptr<QProgressBar>(new QProgressBar(parent));

  // if no parent assume we want this visible
  if (parent == nullptr) {
    m_impl->setVisible(true);
  }

  updatePercentage();
}

/// constructor
OSProgressBar::OSProgressBar(bool visible, QWidget* parent) {
  // make sure a QApplication exists
  openstudio::Application::instance().application(true);

  // create a QProgressBar
  m_impl = std::shared_ptr<QProgressBar>(new QProgressBar(parent));

  // set visibility
  m_impl->setVisible(visible);

  updatePercentage();
}

/// constructor from impl
//OSProgressBar::OSProgressBar(const std::shared_ptr<QProgressBar>& impl)
//  : m_impl(impl), m_percentage(0.0)
//{}

/// virtual destructor
OSProgressBar::~OSProgressBar() {
  m_impl->setVisible(false);
}

/// get min
int OSProgressBar::minimum() const {
  return m_impl->minimum();
}

/// set min
void OSProgressBar::setMinimum(int min) {
  m_impl->setMinimum(min);
  updatePercentage();
}

/// get max
int OSProgressBar::maximum() const {
  return m_impl->maximum();
}

/// set max
void OSProgressBar::setMaximum(int max) {
  m_impl->setMaximum(max);
  updatePercentage();
}

/// get value
int OSProgressBar::value() const {
  return m_impl->value();
}

/// get the window title
std::string OSProgressBar::windowTitle() const {
  return openstudio::toString(m_impl->windowTitle());
}

/// set the window title
void OSProgressBar::setWindowTitle(const std::string& title) {
  this->setWindowTitle(openstudio::toQString(title));
}

/// get the text
std::string OSProgressBar::text() const {
  return openstudio::toString(m_impl->text());
}

/// get if visible
bool OSProgressBar::isVisible() const {
  return m_impl->isVisible();
}

/// set if visible
void OSProgressBar::setVisible(bool visible) {
  m_impl->setVisible(visible);
}

/// virtual method called every time progressUpdated fires
void OSProgressBar::onPercentageUpdated(double percentage) {}

/// set range
void OSProgressBar::setRange(int min, int max) {
  m_impl->setMinimum(min);
  m_impl->setMaximum(max);
  updatePercentage();
}

/// set value
void OSProgressBar::setValue(int value) {
  m_impl->setValue(value);
  updatePercentage();
}

/// set window title
void OSProgressBar::setWindowTitle(const QString& windowTitle) {
  m_impl->setWindowTitle(windowTitle);
}

//std::shared_ptr<QProgressBar> OSProgressBar::impl() const
//{
//  return m_impl;
//}
