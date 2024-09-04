/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_TEST_MODELEDITORFIXTURE_HPP
#define MODELEDITOR_TEST_MODELEDITORFIXTURE_HPP

#include <gtest/gtest.h>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/core/FileLogSink.hpp>

class ModelEditorFixture : public ::testing::Test
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

#endif  // MODELEDITOR_TEST_MODELEDITORFIXTURE_HPP
