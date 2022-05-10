/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
