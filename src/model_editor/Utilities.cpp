/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "Utilities.hpp"

namespace openstudio {
/** QString to UTF-8 encoded std::string. */
std::string toString(const QString& q) {
  const QByteArray& qb = q.toUtf8();
  return std::string(qb.data());
}

/** QString to wstring. */
std::wstring toWString(const QString& q) {
#if (defined(_WIN32) || defined(_WIN64))
  static_assert(sizeof(wchar_t) == sizeof(unsigned short), "Wide characters must have the same size as unsigned shorts");
  std::wstring w(reinterpret_cast<const wchar_t*>(q.utf16()), q.length());
  return w;
#else
  std::wstring w = q.toStdWString();
  return w;
#endif
}

/** UTF-8 encoded std::string to QString. */
QString toQString(const std::string& s) {
  return QString::fromUtf8(s.c_str());
}

/** wstring to QString. */
QString toQString(const std::wstring& w) {
#if (defined(_WIN32) || defined(_WIN64))
  static_assert(sizeof(wchar_t) == sizeof(char16_t), "Wide characters must have the same size as char16_t");
  return QString::fromUtf16(reinterpret_cast<const char16_t*>(w.data()), w.length());
#else
  return QString::fromStdWString(w);
#endif
}

UUID toUUID(const QString& str) {
  return toUUID(toString(str));
}

QString toQString(const UUID& uuid) {
  return toQString(toString(uuid));
}

/** path to QString. */
QString toQString(const path& p) {
#if (defined(_WIN32) || defined(_WIN64))
  return QString::fromStdWString(p.generic_wstring());
#else
  return QString::fromUtf8(p.generic_string().c_str());
#endif
}

/** QString to path*/
path toPath(const QString& q) {
#if (defined(_WIN32) || defined(_WIN64))
  return path(q.toStdWString());
#else
  return path(q.toStdString());
#endif
}

}  // namespace openstudio
