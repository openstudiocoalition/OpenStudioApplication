/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef BIMSERVER_TEST_BIMSERVERFIXTURE_HPP
#define BIMSERVER_TEST_BIMSERVERFIXTURE_HPP

#include <gtest/gtest.h>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/core/FileLogSink.hpp>
#include <openstudio/utilities/core/Path.hpp>

#include <vector>
#include <string>

class BIMserverFixture : public ::testing::Test
{
 protected:
  /// initialize for each test
  virtual void SetUp() override;

  /// tear down after each test
  virtual void TearDown() override;

  /// initialize static members
  static void SetUpTestCase();

  /// tear down static members
  static void TearDownTestCase();

  static std::shared_ptr<openstudio::FileLogSink> logFile;

  REGISTER_LOGGER("BIMserver");
};

#endif  // BIMSERVER_TEST_BIMSERVERFIXTURE_HPP
