/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_TEST_OPENSTUDIOLIBFIXTURE_HPP
#define OPENSTUDIO_TEST_OPENSTUDIOLIBFIXTURE_HPP

#include <gtest/gtest.h>

#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/core/FileLogSink.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <map>
#include <vector>

namespace openstudio {
class DesignDayGridView;
class GridCellLocation;
class GridCellInfo;
class GridViewSubTab;
class OSCellWrapper;
class OSObjectSelector;
class OSGridController;
class OSGridView;
class OSDropZone2;
class OSLineEdit2;
class OSWidgetHolder;
}  // namespace openstudio

class QWidget;
class QString;

class OpenStudioLibFixture : public ::testing::Test
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

  // Process events
  void processEvents();

  // Acceessors for private members
  openstudio::OSGridView* getGridView(openstudio::GridViewSubTab* gvst);
  openstudio::OSGridView* getGridView(openstudio::DesignDayGridView* gv);
  openstudio::OSGridController* getGridController(openstudio::GridViewSubTab* gvst);
  openstudio::OSGridController* getGridController(openstudio::DesignDayGridView* gv);
  openstudio::OSObjectSelector* getObjectSelector(openstudio::OSGridController* gc);

  std::map<openstudio::GridCellLocation*, openstudio::GridCellInfo*> getGridCellLocationToInfoMap(openstudio::OSObjectSelector* os);
  std::vector<openstudio::GridCellLocation*> getSelectorCellLocations(openstudio::OSObjectSelector* os);
  std::vector<openstudio::GridCellLocation*> getParentCellLocations(openstudio::OSObjectSelector* os);

  boost::optional<openstudio::model::ModelObject> getModelObject(openstudio::OSDropZone2* dropZone);
  boost::optional<openstudio::model::ModelObject> getModelObject(openstudio::OSLineEdit2* lineEdit);

  boost::optional<openstudio::model::ModelObject> callGet(openstudio::OSDropZone2* dropZone);

  bool isDefaulted(openstudio::OSDropZone2* dropZone);

  openstudio::GridCellLocation* getGridCellLocationAt(openstudio::OSObjectSelector* os, int modelRow, int gridRow, int column,
                                                      boost::optional<int> subrow);
  openstudio::GridCellInfo* getGridCellInfoAt(openstudio::OSObjectSelector* os, int modelRow, int gridRow, int column, boost::optional<int> subrow);

  openstudio::OSCellWrapper* getWrapperAt(openstudio::OSGridView* gv, int row, int column, boost::optional<int> subrow);
  QWidget* getOSWidgetAt(openstudio::OSGridView* gv, int row, int column, boost::optional<int> subrow);

  std::vector<openstudio::OSWidgetHolder*> getHolders(openstudio::OSCellWrapper* wrapper);

  void updateStyle(QWidget* widget);

  void checkExpected(openstudio::OSObjectSelector* os, openstudio::OSGridView* gv, int modelRow, int gridRow, int column, boost::optional<int> subrow,
                     boost::optional<openstudio::model::ModelObject> mo, bool visible, bool selectable, bool selected, bool selector, bool locked,
                     const std::string& style);

  // static variables
  static boost::optional<openstudio::FileLogSink> logFile;
};

#endif  // OPENSTUDIO_TEST_OPENSTUDIOLIBFIXTURE_HPP
