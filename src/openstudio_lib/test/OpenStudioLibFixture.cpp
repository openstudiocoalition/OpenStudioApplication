/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OpenStudioLibFixture.hpp"

#include "../../model_editor/Application.hpp"

#include "../DesignDayGridView.hpp"
#include "../GridViewSubTab.hpp"
#include "../OSDropZone.hpp"
#include "../../shared_gui_components/OSCellWrapper.hpp"
#include "../../shared_gui_components/OSGridController.hpp"
#include "../../shared_gui_components/OSGridView.hpp"
#include "../../shared_gui_components/OSLineEdit.hpp"
#include "../../shared_gui_components/OSObjectSelector.hpp"
#include "../../shared_gui_components/OSWidgetHolder.hpp"

#include <openstudio/utilities/core/Path.hpp>

#include <boost/optional/optional_io.hpp>

using namespace openstudio;

// static variables
boost::optional<openstudio::FileLogSink> OpenStudioLibFixture::logFile;

int main(int argc, char* argv[]) {
  Q_INIT_RESOURCE(openstudio);
  auto app = openstudio::Application::instance().application(true);

  QTimer::singleShot(0, [&]() {
    ::testing::InitGoogleTest(&argc, argv);
    auto testResult = RUN_ALL_TESTS();
    app->exit(testResult);
  });

  return app->exec();
}

void OpenStudioLibFixture::SetUp() {}

void OpenStudioLibFixture::TearDown() {}

void OpenStudioLibFixture::SetUpTestCase() {
  // set up logging
  logFile = openstudio::FileLogSink(openstudio::toPath("./OpenStudioLibFixture.log"));
  logFile->setLogLevel(Debug);
  openstudio::Logger::instance().standardOutLogger().disable();
}

void OpenStudioLibFixture::TearDownTestCase() {}

void OpenStudioLibFixture::processEvents() {
  openstudio::Application::instance().application(true)->processEvents();
}

OSGridView* OpenStudioLibFixture::getGridView(GridViewSubTab* gvst) {
  return gvst->m_gridView;
}

OSGridView* OpenStudioLibFixture::getGridView(DesignDayGridView* gv) {
  return gv->m_gridView;
}

OSGridController* OpenStudioLibFixture::getGridController(GridViewSubTab* gvst) {
  return gvst->m_gridController;
}

OSGridController* OpenStudioLibFixture::getGridController(DesignDayGridView* gv) {
  return gv->m_gridController;
}

OSObjectSelector* OpenStudioLibFixture::getObjectSelector(OSGridController* gc) {
  return gc->m_objectSelector;
}
std::map<GridCellLocation*, GridCellInfo*> OpenStudioLibFixture::getGridCellLocationToInfoMap(openstudio::OSObjectSelector* os) {
  return os->m_gridCellLocationToInfoMap;
}

std::vector<GridCellLocation*> OpenStudioLibFixture::getSelectorCellLocations(openstudio::OSObjectSelector* os) {
  return os->m_selectorCellLocations;
}

std::vector<GridCellLocation*> OpenStudioLibFixture::getParentCellLocations(openstudio::OSObjectSelector* os) {
  return os->m_parentCellLocations;
}

boost::optional<openstudio::model::ModelObject> OpenStudioLibFixture::getModelObject(openstudio::OSDropZone2* dropZone) {
  boost::optional<openstudio::model::ModelObject> result;
  if (dropZone) {
    result = dropZone->m_modelObject;
  }
  return result;
}

boost::optional<openstudio::model::ModelObject> OpenStudioLibFixture::getModelObject(openstudio::OSLineEdit2* lineEdit) {
  boost::optional<openstudio::model::ModelObject> result;
  if (lineEdit) {
    result = lineEdit->m_modelObject;
  }
  return result;
}

boost::optional<openstudio::model::ModelObject> OpenStudioLibFixture::callGet(openstudio::OSDropZone2* dropZone) {
  boost::optional<openstudio::model::ModelObject> result;
  if (dropZone && dropZone->m_get) {
    result = dropZone->m_get.get()();
  }
  return result;
}

bool OpenStudioLibFixture::isDefaulted(openstudio::OSDropZone2* dropZone) {
  bool isDefaulted = false;
  if (dropZone && dropZone->m_label) {
    QVariant currentDefaulted = dropZone->m_label->property("defaulted");
    if (!currentDefaulted.isNull()) {
      isDefaulted = currentDefaulted.toBool();
    }
  }
  return isDefaulted;
}

openstudio::GridCellLocation* OpenStudioLibFixture::getGridCellLocationAt(openstudio::OSObjectSelector* os, int modelRow, int gridRow, int column,
                                                                          boost::optional<int> subrow) {
  auto m = getGridCellLocationToInfoMap(os);
  for (const auto& locationInfoPair : m) {
    if (locationInfoPair.first->equal(modelRow, gridRow, column, subrow)) {
      return locationInfoPair.first;
    }
  }
  return nullptr;
}

