/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSINTEGEREDIT_HPP
#define SHAREDGUICOMPONENTS_OSINTEGEREDIT_HPP

#include "FieldMethodTypedefs.hpp"

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelExtensibleGroup.hpp>

#include <openstudio/utilities/core/Logger.hpp>

#include <QLineEdit>
#include <QString>
#include <QValidator>
#include <QIntValidator>

class QFocusEvent;

namespace openstudio {

class OSIntegerEdit2
  : public QLineEdit
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit OSIntegerEdit2(QWidget* parent = nullptr);

  virtual ~OSIntegerEdit2();

  void enableClickFocus();

  void disableClickFocus();

  bool hasData();

  bool locked() const;

  void setLocked(bool locked);

  QIntValidator* intValidator();

  void bind(const model::ModelObject& modelObject, IntGetter get, boost::optional<IntSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(const model::ModelObject& modelObject, OptionalIntGetter get, boost::optional<IntSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, IntGetter get, boost::optional<IntSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, OptionalIntGetter get, boost::optional<IntSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void unbind();

 protected:
  virtual void focusInEvent(QFocusEvent* e) override;

  virtual void focusOutEvent(QFocusEvent* e) override;

 signals:

  void inFocus(bool inFocus, bool hasData);

 private slots:

  void onEditingFinished();

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  bool defaulted() const;
  bool autosized() const;
  bool autocalculated() const;
  void updateStyle();

  boost::optional<model::ModelObject> m_modelObject;                    // will be set if attached to ModelObject or ModelExtensibleGroup
  boost::optional<model::ModelExtensibleGroup> m_modelExtensibleGroup;  // will only be set if attached to ModelExtensibleGroup
  boost::optional<IntGetter> m_get;
  boost::optional<OptionalIntGetter> m_getOptional;
  boost::optional<IntSetter> m_set;
  boost::optional<NoFailAction> m_reset;
  boost::optional<NoFailAction> m_autosize;
  boost::optional<NoFailAction> m_autocalculate;
  boost::optional<BasicQuery> m_isDefaulted;
  boost::optional<BasicQuery> m_isAutosized;
  boost::optional<BasicQuery> m_isAutocalculated;

  bool m_isScientific;
  bool m_hasClickFocus = false;
  bool m_focused = false;
  bool m_locked = false;
  boost::optional<int> m_precision;
  QString m_text = "UNINITIALIZED";
  QIntValidator* m_intValidator = nullptr;

  void refreshTextAndLabel();

  void setPrecision(const std::string& str);

  void completeBind();

  REGISTER_LOGGER("openstudio.OSIntegerEdit");
};

// class OSIntegerEdit: public QLineEdit, public Nano::Observer {
//   Q_OBJECT

//  public:

//   OSIntegerEdit(QWidget * parent = nullptr);

//   virtual ~OSIntegerEdit() {}

//   QIntValidator * intValidator() { return m_intValidator; }

//   void bind(const model::ModelObject& modelObject,
//             const char* property,
//             const boost::optional<std::string>& isDefaultedProperty = boost::none,
//             const boost::optional<std::string>& isAutosizedProperty = boost::none,
//             const boost::optional<std::string>& isAutocalucatedProperty = boost::none);

//   void unbind();

//  private slots:

//   void onEditingFinished();

//   void onModelObjectChange();

//   void onModelObjectRemove(const Handle& handle);

//  private:
//   boost::optional<model::ModelObject> m_modelObject;
//   std::string m_property;
//   boost::optional<std::string> m_isDefaultedProperty;
//   boost::optional<std::string> m_isAutosizedProperty;
//   boost::optional<std::string> m_isAutocalculatedProperty;

//   bool m_isScientific;
//   boost::optional<int> m_precision;
//   QIntValidator * m_intValidator = nullptr;

//   void refreshTextAndLabel();

//   void setPrecision(const std::string& str);

//   REGISTER_LOGGER("openstudio.OSIntegerEdit");
// };

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSINTEGEREDIT_HPP
