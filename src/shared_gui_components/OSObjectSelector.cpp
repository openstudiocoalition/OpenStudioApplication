/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSObjectSelector.hpp"

#include "OSCheckBox.hpp"
#include "OSComboBox.hpp"
#include "OSDoubleEdit.hpp"
#include "OSGridView.hpp"
#include "OSIntegerEdit.hpp"
#include "OSLineEdit.hpp"
#include "OSLoadNamePixmapLineEdit.hpp"
#include "OSQuantityEdit.hpp"
#include "OSUnsignedEdit.hpp"
#include "OSWidgetHolder.hpp"

#include "../openstudio_lib/HorizontalTabWidget.hpp"
#include "../openstudio_lib/MainRightColumnController.hpp"
#include "../openstudio_lib/ModelObjectInspectorView.hpp"
#include "../openstudio_lib/ModelObjectItem.hpp"
//#include "../openstudio_lib/ModelSubTabView.hpp"
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"
#include "../openstudio_lib/OSDropZone.hpp"
#include "../openstudio_lib/OSItemSelector.hpp"
#include "../openstudio_lib/RenderingColorWidget.hpp"
#include "../openstudio_lib/SchedulesView.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QWidget>

#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <iterator>

namespace openstudio {

GridCellLocation::GridCellLocation(int t_modelRow, int t_gridRow, int t_column, boost::optional<int> t_subrow, QObject* parent)
  : QObject(parent), modelRow(t_modelRow), gridRow(t_gridRow), column(t_column), subrow(t_subrow) {}

GridCellLocation::~GridCellLocation() = default;

bool GridCellLocation::equal(int t_modelRow, int t_gridRow, int t_column, boost::optional<int> t_subrow) const {
  return (modelRow == t_modelRow) && (gridRow == t_gridRow) && (column == t_column) && (subrow == t_subrow);
}

bool GridCellLocation::operator==(const GridCellLocation& other) const {
  return equal(other.modelRow, other.gridRow, other.column, other.subrow);
}

bool GridCellLocation::operator<(const GridCellLocation& other) const {
  if (gridRow < other.gridRow) {
    return true;
  } else if (gridRow > other.gridRow) {
    return false;
  }

  if (column < other.column) {
    return true;
  } else if (column > other.column) {
    return false;
  }

  if (!subrow && other.subrow) {
    return true;
  } else if (subrow && !other.subrow) {
    return false;
  } else if (!subrow && !other.subrow) {
    return false;
  }

  if (subrow.get() < other.subrow.get()) {
    return true;
  } else if (subrow.get() > other.subrow.get()) {
    return false;
  }

  // equal index
  return false;
}

void GridCellLocation::onInFocus(bool t_inFocus, bool t_hasData) {
  emit inFocus(t_inFocus, t_hasData, modelRow, gridRow, column, subrow);
}
/*
void GridCellLocation::onSelectionChanged(int state) {
  emit selectionChanged(state, modelRow, gridRow, column, subrow);
}
*/

GridCellInfo::GridCellInfo(const boost::optional<model::ModelObject>& t_modelObject, bool t_isSelector, bool t_isVisible, bool t_isSelected,
                           bool t_isLocked, QObject* parent)
  : QObject(parent),
    modelObject(t_modelObject),
    isSelector(t_isSelector),
    m_isVisible(t_isVisible),
    m_isSelected(t_isSelected),
    m_isLocked(t_isLocked) {}

GridCellInfo::~GridCellInfo() = default;

bool GridCellInfo::isVisible() const {
  return m_isVisible;
}

bool GridCellInfo::setVisible(bool visible) {
  if (m_isVisible != visible) {
    m_isVisible = visible;
    if (!visible) {
      m_isSelected = false;
    }
    return true;
  }
  return false;
}

bool GridCellInfo::isSelectable() const {
  return (m_isVisible && !m_isLocked);
}

bool GridCellInfo::isSelected() const {
  return m_isSelected;
}

bool GridCellInfo::setSelected(bool selected) {
  if (m_isSelected != selected) {
    m_isSelected = selected;
    return true;
  }
  return false;
}

bool GridCellInfo::isLocked() const {
  return m_isLocked;
}

bool GridCellInfo::setLocked(bool locked) {
  if (m_isLocked != locked) {
    m_isLocked = locked;
    if (locked) {
      m_isSelected = false;
    }
    return true;
  }
  return false;
}

OSObjectSelector::OSObjectSelector(QObject* parent) : QObject(parent), m_objectFilter(getDefaultFilter()), m_isLocked(getDefaultIsLocked()) {}

OSObjectSelector::~OSObjectSelector() = default;

void OSObjectSelector::clear() {

  auto it = m_gridCellLocationToInfoMap.begin();
  while (it != m_gridCellLocationToInfoMap.end()) {
    delete it->first;
    delete it->second;
    it = m_gridCellLocationToInfoMap.erase(it);
  }

  m_selectorCellLocations.clear();
  m_parentCellLocations.clear();
  m_selectorOrParentCellLocations.clear();

  m_objectFilter = getDefaultFilter();
  m_isLocked = getDefaultIsLocked();
}

void OSObjectSelector::clearCell(int t_modelRow, int t_gridRow, int t_column) {

  auto sameCellLambda = [t_modelRow, t_gridRow, t_column](GridCellLocation* tmp) {
    return t_modelRow == tmp->modelRow && t_gridRow == tmp->gridRow && t_column == tmp->column;
  };

  m_selectorCellLocations.erase(std::remove_if(m_selectorCellLocations.begin(), m_selectorCellLocations.end(), sameCellLambda),
                                m_selectorCellLocations.end());

  m_parentCellLocations.erase(std::remove_if(m_parentCellLocations.begin(), m_parentCellLocations.end(), sameCellLambda),
                              m_parentCellLocations.end());

  m_selectorOrParentCellLocations.erase(
    std::remove_if(m_selectorOrParentCellLocations.begin(), m_selectorOrParentCellLocations.end(), sameCellLambda),
    m_selectorOrParentCellLocations.end());

  auto it = m_gridCellLocationToInfoMap.begin();
  while (it != m_gridCellLocationToInfoMap.end()) {
    if (it->first->modelRow == t_modelRow && it->first->gridRow == t_gridRow && it->first->column == t_column) {
      delete it->first;
      delete it->second;
      it = m_gridCellLocationToInfoMap.erase(it);
    } else {
      ++it;
    }
  }
}

void OSObjectSelector::clearSubCell(int t_modelRow, int t_gridRow, int t_column, int t_subrow) {

  auto sameSubCellLambda = [t_modelRow, t_gridRow, t_column, t_subrow](GridCellLocation* tmp) {
    return t_modelRow == tmp->modelRow && t_gridRow == tmp->gridRow && t_column == tmp->column && tmp->subrow && t_subrow == tmp->subrow.get();
  };

  m_selectorCellLocations.erase(std::remove_if(m_selectorCellLocations.begin(), m_selectorCellLocations.end(), sameSubCellLambda),
                                m_selectorCellLocations.end());

  m_parentCellLocations.erase(std::remove_if(m_parentCellLocations.begin(), m_parentCellLocations.end(), sameSubCellLambda),
                              m_parentCellLocations.end());

  m_selectorOrParentCellLocations.erase(
    std::remove_if(m_selectorOrParentCellLocations.begin(), m_selectorOrParentCellLocations.end(), sameSubCellLambda),
    m_selectorOrParentCellLocations.end());

  auto it = m_gridCellLocationToInfoMap.begin();
  while (it != m_gridCellLocationToInfoMap.end()) {
    if (it->first->modelRow == t_modelRow && it->first->gridRow == t_gridRow && it->first->column == t_column && it->first->subrow
        && it->first->subrow.get() == t_subrow) {
      delete it->first;
      delete it->second;
      it = m_gridCellLocationToInfoMap.erase(it);
    } else {
      ++it;
    }
  }
}

void OSObjectSelector::addObject(const boost::optional<model::ModelObject>& t_obj, OSWidgetHolder* t_holder, int t_modelRow, int t_gridRow,
                                 int t_column, const boost::optional<int>& t_subrow, const bool t_isSelector, const bool t_isParent,
                                 const bool t_isLocked) {

  if (t_subrow) {
    clearSubCell(t_modelRow, t_gridRow, t_column, *t_subrow);
  } else {
    clearCell(t_modelRow, t_gridRow, t_column);
  }

  bool isVisible = true;
  bool isSelected = false;
  bool isLocked = t_isLocked;

  auto* info = new GridCellInfo(t_obj, t_isSelector, isVisible, isSelected, isLocked, this);
  auto* location = new GridCellLocation(t_modelRow, t_gridRow, t_column, t_subrow, this);

  connect(t_holder, &OSWidgetHolder::inFocus, location, &GridCellLocation::onInFocus);
  connect(location, &GridCellLocation::inFocus, this, &OSObjectSelector::inFocus);

  if (t_isSelector) {
    m_selectorCellLocations.push_back(location);
  }

  if (t_isParent) {
    m_parentCellLocations.push_back(location);
  }

  if (t_isSelector || t_isParent) {
    m_selectorOrParentCellLocations.push_back(location);
  }

  m_gridCellLocationToInfoMap.insert(std::make_pair(location, info));

  int numSelected = 0;
  int numSelectable = 0;
  for (const auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info) {
      if (info->isSelected()) {
        ++numSelected;
      }
      if (info->isSelectable()) {
        ++numSelectable;
      }
    }
  }

