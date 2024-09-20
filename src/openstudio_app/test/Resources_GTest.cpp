/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioAppFixture.hpp"

#include <openstudio/osversion/VersionTranslator.hpp>

#include <openstudio/model/Model.hpp>
#include <openstudio/model/SpaceLoad.hpp>
#include <openstudio/model/SpaceLoad_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>

#include "../../model_editor/Utilities.hpp"

// Include our OS App specific one
#include "../../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/PathHelpers.hpp>

#include <QDir>
#include <QFileInfo>

using namespace openstudio;

TEST_F(OpenStudioAppFixture, Resources_Templates) {
  openstudio::path resourcesPath = getOpenStudioApplicationSourceDirectory() / openstudio::toPath("src/openstudio_app/Resources");

  ASSERT_TRUE(openstudio::filesystem::exists(resourcesPath));
  ASSERT_FALSE(isEmptyDirectory(resourcesPath));

  QDir resourcesDir(toQString(resourcesPath));
  QStringList filters;
  filters << QString("*.osm");
  QFileInfoList files = resourcesDir.entryInfoList(filters, QDir::Files, QDir::NoSort);
  EXPECT_FALSE(files.empty());
  for (const QFileInfo& file : files) {
    openstudio::path path = toPath(file.absoluteFilePath());
    EXPECT_TRUE(openstudio::filesystem::exists(path));

    osversion::VersionTranslator vt;
    boost::optional<model::Model> model = vt.loadModel(path);
    ASSERT_TRUE(model);

    // check that each space load has a parent space type
    std::vector<model::SpaceLoad> spaceLoads = model->getModelObjects<model::SpaceLoad>();
    for (const model::SpaceLoad& spaceLoad : spaceLoads) {
      EXPECT_TRUE(spaceLoad.spaceType());
    }

    // uncomment this to save the version translated file to the original path
    // DO NOT leave this in the test execution when you commit!
    //model->save(path, true);
  }
}

TEST_F(OpenStudioAppFixture, Resources_HVACLibrary) {
  openstudio::path hvacPath = getOpenStudioApplicationSourceDirectory() / openstudio::toPath("src/openstudio_app/Resources/default/hvac_library.osm");

  ASSERT_TRUE(openstudio::filesystem::exists(hvacPath));

  osversion::VersionTranslator vt;
  boost::optional<model::Model> model = vt.loadModel(hvacPath);
  ASSERT_TRUE(model);

  // uncomment this to save the version translated file to the original path
  // DO NOT leave this in the test execution when you commit!
  //model->save(hvacPath, true);
}
