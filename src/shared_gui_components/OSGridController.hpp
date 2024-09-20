/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSGRIDCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_OSGRIDCONTROLLER_HPP

#include "OSConcepts.hpp"
#include "OSObjectSelector.hpp"
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
class QColor;
class QLabel;
class OpenStudioLibFixture;

namespace openstudio {

class GridCellLocation;
class GridCellInfo;
class OSCellWrapper;
class OSGridView;
class OSWidgetHolder;

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
  explicit DataSource(const std::function<std::vector<ItemType>(InputType)>& t_sourceFunc, bool t_wantsPlaceholder = false,
                      const QSharedPointer<BaseConcept>& t_dropZoneConcept = QSharedPointer<BaseConcept>())
    : m_wantsPlaceholder(t_wantsPlaceholder), m_dropZoneConcept(t_dropZoneConcept) {

    // Remove const and reference qualifiers: passing a function that takes `const ModelObject&` => `ModelObject` so we can use that for casting
    using ParamType = typename std::remove_reference<typename std::remove_cv<InputType>::type>::type;

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
  OSGridController(bool isIP, const QString& settingsKey, IddObjectType iddObjectType, const model::Model& model,
                   const std::vector<model::ModelObject>& modelObjects, bool displayAdditionalProps = false);

  virtual ~OSGridController();

 private:
  static QSharedPointer<BaseConcept> makeDataSourceAdapter(const QSharedPointer<BaseConcept>& t_inner, const boost::optional<DataSource>& t_source);

 protected:
  void resetBaseConcepts();

  void addSelectColumn(const Heading& heading, const std::string& tooltip, const boost::optional<DataSource>& t_source = boost::none) {
    auto objectSelector = m_objectSelector;
    auto getter = std::function<bool(model::ModelObject*)>([objectSelector](model::ModelObject* t_obj) -> bool {
      assert(t_obj);
      return objectSelector->getObjectSelected(*t_obj);
    });

    auto setter = std::function<void(model::ModelObject*, bool)>([objectSelector](model::ModelObject* t_obj, bool t_set) {
      assert(t_obj);
      objectSelector->setObjectSelected(*t_obj, t_set);
    });

    addCheckBoxColumn(heading, tooltip, getter, setter, t_source);
    m_baseConcepts.back()->setIsSelector(true);
  }

  template <typename DataSourceType>
  void addCheckBoxColumn(const Heading& heading, const std::string& tooltip, std::function<bool(DataSourceType*)> t_getter,
                         std::function<void(DataSourceType*, bool)> t_setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<CheckBoxConcept>(new CheckBoxConceptImpl<DataSourceType>(heading, tooltip, t_getter, t_setter)), t_source));
  }

  template <typename DataSourceType>
  void addCheckBoxColumn(const Heading& heading, const std::string& tooltip, std::function<bool(DataSourceType*)> t_getter,
                         std::function<bool(DataSourceType*, bool)> t_setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<CheckBoxConceptBoolReturn>(new CheckBoxConceptBoolReturnImpl<DataSourceType>(heading, tooltip, t_getter, t_setter)), t_source));
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

  // This is the canonical form for a ValueType which is POD type (int, double, etc)
  // ValueType getter() const;
  // bool setter(T val);
  // void reset();
  // bool isDefaulted() const;
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

  // This one is the canonical/correct form for a ValueType which is a ModelObject or a std::string
  // ValueType getter() const;
  // bool setter(const T& val);
  // void reset();
  // bool isDefaulted() const;
  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<ValueType(DataSourceType*)> getter,
                          std::function<bool(DataSourceType*, const ValueType&)> setter,
                          const boost::optional<std::function<void(DataSourceType*)>> reset = boost::none,
                          const boost::optional<std::function<bool(DataSourceType*)>> isDefaulted = boost::none,
                          const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<ValueEditConcept<ValueType>>(new ValueEditConceptImpl<ValueType, DataSourceType>(heading, getter, setter, reset, isDefaulted)),
      t_source));
  }

  // Corner case when you have a true optional with no default value
  template <typename ValueType, typename DataSourceType>
  void addValueEditColumn(const Heading& heading, std::function<boost::optional<ValueType>(DataSourceType*)> getter,
                          std::function<bool(DataSourceType*, ValueType)> setter, const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<OptionalValueEditConcept<ValueType>>(new OptionalValueEditConceptImpl<ValueType, DataSourceType>(heading, getter, setter)),
      t_source));
  }

  // Faulty API in SDK where the setter returns void
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
  void addNameLineEditColumn(const Heading& heading, bool isInspectable, bool isLocked,
                             const std::function<boost::optional<std::string>(DataSourceType*, bool)>& getter,
                             const std::function<boost::optional<std::string>(DataSourceType*, const std::string&)>& setter,
                             const boost::optional<std::function<void(DataSourceType*)>>& resetter = boost::none,
                             const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                             const boost::optional<DataSource>& t_source = boost::none) {
    const OSLineEditType osLineEditType = OSLineEditType::OSLineEdit2Type;
    const bool hasClickFocus = true;
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<NameLineEditConcept>(new NameLineEditConceptImpl<DataSourceType>(
                              heading, osLineEditType, isInspectable, hasClickFocus, isLocked, getter, setter, resetter, isDefaulted)),
                            t_source));
  }

  // parent here means column 0 in a row with subrows
  template <typename DataSourceType>
  void addParentNameLineEditColumn(const Heading& heading, bool isInspectable,
                                   const std::function<boost::optional<std::string>(DataSourceType*, bool)>& getter,
                                   const std::function<boost::optional<std::string>(DataSourceType*, const std::string&)>& setter,
                                   const boost::optional<std::function<void(DataSourceType*)>>& resetter = boost::none,
                                   const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                                   const boost::optional<DataSource>& t_source = boost::none) {
    const OSLineEditType osLineEditType = OSLineEditType::OSLineEdit2Type;
    const bool hasClickFocus = false;
    const bool isLocked = false;
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<NameLineEditConcept>(new NameLineEditConceptImpl<DataSourceType>(
                              heading, osLineEditType, isInspectable, hasClickFocus, isLocked, getter, setter, resetter, isDefaulted)),
                            t_source));
    m_baseConcepts.back()->setIsParent(true);
  }

  template <typename DataSourceType>
  void addLoadNameColumn(const Heading& heading, const std::function<boost::optional<std::string>(DataSourceType*, bool)>& getter,
                         const std::function<boost::optional<std::string>(DataSourceType*, const std::string&)>& setter,
                         const boost::optional<std::function<void(DataSourceType*)>>& resetter = boost::none,
                         const boost::optional<std::function<bool(DataSourceType*)>>& isDefaulted = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    const OSLineEditType osLineEditType = OSLineEditType::OSLoadNamePixmapLineEditType;
    const bool isInspectable = true;
    const bool hasClickFocus = true;
    const bool isLocked = false;
    m_baseConcepts.push_back(
      makeDataSourceAdapter(QSharedPointer<NameLineEditConcept>(new NameLineEditConceptImpl<DataSourceType>(
                              heading, osLineEditType, isInspectable, hasClickFocus, isLocked, getter, setter, resetter, isDefaulted)),
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
                         boost::optional<std::function<std::vector<model::ModelObject>(const DataSourceType*)>> otherObjects = boost::none,
                         const boost::optional<DataSource>& t_source = boost::none) {
    m_baseConcepts.push_back(makeDataSourceAdapter(
      QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<ValueType, DataSourceType>(heading, getter, setter, reset, isDefaulted, otherObjects)),
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
  // There might be subrows within the specified location.
  OSCellWrapper* createCellWrapper(int gridRow, int column, OSGridView* gridView);

  // Call this function on a model update
  virtual void refreshModelObjects() = 0;

  void connectToModelSignals();

  void disconnectFromModelSignals();

  model::Model& model();

  std::vector<model::ModelObject> modelObjects() const;

  std::set<model::ModelObject> selectorObjects() const;

  std::set<model::ModelObject> selectableObjects() const;

  std::set<model::ModelObject> selectedObjects() const;

  void clearObjectSelector();

  std::function<bool(const model::ModelObject&)> objectFilter() const;

  void setObjectFilter(const std::function<bool(const model::ModelObject&)>& t_filter);

  std::function<bool(const model::ModelObject&)> objectIsLocked() const;

  void setObjectIsLocked(const std::function<bool(const model::ModelObject&)>& t_isLocked);

  IddObjectType iddObjectType() const;

  bool isIP() const;

  bool isDisplayAdditionalProps() const;

 protected:
  void setIddObjectType(const IddObjectType& iddObjectType);

 private:
  // For testing
  friend class ::OpenStudioLibFixture;

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

  model::Model m_model;

  bool m_isIP;
  bool m_displayAdditionalProps;

 protected:
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
  void setCategoriesAndFields();

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
  virtual void setHorizontalHeader(QWidget* gridView);

  // Call this function after the table is constructed
  // to appropriately check user-selected category fields
  // from QSettings and load them into a "Custom" button
  virtual void checkSelectedFields();

  void checkSelectedFields(int category);

  REGISTER_LOGGER("openstudio.OSGridController");

 private:
  friend class OSGridView;        // TODO: remove this
  friend class OSObjectSelector;  // TODO: remove this

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

  QString m_settingsKey;

  int m_oldIndex = -1;

  OSObjectSelector* m_objectSelector;

  std::tuple<int, int, boost::optional<int>> m_focusedCellLocation = std::make_tuple(-1, -1, -1);

  std::vector<std::pair<int, bool>> m_applyToButtonStates = std::vector<std::pair<int, bool>>();

  // temp variable
  std::set<model::ModelObject> m_newModelObjects;

 signals:

  // signal to add a row
  void addRow(int row);

  // signal to parent to recreate all widgets
  void recreateAll();

  // signal to update a widget
  void gridCellChanged(const GridCellLocation& location, const GridCellInfo& info);

  // signal to any created quantity edits to update
  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  // signal when selection changes
  void gridRowSelectionChanged(int numSelected, int numSelectable);

 public slots:

  virtual void onCategorySelected(int index);

  virtual void onItemDropped(const OSItemId& itemId) = 0;

  void onToggleUnits(bool displayIP);

  void onToggleDisplayAdditionalProps(bool displayAdditionalProps);

  virtual void onComboBoxIndexChanged(int index);

  void onSelectionCleared();

  void onInFocus(bool inFocus, bool hasData, int modelRow, int gridRow, int column, boost::optional<int> subrow);

 protected slots:

  void onSelectAllStateChanged(const int newState) const;

 private slots:

  void onHorizontalHeaderChecked(int index);

  void onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void processNewModelObjects();

  void onSetApplyButtonState();
};

class HorizontalHeaderPushButton : public QPushButton
{
  Q_OBJECT

 public:
  explicit HorizontalHeaderPushButton(QWidget* parent = nullptr);

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
  explicit HorizontalHeaderWidget(const QString& fieldName, QWidget* parent = nullptr);

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
