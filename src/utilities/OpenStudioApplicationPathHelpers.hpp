/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