  emit gridRowSelectionChanged(numSelected, numSelectable);
}

void OSObjectSelector::setObjectRemoved(const openstudio::Handle& handle) {
  const PropertyChange visible = ChangeToFalse;
  const PropertyChange selected = ChangeToFalse;
  const PropertyChange locked = ChangeToTrue;
  int numSelected = 0;
  int numSelectable = 0;
  for (auto* const location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info == nullptr) {
      continue;
    }
    if (info->modelObject && info->modelObject->handle() == handle) {
      if (location->subrow) {
        setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
      } else {
        setRowProperties(location->gridRow, visible, selected, locked);
      }
    } else {
      if (info->isSelected()) {
        ++numSelected;
      }
      if (info->isSelectable()) {
        ++numSelectable;
      }
    }
  }
  emit gridRowSelectionChanged(numSelected, numSelectable);
}

//bool OSObjectSelector::containsObject(const openstudio::model::ModelObject& t_obj) const {
//  for (const auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isSelector && locationInfoPair.second->modelObject && locationInfoPair.second->modelObject.get() == t_obj) {
//      return true;
//    }
//  }
//  return false;
//}

boost::optional<model::ModelObject> OSObjectSelector::getObject(const int t_modelRow, const int t_gridRow, const int t_column,
                                                                const boost::optional<int>& t_subrow) const {
  for (const auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.first->equal(t_modelRow, t_gridRow, t_column, t_subrow)) {
      return locationInfoPair.second->modelObject;
    }
  }
  return boost::none;
}

void OSObjectSelector::selectAll() {
  const PropertyChange visible = NoChange;
  const PropertyChange selected = ChangeToTrue;
  const PropertyChange locked = NoChange;
  int numSelected = 0;
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info) {
      if (info->isSelectable()) {
        numSelected += 1;
        if (location->subrow) {
          setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
        } else {
          setRowProperties(location->gridRow, visible, selected, locked);
        }
      } else {
        OS_ASSERT(!(info->isSelected()));
      }
    }
  }

  emit gridRowSelectionChanged(numSelected, numSelected);
}

void OSObjectSelector::clearSelection() {
  const PropertyChange visible = NoChange;
  const PropertyChange selected = ChangeToFalse;
  const PropertyChange locked = NoChange;
  int numSelectable = 0;
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info) {
      if (info->isSelectable()) {
        numSelectable += 1;
        if (location->subrow) {
          setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
        } else {
          setRowProperties(location->gridRow, visible, selected, locked);
        }
      } else {
        OS_ASSERT(!(info->isSelected()));
      }
    }
  }

  emit gridRowSelectionChanged(0, numSelectable);
}

void OSObjectSelector::onRowNeedsStyle(int modelRow, int gridRow) {

  std::vector<std::pair<GridCellLocation*, PropertyChange>> visibleChanges;
  std::vector<std::pair<GridCellLocation*, PropertyChange>> lockedChanges;

  // loop over selector cells first
  for (auto& location : m_selectorCellLocations) {
    if (location->modelRow == modelRow && location->gridRow == gridRow) {
      GridCellInfo* info = getGridCellInfo(location);
      if (info && info->modelObject) {
        PropertyChange lockedChange = PropertyChange::NoChange;

        if (m_isLocked(info->modelObject.get())) {
          lockedChange = PropertyChange::ChangeToTrue;
        }
        lockedChanges.emplace_back(location, lockedChange);

        PropertyChange visibleChange = PropertyChange::ChangeToFalse;
        if (m_objectFilter(info->modelObject.get())) {
          visibleChange = PropertyChange::ChangeToTrue;
        }
        visibleChanges.emplace_back(location, visibleChange);
      }
    }
  }

  // loop over parent cells second
  for (auto& location : m_parentCellLocations) {
    if (location->modelRow == modelRow && location->gridRow == gridRow) {
      GridCellInfo* info = getGridCellInfo(location);
      if (info && info->modelObject) {
        PropertyChange lockedChange = PropertyChange::NoChange;
        if (m_isLocked(info->modelObject.get())) {
          lockedChange = PropertyChange::ChangeToTrue;

          // parent locked, selector always locked
          lockedChanges.erase(
            std::remove_if(lockedChanges.begin(), lockedChanges.end(),
                           [location](std::pair<GridCellLocation*, PropertyChange> tmp) { return location->gridRow == tmp.first->gridRow; }),
            lockedChanges.end());
        }
        lockedChanges.emplace_back(location, lockedChange);

        PropertyChange visibleChange = PropertyChange::ChangeToFalse;
        if (m_objectFilter(info->modelObject.get())) {
          visibleChange = PropertyChange::ChangeToTrue;
        } else {
          visibleChange = PropertyChange::ChangeToFalse;

          // parent hidden, selector always hidden
          visibleChanges.erase(
            std::remove_if(visibleChanges.begin(), visibleChanges.end(),
                           [location](std::pair<GridCellLocation*, PropertyChange> tmp) { return location->gridRow == tmp.first->gridRow; }),
            visibleChanges.end());
        }
        visibleChanges.emplace_back(location, visibleChange);
      }
    }
  }

  // apply the changes in reverse order
  std::reverse(visibleChanges.begin(), visibleChanges.end());
  std::reverse(lockedChanges.begin(), lockedChanges.end());

  updateRowsAndSubrows(visibleChanges, lockedChanges);
}

