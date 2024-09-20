/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP
#define OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP

#include "OpenStudioApplicationUtilitiesAPI.hpp"
#include <openstudio/utilities/core/Path.hpp>
#include <vector>

namespace openstudio {

/// \returns The version of the OpenStudio Application
OSAPP_UTILITIES_API std::string openStudioApplicationVersion();

// Return the version in MAJOR.MINOR.PATCH format (eg '3.0.0')
OSAPP_UTILITIES_API std::string openStudioApplicationVersionWithPrerelease();

// Includes the prerelease tag but not the build sha, eg: '1.1.0-alpha'. Should match to a github tag
OSAPP_UTILITIES_API std::string OpenStudioApplicationVersionMajor();

// Includes prerelease tag if any, and build sha, eg: '3.0.0-rc1+baflkdhsia'
OSAPP_UTILITIES_API std::string OpenStudioApplicationVersionMinor();

OSAPP_UTILITIES_API std::string OpenStudioApplicationVersionPatch();

OSAPP_UTILITIES_API std::string OpenStudioApplicationVersionPrerelease();

OSAPP_UTILITIES_API std::string OpenStudioApplicationVersionBuildSHA();

/// \returns The source directory the application was built from
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationSourceDirectory();

/// \returns The directory the application was built in
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationBuildDirectory();

/// \returns The directory that openstudio-coalition-measures are installed in if running from build directory
OSAPP_UTILITIES_API openstudio::path getOpenStudioCoalitionMeasuresSourceDirectory();

/// \returns The path to the current executable application
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationPath();

/// \returns The directory of the current executable application
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationDirectory();

/// \returns True if the application is running from the build directory
OSAPP_UTILITIES_API bool isOpenStudioApplicationRunningFromBuildDirectory();

/// \returns Will return path to the binary containing OpenStudio Utilities, could be openstudio.exe, openstudio.so, etc.
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationModule();

/// \returns Will return dir containing the binary containing OpenStudio Utilities, could be openstudio.exe, openstudio.so, etc.
OSAPP_UTILITIES_API openstudio::path getOpenStudioApplicationModuleDirectory();

/// \returns True if the OpenStudio Module is running from the build directory
OSAPP_UTILITIES_API bool isOpenStudioApplicationModuleRunningFromBuildDirectory();

/// \returns The path to the OpenStudio Command Line Interface  if it exists.
OSAPP_UTILITIES_API openstudio::path getOpenStudioCoreCLI();

}  // namespace openstudio

#endif  //OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP
