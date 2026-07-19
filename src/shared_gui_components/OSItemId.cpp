/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSItemId.hpp"

#include <QMimeData>

namespace openstudio {

const QString OSItemId::BCL_SOURCE_ID = QString("BCL");

OSItemId::OSItemId() : m_isDefaulted(false) {}

OSItemId::OSItemId(const QString& itemId, const QString& sourceId, bool isDefaulted, const QString& otherData)
  : m_itemId(itemId), m_sourceId(sourceId), m_otherData(otherData), m_isDefaulted(isDefaulted) {}

OSItemId::OSItemId(const QMimeData* mimeData) : m_isDefaulted(false) {
  QStringList strings = mimeData->text().split(",");
  if (!strings.empty()) {
    m_itemId = strings[0];
  }
  if (strings.size() > 1) {
    m_sourceId = strings[1];
  }
  if (strings.size() > 2) {
    m_isDefaulted = (strings[2] == "True");
  }
  if (strings.size() > 3) {
    if (strings[3] == "None") {
      m_position_.reset();
    } else {
      m_position_ = strings[3].toInt();
    }
  }
  for (int i = 4; i < strings.size(); ++i) {
    m_otherData += strings[i];
    if (i < strings.size() - 1) {
      m_otherData += ",";
    }
  }
}

QString OSItemId::itemId() const {
  return m_itemId;
}

QString OSItemId::sourceId() const {
  return m_sourceId;
}

QString OSItemId::otherData() const {
  return m_otherData;
}

QString OSItemId::mimeDataText() const {
  QString isDefaultedString((m_isDefaulted ? "True" : "False"));
  QString positonString((m_position_ ? QString::number(m_position_.get()) : "None"));
  QString result = m_itemId + "," + m_sourceId + "," + isDefaultedString + "," + positonString + "," + m_otherData;
  return result;
}

bool OSItemId::isDefaulted() const {
  return m_isDefaulted;
}

void OSItemId::setIsDefaulted(bool isDefaulted) {
  m_isDefaulted = isDefaulted;
}

boost::optional<int> OSItemId::position() const {
  return m_position_;
}

void OSItemId::setPosition(int position) {
  m_position_ = position;
}

bool OSItemId::operator==(const OSItemId& other) const {
  bool result = false;
  if (!this->mimeDataText().isEmpty()) {
    result = (this->mimeDataText() == other.mimeDataText());
  }
  return result;
}

}  // namespace openstudio
