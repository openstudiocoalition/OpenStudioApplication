/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP
#define OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP

#include <openstudio/utilities/core/Path.hpp>
#include <vector>

namespace openstudio {

/// \returns The version of the OpenStudio Application
std::string openStudioApplicationVersion();

// Return the version in MAJOR.MINOR.PATCH format (eg '3.0.0')
std::string openStudioApplicationVersionWithPrerelease();

// Includes the prerelease tag but not the build sha, eg: '1.1.0-alpha'. Should match to a github tag
std::string OpenStudioApplicationVersionMajor();

// Includes prerelease tag if any, and build sha, eg: '3.0.0-rc1+baflkdhsia'
std::string OpenStudioApplicationVersionMinor();

std::string OpenStudioApplicationVersionPatch();

std::string OpenStudioApplicationVersionPrerelease();

std::string OpenStudioApplicationVersionBuildSHA();

/// \returns The source directory the application was built from
openstudio::path getOpenStudioApplicationSourceDirectory();

/// \returns The directory the application was built in
openstudio::path getOpenStudioApplicationBuildDirectory();

/// \returns The directory that openstudio-coalition-measures are installed in if running from build directory
openstudio::path getOpenStudioCoalitionMeasuresSourceDirectory();

/// \returns The path to the current executable application
openstudio::path getOpenStudioApplicationPath();

/// \returns The directory of the current executable application
openstudio::path getOpenStudioApplicationDirectory();

/// \returns True if the application is running from the build directory
bool isOpenStudioApplicationRunningFromBuildDirectory();

/// \returns Will return path to the binary containing OpenStudio Utilities, could be openstudio.exe, openstudio.so, etc.
openstudio::path getOpenStudioApplicationModule();

/// \returns Will return dir containing the binary containing OpenStudio Utilities, could be openstudio.exe, openstudio.so, etc.
openstudio::path getOpenStudioApplicationModuleDirectory();

/// \returns True if the OpenStudio Module is running from the build directory
bool isOpenStudioApplicationModuleRunningFromBuildDirectory();

/// \returns The path to the OpenStudio Command Line Interface  if it exists.
openstudio::path getOpenStudioCoreCLI();

/// \returns The base URL for the BigLadder EnergyPlus I/O Reference (versioned, trailing slash included)
OSAPP_UTILITIES_API std::string bigladdersoftwareDocBaseUrl();

}  // namespace openstudio

#endif  //OSAPP_UTILITIES_APPLICATIONPATHHELPERS_HPP
