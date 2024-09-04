/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include <openstudio/resources.hxx>

#include "ModelEditorFixture.hpp"

#include "../PathWatcher.hpp"
#include "../Application.hpp"

#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/core/System.hpp>

#include <iostream>
#include <thread>

using openstudio::Application;
using openstudio::System;
using openstudio::toPath;
using std::ios_base;

struct TestPathWatcher : public PathWatcher
{

  // set periodic timer to 1 ms
  explicit TestPathWatcher(const openstudio::path& path) : PathWatcher(path, 1), added(false), changed(false), removed(false) {}

  virtual void onPathAdded() override {
    added = true;
  }
  virtual void onPathChanged() override {
    changed = true;
  }
  virtual void onPathRemoved() override {
    removed = true;
  }

  bool added;
  bool changed;
  bool removed;
};

// writes seem to have to occur in another thread for watcher to detect them
void write_file(const openstudio::path& path, const std::string& contents) {
  openstudio::filesystem::ofstream outFile(path, ios_base::out | ios_base::trunc);
  ASSERT_TRUE(outFile ? true : false);
  outFile << contents;
  outFile.close();
};

// removes seem to have to occur in another thread for watcher to detect them
void remove_file(const openstudio::path& path) {
  openstudio::filesystem::remove(path);
}

TEST_F(ModelEditorFixture, PathWatcher_File) {
  Application::instance().application(false);

  openstudio::path path = toPath("./PathWatcher_File");
  auto w1 = std::thread(write_file, path, "test 1");
  w1.join();

  ASSERT_TRUE(openstudio::filesystem::exists(path));

  TestPathWatcher watcher(path);
  EXPECT_FALSE(watcher.added);
  EXPECT_FALSE(watcher.changed);
  EXPECT_FALSE(watcher.removed);

  EXPECT_EQ(path.string(), watcher.path().string());

  auto w2 = std::thread(write_file, path, "test 2");
  w2.join();

  EXPECT_TRUE(openstudio::filesystem::exists(path));

  openstudio::System::msleep(1000);
  Application::instance().processEvents(10);

  EXPECT_FALSE(watcher.added);
  EXPECT_TRUE(watcher.changed);
  EXPECT_FALSE(watcher.removed);
  watcher.changed = false;
  EXPECT_FALSE(watcher.changed);

  auto r1 = std::thread(remove_file, path);
  r1.join();

  EXPECT_FALSE(openstudio::filesystem::exists(path));

  openstudio::System::msleep(1000);
  Application::instance().processEvents(10);

  EXPECT_FALSE(watcher.added);
  EXPECT_FALSE(watcher.changed);
  EXPECT_TRUE(watcher.removed);
}
