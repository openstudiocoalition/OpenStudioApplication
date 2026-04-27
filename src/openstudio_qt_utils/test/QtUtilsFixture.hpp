/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_QT_UTILS_TEST_QTUTILSFIXTURE_HPP
#define OPENSTUDIO_QT_UTILS_TEST_QTUTILSFIXTURE_HPP

#include <gtest/gtest.h>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/core/FileLogSink.hpp>

class QtUtilsFixture : public ::testing::Test
{
 protected:
  virtual void SetUp() override;
  virtual void TearDown() override;

  static void SetUpTestCase();
  static void TearDownTestCase();

  REGISTER_LOGGER("QtUtilsFixture");
  static boost::optional<openstudio::FileLogSink> logFile;
};

#endif  // OPENSTUDIO_QT_UTILS_TEST_QTUTILSFIXTURE_HPP
