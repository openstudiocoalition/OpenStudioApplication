/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GithubReleases.hpp"
#include "Application.hpp"
#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/core/System.hpp>

#include <json/json.h>

using namespace utility::conversions;

namespace modeleditor {

GithubRelease::GithubRelease(const std::string& tagName, bool preRelease, unsigned numDownloads, const std::string& downloadUrl)
  : m_tagName(tagName), m_preRelease(preRelease), m_numDownloads(numDownloads), m_downloadUrl(downloadUrl) {}

std::string GithubRelease::tagName() const {
  return m_tagName;
}

boost::optional<openstudio::VersionString> GithubRelease::version() const {
  try {
    if ((m_tagName.size() > 0) && (m_tagName[0] == 'v')) {
      return openstudio::VersionString(m_tagName.substr(1));
    } else {
      return openstudio::VersionString(m_tagName);
    }
  } catch (const std::exception&) {
  }
  return boost::none;
}

bool GithubRelease::preRelease() const {
  return m_preRelease;
}

unsigned GithubRelease::numDownloads() const {
  return m_numDownloads;
}

std::string GithubRelease::downloadUrl() const {
  return m_downloadUrl;
}

GithubReleases::GithubReleases(const std::string& orgName, const std::string& repoName)
  : m_orgName(orgName), m_repoName(repoName), m_finished(false), m_error(false) {

  web::http::client::http_client_config config;
  config.set_validate_certificates(false);

  web::http::client::http_client client(to_string_t(apiUrl()), config);
  m_httpResponse = client.request(web::http::methods::GET)
                     .then([](web::http::http_response resp) { return resp.extract_utf8string(); })
                     .then([this](const std::string& xml) { processReply(xml); });
}

std::string GithubReleases::orgName() const {
  return m_orgName;
}

std::string GithubReleases::repoName() const {
  return m_repoName;
}

bool GithubReleases::waitForFinished(int msec) const {
  int msecPerLoop = 20;
  int numTries = msec / msecPerLoop;
  int current = 0;
  while (true) {
    // if no request was made and the optional is empty return
    if (!m_httpResponse) {
      return false;
    }

    if (m_httpResponse->is_done()) {
      return true;
    }

    openstudio::System::msleep(msecPerLoop);

    if (current > numTries) {
      LOG(Warn, "waitForFinished timeout");
      break;
    }

    ++current;
  }

  return false;
}

bool GithubReleases::finished() const {
  return m_finished;
}

bool GithubReleases::error() const {
  return m_error;
}

bool GithubReleases::newReleaseAvailable(bool includePreReleases) const {
  try {
    openstudio::VersionString currentVersion(openstudio::openStudioApplicationVersionWithPrerelease());
    for (const auto& release : m_releases) {
      if (!includePreReleases && release.preRelease()) {
        continue;
      }
      boost::optional<openstudio::VersionString> version = release.version();
      if (version) {
        if (version.get() > currentVersion) {
          return true;
        }
      }
    }
  } catch (const std::exception&) {
  }

  return false;
}

std::vector<GithubRelease> GithubReleases::releases() const {
  return m_releases;
}

std::string GithubReleases::releasesUrl() const {
  return std::string("https://github.com/") + orgName() + std::string("/") + repoName() + std::string("/releases");
}

std::string GithubReleases::apiUrl() const {
  return std::string("https://api.github.com/repos/") + orgName() + std::string("/") + repoName() + std::string("/releases");
}

void GithubReleases::processReply(const std::string& reply) {

  // finished after this
  m_finished = true;
  m_error = false;

  Json::CharReaderBuilder rbuilder;
  std::istringstream ss(reply);
  Json::Value root;
  std::string formattedErrors;
  bool parsingSuccessful = Json::parseFromStream(rbuilder, ss, &root, &formattedErrors);
  if (!parsingSuccessful) {
    LOG(Error, "GithubReleases reply: '" << reply << "' cannot be processed, " << formattedErrors);
    m_error = true;
    return;
  }

  // Iterate over the release elements
  for (Json::ArrayIndex i = 0; i < root.size(); ++i) {
    std::string tagName;
    bool preRelease(true);
    std::string url;
    unsigned downloadCount = 0;

    Json::Value tagNameValue = root[i].get("tag_name", Json::Value(""));
    if (tagNameValue.isString()) {
      tagName = tagNameValue.asString();
    }

    Json::Value preReleaseValue = root[i].get("prerelease", Json::Value(true));
    if (preReleaseValue.isBool()) {
      preRelease = preReleaseValue.asBool();
    }

    Json::Value urlValue = root[i].get("url", Json::Value(""));
    if (urlValue.isString()) {
      url = urlValue.asString();
    }

    Json::Value assetsValue = root[i].get("assets", Json::arrayValue);
    for (Json::ArrayIndex j = 0; j < assetsValue.size(); ++j) {
      Json::Value downloadCountValue = assetsValue[j].get("download_count", Json::Value(0u));
      if (downloadCountValue.isNumeric()) {
        downloadCount += downloadCountValue.asUInt();
      }
    }

    m_releases.push_back(GithubRelease(tagName, preRelease, downloadCount, url));
  }
}

// prints releases and number of downloads
std::ostream& operator<<(std::ostream& os, const GithubRelease& release) {
  Json::Value value = Json::objectValue;
  value["tag_name"] = release.tagName();
  value["prerelease"] = release.preRelease();
  value["url"] = release.downloadUrl();
  value["num_downloads"] = release.numDownloads();

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "   ";
  os << Json::writeString(wbuilder, value);

  return os;
}

std::ostream& operator<<(std::ostream& os, const GithubReleases& releases) {
  Json::Value value = Json::objectValue;
  value["org_name"] = releases.orgName();
  value["repo_name"] = releases.repoName();
  value["finished"] = releases.finished();
  value["error"] = releases.error();
  value["num_releases"] = static_cast<uint>(releases.releases().size());
  value["releases_url"] = releases.releasesUrl();
  value["new_release_available"] = releases.newReleaseAvailable(false);
  value["new_prerelease_available"] = releases.newReleaseAvailable(true);

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "   ";
  os << Json::writeString(wbuilder, value);

  return os;
}

}  // namespace modeleditor
