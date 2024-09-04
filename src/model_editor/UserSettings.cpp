/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "UserSettings.hpp"

#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"

#include <openstudio/utilities/bcl/LocalBCL.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/core/FilesystemHelpers.hpp>

#include "Utilities.hpp"

#include <QCoreApplication>
#include <QString>
#include <QSettings>

std::vector<openstudio::BCLMeasure> localBCLMeasures() {
  return openstudio::OSAppBase::instance()->currentDocument()->getLocalMeasures();
}

std::vector<openstudio::BCLMeasure> userMeasures() {
  openstudio::path path = userMeasuresDir();
  return openstudio::BCLMeasure::getMeasuresInDir(path);
}

openstudio::path userMeasuresDir() {
  QSettings settings(QCoreApplication::organizationName(), "BCLMeasure");
  QString value =
    settings.value("userMeasuresDir", openstudio::toQString(openstudio::filesystem::home_path() / openstudio::toPath("OpenStudio/Measures")))
      .toString();
  openstudio::path result = openstudio::toPath(value);
  return openstudio::filesystem::system_complete(result);
}

bool setUserMeasuresDir(const openstudio::path& userMeasuresDir) {
  if (!userMeasuresDir.is_complete()) {
    return false;
  }
  if (!exists(userMeasuresDir)) {
    if (!openstudio::filesystem::create_directories(userMeasuresDir)) {
      return false;
    }
  }
  if (!is_directory(userMeasuresDir)) {
    return false;
  }

  QSettings settings(QCoreApplication::organizationName(), "BCLMeasure");
  settings.setValue("userMeasuresDir", openstudio::toQString(userMeasuresDir));
  return true;
}

void clearUserMeasuresDir() {
  QSettings settings(QCoreApplication::organizationName(), "BCLMeasure");
  settings.remove("userMeasuresDir");
}
