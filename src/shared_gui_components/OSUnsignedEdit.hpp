/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSUNSIGNEDEDIT_HPP
#define SHAREDGUICOMPONENTS_OSUNSIGNEDEDIT_HPP

#include "FieldMethodTypedefs.hpp"

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelExtensibleGroup.hpp>

#include <openstudio/utilities/core/Logger.hpp>

#include <QLineEdit>
#include <QString>
#include <QValidator>

class QFocusEvent;

namespace openstudio {

class OSUnsignedEdit2 : public QLineEdit
{
  Q_OBJECT

 public:
  explicit OSUnsignedEdit2(QWidget* parent = nullptr);

  virtual ~OSUnsignedEdit2();

  void enableClickFocus();

  void disableClickFocus();

  bool hasData();

  bool locked() const;

  void setLocked(bool locked);

  QIntValidator* intValidator();

  void bind(const model::ModelObject& modelObject, UnsignedGetter get, boost::optional<UnsignedSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(const model::ModelObject& modelObject, OptionalUnsignedGetter get, boost::optional<UnsignedSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, UnsignedGetter get, boost::optional<UnsignedSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<NoFailAction> autosize = boost::none,
            boost::optional<NoFailAction> autocalculate = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none,
            boost::optional<BasicQuery> isAutosized = boost::none, boost::optional<BasicQuery> isAutocalculated = boost::none);

  void bind(model::ModelExtensibleGroup& modelExtensibleGroup, OptionalUnsignedGetter get, boost::optional<UnsignedSetter> set = boost::none,
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
  boost::optional<UnsignedGetter> m_get;
  boost::optional<OptionalUnsignedGetter> m_getOptional;
  boost::optional<UnsignedSetter> m_set;
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
  QIntValidator* m_intValidator = nullptr;

  void refreshTextAndLabel();

  void setPrecision(const std::string& str);

  void completeBind();

  REGISTER_LOGGER("openstudio.OSUnsignedEdit");
};

// class OSUnsignedEdit: public QLineEdit {
//   Q_OBJECT

//  public:

//   OSUnsignedEdit(QWidget * parent = nullptr);

//   virtual ~OSUnsignedEdit() {}

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

//   REGISTER_LOGGER("openstudio.OSUnsignedEdit");
// };

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSUNSIGNEDEDIT_HPP
