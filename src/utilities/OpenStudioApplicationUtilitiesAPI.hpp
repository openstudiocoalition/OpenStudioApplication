/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OSAPP_UTILITIES_OSAPP_UTILITIESAPI_HPP
#define OSAPP_UTILITIES_OSAPP_UTILITIESAPI_HPP

#if (_WIN32 || _MSC_VER) && SHARED_OSAPP_LIBS

#  ifdef openstudioapp_utilities_EXPORTS
#    define OSAPP_UTILITIES_API __declspec(dllexport)
#  else
#    define OSAPP_UTILITIES_API __declspec(dllimport)
#  endif
#else
#  define OSAPP_UTILITIES_API
#endif

#endif