void OSObjectSelector::updateRowsAndSubrows(const std::vector<std::pair<GridCellLocation*, PropertyChange>>& visibleChanges,
                                            const std::vector<std::pair<GridCellLocation*, PropertyChange>>& lockedChanges) {

  for (const auto& [location, visible] : visibleChanges) {
    PropertyChange selected = NoChange;
    PropertyChange locked = NoChange;

    if (location->subrow) {
      setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
    } else {
      setRowProperties(location->gridRow, visible, selected, locked);
    }
  }

  for (const auto& [location, locked] : lockedChanges) {
    PropertyChange visible = NoChange;
    PropertyChange selected = NoChange;

    if (location->subrow) {
      setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
    } else {
      setRowProperties(location->gridRow, visible, selected, locked);
    }
  }
}

void OSObjectSelector::setRowProperties(const int t_gridRow, PropertyChange t_visible, PropertyChange t_selected, PropertyChange t_locked) {
  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.first->gridRow == t_gridRow) {
      bool changed = false;

      if (t_visible == ChangeToFalse) {
        changed = locationInfoPair.second->setVisible(false) || changed;
      } else if (t_visible == ChangeToTrue) {
        changed = locationInfoPair.second->setVisible(true) || changed;
      } else if (t_visible == ToggleChange) {
        changed = locationInfoPair.second->setVisible(!locationInfoPair.second->isVisible()) || changed;
      }

      if (t_selected == ChangeToFalse) {
        changed = locationInfoPair.second->setSelected(false) || changed;
      } else if (t_selected == ChangeToTrue) {
        changed = locationInfoPair.second->setSelected(true) || changed;
      } else if (t_selected == ToggleChange) {
        changed = locationInfoPair.second->setSelected(!locationInfoPair.second->isSelected()) || changed;
      }

      if (t_locked == ChangeToFalse) {
        changed = locationInfoPair.second->setLocked(false) || changed;
      } else if (t_locked == ChangeToTrue) {
        changed = locationInfoPair.second->setLocked(true) || changed;
      } else if (t_locked == ToggleChange) {
        changed = locationInfoPair.second->setLocked(!locationInfoPair.second->isLocked()) || changed;
      }

      if (changed) {
        emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
      }
    }
  }
}

