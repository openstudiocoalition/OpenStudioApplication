/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_TEST_OPENSTUDIOAPPFIXTURE_HPP
#define OPENSTUDIO_TEST_OPENSTUDIOAPPFIXTURE_HPP

#include <gtest/gtest.h>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/core/FileLogSink.hpp>

class OpenStudioAppFixture : public ::testing::Test
{
 protected:
  // initialize for each test
  virtual void SetUp() override;

  // tear down after each test
  virtual void TearDown() override;

  // initialize static members
  static void SetUpTestCase();

  // tear down static members
  static void TearDownTestCase();

  // static variables
  static boost::optional<openstudio::FileLogSink> logFile;
};

#endif  // OPENSTUDIO_TEST_OPENSTUDIOAPPFIXTURE_HPP
