/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSDOUBLEEDIT_HPP
#define SHAREDGUICOMPONENTS_OSDOUBLEEDIT_HPP

#include "FieldMethodTypedefs.hpp"

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelExtensibleGroup.hpp>

#include <openstudio/utilities/core/Logger.hpp>

#include <QLineEdit>
#include <QString>
#include <QValidator>

class QFocusEvent;

namespace openstudio {

/** Should only be used for dimensionless real fields. Real fields with units should use
 *  OSQuantityEdit. */
class OSDoubleEdit2
  : public QLineEdit
  , public Nano::Observer
{
  Q_OBJECT
 public:
  explicit OSDoubleEdit2(QWidget* parent = nullptr);

  virtual ~OSDoubleEdit2();

  void enableClickFocus();

  void disableClickFocus();

  bool hasData();

  bool locked() const;

  void setLocked(bool locked);

  QDoubleValidator* doubleValidator();

  void bind(const model::ModelObject& modelObject, DoubleGetter get, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(const model::ModelObject& modelObject, OptionalDoubleGetter get, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(const model::ModelObject& modelObject, DoubleGetter get, DoubleSetterVoidReturn set, boost::optional<NoFailAction> reset = boost::none,
            boost::optional<NoFailAction> autosize = boost::none, boost::optional<NoFailAction> autocalculate = boost::none,
            boost::optional<BasicQuery> isDefaulted = boost::none, boost::optional<BasicQuery> isAutosized = boost::none,
            boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(const model::ModelObject& modelObject, OptionalDoubleGetter get, DoubleSetterVoidReturn set,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, DoubleGetter get, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, OptionalDoubleGetter get, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void unbind();

 signals:

  void inFocus(bool inFocus, bool hasData);

 protected:
  virtual void focusInEvent(QFocusEvent* e) override;

  virtual void focusOutEvent(QFocusEvent* e) override;

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
  boost::optional<DoubleGetter> m_get;
  boost::optional<OptionalDoubleGetter> m_getOptional;
  boost::optional<DoubleSetter> m_set;
  boost::optional<DoubleSetterVoidReturn> m_setVoidReturn;
  boost::optional<NoFailAction> m_reset;
  boost::optional<NoFailAction> m_autosize;
  boost::optional<NoFailAction> m_autocalculate;
  boost::optional<BasicQuery> m_isDefaulted;
  boost::optional<BasicQuery> m_isAutosized;
  boost::optional<BasicQuery> m_isAutocalculated;

  bool m_isScientific;
  bool m_hasClickFocus = false;
  bool m_locked = false;
  bool m_focused = false;

  boost::optional<int> m_precision;
  QString m_text = "UNINITIALIZED";
  QDoubleValidator* m_doubleValidator;

  void refreshTextAndLabel();

  void setPrecision(const std::string& str);

  void completeBind();

  REGISTER_LOGGER("openstudio.OSDoubleEdit");
};

/** Should only be used for dimensionless real fields. Real fields with units should use
 *  OSQuantityEdit. */
// class OSDoubleEdit: public QLineEdit, public Nano::Observer {
//   Q_OBJECT

//  public:

//   OSDoubleEdit(QWidget * parent = nullptr);

//   virtual ~OSDoubleEdit() {}

//   QDoubleValidator * doubleValidator() { return m_doubleValidator; }

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
//   QDoubleValidator * m_doubleValidator;

//   void refreshTextAndLabel();

//   void setPrecision(const std::string& str);

//   REGISTER_LOGGER("openstudio.OSDoubleEdit");
// };

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSDOUBLEEDIT_HPP