void OSObjectSelector::setSubrowProperties(const int t_gridRow, const int t_subrow, PropertyChange t_visible, PropertyChange t_selected,
                                           PropertyChange t_locked) {
  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.first->gridRow == t_gridRow && locationInfoPair.first->subrow == t_subrow) {
      bool changed = false;

      if (t_visible == ChangeToFalse) {
        changed = locationInfoPair.second->setVisible(false) || changed;
      } else if (t_visible == ChangeToTrue) {
        changed = locationInfoPair.second->setVisible(true) || changed;
      } else if (t_visible == ToggleChange) {
        changed = locationInfoPair.second->setVisible(!locationInfoPair.second->isVisible()) || changed;
      }

      if (t_selected == ChangeToFalse) {
        changed = locationInfoPair.second->setSelected(false) || changed;
      } else if (t_selected == ChangeToTrue) {
        changed = locationInfoPair.second->setSelected(true) || changed;
      } else if (t_selected == ToggleChange) {
        changed = locationInfoPair.second->setSelected(!locationInfoPair.second->isSelected()) || changed;
      }

      if (t_locked == ChangeToFalse) {
        changed = locationInfoPair.second->setLocked(false) || changed;
      } else if (t_locked == ChangeToTrue) {
        changed = locationInfoPair.second->setLocked(true) || changed;
      } else if (t_locked == ToggleChange) {
        changed = locationInfoPair.second->setLocked(!locationInfoPair.second->isLocked()) || changed;
      }

      if (changed) {
        emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
      }
    }
  }
}

bool OSObjectSelector::getObjectSelected(const model::ModelObject& t_obj) const {
  for (const auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject && info->modelObject.get() == t_obj) {
      if (info->isSelected()) {
        return true;
      }
    }
  }
  return false;
}

void OSObjectSelector::setObjectSelected(const model::ModelObject& t_obj, bool t_selected) {
  const PropertyChange visible = NoChange;
  const PropertyChange selected = (t_selected ? ChangeToTrue : ChangeToFalse);
  const PropertyChange locked = NoChange;
  int numSelected = 0;
  int numSelectable = 0;
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelectable() && info->modelObject && info->modelObject.get() == t_obj) {
      if (location->subrow) {
        setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
      } else {
        setRowProperties(location->gridRow, visible, selected, locked);
      }
    }

    if (info && info->isSelectable()) {
      numSelectable += 1;
      if (info->isSelected()) {
        numSelected += 1;
      }
    }
  }

  emit gridRowSelectionChanged(numSelected, numSelectable);
}

std::set<model::ModelObject> OSObjectSelector::selectorObjects() const {
  std::set<model::ModelObject> result;
  for (const auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelector && info->modelObject) {
      result.insert(info->modelObject.get());
    }
  }
  return result;
}

std::set<model::ModelObject> OSObjectSelector::selectableObjects() const {
  std::set<model::ModelObject> result;
  for (const auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelectable() && info->modelObject) {
      result.insert(info->modelObject.get());
    }
  }
  return result;
}

std::set<model::ModelObject> OSObjectSelector::selectedObjects() const {
  std::set<model::ModelObject> result;
  for (const auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelected() && info->modelObject) {
      result.insert(info->modelObject.get());
    }
  }
  return result;
}

//bool OSObjectSelector::getObjectVisible(const model::ModelObject& t_obj) const {
//  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isVisible() && locationInfoPair.second->modelObject && (locationInfoPair.second->modelObject.get() == t_obj)) {
//      return true;
//    }
//  }
//  return false;
//}

std::function<bool(const model::ModelObject&)> OSObjectSelector::objectFilter() const {
  return m_objectFilter;
}

void OSObjectSelector::setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter) {
  m_objectFilter = t_filter;

  std::vector<std::pair<GridCellLocation*, PropertyChange>> visibleChanges;
  std::vector<std::pair<GridCellLocation*, PropertyChange>> lockedChanges;

  // loop over selector cells first
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject) {
      PropertyChange visibleChange = PropertyChange::ChangeToFalse;
      if (m_objectFilter(info->modelObject.get())) {
        visibleChange = PropertyChange::ChangeToTrue;
      }
      visibleChanges.emplace_back(location, visibleChange);
    }
  }

  // loop over parent cells second
  for (auto& location : m_parentCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject) {
      PropertyChange visibleChange = PropertyChange::ChangeToTrue;
      if (!m_objectFilter(info->modelObject.get())) {
        visibleChange = PropertyChange::ChangeToFalse;

        // parent hidden, selector always hidden
        visibleChanges.erase(
          std::remove_if(visibleChanges.begin(), visibleChanges.end(),
                         [location](std::pair<GridCellLocation*, PropertyChange> tmp) { return location->gridRow == tmp.first->gridRow; }),
          visibleChanges.end());
      }
      visibleChanges.emplace_back(location, visibleChange);
    }
  }

  // apply the changes in reverse order
  std::reverse(visibleChanges.begin(), visibleChanges.end());

  updateRowsAndSubrows(visibleChanges, lockedChanges);
}

