/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BridgeClasses.hpp"

ComboHighlightBridge::ComboHighlightBridge(QObject* parent) : QObject(parent), m_source(nullptr) {}

QObject* ComboHighlightBridge::bridgeSender() {
  return m_source;
}

void ComboHighlightBridge::highlighted(const QString& val) {
  m_source = sender();
  emit highlightEmitted(val);
}

void ComboHighlightBridge::activated(const QString& val) {
  m_source = sender();
  emit activatedEmitted(val);
}
