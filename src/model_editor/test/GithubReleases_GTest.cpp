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