void OSObjectSelector::resetObjectFilter() {
  setObjectFilter(getDefaultFilter());
}

//bool OSObjectSelector::getObjectIsLocked(const model::ModelObject& t_obj) const {
//  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isLocked() && locationInfoPair.second->modelObject && (locationInfoPair.second->modelObject.get() == t_obj)) {
//      return true;
//    }
//  }
//  return false;
//}

std::function<bool(const model::ModelObject&)> OSObjectSelector::objectIsLocked() const {
  return m_isLocked;
}

void OSObjectSelector::setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked) {
  m_isLocked = t_isLocked;

  std::vector<std::pair<GridCellLocation*, PropertyChange>> visibleChanges;
  std::vector<std::pair<GridCellLocation*, PropertyChange>> lockedChanges;

  // loop over selector cells first
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject) {
      PropertyChange lockedChange = PropertyChange::NoChange;
      if (m_isLocked(info->modelObject.get())) {
        lockedChange = PropertyChange::ChangeToTrue;
      }
      lockedChanges.emplace_back(location, lockedChange);
    }
  }

  // loop over parent cells second
  for (auto& location : m_parentCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject) {
      PropertyChange lockedChange = PropertyChange::NoChange;
      if (m_isLocked(info->modelObject.get())) {
        lockedChange = PropertyChange::ChangeToTrue;

        // parent locked, selector always locked
        lockedChanges.erase(
          std::remove_if(lockedChanges.begin(), lockedChanges.end(),
                         [location](std::pair<GridCellLocation*, PropertyChange> tmp) { return location->gridRow == tmp.first->gridRow; }),
          lockedChanges.end());
      }
      lockedChanges.emplace_back(location, lockedChange);
    }
  }

  // apply the changes in reverse order
  std::reverse(lockedChanges.begin(), lockedChanges.end());

  updateRowsAndSubrows(visibleChanges, lockedChanges);
}

void OSObjectSelector::resetObjectIsLocked() {
  setObjectIsLocked(getDefaultIsLocked());
}

/*
void OSObjectSelector::applyLocks() {
  std::set<int> gridRowsToLock;
  std::set<std::pair<int, int>> gridRowSubrowsToLock;
  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.second->isLocked() && locationInfoPair.second->isSelector) {
      if (locationInfoPair.first->subrow) {
        gridRowSubrowsToLock.insert(std::make_pair(locationInfoPair.first->gridRow, locationInfoPair.first->subrow.get()));
      } else {
        gridRowsToLock.insert(locationInfoPair.first->gridRow);
      }
    }
  }

  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.first->subrow) {
      if (gridRowSubrowsToLock.count(std::make_pair(locationInfoPair.first->gridRow, locationInfoPair.first->subrow.get())) > 0) {
        bool changed = locationInfoPair.second->setLocked(true);
        if (changed) {
          emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
        }
      }
    } else {
      if (gridRowsToLock.count(locationInfoPair.first->gridRow) > 0) {
        bool changed = locationInfoPair.second->setLocked(true);
        if (changed) {
          emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
        }
      }
    }
  }
}


void OSObjectSelector::onSelectionChanged(int state, int row, int column, boost::optional<int> subrow) {
  const PropertyChange visible = NoChange;
  const PropertyChange selected = ToggleChange;
  const PropertyChange locked = NoChange;
  if (subrow) {
    setSubrowProperties(row, subrow.get(), visible, selected, locked);
  } else {
    setRowProperties(row, visible, selected, locked);
  }
}
*/

GridCellInfo* OSObjectSelector::getGridCellInfo(GridCellLocation* location) const {
  GridCellInfo* result = nullptr;
  auto it = m_gridCellLocationToInfoMap.find(location);
  if (it != m_gridCellLocationToInfoMap.end()) {
    result = it->second;
  }
  return result;
}

std::function<bool(const model::ModelObject&)> OSObjectSelector::getDefaultFilter() {
  return [](const model::ModelObject&) { return true; };
}

std::function<bool(const model::ModelObject&)> OSObjectSelector::getDefaultIsLocked() {
  return [](const model::ModelObject&) { return false; };
}

}  // namespace openstudio
