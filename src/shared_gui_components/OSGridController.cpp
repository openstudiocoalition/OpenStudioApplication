/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2020, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSGridController.hpp"

#include "OSCheckBox.hpp"
#include "OSComboBox.hpp"
#include "OSDoubleEdit.hpp"
#include "OSGridView.hpp"
#include "OSIntegerEdit.hpp"
#include "OSLineEdit.hpp"
#include "OSLoadNamePixmapLineEdit.hpp"
#include "OSQuantityEdit.hpp"
#include "OSUnsignedEdit.hpp"

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
#include <openstudio/model/PlanarSurface.hpp>
#include <openstudio/model/PlanarSurface_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>

// Which is faster?
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
// or
// #include <utilities/idd/IddEnums.hxx>

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

const std::vector<QColor> OSGridController::m_colors = SchedulesView::initializeColors();

GridCellLocation::GridCellLocation(int t_modelRow, int t_gridRow, int t_column, boost::optional<int> t_subrow, QObject* parent)
  : QObject(parent), modelRow(t_modelRow), gridRow(t_gridRow), column(t_column), subrow(t_subrow) {}

GridCellLocation::~GridCellLocation() {}

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

GridCellInfo::~GridCellInfo() {}

bool GridCellInfo::isVisible() const {
  return m_isVisible;
}

bool GridCellInfo::setVisible(bool visible) {
  if (m_isVisible != visible) {
    m_isVisible = visible;
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

ObjectSelector::ObjectSelector(QObject* parent) : QObject(parent), m_objectFilter(getDefaultFilter()) {}

ObjectSelector::~ObjectSelector() {}

void ObjectSelector::clear() {

  auto it = m_gridCellLocationToInfoMap.begin();
  while (it != m_gridCellLocationToInfoMap.end()) {
    delete it->first;
    delete it->second;
    it = m_gridCellLocationToInfoMap.erase(it);
  }

  m_selectorCellLocations.clear();
  m_selectorCellInfos.clear();

  m_objectFilter = getDefaultFilter();
  m_isLocked = getDefaultIsLocked();
}

void ObjectSelector::addObject(const boost::optional<model::ModelObject>& t_obj, Holder* t_holder, int t_modelRow, int t_gridRow, int t_column,
                               const boost::optional<int>& t_subrow, const bool t_isSelector) {

  const bool isVisible = true;
  const bool isSelected = false;
  const bool isLocked = false;
  GridCellInfo* info = new GridCellInfo(t_obj, t_isSelector, isVisible, isSelected, isLocked, this);
  GridCellLocation* location = new GridCellLocation(t_modelRow, t_gridRow, t_column, t_subrow, this);

  connect(t_holder, &Holder::inFocus, location, &GridCellLocation::onInFocus);
  connect(location, &GridCellLocation::inFocus, this, &ObjectSelector::inFocus);

  if (t_isSelector) {
    m_selectorCellLocations.push_back(location);
    m_selectorCellInfos.push_back(info);
  }

  m_gridCellLocationToInfoMap.insert(std::make_pair(location, info));
}

void ObjectSelector::setObjectRemoved(const openstudio::Handle& handle) {
  const PropertyChange visible = NoChange;
  const PropertyChange selected = ChangeToFalse;
  const PropertyChange locked = ChangeToTrue;
  for (const auto location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject && info->modelObject->handle() == handle) {
      if (location->subrow) {
        setSubrowProperties(location->gridRow, location->subrow.get(), visible, selected, locked);
      } else {
        setRowProperties(location->gridRow, visible, selected, locked);
      }
    }
  }
}

//bool ObjectSelector::containsObject(const openstudio::model::ModelObject& t_obj) const {
//  for (const auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isSelector && locationInfoPair.second->modelObject && locationInfoPair.second->modelObject.get() == t_obj) {
//      return true;
//    }
//  }
//  return false;
//}

boost::optional<model::ModelObject> ObjectSelector::getObject(const int t_modelRow, const int t_gridRow, const int t_column,
                                                              const boost::optional<int>& t_subrow) const {
  for (const auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.first->equal(t_modelRow, t_gridRow, t_column, t_subrow)) {
      return locationInfoPair.second->modelObject;
    }
  }
  return boost::none;
}

void ObjectSelector::selectAll() {
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

void ObjectSelector::clearSelection() {
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

void ObjectSelector::setRowProperties(const int t_gridRow, PropertyChange t_visible, PropertyChange t_selected, PropertyChange t_locked) {
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

void ObjectSelector::setSubrowProperties(const int t_gridRow, const int t_subrow, PropertyChange t_visible, PropertyChange t_selected,
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

bool ObjectSelector::getObjectSelected(const model::ModelObject& t_obj) const {
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->modelObject && info->modelObject.get() == t_obj) {
      if (info->isSelected()) {
        return true;
      }
    }
  }
  return false;
}

void ObjectSelector::setObjectSelected(const model::ModelObject& t_obj, bool t_selected) {
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

std::set<model::ModelObject> ObjectSelector::selectableObjects() const {
  std::set<model::ModelObject> result;
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelectable() && info->modelObject) {
      result.insert(info->modelObject.get());
    }
  }
  return result;
}

std::set<model::ModelObject> ObjectSelector::selectedObjects() const {
  std::set<model::ModelObject> result;
  for (auto& location : m_selectorCellLocations) {
    GridCellInfo* info = getGridCellInfo(location);
    if (info && info->isSelected() && info->modelObject) {
      result.insert(info->modelObject.get());
    }
  }
  return result;
}

//bool ObjectSelector::getObjectVisible(const model::ModelObject& t_obj) const {
//  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isVisible() && locationInfoPair.second->modelObject && (locationInfoPair.second->modelObject.get() == t_obj)) {
//      return true;
//    }
//  }
//  return false;
//}

void ObjectSelector::setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter) {
  m_objectFilter = t_filter;

  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    if (locationInfoPair.second->modelObject) {
      bool visible = m_objectFilter(locationInfoPair.second->modelObject.get());
      bool changed = locationInfoPair.second->setVisible(visible);
      if (!visible) {
        // deselect object when it gets filtered out
        changed = locationInfoPair.second->setSelected(false) || changed;
      }
      if (changed) {
        emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
      }
    }
  }
}

void ObjectSelector::resetObjectFilter() {
  setObjectFilter(getDefaultFilter());
}

//bool ObjectSelector::getObjectIsLocked(const model::ModelObject& t_obj) const {
//  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
//    if (locationInfoPair.second->isLocked() && locationInfoPair.second->modelObject && (locationInfoPair.second->modelObject.get() == t_obj)) {
//      return true;
//    }
//  }
//  return false;
//}

void ObjectSelector::setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked) {
  m_isLocked = t_isLocked;

  for (auto& locationInfoPair : m_gridCellLocationToInfoMap) {
    bool locked = false;
    if (locationInfoPair.second->modelObject) {
      locked = m_isLocked(locationInfoPair.second->modelObject.get());
    }
    bool changed = locationInfoPair.second->setLocked(locked);
    if (locked) {
      // deselect object when it gets locked
      changed = locationInfoPair.second->setSelected(false) || changed;
    }
    if (changed) {
      emit gridCellChanged(*locationInfoPair.first, *locationInfoPair.second);
    }
  }
}

void ObjectSelector::resetObjectIsLocked() {
  setObjectIsLocked(getDefaultIsLocked());
}

