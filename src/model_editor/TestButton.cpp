/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "TestButton.hpp"

#include <QTimer>

TestButton::TestButton(bool clickValue) : m_clickValue(clickValue) {}

void TestButton::doClick() {
  emit clicked(m_clickValue);
}

void TestButton::doClick(double msecDelay) {
  auto* timer = new QTimer(this);
  timer->setSingleShot(true);
  connect(timer, &QTimer::timeout, this, &TestButton::onTimeout);
  timer->start(msecDelay);
}

void TestButton::onTimeout() {
  emit clicked(m_clickValue);
}
