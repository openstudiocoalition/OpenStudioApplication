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

#ifndef SHAREDGUICOMPONENTS_OSGRIDCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_OSGRIDCONTROLLER_HPP

#include "OSConcepts.hpp"
#include "../model_editor/QMetaTypes.hpp"
#include "../openstudio_lib/OSItem.hpp"
#include "../openstudio_lib/OSVectorController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <openstudio/utilities/idd/IddObject.hpp>

#include <string>
#include <functional>
#include <vector>

#include <QObject>
#include <QPushButton>
#include <QSharedPointer>
#include <QWidget>
#include <QVBoxLayout>

class QButtonGroup;
class QCheckBox;
class QColor;
class QLabel;
class QPaintEvent;

namespace openstudio {

class OSComboBox2;
class OSGridView;

// forward declaration
class Holder;

/// Provides a Concept with an alternative source of data.
///
/// Generally, Concepts take a ModelObject and display some field of it.
///
/// This has been extended with the ProxyAdapter, which inserts itself between the `getter`, `setter`
/// and the Concept. This ProxyAdapter allows you to not just display data with the ModelObject
/// but to actually display data on something the ModelObject contains, for example, showing the name
/// of a SizingZone which is associated with a ThermalZone
///
/// DataSource takes the ProxyAdapter concept one step further, by allowing you to examine a `std::vector`
/// of items which are associated with the ModelObject that was passed in. The vector could be a list
/// of one, if you were so inclined, and all of the interactions with the ModelObject
/// are abstracted via std::function objects, so the data you display can truly come from anywhere.
///
///
/// Essentially, by using this class you are creating a stack of widgets in the grid cell:
///
/// \code
/// <widget for t_sourceFunc item 1>
/// <widget for t_sourceFunc item 2>
/// ...
/// <widget for t_sourceFunc item N>
/// <widget for t_dropZoneConcept (see notes below)>
/// <space holder widget if enabled>
/// \endcode
class DataSource
{
 public:
  /// Create a new DataSource object, which is optionally passed into the various
  /// add.*Column functions.
  ///
  ///
  /// \param[in] t_sourceFunc Takes an Input object (practically speaking, a ModelObject derivation) and returns a vector
  ///                         of items (probably some derived class from ModelObject)
  /// \param[in] t_wantsPlaceholder Informs the system to add a placeholder gap at the bottom of the list when it is
  ///                         rendering the list of widgets. This is to help non-dropzone enabled stacks of widgets
  ///                         line up with those stacks that do contain dropzones
  /// \param[in] t_dropZoneConcept The `BaseConcept` you want displayed at the bottom of the list of widgets.
  ///                         It will be displayed after the PlaceHolder, but it is probably illogical to enable both.
  ///                         It is named "t_dropZoneConcept" because it's intended for DropZone widgets. However,
  ///                         it's rendered with the function OSGridController::makeWidget function just like every
  ///                         other widget, so it can really be anything.
  template <typename ItemType, typename InputType>
  DataSource(const std::function<std::vector<ItemType>(InputType)>& t_sourceFunc, bool t_wantsPlaceholder = false,
             const QSharedPointer<BaseConcept>& t_dropZoneConcept = QSharedPointer<BaseConcept>())
    : m_wantsPlaceholder(t_wantsPlaceholder), m_dropZoneConcept(t_dropZoneConcept) {
    typedef decltype(t_sourceFunc) IncommingFuncType;
    typedef typename std::remove_reference<typename std::remove_cv<IncommingFuncType>::type>::type FunctionType;
    typedef typename std::remove_reference<typename std::remove_cv<typename FunctionType::argument_type>::type>::type ParamType;

    m_sourceFunc = [t_sourceFunc](ConceptProxy t_proxy) {
      auto result = t_sourceFunc(t_proxy.cast<ParamType>());
      return std::vector<boost::optional<ConceptProxy>>(result.begin(), result.end());
    };
  }

  std::vector<boost::optional<ConceptProxy>> items(const ConceptProxy& t_proxy) const {
    return m_sourceFunc(t_proxy);
  }

  QSharedPointer<BaseConcept> dropZoneConcept() const {
    return m_dropZoneConcept;
  }

  bool wantsPlaceholder() const {
    return m_wantsPlaceholder;
  }

 private:
  std::function<std::vector<boost::optional<ConceptProxy>>(const ConceptProxy&)> m_sourceFunc;
  bool m_wantsPlaceholder;
  QSharedPointer<BaseConcept> m_dropZoneConcept;
};

class DataSourceAdapter : public BaseConcept
{
 public:
  DataSourceAdapter(DataSource t_source, QSharedPointer<BaseConcept> t_inner)
    : BaseConcept(t_inner->heading()), m_source(t_source), m_inner(t_inner) {}

  const DataSource& source() const {
    return m_source;
  }

  QSharedPointer<BaseConcept> innerConcept() const {
    return m_inner;
  }

 private:
  DataSource m_source;
  QSharedPointer<BaseConcept> m_inner;
};

class OSGridController;

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

  void selectionChanged(int state, int modelRow, int gridRow, int column, boost::optional<int> subrow);

 public slots:
 
  void onInFocus(bool inFocus, bool hasData);

  void onSelectionChanged(int state);
};

class GridCellInfo : public QObject
{
  Q_OBJECT;

 public:
  GridCellInfo(const boost::optional<model::ModelObject>& t_modelObject, bool t_isSelector, bool t_isVisible, bool t_isSelected, bool t_isLocked, QObject* parent);

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

/// ObjectSelector keeps track of which cells are selected, filtered/not visible, and locked
class ObjectSelector : public QObject
{
  Q_OBJECT;

 public:
  ObjectSelector(QObject* parent);

  virtual ~ObjectSelector();

  // Reset all state
  void clear();

  // Adds object to the internal maps
  void addObject(const boost::optional<model::ModelObject>& t_obj, Holder* t_holder, int t_modelRow, int t_gridRow, int t_column, const boost::optional<int>& t_subrow, bool t_isSelector);

  // Remove an object from all rows and subrows
  void removeObject(const openstudio::model::ModelObject& t_obj);

  // Check if object is included in any row or subrow
  bool containsObject(const openstudio::model::ModelObject& t_obj) const;

  // Get object at given location
  boost::optional<model::ModelObject> getObject(const int t_modelRow, const int t_gridRow, const int t_column, const boost::optional<int>& t_subrow) const;

  // Get GridCellInfo at given location
  GridCellInfo* getGridCellInfo(const int t_row, const int t_column, const boost::optional<int>& t_subrow) const;

  // Select all selectable objects
  void selectAll();

  // Clear the selection
  void clearSelection();

  // Set a selectable object as selected
  void setObjectSelected(const model::ModelObject& t_obj, bool t_selected);

  // Check if an object is selected
  bool getObjectSelected(const model::ModelObject& t_obj) const;

  // Gets selectable objects
  std::set<model::ModelObject> selectableObjects() const;

  // Gets selected objects
  std::set<model::ModelObject> selectedObjects() const;

  // Check if an object is visible (passes the filter)
  bool getObjectVisible(const model::ModelObject& t_obj) const;

  // Set the object filter function, function true if object is visible
  void setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter);
  
  // Reset the object filter function
  void resetObjectFilter();

  // Check if an object is locked
  bool getObjectIsLocked(const model::ModelObject& t_obj) const;

  // Set the object is locked function, function true if object is locked
  void setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked);

  // Reset the object is locked function
  void resetObjectIsLocked();

  // Applies locks to entire rows and subrows
  void applyLocks();

signals:

  void inFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow);

  void gridCellChanged(const GridCellLocation& location, const GridCellInfo& info);

  void gridRowSelectionChanged();

public slots:
 
  void onSelectionChanged(int state, int row, int column, boost::optional<int> subrow);

 protected:
  REGISTER_LOGGER("openstudio.ObjectSelector");

 private:

  std::map<GridCellLocation*, GridCellInfo*> m_gridCellLocationToInfoMap;

  // returns true if object is visible
  // e.g. a lights object would not be visible if the user filted only people objects
  std::function<bool(const model::ModelObject&)> m_objectFilter;
  static std::function<bool(const model::ModelObject&)> getDefaultFilter();

  // returns true if object is locked
  // e.g. an inherited space load would be locked
  std::function<bool(const model::ModelObject&)> m_isLocked;
  static std::function<bool(const model::ModelObject&)> getDefaultIsLocked();
};

class OSGridController : public QObject
{
  Q_OBJECT

 public:
  // This form requires clients to subclass OSGridController and
  // reimplement rowCount(), and itemAt()
  OSGridController();

  // This form utilizes the default implementations of
  // rowCount() and itemAt(), showing one row for each object
  // in the model that is iddObjectType
  OSGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                   const std::vector<model::ModelObject>& modelObjects);

  virtual ~OSGridController();

private:

  static QSharedPointer<BaseConcept> makeDataSourceAdapter(const QSharedPointer<BaseConcept>& t_inner, const boost::optional<DataSource>& t_source);