/*
void ObjectSelector::applyLocks() {
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


void ObjectSelector::onSelectionChanged(int state, int row, int column, boost::optional<int> subrow) {
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

GridCellInfo* ObjectSelector::getGridCellInfo(GridCellLocation* location) const {
  GridCellInfo* result = nullptr;
  auto it = m_gridCellLocationToInfoMap.find(location);
  if (it != m_gridCellLocationToInfoMap.end()) {
    result = it->second;
  }
  return result;
}

std::function<bool(const model::ModelObject&)> ObjectSelector::getDefaultFilter() {
  return [](const model::ModelObject&) { return true; };
}

std::function<bool(const model::ModelObject&)> ObjectSelector::getDefaultIsLocked() {
  return [](const model::ModelObject&) { return false; };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OSGridController::OSGridController()
  : QObject(),
    m_hasHorizontalHeader(true),
    m_currentCategoryIndex(0),
    m_isIP(false),
    m_horizontalHeaderBtnGrp(nullptr),
    m_objectSelector(std::make_shared<ObjectSelector>(this)) {}

OSGridController::OSGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                   const std::vector<model::ModelObject>& modelObjects)
  : QObject(),
    m_iddObjectType(iddObjectType),
    m_modelObjects(modelObjects),
    m_categoriesAndFields(std::vector<std::pair<QString, std::vector<QString>>>()),
    m_baseConcepts(std::vector<QSharedPointer<BaseConcept>>()),
    m_horizontalHeaders(std::vector<QWidget*>()),
    m_hasHorizontalHeader(true),
    m_currentCategory(QString()),
    m_currentCategoryIndex(0),
    m_currentFields(std::vector<QString>()),
    m_customFields(std::vector<QString>()),
    m_model(model),
    m_isIP(isIP),
    m_horizontalHeaderBtnGrp(nullptr),
    m_headerText(headerText),
    m_objectSelector(std::make_shared<ObjectSelector>(this)) {
  loadQSettings();

  connect(m_objectSelector.get(), &ObjectSelector::inFocus, this, &OSGridController::onInFocus);
  connect(m_objectSelector.get(), &ObjectSelector::gridRowSelectionChanged, this, &OSGridController::gridRowSelectionChanged);
  connect(m_objectSelector.get(), &ObjectSelector::gridCellChanged, this, &OSGridController::gridCellChanged);
}

OSGridController::~OSGridController() {
  disconnectFromModelSignals();
  saveQSettings();
}

void OSGridController::loadQSettings() {
  QSettings settings("OpenStudio", m_headerText);
  auto temp = settings.value("customFields").toStringList().toVector();
  m_customFields = std::vector<QString>(temp.begin(), temp.end());
}

void OSGridController::saveQSettings() const {
  QSettings settings("OpenStudio", m_headerText);
  QVector<QVariant> vector;
  for (unsigned i = 0; i < m_customFields.size(); i++) {
    QVariant variant = m_customFields.at(i);
    vector.push_back(variant);
  }
  QList<QVariant> list = vector.toList();
  settings.setValue("customCategories", list);
}

IddObjectType OSGridController::iddObjectType() const {
  return m_iddObjectType;
}

bool OSGridController::isIP() const {
  return m_isIP;
}

bool OSGridController::hasHorizontalHeader() const {
  return m_hasHorizontalHeader;
}

std::vector<QWidget*> OSGridController::horizontalHeaders() const {
  return m_horizontalHeaders;
}

void OSGridController::setConstructionColumn(int constructionColumn) {
  m_constructionColumn = constructionColumn;
}

model::Model& OSGridController::model() {
  return m_model;
}

std::vector<model::ModelObject> OSGridController::modelObjects() const {
  return m_modelObjects;
}

void OSGridController::setModelObjects(const std::vector<model::ModelObject>& modelObjects) {
  m_modelObjects = modelObjects;
}
/*
std::vector<model::ModelObject> OSGridController::inheritedModelObjects() const {
  return m_inheritedModelObjects;
}

void OSGridController::setInheritedModelObjects(const std::vector<model::ModelObject>& inheritedModelObjects) {
  m_inheritedModelObjects = inheritedModelObjects;
}
*/
void OSGridController::addCategoryAndFields(const std::pair<QString, std::vector<QString>>& categoryAndFields) {
  m_categoriesAndFields.push_back(categoryAndFields);
}

void OSGridController::resetCategoryAndFields() {
  m_categoriesAndFields.clear();
}

void OSGridController::setCategoriesAndFields() {
  std::vector<QString> fields;
  std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Custom"), fields);
  addCategoryAndFields(categoryAndFields);

  setCustomCategoryAndFields();
}

std::vector<QString> OSGridController::categories() {
  std::vector<QString> categories;

  for (unsigned i = 0; i < m_categoriesAndFields.size(); i++) {
    categories.push_back(m_categoriesAndFields.at(i).first);
  }

  return categories;
}

std::vector<std::pair<QString, std::vector<QString>>> OSGridController::categoriesAndFields() {
  return m_categoriesAndFields;
}

void OSGridController::onCategorySelected(int index) {
  m_objectSelector->clear();
  m_currentCategoryIndex = index;
  m_focusedCellLocation = std::make_tuple(-1, -1, -1);

  m_currentCategory = m_categoriesAndFields.at(index).first;

  m_currentFields = m_categoriesAndFields.at(index).second;

  addColumns(m_currentCategory, m_currentFields);

  // One of the only times we request a recreate all
  emit recreateAll();
}

void OSGridController::setHorizontalHeader(QWidget* gridView) {
  m_horizontalHeaders.clear();

  if (m_horizontalHeaderBtnGrp == nullptr) {
    m_horizontalHeaderBtnGrp = new QButtonGroup();
    m_horizontalHeaderBtnGrp->setExclusive(false);

    connect(m_horizontalHeaderBtnGrp, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this,
            &OSGridController::onHorizontalHeaderChecked);

  } else {
    QList<QAbstractButton*> buttons = m_horizontalHeaderBtnGrp->buttons();
    for (int i = 0; i < buttons.size(); i++) {
      m_horizontalHeaderBtnGrp->removeButton(buttons.at(i));
      OS_ASSERT(buttons.at(i));
      delete buttons.at(i);
    }
  }

  QList<QAbstractButton*> buttons = m_horizontalHeaderBtnGrp->buttons();
  OS_ASSERT(buttons.size() == 0);

  for (const QString& field : m_currentFields) {
    auto horizontalHeaderWidget = new HorizontalHeaderWidget(field, gridView);
    m_horizontalHeaderBtnGrp->addButton(horizontalHeaderWidget->m_checkBox, m_horizontalHeaderBtnGrp->buttons().size());
    m_horizontalHeaders.push_back(horizontalHeaderWidget);
  }

  checkSelectedFields();
}

