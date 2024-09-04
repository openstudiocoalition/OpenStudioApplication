/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_USERSETTINGS_HPP
#define MODELEDITOR_USERSETTINGS_HPP

#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>

/// Load all measures in the local BCL.
std::vector<openstudio::BCLMeasure> localBCLMeasures();

/// Load all measures in the user measures directory.
std::vector<openstudio::BCLMeasure> userMeasures();

/// Returns the path to the user measures directory stored in settings.
openstudio::path userMeasuresDir();

/// Changes the path to the user measures directory stored in settings.
bool setUserMeasuresDir(const openstudio::path& userMeasuresDir);

/// Clears the path to the user measures directory stored in settings.
void clearUserMeasuresDir();

#endif  // MODELEDITOR_USERSETTINGS_HPP
