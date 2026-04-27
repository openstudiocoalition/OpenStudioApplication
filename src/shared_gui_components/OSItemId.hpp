/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSITEMID_HPP
#define SHAREDGUICOMPONENTS_OSITEMID_HPP

#include <QString>
#include <boost/optional.hpp>
#include <QMetaType>

class QMimeData;

namespace openstudio {

class OSItemId
{
 public:
  static const QString BCL_SOURCE_ID;
  OSItemId();
  OSItemId(const QString& itemId, const QString& sourceId, bool isDefaulted, const QString& otherData = "");
  explicit OSItemId(const QMimeData* mimeData);
  QString itemId() const;
  QString sourceId() const;
  QString otherData() const;
  QString mimeDataText() const;

  bool isDefaulted() const;
  void setIsDefaulted(bool isDefaulted);

  boost::optional<int> position() const;
  void setPosition(int position);

  bool operator==(const OSItemId& other) const;

 private:
  QString m_itemId;
  QString m_sourceId;
  QString m_otherData;
  bool m_isDefaulted;
  boost::optional<int> m_position_;
};

}  // namespace openstudio

Q_DECLARE_METATYPE(openstudio::OSItemId)
Q_DECLARE_METATYPE(std::vector<openstudio::OSItemId>)

#endif  // SHAREDGUICOMPONENTS_OSITEMID_HPP
