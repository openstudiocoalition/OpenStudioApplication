/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_UTILITIES_HPP
#define MODELEDITOR_UTILITIES_HPP

#include <string>
#include <QString>

#include "ModelEditorAPI.hpp"

namespace openstudio {
/** QString to UTF-8 encoded std::string. */
MODELEDITOR_API std::string toString(const QString& q);

/** QString to wstring. */
MODELEDITOR_API std::wstring toWString(const QString& q);

/** UTF-8 encoded std::string to QString. */
MODELEDITOR_API QString toQString(const std::string& s);

/** wstring to QString. */
MODELEDITOR_API QString toQString(const std::wstring& w);

/// create a UUID from a std::string, does not throw, may return a null UUID
MODELEDITOR_API UUID toUUID(const QString& str);

/// create a QString from a UUID
MODELEDITOR_API QString toQString(const UUID& uuid);

/** path to QString. */
MODELEDITOR_API QString toQString(const path& p);

/** QString to path*/
MODELEDITOR_API path toPath(const QString& q);

}  // namespace openstudio

#endif
