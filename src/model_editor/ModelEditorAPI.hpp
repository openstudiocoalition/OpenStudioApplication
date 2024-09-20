/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "QMetaTypes.hpp"

#ifndef MODELEDITOR_MODELEDITORAPI_HPP
#  define MODELEDITOR_MODELEDITORAPI_HPP

#  if (_WIN32 || _MSC_VER) && SHARED_OSAPP_LIBS

#    ifdef openstudio_modeleditor_EXPORTS
#      define MODELEDITOR_API __declspec(dllexport)
#      define MODELEDITOR_TEMPLATE_EXT
#    else
#      define MODELEDITOR_API __declspec(dllimport)
#      define MODELEDITOR_TEMPLATE_EXT extern
#    endif

#  else

#    define MODELEDITOR_API
#    define MODELEDITOR_TEMPLATE_EXT

#  endif

#endif
