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

#ifndef MODELEDITOR_GITHUB_RELEASES_HPP
#define MODELEDITOR_GITHUB_RELEASES_HPP

#include "ModelEditorAPI.hpp"
#include <openstudio/utilities/core/Logger.hpp>

#include <string>
#include <vector>

#if (defined(__GNUC__))
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
#define _TURN_OFF_PLATFORM_STRING
#include <cpprest/http_client.h>
#if (defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif

namespace modeleditor {

/** Class to represent a single release on Github.
**/
class MODELEDITOR_API GithubRelease
{
 public:
  GithubRelease(const std::string& tagName, bool preRelease, unsigned numDownloads, const std::string& downloadUrl);

  std::string tagName() const;
  boost::optional<openstudio::VersionString> version() const;
  bool preRelease() const;
  unsigned numDownloads() const;
  std::string downloadUrl() const;

 private:
  std::string m_tagName;
  bool m_preRelease;
  unsigned m_numDownloads;
  std::string m_downloadUrl;
};

/** Class for checking releases on Github.
  **/
class MODELEDITOR_API GithubReleases
{

 public:
  /// Constructor with organization and repo name
  GithubReleases(const std::string& orgName, const std::string& repoName);

  // virtual destructor
  virtual ~GithubReleases() {}

  /// returns the org name
  std::string orgName() const;

  /// returns the repo name
  std::string repoName() const;

  /// waits up to msec milliseconds and returns true if finished checking for updates
  bool waitForFinished(int msec = 120000) const;

  /// returns true when finished checking for updates
  bool finished() const;

  /// returns true if an error occurred while checking for releases,
  /// must call after update manager is finished
  bool error() const;

  /// returns true if a new release - or new prerelease if includePreReleases - is available
  bool newReleaseAvailable(bool includePreReleases = false) const;

  /// returns all releases
  std::vector<GithubRelease> releases() const;

  // url for user to check releases
  std::string releasesUrl() const;

 private:
  REGISTER_LOGGER("GithubReleases");

  std::string m_orgName;
  std::string m_repoName;
  bool m_finished;
  bool m_error;
  std::vector<GithubRelease> m_releases;

  boost::optional<pplx::task<void>> m_httpResponse;

  void processReply(const std::string& reply);

  // api url used for checking releases
  std::string apiUrl() const;
};

// prints releases and number of downloads
MODELEDITOR_API std::ostream& operator<<(std::ostream& os, const GithubRelease& release);
MODELEDITOR_API std::ostream& operator<<(std::ostream& os, const GithubReleases& releases);

}  // namespace modeleditor

#endif  // MODELEDITOR_GITHUB_RELEASES_HPP