QWidget* OSGridController::makeWidget(model::ModelObject t_mo, const QSharedPointer<BaseConcept>& t_baseConcept, OSGridView* gridView) {
  QWidget* widget = nullptr;

  if (QSharedPointer<CheckBoxConcept> checkBoxConcept = t_baseConcept.dynamicCast<CheckBoxConcept>()) {

    // This is basically for a row in the "Select All" column
    auto checkBox = new OSCheckBox3(gridView);  // OSCheckBox3 is derived from QCheckBox, whereas OSCheckBox2 is derived from QPushButton
    if (checkBoxConcept->tooltip().size()) {
      checkBox->setToolTip(checkBoxConcept->tooltip().c_str());
    }

    checkBox->bind(t_mo, BoolGetter(std::bind(&CheckBoxConcept::get, checkBoxConcept.data(), t_mo)),
                   boost::optional<BoolSetter>(std::bind(&CheckBoxConcept::set, checkBoxConcept.data(), t_mo, std::placeholders::_1)));

    if (checkBoxConcept->isLocked(t_mo)) {
      checkBox->setLocked(true);
    }

    widget = checkBox;

  } else if (auto checkBoxConceptBoolReturn = t_baseConcept.dynamicCast<CheckBoxConceptBoolReturn>()) {
    // This is for a proper setter **that returns a bool**, such as Ideal Air Loads column
    auto checkBoxBoolReturn = new OSCheckBox3(gridView);
    if (checkBoxConceptBoolReturn->tooltip().size()) {
      checkBoxBoolReturn->setToolTip(checkBoxConceptBoolReturn->tooltip().c_str());
    }

    if (checkBoxConceptBoolReturn->hasClickFocus()) {
      checkBoxBoolReturn->enableClickFocus();
    }

    checkBoxBoolReturn->bind(t_mo, BoolGetter(std::bind(&CheckBoxConceptBoolReturn::get, checkBoxConceptBoolReturn.data(), t_mo)),
                             boost::optional<BoolSetterBoolReturn>(
                               std::bind(&CheckBoxConceptBoolReturn::set, checkBoxConceptBoolReturn.data(), t_mo, std::placeholders::_1)));

    if (checkBoxConceptBoolReturn->isLocked(t_mo)) {
      checkBoxBoolReturn->setLocked(true);
    }

    widget = checkBoxBoolReturn;

  } else if (QSharedPointer<ComboBoxConcept> comboBoxConcept = t_baseConcept.dynamicCast<ComboBoxConcept>()) {

    auto choiceConcept = comboBoxConcept->choiceConcept(t_mo);

    auto comboBox = new OSComboBox2(gridView, choiceConcept->editable());
    if (comboBoxConcept->hasClickFocus()) {
      comboBox->enableClickFocus();
    }

    comboBox->bind(t_mo, choiceConcept);

    if (comboBoxConcept->isLocked(t_mo)) {
      comboBox->setLocked(true);
    }

    widget = comboBox;

  } else if (QSharedPointer<ValueEditConcept<double>> doubleEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<double>>()) {

    auto doubleEdit = new OSDoubleEdit2(gridView);
    if (doubleEditConcept->hasClickFocus()) {
      doubleEdit->enableClickFocus();
    }

    doubleEdit->bind(t_mo, DoubleGetter(std::bind(&ValueEditConcept<double>::get, doubleEditConcept.data(), t_mo)),
                     boost::optional<DoubleSetter>(std::bind(&ValueEditConcept<double>::set, doubleEditConcept.data(), t_mo, std::placeholders::_1)),
                     boost::optional<NoFailAction>(std::bind(&ValueEditConcept<double>::reset, doubleEditConcept.data(), t_mo)),
                     boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
                     boost::optional<BasicQuery>(std::bind(&ValueEditConcept<double>::isDefaulted, doubleEditConcept.data(), t_mo)));

    if (doubleEditConcept->isLocked(t_mo)) {
      doubleEdit->setLocked(true);
    }

    widget = doubleEdit;

  } else if (QSharedPointer<OptionalValueEditConcept<double>> optionalDoubleEditConcept =
               t_baseConcept.dynamicCast<OptionalValueEditConcept<double>>()) {

    auto optionalDoubleEdit = new OSDoubleEdit2(gridView);
    if (optionalDoubleEditConcept->hasClickFocus()) {
      optionalDoubleEdit->enableClickFocus();
    }

    optionalDoubleEdit->bind(t_mo, OptionalDoubleGetter(std::bind(&OptionalValueEditConcept<double>::get, optionalDoubleEditConcept.data(), t_mo)),
                             boost::optional<DoubleSetter>(
                               std::bind(&OptionalValueEditConcept<double>::set, optionalDoubleEditConcept.data(), t_mo, std::placeholders::_1)));

    if (optionalDoubleEditConcept->isLocked(t_mo)) {
      optionalDoubleEdit->setLocked(true);
    }

    widget = optionalDoubleEdit;

  } else if (QSharedPointer<ValueEditVoidReturnConcept<double>> doubleEditVoidReturnConcept =
               t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<double>>()) {

    auto doubleEditVoidReturn = new OSDoubleEdit2(gridView);
    if (doubleEditVoidReturnConcept->hasClickFocus()) {
      doubleEditVoidReturn->enableClickFocus();
    }

    doubleEditVoidReturn->bind(
      t_mo, DoubleGetter(std::bind(&ValueEditVoidReturnConcept<double>::get, doubleEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(std::bind(&ValueEditVoidReturnConcept<double>::set, doubleEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&ValueEditVoidReturnConcept<double>::reset, doubleEditVoidReturnConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&ValueEditVoidReturnConcept<double>::isDefaulted, doubleEditVoidReturnConcept.data(), t_mo)));

    if (doubleEditVoidReturnConcept->isLocked(t_mo)) {
      doubleEditVoidReturn->setLocked(true);
    }

    widget = doubleEditVoidReturn;

  } else if (QSharedPointer<OptionalValueEditVoidReturnConcept<double>> optionalDoubleEditVoidReturnConcept =
               t_baseConcept.dynamicCast<OptionalValueEditVoidReturnConcept<double>>()) {

    auto optionalDoubleEditVoidReturn = new OSDoubleEdit2(gridView);
    if (optionalDoubleEditVoidReturnConcept->hasClickFocus()) {
      optionalDoubleEditVoidReturn->enableClickFocus();
    }

    optionalDoubleEditVoidReturn->bind(
      t_mo, OptionalDoubleGetter(std::bind(&OptionalValueEditVoidReturnConcept<double>::get, optionalDoubleEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&OptionalValueEditVoidReturnConcept<double>::set, optionalDoubleEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)));

    if (optionalDoubleEditVoidReturnConcept->isLocked(t_mo)) {
      optionalDoubleEditVoidReturn->setLocked(true);
    }

    widget = optionalDoubleEditVoidReturn;

  } else if (QSharedPointer<ValueEditConcept<int>> integerEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<int>>()) {

    auto integerEdit = new OSIntegerEdit2(gridView);
    if (integerEditConcept->hasClickFocus()) {
      integerEdit->enableClickFocus();
    }

    integerEdit->bind(t_mo, IntGetter(std::bind(&ValueEditConcept<int>::get, integerEditConcept.data(), t_mo)),
                      boost::optional<IntSetter>(std::bind(&ValueEditConcept<int>::set, integerEditConcept.data(), t_mo, std::placeholders::_1)),
                      boost::optional<NoFailAction>(std::bind(&ValueEditConcept<int>::reset, integerEditConcept.data(), t_mo)),
                      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
                      boost::optional<BasicQuery>(std::bind(&ValueEditConcept<int>::isDefaulted, integerEditConcept.data(), t_mo)));

    if (integerEditConcept->isLocked(t_mo)) {
      integerEdit->setLocked(true);
    }

    widget = integerEdit;

  } else if (QSharedPointer<ValueEditConcept<std::string>> lineEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<std::string>>()) {

    auto lineEdit = new OSLineEdit2(gridView);
    if (lineEditConcept->hasClickFocus()) {
      lineEdit->enableClickFocus();
    }

    lineEdit->bind(t_mo, StringGetter(std::bind(&ValueEditConcept<std::string>::get, lineEditConcept.data(), t_mo)),
                   boost::optional<StringSetter>(std::bind(&ValueEditConcept<std::string>::set, lineEditConcept.data(), t_mo, std::placeholders::_1)),
                   boost::optional<NoFailAction>(std::bind(&ValueEditConcept<std::string>::reset, lineEditConcept.data(), t_mo)),
                   boost::optional<BasicQuery>(std::bind(&ValueEditConcept<std::string>::isDefaulted, lineEditConcept.data(), t_mo)));

    if (lineEditConcept->isLocked(t_mo)) {
      lineEdit->setLocked(true);
    }

    connect(lineEdit, &OSLineEdit2::objectRemoved, this, &OSGridController::onObjectRemoved);

    widget = lineEdit;

  } else if (QSharedPointer<ValueEditVoidReturnConcept<std::string>> lineEditConcept =
               t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<std::string>>()) {

    auto lineEdit = new OSLineEdit2(gridView);
    if (lineEditConcept->hasClickFocus()) {
      lineEdit->enableClickFocus();
    }

    lineEdit->bind(t_mo, StringGetter(std::bind(&ValueEditVoidReturnConcept<std::string>::get, lineEditConcept.data(), t_mo)),
                   boost::optional<StringSetterVoidReturn>(
                     std::bind(&ValueEditVoidReturnConcept<std::string>::set, lineEditConcept.data(), t_mo, std::placeholders::_1)),
                   boost::optional<NoFailAction>(std::bind(&ValueEditVoidReturnConcept<std::string>::reset, lineEditConcept.data(), t_mo)),
                   boost::optional<BasicQuery>(std::bind(&ValueEditVoidReturnConcept<std::string>::isDefaulted, lineEditConcept.data(), t_mo)));

    if (lineEditConcept->isLocked(t_mo)) {
      lineEdit->setLocked(true);
    }

    connect(lineEdit, &OSLineEdit2::objectRemoved, this, &OSGridController::onObjectRemoved);

    widget = lineEdit;

  } else if (QSharedPointer<NameLineEditConcept> nameLineEditConcept = t_baseConcept.dynamicCast<NameLineEditConcept>()) {

    OSLineEdit2Interface* nameLineEdit = nameLineEditConcept->makeWidget(gridView);
    if (nameLineEditConcept->hasClickFocus()) {
      nameLineEdit->enableClickFocus();
    }

    if (nameLineEditConcept->deleteObject()) {
      nameLineEdit->setDeleteType(DeleteType::DeleteIfNotDefaulted);
    } else {
      nameLineEdit->setDeleteType(DeleteType::NoDelete);
    }

    nameLineEdit->bind(
      t_mo, OptionalStringGetter(std::bind(&NameLineEditConcept::get, nameLineEditConcept.data(), t_mo, true)),
      boost::optional<StringSetter>(std::bind(&NameLineEditConcept::setReturnBool, nameLineEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&NameLineEditConcept::reset, nameLineEditConcept.data(), t_mo)),
      boost::optional<BasicQuery>(std::bind(&NameLineEditConcept::isInherited, nameLineEditConcept.data(), t_mo)),
      boost::optional<BasicQuery>(std::bind(&NameLineEditConcept::isLocked, nameLineEditConcept.data(), t_mo)));

    if (nameLineEditConcept->isLocked(t_mo)) {
      nameLineEdit->setLocked(true);
    }

    widget = nameLineEdit->qwidget();

    if (nameLineEditConcept->isInspectable()) {
      if (OSLineEdit2* normalLineEdit = qobject_cast<OSLineEdit2*>(widget)) {
        connect(normalLineEdit, &OSLineEdit2::itemClicked, gridView, &OSGridView::dropZoneItemClicked);
        connect(normalLineEdit, &OSLineEdit2::objectRemoved, this, &OSGridController::onObjectRemoved);
      } else if (OSLoadNamePixmapLineEdit* pixmapLineEdit = qobject_cast<OSLoadNamePixmapLineEdit*>(widget)) {
        connect(pixmapLineEdit, &OSLoadNamePixmapLineEdit::itemClicked, gridView, &OSGridView::dropZoneItemClicked);
        connect(pixmapLineEdit, &OSLoadNamePixmapLineEdit::objectRemoved, this, &OSGridController::onObjectRemoved);
      }
    }

  } else if (QSharedPointer<QuantityEditConcept<double>> quantityEditConcept = t_baseConcept.dynamicCast<QuantityEditConcept<double>>()) {

    OSQuantityEdit2* quantityEdit =
      new OSQuantityEdit2(quantityEditConcept->modelUnits().toStdString().c_str(), quantityEditConcept->siUnits().toStdString().c_str(),
                          quantityEditConcept->ipUnits().toStdString().c_str(), quantityEditConcept->isIP(), gridView);
    if (quantityEditConcept->hasClickFocus()) {
      quantityEdit->enableClickFocus();
    }

    quantityEdit->bind(
      m_isIP, t_mo, DoubleGetter(std::bind(&QuantityEditConcept<double>::get, quantityEditConcept.data(), t_mo)),
      boost::optional<DoubleSetter>(std::bind(&QuantityEditConcept<double>::set, quantityEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&QuantityEditConcept<double>::reset, quantityEditConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&QuantityEditConcept<double>::isDefaulted, quantityEditConcept.data(), t_mo)));

    if (quantityEditConcept->isLocked(t_mo)) {
      quantityEdit->setLocked(true);
    }

    connect(this, &OSGridController::toggleUnitsClicked, quantityEdit, &OSQuantityEdit2::onUnitSystemChange);

    widget = quantityEdit;

  } else if (QSharedPointer<OptionalQuantityEditConcept<double>> optionalQuantityEditConcept =
               t_baseConcept.dynamicCast<OptionalQuantityEditConcept<double>>()) {

    OSQuantityEdit2* optionalQuantityEdit = new OSQuantityEdit2(
      optionalQuantityEditConcept->modelUnits().toStdString().c_str(), optionalQuantityEditConcept->siUnits().toStdString().c_str(),
      optionalQuantityEditConcept->ipUnits().toStdString().c_str(), optionalQuantityEditConcept->isIP(), gridView);
    if (optionalQuantityEditConcept->hasClickFocus()) {
      optionalQuantityEdit->enableClickFocus();
    }

    optionalQuantityEdit->bind(m_isIP, t_mo,
                               OptionalDoubleGetter(std::bind(&OptionalQuantityEditConcept<double>::get, optionalQuantityEditConcept.data(), t_mo)),
                               boost::optional<DoubleSetter>(std::bind(&OptionalQuantityEditConcept<double>::set, optionalQuantityEditConcept.data(),
                                                                       t_mo, std::placeholders::_1)));

    if (optionalQuantityEditConcept->isLocked(t_mo)) {
      optionalQuantityEdit->setLocked(true);
    }

    connect(this, &OSGridController::toggleUnitsClicked, optionalQuantityEdit, &OSQuantityEdit2::onUnitSystemChange);

    widget = optionalQuantityEdit;

  } else if (QSharedPointer<QuantityEditVoidReturnConcept<double>> quantityEditVoidReturnConcept =
               t_baseConcept.dynamicCast<QuantityEditVoidReturnConcept<double>>()) {

    OSQuantityEdit2* quantityEditVoidReturn = new OSQuantityEdit2(
      quantityEditVoidReturnConcept->modelUnits().toStdString().c_str(), quantityEditVoidReturnConcept->siUnits().toStdString().c_str(),
      quantityEditVoidReturnConcept->ipUnits().toStdString().c_str(), quantityEditVoidReturnConcept->isIP(), gridView);
    if (quantityEditVoidReturnConcept->hasClickFocus()) {
      quantityEditVoidReturn->enableClickFocus();
    }

    quantityEditVoidReturn->bind(
      m_isIP, t_mo, DoubleGetter(std::bind(&QuantityEditVoidReturnConcept<double>::get, quantityEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&QuantityEditVoidReturnConcept<double>::set, quantityEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&QuantityEditVoidReturnConcept<double>::reset, quantityEditVoidReturnConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&QuantityEditVoidReturnConcept<double>::isDefaulted, quantityEditVoidReturnConcept.data(), t_mo)));

    if (quantityEditVoidReturnConcept->isLocked(t_mo)) {
      quantityEditVoidReturn->setLocked(true);
    }

    connect(this, &OSGridController::toggleUnitsClicked, quantityEditVoidReturn, &OSQuantityEdit2::onUnitSystemChange);

    widget = quantityEditVoidReturn;

  } else if (QSharedPointer<OptionalQuantityEditVoidReturnConcept<double>> optionalQuantityEditVoidReturnConcept =
               t_baseConcept.dynamicCast<OptionalQuantityEditVoidReturnConcept<double>>()) {

    OSQuantityEdit2* optionalQuantityEditVoidReturn = new OSQuantityEdit2(optionalQuantityEditVoidReturnConcept->modelUnits().toStdString().c_str(),
                                                                          optionalQuantityEditVoidReturnConcept->siUnits().toStdString().c_str(),
                                                                          optionalQuantityEditVoidReturnConcept->ipUnits().toStdString().c_str(),
                                                                          optionalQuantityEditVoidReturnConcept->isIP(), gridView);
    if (optionalQuantityEditVoidReturnConcept->hasClickFocus()) {
      optionalQuantityEditVoidReturn->enableClickFocus();
    }

    optionalQuantityEditVoidReturn->bind(
      m_isIP, t_mo,
      OptionalDoubleGetter(std::bind(&OptionalQuantityEditVoidReturnConcept<double>::get, optionalQuantityEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&OptionalQuantityEditVoidReturnConcept<double>::set, optionalQuantityEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)));

    if (optionalQuantityEditVoidReturnConcept->isLocked(t_mo)) {
      optionalQuantityEditVoidReturn->setLocked(true);
    }

    connect(this, &OSGridController::toggleUnitsClicked, optionalQuantityEditVoidReturn, &OSQuantityEdit2::onUnitSystemChange);

    widget = optionalQuantityEditVoidReturn;

  } else if (QSharedPointer<ValueEditConcept<unsigned>> unsignedEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<unsigned>>()) {

    auto unsignedEdit = new OSUnsignedEdit2(gridView);
    if (unsignedEditConcept->hasClickFocus()) {
      unsignedEdit->enableClickFocus();
    }

    unsignedEdit->bind(
      t_mo, UnsignedGetter(std::bind(&ValueEditConcept<unsigned>::get, unsignedEditConcept.data(), t_mo)),
      boost::optional<UnsignedSetter>(std::bind(&ValueEditConcept<unsigned>::set, unsignedEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&ValueEditConcept<unsigned>::reset, unsignedEditConcept.data(), t_mo)), boost::optional<NoFailAction>(),
      boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&ValueEditConcept<unsigned>::isDefaulted, unsignedEditConcept.data(), t_mo)));

    if (unsignedEditConcept->isLocked(t_mo)) {
      unsignedEdit->setLocked(true);
    }

    widget = unsignedEdit;

  } else if (QSharedPointer<DropZoneConcept> dropZoneConcept = t_baseConcept.dynamicCast<DropZoneConcept>()) {
    auto dropZone = new OSDropZone2();
    if (dropZoneConcept->hasClickFocus()) {
      dropZone->enableClickFocus();
    }

    dropZone->bind(t_mo, OptionalModelObjectGetter(std::bind(&DropZoneConcept::get, dropZoneConcept.data(), t_mo)),
                   ModelObjectSetter(std::bind(&DropZoneConcept::set, dropZoneConcept.data(), t_mo, std::placeholders::_1)),
                   NoFailAction(std::bind(&DropZoneConcept::reset, dropZoneConcept.data(), t_mo)),
                   ModelObjectIsDefaulted(std::bind(&DropZoneConcept::isDefaulted, dropZoneConcept.data(), t_mo)));

    if (dropZoneConcept->isLocked(t_mo)) {
      dropZone->setLocked(true);
    }

    //connect(dropZone, OSDropZone2::itemClicked, gridView(), OSGridView::dropZoneItemClicked);
    connect(dropZone, &OSDropZone2::itemClicked, gridView, &OSGridView::dropZoneItemClicked);

    connect(dropZone, &OSDropZone2::objectRemoved, this, &OSGridController::onObjectRemoved);

    widget = dropZone;

  } else if (QSharedPointer<RenderingColorConcept> renderingColorConcept = t_baseConcept.dynamicCast<RenderingColorConcept>()) {
    auto renderingColorWidget = new RenderingColorWidget2(gridView);

    renderingColorWidget->bind(t_mo, OptionalModelObjectGetter(std::bind(&RenderingColorConcept::get, renderingColorConcept.data(), t_mo)),
                               ModelObjectSetter(std::bind(&RenderingColorConcept::set, renderingColorConcept.data(), t_mo, std::placeholders::_1)));

    if (renderingColorConcept->isLocked(t_mo)) {
      renderingColorWidget->setLocked(true);
    }

    widget = renderingColorWidget;

  } else {
    // Unknown type
    OS_ASSERT(false);
  }

  return widget;
}