protected:

  void resetBaseConcepts();

  template <typename DataSourceType>
  void addSelectColumn(const Heading& heading, const std::string& tooltip, std::function<bool(DataSourceType*)> isLocked, 
                       const boost::optional<DataSource>& t_source = boost::none)
  {
    auto objectSelector = m_objectSelector;
    auto getter = std::function<bool(model::ModelObject*)>([objectSelector](model::ModelObject* t_obj) -> bool {
      assert(t_obj);
      return objectSelector->getObjectSelected(*t_obj);
    });

    auto setter = std::function<void(model::ModelObject*, bool)>([objectSelector](model::ModelObject* t_obj, bool t_set) {
      assert(t_obj);
      objectSelector->setObjectSelected(*t_obj, t_set);
    });

    addCheckBoxColumn(heading, tooltip, getter, setter, isLocked, t_source);
    m_baseConcepts.back()->setIsSelector(true);
  }

  template <typename DataSourceType>
  void addCheckBoxColumn(const Heading& heading, const std::string& tooltip, std::function<bool(DataSourceType*)> t_getter,
                         std::function<void(DataSourceType*, bool)> t_setter, std::function<bool(DataSourceType*)> t_isLocked,
                         const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<CheckBoxConcept>(new CheckBoxConceptImpl<DataSourceType>(heading, tooltip, t_getter, t_setter, t_isLocked)), t_source));
  }

  template <typename DataSourceType>
  void addCheckBoxColumn(const Heading& heading, const std::string& tooltip, std::function<bool(DataSourceType*)> t_getter,
                         std::function<bool(DataSourceType*, bool)> t_setter, std::function<bool(DataSourceType*)> t_isLocked, 
                         const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<CheckBoxConceptBoolReturn>(new CheckBoxConceptBoolReturnImpl<DataSourceType>(heading, tooltip, t_getter, t_setter, t_isLocked)),
      t_source));
  }
  
  template <typename ChoiceType, typename DataSourceType>
  void addComboBoxColumn(const Heading& heading, std::function<std::string(const ChoiceType&)> toString,
                         std::function<std::vector<ChoiceType>()> choices, std::function<ChoiceType(DataSourceType*)> getter,
                         std::function<bool(DataSourceType*, ChoiceType)> setter,
                         const boost::optional<std::function<void(DataSourceType*)>>& reset = boost::none,
                         const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    addComboBoxColumn<ChoiceType, DataSourceType>(
      heading, toString, std::function<std::vector<ChoiceType>(DataSourceType*)>([choices](DataSourceType*) { return choices(); }), getter, setter,
      reset, isDefaulted, t_source);
  }

  template <typename ChoiceType, typename DataSourceType>
  void addComboBoxColumn(const Heading& heading, std::function<std::string(const ChoiceType&)> toString,
                         std::function<std::vector<ChoiceType>()> choices, std::function<boost::optional<ChoiceType>(DataSourceType*)> getter,
                         std::function<bool(DataSourceType*, ChoiceType)> setter,
                         boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none, bool editable = true) {
    addComboBoxColumn<ChoiceType, DataSourceType>(
      heading, toString, std::function<std::vector<ChoiceType>(DataSourceType*)>([choices](DataSourceType*) { return choices(); }), getter, setter,
      reset, t_source, editable);
  }

  template <typename ChoiceType, typename DataSourceType>
  void addComboBoxColumn(const Heading& heading, std::function<std::string(const ChoiceType&)> toString,
                         std::function<std::vector<ChoiceType>(DataSourceType*)> choices, std::function<ChoiceType(DataSourceType*)> getter,
                         std::function<bool(DataSourceType*, ChoiceType)> setter,
                         const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                         const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(QSharedPointer<ComboBoxConcept>(new ComboBoxRequiredChoiceImpl<ChoiceType, DataSourceType>(
                                                     heading, toString, choices, getter, setter, reset, isDefaulted)),
                                                   t_source));
  }

  template <typename ChoiceType, typename DataSourceType>
  void addComboBoxColumn(const Heading& heading, std::function<std::string(const ChoiceType&)> toString,
                         std::function<std::vector<ChoiceType>(DataSourceType*)> choices, std::function<ChoiceType(DataSourceType*)> getter,
                         std::function<bool(DataSourceType*, ChoiceType)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<ComboBoxConcept>(new ComboBoxRequiredChoiceImpl<ChoiceType, DataSourceType>(heading, toString, choices, getter, setter)),
      t_source));
  }

  template <typename ChoiceType, typename DataSourceType>
  void addComboBoxColumn(const Heading& heading, std::function<std::string(const ChoiceType&)> toString,
                         std::function<std::vector<ChoiceType>(DataSourceType*)> choices,
                         std::function<boost::optional<ChoiceType>(DataSourceType*)> getter, std::function<bool(DataSourceType*, ChoiceType)> setter,
                         boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none, bool editable = false) {
    m_baseConcepts.push_back(makeDataSourceAdapter(QSharedPointer<ComboBoxConcept>(new ComboBoxOptionalChoiceImpl<ChoiceType, DataSourceType>(
                                                     heading, toString, choices, getter, setter, reset, editable)),
                                                   t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<ValueType(DataSourceType*)> getter,
                          std::function<bool(DataSourceType*, ValueType)> setter,
                          const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                          const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                          const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<ValueEditConcept<ValueType>>(new ValueEditConceptImpl<ValueType, DataSourceType>(heading, getter, setter, reset, isDefaulted)),
      t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                          std::function<bool(DataSourceType*, ValueType)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<OptionalValueEditConcept<ValueType>>(new OptionalValueEditConceptImpl<ValueType, DataSourceType>(heading, getter, setter)),
      t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<ValueType(DataSourceType*)> getter,
                          std::function<void(DataSourceType*, ValueType)> setter,
                          const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                          const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                          const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<ValueEditVoidReturnConcept<ValueType>>(
                              new ValueEditVoidReturnConceptImpl<ValueType, DataSourceType>(heading, getter, setter, reset, isDefaulted)),
                            t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                          std::function<void(DataSourceType*, double)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(QSharedPointer<OptionalValueEditVoidReturnConcept<ValueType>>(
                                                     new OptionalValueEditVoidReturnConceptImpl<ValueType, DataSourceType>(heading, getter, setter)),
                                                   t_source));
  }

  template <typename DataSourceType>
  void addNameLineEditColumn(const Heading& heading, bool isInspectable, bool deleteObject,
                             const std::function<boost::optional<std::string>(DataSourceType*, bool)>& getter,
                             const std::function<boost::optional<std::string>(DataSourceType*, const std::string&)>& setter,
                             const boost::optional<std::function<void(DataSourceType*)>>& resetter = boost::none,
                             const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                             const boost::optional<std::function<bool(DataSourceType*)>>& isLocked = boost::none,
                             const boost::optional<DataSource>& t_source = boost::none) {
    const OSLineEditType osLineEditType = OSLineEditType::OSLineEdit2Type;
    const bool hasClickFocus = true;
    m_baseConcepts.push_back(makeDataSourceAdapter(QSharedPointer<NameLineEditConcept>(new NameLineEditConceptImpl<DataSourceType>(
        heading, osLineEditType, isInspectable, deleteObject, hasClickFocus, getter, setter, resetter, isDefaulted, isLocked)),
                                                   t_source));
  }

  template <typename DataSourceType>
  void addLoadNameColumn(const Heading& heading, const std::function<boost::optional<std::string>(DataSourceType*, bool)>& getter,
                         const std::function<boost::optional<std::string>(DataSourceType*, const std::string&)>& setter,
                         const boost::optional<std::function<void(DataSourceType*)>>& resetter = boost::none,
                         const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                         const boost::optional<std::function<bool(DataSourceType*)>>& isLocked = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    const OSLineEditType osLineEditType = OSLineEditType::OSLoadNamePixmapLineEditType;
    const bool isInspectable = true;
    const bool deleteObject = true;
    const bool hasClickFocus = true;
    m_baseConcepts.push_back(makeDataSourceAdapter(QSharedPointer<NameLineEditConcept>(new NameLineEditConceptImpl<DataSourceType>(
        heading, osLineEditType, isInspectable, deleteObject, hasClickFocus, getter, setter, resetter, isDefaulted, isLocked)),
                            t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addQuantityEditColumn(const Heading& heading, QString modelUnits, QString siUnits, QString ipUnits, bool isIP,
                             std::function<ValueType(DataSourceType*)> getter, std::function<bool(DataSourceType*, ValueType)> setter,
                             const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                             const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                             const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<QuantityEditConcept<ValueType>>(new QuantityEditConceptImpl<ValueType, DataSourceType>(
                              heading, modelUnits, siUnits, ipUnits, isIP, getter, setter, reset, isDefaulted)),
                            t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addQuantityEditColumn(const Heading& heading, QString modelUnits, QString siUnits, QString ipUnits, bool isIP,
                             std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                             std::function<bool(DataSourceType*, ValueType)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<OptionalQuantityEditConcept<ValueType>>(new OptionalQuantityEditConceptImpl<ValueType, DataSourceType>(
                              heading, modelUnits, siUnits, ipUnits, isIP, getter, setter)),
                            t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addQuantityEditColumn(const Heading& heading, QString modelUnits, QString siUnits, QString ipUnits, bool isIP,
                             std::function<ValueType(DataSourceType*)> getter, std::function<void(DataSourceType*, ValueType)> setter,
                             const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                             const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                             const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<QuantityEditVoidReturnConcept<ValueType>>(new QuantityEditVoidReturnConceptImpl<ValueType, DataSourceType>(
                              heading, modelUnits, siUnits, ipUnits, isIP, getter, setter, reset, isDefaulted)),
                            t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addQuantityEditColumn(const Heading& heading, QString modelUnits, QString siUnits, QString ipUnits, bool isIP,
                             std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                             std::function<void(DataSourceType*, ValueType)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<OptionalQuantityEditVoidReturnConcept<ValueType>>(
        new OptionalQuantityEditVoidReturnConceptImpl<ValueType, DataSourceType>(heading, modelUnits, siUnits, ipUnits, isIP, getter, setter)),
      t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addDropZoneColumn(const Heading& heading, std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                         std::function<bool(DataSourceType*, const ValueType&)> setter,
                         boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                         boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                         boost::optional<std::function<bool(DataSourceType*)>> isLocked = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<ValueType, DataSourceType>(heading, getter, setter, reset, isDefaulted, isLocked)),
      t_source));
  }

  template <typename ValueType, typename DataSourceType>
  void addRenderingColorColumn(const Heading& heading, std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                               std::function<bool(DataSourceType*, const ValueType&)> setter,
                               const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(
      QSharedPointer<RenderingColorConcept>(new RenderingColorConceptImpl<ValueType, DataSourceType>(heading, getter, setter)));
  }

public:

  std::vector<QString> categories();

  std::vector<std::pair<QString, std::vector<QString>>> categoriesAndFields();

  virtual int rowCount() const;

  virtual int columnCount() const;

  model::ModelObject modelObjectFromGridRow(int gridRow);

  //virtual std::vector<QWidget*> row(int gridRow);

  //void selectRow(int gridRow, bool select);

  int gridRowFromModelRow(int modelRow);
  int modelRowFromGridRow(int gridRow);

  // Return a new widget at a "top level" row and column specified by arguments.
  // There might be sub rows within the specified location.
  // In that case a QWidget with sub rows (inner grid layout) will be returned.
  QWidget* createWidget(int gridRow, int column, OSGridView* gridView);

  // Call this function on a model update
  virtual void refreshModelObjects() = 0;

  void connectToModelSignals();

  void disconnectFromModelSignals();

  model::Model& model();

  std::vector<model::ModelObject> modelObjects() const;

  std::set<model::ModelObject> selectableObjects() const;

  std::set<model::ModelObject> selectedObjects() const;

  void setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter);

  void setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked);

  IddObjectType iddObjectType() const;

 private:

  // For testing
  friend class OpenStudioLibFixture;

  IddObjectType m_iddObjectType;

  std::vector<model::ModelObject> m_modelObjects; 

  //std::vector<model::ModelObject> m_inheritedModelObjects; 

  // If a column contains information about a construction, it may be an inherited construction
  // (as determined by calling PlanarSurface::isConstructionDefaulted). An instantiated gridview
  // should set this value, if appropriate.
// TODO: remove this
  int m_constructionColumn = -1;

  std::vector<std::pair<QString, std::vector<QString>>> m_categoriesAndFields;

  std::vector<QSharedPointer<BaseConcept>> m_baseConcepts;

  std::vector<QWidget*> m_horizontalHeaders;

  bool m_hasHorizontalHeader;

  QString m_currentCategory;

  int m_currentCategoryIndex;

  std::vector<QString> m_currentFields;

  std::vector<QString> m_customFields;

  static const std::vector<QColor> m_colors;

  model::Model m_model;

  bool m_isIP;

 protected:
  
  bool isIP() const;

  bool hasHorizontalHeader() const;
  std::vector<QWidget*> horizontalHeaders() const;

  void setConstructionColumn(int constructionColumn);

  void setModelObjects(const std::vector<model::ModelObject>& modelObjects);

  //std::vector<model::ModelObject> inheritedModelObjects() const;
  //void setInheritedModelObjects(const std::vector<model::ModelObject>& inheritedModelObjects);

  void addCategoryAndFields(const std::pair<QString, std::vector<QString>>& categoryAndFields);
  void resetCategoryAndFields();

  // This function determines the category for
  // each button, and the fields associated with
  // each category
  virtual void setCategoriesAndFields();

  // Call this function with the fields required,
  // and it adds the columns and does the binds.
  // This provides a mechanism to easily manage
  // a dynamic, user-preference column.
  // This function will be called from the slot
  // connected to the QButtonGroup signal
  virtual void addColumns(const QString& category, std::vector<QString>& fields) = 0;

  // Call this function to get the color for the cell color
  virtual QString getColor(const model::ModelObject& modelObject) = 0;

  // This function sets the column header caption
  virtual void setHorizontalHeader(QWidget* parent);

  // Call this function after the table is constructed
  // to appropriately check user-selected category fields
  // from QSettings and load them into a "Custom" button
  virtual void checkSelectedFields();

  void checkSelectedFields(int category);

  REGISTER_LOGGER("openstudio.OSGridController");

 private:
  friend class OSGridView; // TODO: remove this
  friend class ObjectSelector; // TODO: remove this

  // Make the lowest level widgets that corresponds to concepts.
  // These will be put in container widgets to form the cell, regardless of the presence of sub rows.
  QWidget* makeWidget(model::ModelObject t_mo, const QSharedPointer<BaseConcept>& t_baseConcept, OSGridView* gridView);

  void loadQSettings();

  void saveQSettings() const;

  void setCustomCategoryAndFields();

  OSItem* getSelectedItemFromModelSubTabView();

  bool getgridRowByItem(OSItem* item, int& gridRow);

  void setConceptValue(model::ModelObject t_setterMO, model::ModelObject t_getterMO, const QSharedPointer<BaseConcept>& t_baseConcept);

  void resetConceptValue(model::ModelObject t_resetMO, const QSharedPointer<BaseConcept>& t_baseConcept);

  void setConceptValue(model::ModelObject t_setterMO, model::ModelObject t_getterMO, const QSharedPointer<BaseConcept>& t_setterBaseConcept,
                       const QSharedPointer<BaseConcept>& t_getterBaseConcept);

  QButtonGroup* m_horizontalHeaderBtnGrp;

  QString m_headerText;

  int m_oldIndex = -1;

  std::shared_ptr<ObjectSelector> m_objectSelector;

  std::tuple<int, int, boost::optional<int>> m_selectedCellLocation = std::make_tuple(-1, -1, -1);

  std::vector<std::pair<int, bool>> m_applyToButtonStates = std::vector<std::pair<int, bool>>();

 signals:

  // signal to parent to recreate all widgets
  void recreateAll();

  // signal to update a widget
  void gridCellChanged(const GridCellLocation& location, const GridCellInfo& info);

  // signal to any created quantity edits to update
  void toggleUnitsClicked(bool displayIP);

  // signal when selection changes
  void gridRowSelectionChanged();

 public slots:

  virtual void onCategorySelected(int index);

  virtual void onItemDropped(const OSItemId& itemId) = 0;

  void onToggleUnits(bool displayIP);

  virtual void onComboBoxIndexChanged(int index);

  void onSelectionCleared();

  void onInFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow);

 protected slots:

  void onSelectAllStateChanged(const int newState) const;

 private slots:

  void onHorizontalHeaderChecked(int index);

  void onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void onObjectRemoved(boost::optional<model::ParentObject> parent);  // TODO: remove?

  void onSetApplyButtonState();
};

// Possible solution for user facing column resize
// Hardst part is addressing persitance when grid redraws
//class ColumnSizer : public QWidget
//{
//  Q_OBJECT
//
//  void mouseMoveEvent ( QMouseEvent * event );
//}

class Holder : public QWidget
{
  Q_OBJECT

 public:
  Holder(QWidget* parent = nullptr);

  virtual ~Holder();

  QWidget* widget = nullptr;

 protected:
  void paintEvent(QPaintEvent* event) override;

 signals:

  void inFocus(bool inFocus, bool hasData);

  void selectionChanged(int state);
};

class HorizontalHeaderPushButton : public QPushButton
{
  Q_OBJECT

 public:
  HorizontalHeaderPushButton(QWidget* parent = nullptr);

  virtual ~HorizontalHeaderPushButton();

 protected:
  virtual void focusInEvent(QFocusEvent* e) override;

  virtual void focusOutEvent(QFocusEvent* e) override;

 signals:

  void inFocus(bool inFocus, bool hasData);
};

class HorizontalHeaderWidget : public QWidget
{
  Q_OBJECT

 public:
  HorizontalHeaderWidget(const QString& fieldName, QWidget* parent = nullptr);

  virtual ~HorizontalHeaderWidget();
  void addWidget(const QSharedPointer<QWidget>& t_widget);

  QLabel* m_label = nullptr;

  QPushButton* m_checkBox = nullptr;

  HorizontalHeaderPushButton* m_pushButton = nullptr;

  std::vector<QSharedPointer<QWidget>> m_addedWidgets;

 signals:

  void inFocus(bool inFocus, bool hasData);

 private:
  QVBoxLayout* m_innerLayout;
};

// DropZone controller that is always empty
class GridViewDropZoneVectorController : public OSVectorController
{
 protected:
  virtual std::vector<OSItemId> makeVector() override {
    return std::vector<OSItemId>();
  }
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSGRIDCONTROLLER_HPP
