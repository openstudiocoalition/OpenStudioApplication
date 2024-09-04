/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelEditorFixture.hpp"

#include "../GithubReleases.hpp"
#include "../Application.hpp"

#include <openstudio/utilities/core/Compare.hpp>

#include <iostream>
#include <thread>
#include <json/json.h>

using openstudio::Application;

TEST_F(ModelEditorFixture, GithubRelease_Release) {
  Application::instance().application(false);

  modeleditor::GithubRelease release("v1.0.1", false, 100, "https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1");
  ASSERT_TRUE(release.version());
  EXPECT_EQ(openstudio::VersionString("1.0.1"), release.version().get());
  EXPECT_FALSE(release.preRelease());
  EXPECT_EQ(100, release.numDownloads());
  EXPECT_EQ("https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1", release.downloadUrl());

  std::stringstream ss;
  ss << release;
  Json::Value root;
  ss >> root;
  EXPECT_EQ(100u, root["num_downloads"].asUInt());
  EXPECT_FALSE(root["prerelease"].asBool());
  EXPECT_EQ("v1.0.1", root["tag_name"].asString());
  EXPECT_EQ("https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1", root["url"].asString());
}

TEST_F(ModelEditorFixture, GithubRelease_Prerelease) {
  Application::instance().application(false);

  modeleditor::GithubRelease release("v1.0.1-pre1", true, 100,
                                     "https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1-pre1");
  ASSERT_TRUE(release.version());
  EXPECT_EQ(openstudio::VersionString("1.0.1-pre1"), release.version().get());
  EXPECT_TRUE(release.preRelease());
  EXPECT_EQ(100, release.numDownloads());
  EXPECT_EQ("https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1-pre1", release.downloadUrl());

  std::stringstream ss;
  ss << release;
  Json::Value root;
  ss >> root;
  EXPECT_EQ(100u, root["num_downloads"].asUInt());
  EXPECT_TRUE(root["prerelease"].asBool());
  EXPECT_EQ("v1.0.1-pre1", root["tag_name"].asString());
  EXPECT_EQ("https://github.com/openstudiocoalition/OpenStudioApplication/releases/tag/v1.0.1-pre1", root["url"].asString());
}

TEST_F(ModelEditorFixture, GithubReleases) {

  modeleditor::GithubReleases releases("openstudiocoalition", "OpenStudioApplication");
  releases.waitForFinished();
  EXPECT_GT(releases.releases().size(), 0);
  std::stringstream ss;
  ss << releases;
  // Debug: EXPECT_EQ("", ss.str());

  Json::Value root;
  ss >> root;
  EXPECT_EQ("openstudiocoalition", root["org_name"].asString());
  EXPECT_EQ("OpenStudioApplication", root["repo_name"].asString());
  EXPECT_EQ("https://github.com/openstudiocoalition/OpenStudioApplication/releases", root["releases_url"].asString());
  EXPECT_FALSE(root["error"].asBool());
  EXPECT_TRUE(root["finished"].asBool());
  EXPECT_GT(root["num_releases"].asUInt(), 0u);
  EXPECT_TRUE(root["new_release_available"].isBool());
  EXPECT_TRUE(root["new_prerelease_available"].isBool());
}
