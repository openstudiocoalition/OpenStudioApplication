/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIOQTUTILS_API_HPP
#define OPENSTUDIOQTUTILS_API_HPP

#if (_WIN32 || _MSC_VER) && SHARED_OSAPP_LIBS

#  ifdef openstudio_qt_utils_EXPORTS
#    define OPENSTUDIOQTUTILS_API __declspec(dllexport)
#    define OPENSTUDIOQTUTILS_TEMPLATE_EXT
#  else
#    define OPENSTUDIOQTUTILS_API __declspec(dllimport)
#    define OPENSTUDIOQTUTILS_TEMPLATE_EXT extern
#  endif

#else

#  define OPENSTUDIOQTUTILS_API
#  define OPENSTUDIOQTUTILS_TEMPLATE_EXT

#endif

#endif  // OPENSTUDIOQTUTILS_API_HPP
