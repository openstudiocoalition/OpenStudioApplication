/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
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

#ifndef SHAREDGUICOMPONENTS_OSOBJECTSELECTOR_HPP
#define SHAREDGUICOMPONENTS_OSOBJECTSELECTOR_HPP

#include "OSConcepts.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <string>
#include <functional>
#include <vector>

#include <QObject>

class OpenStudioLibFixture;

namespace openstudio {

class OSWidgetHolder;

class GridCellLocation : public QObject
{
  Q_OBJECT;

 public:
  GridCellLocation(int t_modelRow, int t_gridRow, int t_column, boost::optional<int> t_subrow, QObject* parent);

  virtual ~GridCellLocation();

  const int modelRow;
  const int gridRow;
  const int column;
  const boost::optional<int> subrow;

  bool equal(int t_modelRow, int t_gridRow, int t_column, boost::optional<int> t_subrow) const;
  bool operator==(const GridCellLocation& other) const;
  bool operator<(const GridCellLocation& other) const;

 signals:

  void inFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow);

  //void selectionChanged(int state, int modelRow, int gridRow, int column, boost::optional<int> subrow);

 public slots:

  void onInFocus(bool t_inFocus, bool t_hasData);

  //void onSelectionChanged(int state);
};

class GridCellInfo : public QObject
{
  Q_OBJECT;

 public:
  GridCellInfo(const boost::optional<model::ModelObject>& t_modelObject, bool t_isSelector, bool t_isVisible, bool t_isSelected, bool t_isLocked,
               QObject* parent);

  virtual ~GridCellInfo();

  const boost::optional<model::ModelObject> modelObject;
  const bool isSelector;

  bool isVisible() const;

  // returns true if changed
  bool setVisible(bool visible);

  bool isSelectable() const;

  bool isSelected() const;

  // returns true if changed
  bool setSelected(bool selected);

  bool isLocked() const;

  // returns true if changed
  bool setLocked(bool locked);

 private:
  bool m_isVisible;
  bool m_isSelected;
  bool m_isLocked;
};

/// OSObjectSelector keeps track of which cells are selected, filtered/not visible, and locked
class OSObjectSelector : public QObject
{
  Q_OBJECT;

 public:
  explicit OSObjectSelector(QObject* parent);

  enum PropertyChange
  {
    NoChange,
    ToggleChange,
    ChangeToFalse,
    ChangeToTrue
  };

  virtual ~OSObjectSelector();

  // Reset all state
  void clear();

  // Reset all state in a cell
  void clearCell(int t_modelRow, int t_gridRow, int t_column);
  void clearSubCell(int t_modelRow, int t_gridRow, int t_column, int t_subrow);

  // Adds object to the internal maps
  void addObject(const boost::optional<model::ModelObject>& t_obj, OSWidgetHolder* t_holder, int t_modelRow, int t_gridRow, int t_column,
                 const boost::optional<int>& t_subrow, bool t_isSelector, bool t_isParent, bool t_isLocked);

  // Lock and hide any grid cells referencing this object
  void setObjectRemoved(const openstudio::Handle& handle);

  // Check if object is included in any row or subrow
  //bool containsObject(const openstudio::model::ModelObject& t_obj) const;

  // Get object at given location
  boost::optional<model::ModelObject> getObject(const int t_modelRow, const int t_gridRow, const int t_column,
                                                const boost::optional<int>& t_subrow) const;

  // Get GridCellInfo at given location
  // GridCellInfo* getGridCellInfo(const int t_row, const int t_column, const boost::optional<int>& t_subrow) const;

  // Select all selectable objects
  void selectAll();

  // Clear the selection
  void clearSelection();

  // Check if an object is selected
  bool getObjectSelected(const model::ModelObject& t_obj) const;

  // Set a selectable object as selected
  void setObjectSelected(const model::ModelObject& t_obj, bool t_selected);

  // Gets selector objects
  std::set<model::ModelObject> selectorObjects() const;

  // Gets selectable objects
  std::set<model::ModelObject> selectableObjects() const;

  // Gets selected objects
  std::set<model::ModelObject> selectedObjects() const;

  // Get the object filter function, function true if object is visible
  std::function<bool(const model::ModelObject&)> objectFilter() const;

  // Set the object filter function, function true if object is visible
  void setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter);

  // Reset the object filter function
  void resetObjectFilter();

  // Get the object is locked function, function true if object is locked
  std::function<bool(const model::ModelObject&)> objectIsLocked() const;

  // Set the object is locked function, function true if object is locked
  void setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked);

  // Reset the object is locked function
  void resetObjectIsLocked();

  // Applies locks to entire rows and subrows
  //void applyLocks();

 signals:

  void inFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow);

  void gridCellChanged(const GridCellLocation& location, const GridCellInfo& info);

  void gridRowSelectionChanged(int numSelected, int numSelectable);

 public slots:

  void onRowNeedsStyle(int modelRow, int gridRow);

 protected:
  REGISTER_LOGGER("openstudio.ObjectSelector");

 private:
  // for testing
  friend class ::OpenStudioLibFixture;

  std::map<GridCellLocation*, GridCellInfo*> m_gridCellLocationToInfoMap;

  // selector cells are the ones with checkboxes, performance optimization
  std::vector<GridCellLocation*> m_selectorCellLocations;

  // parent cells are the first column in a row that has sub rows, performance optimization
  std::vector<GridCellLocation*> m_parentCellLocations;

  std::vector<GridCellLocation*> m_selectorOrParentCellLocations;

  // Apply locked and not visible properties to rows and subrows
  void updateRowsAndSubrows(const std::vector<std::pair<GridCellLocation*, PropertyChange>>& visibleChanges,
                            const std::vector<std::pair<GridCellLocation*, PropertyChange>>& lockedChanges);

  // Set an entire row as selected
  void setRowProperties(const int t_gridRow, PropertyChange t_visible, PropertyChange t_selected, PropertyChange t_locked);

  // Set a subrow as selected
  void setSubrowProperties(const int t_gridRow, const int t_subrow, PropertyChange t_visible, PropertyChange t_selected, PropertyChange t_locked);

  GridCellInfo* getGridCellInfo(GridCellLocation* location) const;

  // returns true if object is visible
  // e.g. a lights object would not be visible if the user filted only people objects
  std::function<bool(const model::ModelObject&)> m_objectFilter;
  static std::function<bool(const model::ModelObject&)> getDefaultFilter();

  // returns true if object is locked
  // e.g. an inherited space load would be locked
  std::function<bool(const model::ModelObject&)> m_isLocked;
  static std::function<bool(const model::ModelObject&)> getDefaultIsLocked();
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSOBJECTSELECTOR_HPP
