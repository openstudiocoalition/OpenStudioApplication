/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
