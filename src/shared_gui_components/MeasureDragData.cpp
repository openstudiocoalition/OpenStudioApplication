/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MeasureDragData.hpp"
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>

#include "../model_editor/Utilities.hpp"

namespace openstudio {

QString MeasureDragData::mimeType() {
  return "MeasureDragData";
}

QString MeasureDragData::mimeType(openstudio::MeasureType type) {
  return mimeType() + openstudio::toQString(":" + type.valueName());
}

MeasureDragData::MeasureDragData(const QByteArray& data) : QObject() {
  QString stringData(data);
  QDomDocument doc;
  doc.setContent(stringData);

  QDomElement measureDragDataElement = doc.documentElement();

  // Source Enum

  QDomElement sourceEnumElement = measureDragDataElement.firstChildElement("Source");
  QString sourceEnumString = sourceEnumElement.text();

  if (sourceEnumString == "BCL") {
    m_sourceEnum = LocalLibrary::BCL;
  } else if (sourceEnumString == "USER") {
    m_sourceEnum = LocalLibrary::USER;
  }

  // ID

  QDomElement idElement = measureDragDataElement.firstChildElement("ID");
  QString idString = idElement.text();

  m_id = toUUID(idString);
}

MeasureDragData::MeasureDragData(const UUID& id) : QObject(), m_id(id), m_sourceEnum(LocalLibrary::USER) {}

LocalLibrary::LibrarySource MeasureDragData::source() {
  return m_sourceEnum;
}

UUID MeasureDragData::id() {
  return m_id;
}

QString MeasureDragData::toXml() {
  QDomDocument doc;
  QDomElement measureDragDataElement = doc.createElement("MeasureDragData");
  doc.appendChild(measureDragDataElement);

  // Source Enum

  //QDomElement sourceEnumElement = doc.createElement("Source");
  //measureDragDataElement.appendChild(sourceEnumElement);

  //if( m_sourceEnum == LocalLibraryController::BCL )
  //{
  //  QDomText text = doc.createTextNode("BCL");
  //  sourceEnumElement.appendChild(text);
  //}
  //else if ( m_sourceEnum == LocalLibraryController::USER )
  //{
  //  QDomText text = doc.createTextNode("USER");
  //  sourceEnumElement.appendChild(text);
  //}

  // ID

  QDomElement idElement = doc.createElement("ID");
  measureDragDataElement.appendChild(idElement);

  QDomText idText = doc.createTextNode(toQString(m_id));
  idElement.appendChild(idText);

  return doc.toString();
}

QByteArray MeasureDragData::data() {
  return toXml().toUtf8();
}

}  // namespace openstudio
