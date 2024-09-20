/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OPENSTUDIOAPI_HPP
#define OPENSTUDIO_OPENSTUDIOAPI_HPP

#if (_WIN32 || _MSC_VER) && SHARED_OSAPP_LIBS

#  ifdef openstudio_lib_EXPORTS
#    define OPENSTUDIO_API __declspec(dllexport)
#  else
#    define OPENSTUDIO_API __declspec(dllimport)
#  endif
#else
#  define OPENSTUDIO_API
#endif

#endif
