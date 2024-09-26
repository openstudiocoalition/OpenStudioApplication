/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSQUANTITYEDIT_HPP
#define SHAREDGUICOMPONENTS_OSQUANTITYEDIT_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include "FieldMethodTypedefs.hpp"

#include <openstudio/model/ModelObject.hpp>

#include <openstudio/utilities/units/Unit.hpp>
#include <openstudio/utilities/core/Logger.hpp>

#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QValidator>

class QFocusEvent;

namespace openstudio {

class Unit;

class QuantityLineEdit : public QLineEdit
{
  Q_OBJECT
 public:
  explicit QuantityLineEdit(QWidget* parent = nullptr);

  virtual ~QuantityLineEdit() {}

  void enableClickFocus();

  void disableClickFocus();

  bool hasData() const;

  bool focused() const;

  void setDefaultedAndAuto(bool defaulted, bool isAuto);

  bool locked() const;

  void setLocked(bool locked);

  void updateStyle();

 protected:
  virtual void focusInEvent(QFocusEvent* e) override;

  virtual void focusOutEvent(QFocusEvent* e) override;

 private:
  bool m_hasClickFocus = false;
  bool m_defaulted = false;
  bool m_auto = false;
  bool m_focused = false;
  bool m_locked = false;

 signals:

  void inFocus(bool inFocus, bool hasData);
};

class OSQuantityEdit2
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT
 public:
  OSQuantityEdit2(const std::string& modelUnits, const std::string& siUnits, const std::string& ipUnits, bool isIP, QWidget* parent = nullptr);

  virtual ~OSQuantityEdit2();

  void enableClickFocus();

  void disableClickFocus();

  bool locked() const;

  void setLocked(bool locked);

  QDoubleValidator* doubleValidator();

  void bind(bool isIP, const model::ModelObject& modelObject, DoubleGetter get, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(bool isIP, const model::ModelObject& modelObject, OptionalDoubleGetter optionalGet, boost::optional<DoubleSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(bool isIP, const model::ModelObject& modelObject, DoubleGetter get, DoubleSetterVoidReturn set,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(bool isIP, const model::ModelObject& modelObject, OptionalDoubleGetter optionalGet, DoubleSetterVoidReturn set,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void unbind();

 signals:

  void inFocus(bool inFocus, bool hasData);

 public slots:

  void onUnitSystemChange(bool isIP);

 private slots:

  void onEditingFinished();

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  bool defaulted() const;
  bool autosized() const;
  bool autocalculated() const;
  void updateStyle();

  QuantityLineEdit* m_lineEdit;
  QLabel* m_units;
  QString m_text = "UNINITIALIZED";
  std::string m_unitsStr = "";
  QDoubleValidator* m_doubleValidator;

  bool m_isIP;
  std::string m_modelUnits;
  std::string m_siUnits;
  std::string m_ipUnits;
  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<DoubleGetter> m_get;
  boost::optional<OptionalDoubleGetter> m_optionalGet;
  boost::optional<DoubleSetter> m_set;
  boost::optional<DoubleSetterVoidReturn> m_setVoidReturn;
  boost::optional<NoFailAction> m_reset;
  boost::optional<NoFailAction> m_autosize;
  boost::optional<NoFailAction> m_autocalculate;
  boost::optional<BasicQuery> m_isDefaulted;
  boost::optional<BasicQuery> m_isAutosized;
  boost::optional<BasicQuery> m_isAutocalculated;

  bool m_isScientific;
  boost::optional<int> m_precision;

  void refreshTextAndLabel();

  void setPrecision(const std::string& str);

  void completeBind(bool isIP, const model::ModelObject& modelObject, boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize,
                    boost::optional<NoFailAction> autocalculate, boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                    boost::optional<BasicQuery> isAutocalculated);

  REGISTER_LOGGER("openstudio.OSQuantityEdit");
};

/** \deprecated Use OSQuantityEdit2. */
// class OSQuantityEdit: public QWidget, public Nano::Observer {
//   Q_OBJECT

//  public:

//   OSQuantityEdit(bool isIP, QWidget * parent = nullptr);

//   virtual ~OSQuantityEdit() {}

//   QDoubleValidator * doubleValidator() { return m_doubleValidator; }

//   void bind(model::ModelObject& modelObject,
//             const char* property,
//             bool isIP,
//             const boost::optional<std::string>& isDefaultedProperty = boost::none,
//             const boost::optional<std::string>& isAutosizedProperty = boost::none,
//             const boost::optional<std::string>& isAutocalucatedProperty = boost::none);

//   void unbind();

//  public slots:

//   void onUnitSystemChange(bool isIP);

//  private slots:

//   void onEditingFinished();

//   void onModelObjectChange();

//   void onModelObjectRemove(const Handle& handle);

//  private:

//   QLineEdit* m_lineEdit;
//   QLabel* m_units;

//   boost::optional<model::ModelObject> m_modelObject;
//   std::string m_property;
//   bool m_isIP;
//   boost::optional<std::string> m_isDefaultedProperty;
//   boost::optional<std::string> m_isAutosizedProperty;
//   boost::optional<std::string> m_isAutocalculatedProperty;

//   bool m_isScientific;
//   boost::optional<int> m_precision;
//   QDoubleValidator * m_doubleValidator;

//   std::string propertyWithSystem() const;

//   void refreshTextAndLabel();

//   void setPrecision(const std::string& str);

//   REGISTER_LOGGER("openstudio.OSQuantityEdit");
// };

class OSNonModelObjectQuantityEdit : public QWidget
{
  Q_OBJECT
 public:
  OSNonModelObjectQuantityEdit(const std::string& modelUnits, const std::string& siUnits, const std::string& ipUnits, bool isIP,
                               QWidget* parent = nullptr);

  virtual ~OSNonModelObjectQuantityEdit() = default;

  void enableClickFocus();

  void disableClickFocus();

  bool locked() const;

  void setLocked(bool locked);

  QDoubleValidator* doubleValidator();
  void setMinimumValue(double min);
  void setMaximumValue(double max);

  bool setDefault(double defaultValue);

  double currentValue() const;
  bool setCurrentValue(double valueModelUnits);

  void refreshTextAndLabel();

  void setFixedPrecision(int numberDecimals);

 signals:

  void inFocus(bool inFocus, bool hasData);
  void valueChanged(double valueModelUnits);

 public slots:

  void onUnitSystemChange(bool isIP);

 private slots:

  void onEditingFinished();

 private:
  bool defaulted() const;
  void updateStyle();

  QuantityLineEdit* m_lineEdit;
  QLabel* m_units;
  QString m_text = "UNINITIALIZED";
  std::string m_unitsStr;
  QDoubleValidator* m_doubleValidator;
  double m_defaultValue = 0.0;
  boost::optional<double> m_valueModelUnits;

  bool m_isIP;
  std::string m_modelUnits;
  std::string m_siUnits;
  std::string m_ipUnits;

  bool m_isFixedPrecision = false;
  bool m_isScientific;
  boost::optional<int> m_precision;

  void setPrecision(const std::string& str);
};

}  // namespace openstudio
//
#endif  // SHAREDGUICOMPONENTS_OSQUANTITYEDIT_HPP