void OSGridController::setConceptValue(model::ModelObject t_setterMO, model::ModelObject t_getterMO,
                                       const QSharedPointer<BaseConcept>& t_baseConcept) {
  if (QSharedPointer<CheckBoxConcept> concept = t_baseConcept.dynamicCast<CheckBoxConcept>()) {
    auto setter = std::bind(&CheckBoxConcept::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&CheckBoxConcept::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<CheckBoxConceptBoolReturn> concept = t_baseConcept.dynamicCast<CheckBoxConceptBoolReturn>()) {
    auto setter = std::bind(&CheckBoxConceptBoolReturn::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&CheckBoxConceptBoolReturn::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<ComboBoxConcept> concept = t_baseConcept.dynamicCast<ComboBoxConcept>()) {
    auto getterChoiceConcept = concept->choiceConcept(t_getterMO);
    auto setterChoiceConcept = concept->choiceConcept(t_setterMO);
    auto getter = std::bind(&ChoiceConcept::get, getterChoiceConcept.get());
    auto setter = std::bind(&ChoiceConcept::set, setterChoiceConcept.get(), std::placeholders::_1);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<ValueEditConcept<double>> concept = t_baseConcept.dynamicCast<ValueEditConcept<double>>()) {
    auto setter = std::bind(&ValueEditConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<OptionalValueEditConcept<double>> concept = t_baseConcept.dynamicCast<OptionalValueEditConcept<double>>()) {
    auto setter = std::bind(&OptionalValueEditConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&OptionalValueEditConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<ValueEditVoidReturnConcept<double>> concept = t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<double>>()) {
    auto setter = std::bind(&ValueEditVoidReturnConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditVoidReturnConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<OptionalValueEditVoidReturnConcept<double>> concept =
               t_baseConcept.dynamicCast<OptionalValueEditVoidReturnConcept<double>>()) {
    auto setter = std::bind(&OptionalValueEditVoidReturnConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&OptionalValueEditVoidReturnConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<ValueEditConcept<int>> concept = t_baseConcept.dynamicCast<ValueEditConcept<int>>()) {
    auto setter = std::bind(&ValueEditConcept<int>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditConcept<int>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<ValueEditConcept<std::string>> concept = t_baseConcept.dynamicCast<ValueEditConcept<std::string>>()) {
    auto setter = std::bind(&ValueEditConcept<std::string>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditConcept<std::string>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<ValueEditVoidReturnConcept<std::string>> concept = t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<std::string>>()) {
    auto setter = std::bind(&ValueEditVoidReturnConcept<std::string>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditVoidReturnConcept<std::string>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<NameLineEditConcept> concept = t_baseConcept.dynamicCast<NameLineEditConcept>()) {
    auto setter = std::bind(&NameLineEditConcept::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&NameLineEditConcept::get, concept.data(), t_getterMO, true);  // NOTE Evan: Do we always want true?
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<QuantityEditConcept<double>> concept = t_baseConcept.dynamicCast<QuantityEditConcept<double>>()) {
    auto setter = std::bind(&QuantityEditConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&QuantityEditConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<OptionalQuantityEditConcept<double>> concept = t_baseConcept.dynamicCast<OptionalQuantityEditConcept<double>>()) {
    auto setter = std::bind(&OptionalQuantityEditConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&OptionalQuantityEditConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<QuantityEditVoidReturnConcept<double>> concept = t_baseConcept.dynamicCast<QuantityEditVoidReturnConcept<double>>()) {
    auto setter = std::bind(&QuantityEditVoidReturnConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&QuantityEditVoidReturnConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<OptionalQuantityEditVoidReturnConcept<double>> concept =
               t_baseConcept.dynamicCast<OptionalQuantityEditVoidReturnConcept<double>>()) {
    auto setter = std::bind(&OptionalQuantityEditVoidReturnConcept<double>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&OptionalQuantityEditVoidReturnConcept<double>::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<ValueEditConcept<unsigned>> concept = t_baseConcept.dynamicCast<ValueEditConcept<unsigned>>()) {
    auto setter = std::bind(&ValueEditConcept<unsigned>::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&ValueEditConcept<unsigned>::get, concept.data(), t_getterMO);
    auto temp = getter();
    setter(temp);
  } else if (QSharedPointer<DropZoneConcept> concept = t_baseConcept.dynamicCast<DropZoneConcept>()) {
    auto setter = std::bind(&DropZoneConcept::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&DropZoneConcept::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else if (QSharedPointer<RenderingColorConcept> concept = t_baseConcept.dynamicCast<RenderingColorConcept>()) {
    auto setter = std::bind(&RenderingColorConcept::set, concept.data(), t_setterMO, std::placeholders::_1);
    auto getter = std::bind(&RenderingColorConcept::get, concept.data(), t_getterMO);
    auto temp = getter();
    if (temp) setter(temp.get());
  } else {
    // Unknown type
    OS_ASSERT(false);
  }
}

void OSGridController::setConceptValue(model::ModelObject t_setterMO, model::ModelObject t_getterMO,
                                       const QSharedPointer<BaseConcept>& t_setterBaseConcept,
                                       const QSharedPointer<BaseConcept>& t_getterBaseConcept) {
  if (t_getterBaseConcept.dynamicCast<NameLineEditConcept>()) {
    if (QSharedPointer<DropZoneConcept> setterConcept = t_setterBaseConcept.dynamicCast<DropZoneConcept>()) {
      auto mo = m_model.getModelObject<model::ModelObject>(t_getterMO.handle());
      OS_ASSERT(mo);
      auto setter = std::bind(&DropZoneConcept::set, setterConcept.data(), t_setterMO, std::placeholders::_1);
      setter(mo.get());
    }
  } else {
    // No other combination is currently in use
    // Should never get here
    OS_ASSERT(false);
  }
}

void OSGridController::resetConceptValue(model::ModelObject t_resetMO, const QSharedPointer<BaseConcept>& t_baseConcept) {
  if (QSharedPointer<ValueEditConcept<double>> concept = t_baseConcept.dynamicCast<ValueEditConcept<double>>()) {
    auto reset = std::bind(&ValueEditConcept<double>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<ValueEditVoidReturnConcept<double>> concept = t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<double>>()) {
    auto reset = std::bind(&ValueEditVoidReturnConcept<double>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<ValueEditConcept<int>> concept = t_baseConcept.dynamicCast<ValueEditConcept<int>>()) {
    auto reset = std::bind(&ValueEditConcept<int>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<ValueEditConcept<std::string>> concept = t_baseConcept.dynamicCast<ValueEditConcept<std::string>>()) {
    auto reset = std::bind(&ValueEditConcept<std::string>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<NameLineEditConcept> concept = t_baseConcept.dynamicCast<NameLineEditConcept>()) {
    auto reset = std::bind(&NameLineEditConcept::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<QuantityEditConcept<double>> concept = t_baseConcept.dynamicCast<QuantityEditConcept<double>>()) {
    auto reset = std::bind(&QuantityEditConcept<double>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<QuantityEditVoidReturnConcept<double>> concept = t_baseConcept.dynamicCast<QuantityEditVoidReturnConcept<double>>()) {
    auto reset = std::bind(&QuantityEditVoidReturnConcept<double>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<ValueEditConcept<unsigned>> concept = t_baseConcept.dynamicCast<ValueEditConcept<unsigned>>()) {
    auto reset = std::bind(&ValueEditConcept<unsigned>::reset, concept.data(), t_resetMO);
    reset();
  } else if (QSharedPointer<DropZoneConcept> concept = t_baseConcept.dynamicCast<DropZoneConcept>()) {
    auto reset = std::bind(&DropZoneConcept::reset, concept.data(), t_resetMO);
    reset();
  } else {
    // Unknown type
    OS_ASSERT(false);
  }
}

QWidget* OSGridController::createWidget(int gridRow, int column, OSGridView* gridView) {
  OS_ASSERT(gridRow >= 0);
  OS_ASSERT(column >= 0);

  // Note: If there is a horizontal header row,  m_modelObjects[0] starts on gridLayout[1]
  int modelRow = modelRowFromGridRow(gridRow);  //m_hasHorizontalHeader ? gridRow - 1 : gridRow;

  OS_ASSERT(static_cast<int>(m_modelObjects.size()) > modelRow);
  OS_ASSERT(static_cast<int>(m_baseConcepts.size()) > column);

  auto layout = new QGridLayout();
  const int widgetHeight = 30;
  int numWidgets = 0;

  // start with sane default values
  const bool isSelector = false;
  const bool isVisible = true;
  const bool isSelected = false;
  const bool isLocked = false;
  bool hasSubRows = false;

  // wrapper - this is the thing that will be returned by this method.  The outermost widget that forms a gridview cell.
  // May contain sub rows.
  auto wrapper = new QWidget(gridView);
  wrapper->setObjectName("TableCell");
  gridView->setCellProperties(wrapper, isSelector, gridRow, column, boost::none, isVisible, isSelected, isLocked);
  wrapper->setStyleSheet(gridView->cellStyle());
  layout->setSpacing(0);
  layout->setVerticalSpacing(0);
  layout->setHorizontalSpacing(0);
  layout->setContentsMargins(5, 5, 5, 5);
  wrapper->setLayout(layout);
  // end wrapper

  // Holder is a Widget that corresponds to sub row cell, it is present even if the cell does not have sub rows
  // When no subrows wrapper -> holder -> bindable control provided by ::makeWidget
  std::vector<Holder*> holders;
  // addWidget lambda adds to holders (by first creating a new holder).
  // Also adds to layout, which is the layout of the main cell (wrapper).
  // holders and layout are accessible in the lambda through capture.
  // t_widget will be provided by ::makeWidget, it is the bindable control

  ///////////////////////////////////////////////////////////////////////////////////////
  // Begin lambda
  ///////////////////////////////////////////////////////////////////////////////////////
  auto addWidgetLambda = [&](QWidget* t_widget, const boost::optional<model::ModelObject>& t_obj, const bool t_isSelector) {
    auto holder = new Holder(gridView);
    holder->setMinimumHeight(widgetHeight);
    auto l = new QVBoxLayout();
    l->setAlignment(Qt::AlignCenter);
    l->setSpacing(0);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(t_widget);
    holder->widget = t_widget;
    holder->setLayout(l);
    // layout is defined outside the lambda and brought in through capture!
    layout->addWidget(holder, numWidgets, 0);

    //if (hasSubRows) {
    //  holder->setObjectName("InnerCell");
    //}
    // holders is defined outside the lambda and brought in through capture!
    holders.push_back(holder);

    if (OSComboBox2* comboBox = qobject_cast<OSComboBox2*>(t_widget)) {
      connect(comboBox, &OSComboBox2::inFocus, holder, &Holder::inFocus);
    } else if (OSDoubleEdit2* doubleEdit = qobject_cast<OSDoubleEdit2*>(t_widget)) {
      connect(doubleEdit, &OSDoubleEdit2::inFocus, holder, &Holder::inFocus);
    } else if (OSIntegerEdit2* integerEdit = qobject_cast<OSIntegerEdit2*>(t_widget)) {
      connect(integerEdit, &OSIntegerEdit2::inFocus, holder, &Holder::inFocus);
    } else if (OSQuantityEdit2* quantityEdit = qobject_cast<OSQuantityEdit2*>(t_widget)) {
      connect(quantityEdit, &OSQuantityEdit2::inFocus, holder, &Holder::inFocus);
    } else if (OSLineEdit2* lineEdit = qobject_cast<OSLineEdit2*>(t_widget)) {
      connect(lineEdit, &OSLineEdit2::inFocus, holder, &Holder::inFocus);
    } else if (OSUnsignedEdit2* unsignedEdit = qobject_cast<OSUnsignedEdit2*>(t_widget)) {
      connect(unsignedEdit, &OSUnsignedEdit2::inFocus, holder, &Holder::inFocus);
    } else if (OSDropZone2* dropZone = qobject_cast<OSDropZone2*>(t_widget)) {
      connect(dropZone, &OSDropZone2::inFocus, holder, &Holder::inFocus);
      // Is this widget's subrow a surface with a defaulted construction?
      //if (t_obj) {
      //  if (auto planarSurface = t_obj->optionalCast<model::PlanarSurface>()) {
      //    if (planarSurface->isConstructionDefaulted()) {
      //      // Is this column a construction?
      //      if (column == m_constructionColumn) {
      //        dropZone->setIsDefaulted(true);
      //      }
      //    }
      //  }
      //}
    } else if (HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(t_widget)) {
      connect(horizontalHeaderWidget, &HorizontalHeaderWidget::inFocus, holder, &Holder::inFocus);
    } else if (OSCheckBox3* checkBox = qobject_cast<OSCheckBox3*>(t_widget)) {
      connect(checkBox, &OSCheckBox3::inFocus, holder, &Holder::inFocus);
    }

    m_objectSelector->addObject(t_obj, holder, modelRow, gridRow, column, hasSubRows ? numWidgets : boost::optional<int>(), t_isSelector);

    ++numWidgets;
  };
  ///////////////////////////////////////////////////////////////////////////////////////
  // End of lambda
  ///////////////////////////////////////////////////////////////////////////////////////

  if (m_hasHorizontalHeader && gridRow == 0) {
    if (column == 0) {
      setHorizontalHeader(gridView);
      // Each concept should have its own column
      OS_ASSERT(m_horizontalHeaders.size() == m_baseConcepts.size());
    }
    layout->setContentsMargins(0, 1, 1, 0);
    addWidgetLambda(m_horizontalHeaders.at(column), boost::none, false);
    QSharedPointer<BaseConcept> baseConcept = m_baseConcepts[column];
    const Heading& heading = baseConcept->heading();
    HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(m_horizontalHeaders.at(column));
    OS_ASSERT(horizontalHeaderWidget);
    if (!heading.showCheckbox()) {
      horizontalHeaderWidget->m_checkBox->hide();
    }
    if (!heading.showButton()) {
      horizontalHeaderWidget->m_pushButton->hide();
    }

    horizontalHeaderWidget->addWidget(heading.widget());
  } else {

    model::ModelObject modelObject = m_modelObjects[modelRow];

    //cellColor = getColor(modelObject);  TODO

    QSharedPointer<BaseConcept> baseConcept = m_baseConcepts[column];

    if (QSharedPointer<DataSourceAdapter> dataSource = baseConcept.dynamicCast<DataSourceAdapter>()) {
      hasSubRows = true;
      // here we magically create a multi-row column of any type that was constructed
      //
      // The details need to be fleshed out. The ideas all work, and it's rendering as expected,
      // however the placeHolder isn't doing its job, it might need to be a QSpacer of some kind.
      // The spacing around the list is a little awkward. The padding might need to be set to 0
      // all the way around.
      auto items = dataSource->source().items(modelObject);

      size_t subrowCounter = 0;
      for (auto& item : items) {
        if (item) {
          auto mo = item->cast<model::ModelObject>();
          auto innerConcept = dataSource->innerConcept();
          bool isThisSelector = (baseConcept->isSelector() || innerConcept->isSelector());

          if (isThisSelector) {
            bool isThisLocked = innerConcept->isLocked(mo);
            innerConcept->setBaseLocked(isThisLocked);
          }

          addWidgetLambda(makeWidget(mo, innerConcept, gridView), mo, isThisSelector);
        } else {
          addWidgetLambda(new QWidget(gridView), boost::none, false);
        }
        subrowCounter++;
      }

      if (dataSource->source().wantsPlaceholder()) {
        // use this space to put in a blank placeholder of some kind to make sure the
        // widget is evenly laid out relative to its friends in the adjacent columns
        addWidgetLambda(new QWidget(gridView), boost::none, false);
      }

      if (dataSource->source().dropZoneConcept()) {
        // it makes sense to me that the drop zone would need a reference to the parent containing object
        // not an object the rest in the list was derived from
        // this should also be working and doing what you want
        addWidgetLambda(makeWidget(modelObject, dataSource->source().dropZoneConcept(), gridView), boost::none, false);
      }

      // right here you probably want some kind of container that's smart enough to know how to grow
      // and shrink as the contained items change. But I don't know enough about the model
      // to know how you'd want to do that. For now we make a fixed list that's got a VBoxLayout
      //
      // And think about this.
    } else {
      // This case is exactly what it used to do before the DataSource idea was added.

      // just the one
      addWidgetLambda(makeWidget(modelObject, baseConcept, gridView), modelObject, baseConcept->isSelector());
    }
  }

  if (hasSubRows) {
    // You need a holder, either by specifying a column which has a placeholder,
    // or by specifying a column which has a DataSource DropZoneConcept
    OS_ASSERT(!holders.empty());
    holders.back()->setObjectName("InnerCellBottom");
  }

  return wrapper;
}

void OSGridController::checkSelectedFields() {
  // If there is a header row, investigate which columns were previously checked
  // (and loaded into m_customFields by QSettings) and check the respective
  // header widgets
  if (!this->m_hasHorizontalHeader) return;

  std::vector<QString>::iterator it;
  for (unsigned j = 0; j < m_customFields.size(); j++) {
    it = std::find(m_currentFields.begin(), m_currentFields.end(), m_customFields.at(j));
    if (it != m_currentFields.end()) {
      int index = std::distance(m_currentFields.begin(), it);
      HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(m_horizontalHeaders.at(index));
      OS_ASSERT(horizontalHeaderWidget);
      horizontalHeaderWidget->m_checkBox->blockSignals(true);
      horizontalHeaderWidget->m_checkBox->setChecked(true);
      horizontalHeaderWidget->m_checkBox->blockSignals(false);
    }
  }
}

void OSGridController::setCustomCategoryAndFields() {
  // First, find and erase the old fields for custom
  std::vector<QString> categories = this->categories();
  std::vector<QString>::iterator it;
  it = std::find(categories.begin(), categories.end(), QString("Custom"));
  if (it != categories.end()) {
    int index = std::distance(categories.begin(), it);
    m_categoriesAndFields.erase(m_categoriesAndFields.begin() + index);
  }

  // Make a new set of fields for custom
  std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Custom"), m_customFields);
  m_categoriesAndFields.push_back(categoryAndFields);
}

int OSGridController::rowCount() const {
  if (m_hasHorizontalHeader) {
    return m_modelObjects.size() + 1;
  } else {
    return m_modelObjects.size();
  }
}

int OSGridController::columnCount() const {
  return m_baseConcepts.size();
}
/*
QWidget* OSGridController::cell(int gridRow, int columnIndex) {
  QWidget* widget = nullptr;

  QLayoutItem* child = gridView()->itemAtPosition(gridRow, columnIndex);
  if (child) {
    widget = child->widget();
  }

  return widget;
}
*/
model::ModelObject OSGridController::modelObjectFromGridRow(int gridRow) {
  return m_modelObjects.at(modelRowFromGridRow(gridRow));
}

int OSGridController::gridRowFromModelRow(int modelRow) {
  if (m_hasHorizontalHeader) {
    return modelRow + 1;
  }
  return modelRow;
}

int OSGridController::modelRowFromGridRow(int gridRow) {
  if (m_hasHorizontalHeader) {
    return gridRow - 1;
  }
  return gridRow;
}

/*
std::vector<QWidget*> OSGridController::row(int gridRow) {
  std::vector<QWidget*> row;

  for (unsigned columnIndex = 0; columnIndex < m_currentFields.size(); columnIndex++) {
    row.push_back(cell(gridRow, columnIndex));
  }

  return row;
}

void OSGridController::selectRow(int gridRow, bool select) {
  std::vector<QWidget*> row = this->row(gridRow);
  for (auto widget : row) {
    auto button = qobject_cast<QPushButton*>(widget);
    if (!button) {
      return;
    }
    button->blockSignals(true);
    button->setChecked(select);
    button->blockSignals(false);
  }
}
*/
void OSGridController::onHorizontalHeaderChecked(int index) {
  // Push_back or erase the field from the user-selected fields
  auto checkBox = qobject_cast<QAbstractButton*>(m_horizontalHeaderBtnGrp->button(index));
  OS_ASSERT(checkBox);
  if (checkBox->isChecked()) {
    m_customFields.push_back(m_currentFields.at(index));
  } else {
    std::vector<QString>::iterator it;
    it = std::find(m_customFields.begin(), m_customFields.end(), m_currentFields.at(index));
    if (it != m_customFields.end()) {
      m_customFields.erase(it);
    }
  }

  // Update the user-selected fields
  setCustomCategoryAndFields();
}

void OSGridController::onToggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

void OSGridController::onComboBoxIndexChanged(int index) {}

//bool OSGridController::selectRowByItem(OSItem * item, bool isSelected)
//{
//  auto success = false;
//  int i = 0;
//
//  for (auto modelObject : m_modelObjects){
//    OSItemId itemId = modelObjectToItemId(modelObject, false);
//    if (item->itemId() == itemId){
//      selectRow(gridRowFromModelIndex(i), isSelected);
//      success = true;
//      break;
//    }
//    i++;
//  }
//  return success;
//}

bool OSGridController::getgridRowByItem(OSItem* item, int& gridRow) {
  auto success = false;
  gridRow = -1;

  for (auto modelObject : m_modelObjects) {
    gridRow++;
    OSItemId itemId = modelObjectToItemId(modelObject, false);
    if (item->itemId() == itemId) {
      success = true;
      break;
    }
  }

  if (!success) {
    // At this point, none of the itemIds exactly matched,
    // let's try to match a subset.
    gridRow = -1;

    QString handle(""), handle2("");
    QStringList strings = item->itemId().otherData().split(",");
    if (strings.size() > 2) {
      QString temp = strings[2];
      strings = temp.split(";");
      if (strings.size() > 0) {
        handle = strings[0];
      }
    }

    for (auto modelObject : m_modelObjects) {
      gridRow++;
      OSItemId itemId = modelObjectToItemId(modelObject, false);
      strings = itemId.otherData().split(",");
      if (strings.size() > 2) {
        QString temp = strings[2];
        strings = temp.split(";");
        if (strings.size() > 0) {
          handle2 = strings[0];
        }
      }

      if (handle == handle2) {
        success = true;
        break;
      }
    }
  }

  if (success) {
    // We found the model index and must convert it to the row index
    gridRow = gridRowFromModelRow(gridRow);
  } else {
    // We could never find a valid index
    gridRow = -1;
  }

  return success;
}
/*
OSItem* OSGridController::getSelectedItemFromModelSubTabView() {
  OSItem* item = nullptr;

  auto modelSubTabView = gridView()->modelSubTabView();
  if (!modelSubTabView) return item;

  item = modelSubTabView->itemSelector()->selectedItem();

  return item;
}
*/
void OSGridController::connectToModelSignals() {
  m_model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObject.connect<OSGridController, &OSGridController::onAddWorkspaceObject>(this);
  m_model.getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObject.connect<OSGridController, &OSGridController::onRemoveWorkspaceObject>(
    this);
}

void OSGridController::disconnectFromModelSignals() {
  m_model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObject.disconnect<OSGridController, &OSGridController::onAddWorkspaceObject>(this);
  m_model.getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObject.disconnect<OSGridController, &OSGridController::onRemoveWorkspaceObject>(
    this);
}

void OSGridController::onSelectionCleared() {
  m_objectSelector->clearSelection();
}

void OSGridController::onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType,
                                               const openstudio::UUID& handle) {
  m_objectSelector->setObjectRemoved(handle);
}

void OSGridController::onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType,
                                            const openstudio::UUID& handle) {
  if (iddObjectType == m_iddObjectType) {

    // do not call refreshModelObjects right now because that could resort the grid which is very costly
    //refreshModelObjects();

    m_modelObjects.push_back(object.cast<model::ModelObject>());

    emit addRow(rowCount() - 1);
  }
}

void OSGridController::onObjectRemoved(boost::optional<model::ParentObject> parent) {
  // no-op
}

void OSGridController::onSelectAllStateChanged(const int newState) const {
  LOG(Debug, "Select all state changed: " << newState);

  if (newState == 0) {
    m_objectSelector->clearSelection();
  } else {
    m_objectSelector->selectAll();
  }
}

void OSGridController::onInFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow) {

  // First thing to do is to check if row is 0, because that means that the apply button was clicked
  if (gridRow == 0 && this->m_hasHorizontalHeader) {
    // Set selected objects to the focused object's value for given column
    auto focusedGridRow = std::get<0>(m_focusedCellLocation);
    auto focusedModelRow = modelRowFromGridRow(focusedGridRow);
    auto focusedColumn = std::get<1>(m_focusedCellLocation);
    auto focusedSubrow = std::get<2>(m_focusedCellLocation);

    if (focusedGridRow < 0) {
      // something has gone wrong
      return;
    }

    OS_ASSERT(focusedColumn == column);

    std::set<model::ModelObject> selectedObjects = this->m_objectSelector->selectedObjects();
    boost::optional<model::ModelObject> focusedObject =
      this->m_objectSelector->getObject(focusedModelRow, focusedGridRow, focusedColumn, focusedSubrow);
    if (!focusedObject) {
      // we don't have a focused object to apply values from
      return;
    }

    QSharedPointer<DataSourceAdapter> dataSource = m_baseConcepts[column].dynamicCast<DataSourceAdapter>();
    if (focusedSubrow && dataSource) {
      // Sub rows present, either in a widget, or in a row
      const DataSource& source = dataSource->source();
      QSharedPointer<BaseConcept> dropZoneConcept = source.dropZoneConcept();
      for (auto modelObject : selectedObjects) {
        // Don't set the chosen object when iterating through the selected objects
        if (modelObject != focusedObject.get()) {
          OS_ASSERT(dataSource.data()->innerConcept());
          if (dropZoneConcept) {
            // Widget has sub rows
            setConceptValue(modelObject, focusedObject.get(), dropZoneConcept, dataSource.data()->innerConcept());
          } else {
            // Row has sub rows
            setConceptValue(modelObject, focusedObject.get(), dataSource.data()->innerConcept());
          }
        }
      }
    } else if (!focusedSubrow) {
      for (auto modelObject : selectedObjects) {
        // Don't set the chosen object when iterating through the selected objects
        if (modelObject != focusedObject.get()) {
          setConceptValue(modelObject, focusedObject.get(), m_baseConcepts[column]);
        }
      }
    } else {
      // Should never get here
      OS_ASSERT(false);
    }

  } else {
    // not in a header row, an object was selected
    OS_ASSERT(gridRow >= 0);

    HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(m_horizontalHeaders.at(column));
    OS_ASSERT(horizontalHeaderWidget);
    auto button = horizontalHeaderWidget->m_pushButton;
    OS_ASSERT(button);

    if (inFocus) {
      m_focusedCellLocation = std::make_tuple(gridRow, column, subrow);
      button->setText("Apply to Selected");
    } else {
      // do not reset m_focusedCellLocation here because the focused cell goes out of focus when the apply button is clicked
      button->setText("Apply to Selected");
    }

    m_applyToButtonStates.push_back(std::make_pair(column, inFocus && hasData));

    QTimer::singleShot(0, this, &OSGridController::onSetApplyButtonState);
  }
}

void OSGridController::onSetApplyButtonState() {
  for (auto pair : m_applyToButtonStates) {
    HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(m_horizontalHeaders.at(pair.first));
    OS_ASSERT(horizontalHeaderWidget);
    auto button = horizontalHeaderWidget->m_pushButton;
    OS_ASSERT(button);
    button->setEnabled(pair.second);
  }

  m_applyToButtonStates.clear();
}

std::set<model::ModelObject> OSGridController::selectableObjects() const {
  return m_objectSelector->selectableObjects();
}

std::set<model::ModelObject> OSGridController::selectedObjects() const {
  return m_objectSelector->selectedObjects();
}

void OSGridController::setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter) {
  m_objectSelector->setObjectFilter(t_filter);
}

void OSGridController::setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked) {
  m_objectSelector->setObjectIsLocked(t_isLocked);
}

QSharedPointer<BaseConcept> OSGridController::makeDataSourceAdapter(const QSharedPointer<BaseConcept>& t_inner,
                                                                    const boost::optional<DataSource>& t_source) {
  if (t_source) {
    return QSharedPointer<BaseConcept>(new DataSourceAdapter(*t_source, t_inner));
  } else {
    // if there is no t_source passed in, we don't want to wrap, just pass through
    return t_inner;
  }
}

void OSGridController::resetBaseConcepts() {
  m_baseConcepts.clear();
}

HorizontalHeaderPushButton::HorizontalHeaderPushButton(QWidget* parent) : QPushButton() {
  QString style = "QPushButton {"
                  "    font-size: 8pt;"
                  "}";
  setStyleSheet(style);
  setFocusPolicy(Qt::StrongFocus);
}

HorizontalHeaderPushButton::~HorizontalHeaderPushButton() {}

void HorizontalHeaderPushButton::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason) {
    emit inFocus(true, true);  // TODO should hasData = true???
  }

  QPushButton::focusInEvent(e);
}

void HorizontalHeaderPushButton::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason) {
    emit inFocus(false, false);
  }

  QPushButton::focusOutEvent(e);
}

//ColumnSizer::mouseMoveEvent ( QMouseEvent * event )
//{
//  if( event->buttons == Qt::LeftButton ) {
//
//  }
//}

Holder::Holder(QWidget* parent) : QWidget(parent) {}

Holder::~Holder() {}

void Holder::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

HorizontalHeaderWidget::HorizontalHeaderWidget(const QString& fieldName, QWidget* parent)
  : QWidget(parent), m_label(new QLabel(fieldName, this)), m_checkBox(new QPushButton(this)), m_pushButton(new HorizontalHeaderPushButton(this)) {
  auto mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 5);
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);

  mainLayout->addWidget(m_checkBox);
  m_checkBox->setToolTip("Check to add this column to \"Custom\"");
  m_checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  QString style = "\
    QPushButton {\
      border: none;\
      background: #808080;\
      padding: 0px;\
    }\
    QPushButton:checked {\
      background: #94b3de;\
    }\
  ";

  m_checkBox->setStyleSheet(style);
  m_checkBox->setCheckable(true);

  m_label->setWordWrap(true);
  m_label->setStyleSheet("QLabel { padding: 5px; }");
  m_label->setAlignment(Qt::AlignHCenter);
  mainLayout->addWidget(m_label);

  mainLayout->addStretch();

  m_pushButton->setText("Apply to Selected");
  m_pushButton->setMaximumWidth(150);
  m_pushButton->setEnabled(false);
  connect(m_pushButton, &HorizontalHeaderPushButton::inFocus, this, &HorizontalHeaderWidget::inFocus);
  mainLayout->addWidget(m_pushButton, 0, Qt::AlignCenter);
}

HorizontalHeaderWidget::~HorizontalHeaderWidget() {
  for (auto& widget : m_addedWidgets) {
    layout()->removeWidget(widget.data());
    widget->setVisible(false);
    widget->setParent(nullptr);
  }
}

void HorizontalHeaderWidget::addWidget(const QSharedPointer<QWidget>& t_widget) {
  if (!t_widget.isNull()) {
    m_addedWidgets.push_back(t_widget);
    auto hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 0, 5, 0);
    qobject_cast<QVBoxLayout*>(layout())->addLayout(hLayout);
    hLayout->addWidget(t_widget.data());
    t_widget->setVisible(true);
  }
}

}  // namespace openstudio
