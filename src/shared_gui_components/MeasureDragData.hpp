/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_MEASUREDRAGDATA_HPP
#define SHAREDGUICOMPONENTS_MEASUREDRAGDATA_HPP

#include "LocalLibrary.hpp"
#include <openstudio/utilities/core/UUID.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <QObject>
#include <QByteArray>

namespace openstudio {

class MeasureDragData : QObject
{
  Q_OBJECT

 public:
  static QString mimeType();

  static QString mimeType(openstudio::MeasureType type);

  explicit MeasureDragData(const QByteArray& data);

  explicit MeasureDragData(const UUID& id);

  virtual ~MeasureDragData() {}

  LocalLibrary::LibrarySource source();

  UUID id();

  QString toXml();

  QByteArray data();

 private:
  UUID m_id;

  LocalLibrary::LibrarySource m_sourceEnum;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_MEASUREDRAGDATA_HPP
