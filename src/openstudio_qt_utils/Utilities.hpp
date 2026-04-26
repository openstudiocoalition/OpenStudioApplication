/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIOQTUTILS_UTILITIES_HPP
#define OPENSTUDIOQTUTILS_UTILITIES_HPP

#include <string>
#include <QString>

#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/core/UUID.hpp>
namespace openstudio {
/** QString to UTF-8 encoded std::string. */
std::string toString(const QString& q);

/** QString to wstring. */
std::wstring toWString(const QString& q);

/** UTF-8 encoded std::string to QString. */
QString toQString(const std::string& s);

/** wstring to QString. */
QString toQString(const std::wstring& w);

/// create a UUID from a std::string, does not throw, may return a null UUID
UUID toUUID(const QString& str);

/// create a QString from a UUID
QString toQString(const UUID& uuid);

/** path to QString. */
QString toQString(const path& p);

/** QString to path*/
path toPath(const QString& q);

}  // namespace openstudio

#endif  // OPENSTUDIOQTUTILS_UTILITIES_HPP
