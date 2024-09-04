/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../AnalyticsHelper.hpp"
#include <AnalyticsHelperSecrets.hxx>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, AnalyticsHelperSecrets) {
  EXPECT_FALSE(apiSecret().isEmpty());
  EXPECT_FALSE(measurementId().isEmpty());
}