openstudio::GridCellInfo* OpenStudioLibFixture::getGridCellInfoAt(openstudio::OSObjectSelector* os, int modelRow, int gridRow, int column,
                                                                  boost::optional<int> subrow) {
  auto m = getGridCellLocationToInfoMap(os);
  for (const auto& locationInfoPair : m) {
    if (locationInfoPair.first->equal(modelRow, gridRow, column, subrow)) {
      return locationInfoPair.second;
    }
  }
  return nullptr;
}

OSCellWrapper* OpenStudioLibFixture::getWrapperAt(openstudio::OSGridView* gv, int row, int column, boost::optional<int> subrow) {
  OSCellWrapper* result = nullptr;
  QLayoutItem* item = gv->itemAtPosition(row, column);
  if (item) {
    result = qobject_cast<OSCellWrapper*>(item->widget());
  }
  return result;
}

QWidget* OpenStudioLibFixture::getOSWidgetAt(openstudio::OSGridView* gv, int row, int column, boost::optional<int> subrow) {
  QWidget* result = nullptr;
  QLayoutItem* item = gv->itemAtPosition(row, column);
  if (item) {
    QWidget* wrapper = item->widget();
    auto* innerLayout = qobject_cast<QGridLayout*>(wrapper->layout());
    if (innerLayout) {
      QLayoutItem* innerItem;
      // If it is a subrow, we get the subrow
      if (subrow) {
        innerItem = innerLayout->itemAtPosition(subrow.get(), 0);
      } else {
        innerItem = innerLayout->itemAtPosition(0, 0);
      }
      OS_ASSERT(innerItem);
      auto* holder = qobject_cast<OSWidgetHolder*>(innerItem->widget());
      OS_ASSERT(holder);
      result = holder->widget();
    }
  }
  return result;
}

std::vector<OSWidgetHolder*> OpenStudioLibFixture::getHolders(openstudio::OSCellWrapper* wrapper) {
  return wrapper->m_holders;
}

void OpenStudioLibFixture::updateStyle(QWidget* widget) {
  if (auto* lineEdit = qobject_cast<OSLineEdit2*>(widget)) {
    lineEdit->updateStyle();
  }
}

void OpenStudioLibFixture::checkExpected(OSObjectSelector* os, OSGridView* gv, int modelRow, int gridRow, int column, boost::optional<int> subrow,
                                         boost::optional<model::ModelObject> mo, bool visible, bool selectable, bool selected, bool selector,
                                         bool locked, const std::string& style) {
  GridCellLocation* location;
  GridCellInfo* info;
  OSCellWrapper* wrapper;
  QWidget* widget;

  bool isEven = ((gridRow % 2) == 0);

  location = getGridCellLocationAt(os, modelRow, gridRow, column, subrow);
  ASSERT_TRUE(location) << gridRow << ", " << column << ", " << subrow;
  info = getGridCellInfoAt(os, modelRow, gridRow, column, subrow);
  ASSERT_TRUE(info) << gridRow << ", " << column << ", " << subrow;
  wrapper = getWrapperAt(gv, gridRow, column, subrow);
  ASSERT_TRUE(wrapper) << gridRow << ", " << column << ", " << subrow;
  widget = getOSWidgetAt(gv, gridRow, column, subrow);
  ASSERT_TRUE(widget) << gridRow << ", " << column << ", " << subrow;

  EXPECT_EQ(visible, info->isVisible()) << gridRow << ", " << column << ", " << subrow;
  EXPECT_EQ(selectable, info->isSelectable()) << gridRow << ", " << column << ", " << subrow;
  EXPECT_EQ(selected, info->isSelected()) << gridRow << ", " << column << ", " << subrow;
  EXPECT_EQ(selector, info->isSelector) << gridRow << ", " << column << ", " << subrow;
  EXPECT_EQ(locked, info->isLocked()) << gridRow << ", " << column << ", " << subrow;

  if (mo) {
    ASSERT_TRUE(info->modelObject) << gridRow << ", " << column << ", " << subrow;
    EXPECT_EQ(mo->handle(), info->modelObject->handle()) << gridRow << ", " << column << ", " << subrow;
  } else {
    EXPECT_FALSE(info->modelObject) << gridRow << ", " << column << ", " << subrow;
  }

  auto holders = getHolders(wrapper);
  OSWidgetHolder* holder;
  if (subrow) {
    ASSERT_TRUE(subrow.get() < holders.size()) << gridRow << ", " << column << ", " << subrow;
    holder = holders[subrow.get()];
  } else {
    ASSERT_TRUE(!holders.empty());
    holder = holders[0];
  }

  QVariant var = holder->property("selected");
  EXPECT_EQ(var.toBool(), selected) << gridRow << ", " << column << ", " << subrow;

  var = holder->property("even");
  EXPECT_EQ(var.toBool(), isEven) << gridRow << ", " << column << ", " << subrow;

  var = widget->property("style");
  EXPECT_EQ(var.toString().toStdString(), style) << gridRow << ", " << column << ", " << subrow;
}