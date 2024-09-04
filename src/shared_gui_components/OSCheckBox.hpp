/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSCHECKBOX_HPP
#define SHAREDGUICOMPONENTS_OSCHECKBOX_HPP

#include "FieldMethodTypedefs.hpp"

#include <openstudio/model/Model.hpp>

#include <QCheckBox>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <QPushButton>

// Forward declaration
class QFocusEvent;

namespace openstudio {

class OSSelectAllCheckBox : public QCheckBox
{
  Q_OBJECT

 public:
  explicit OSSelectAllCheckBox(QWidget* parent = nullptr);

  virtual ~OSSelectAllCheckBox();

 public slots:

  void onGridRowSelectionChanged(int numSelected, int numSelectable);
};

class OSCheckBox3
  : public QCheckBox
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit OSCheckBox3(QWidget* parent = nullptr);

  virtual ~OSCheckBox3();

  // This method will be called to enable the Checkbox to accept focus
  // (typically by the OSGridController depending on whether the underlying BaseConcept allows it)
  void enableClickFocus();

  void disableClickFocus();

  bool locked() const;

  void setLocked(bool locked);

  void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);

  void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetterBoolReturn> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);

  void unbind();

 signals:

  void inFocus(bool inFocus, bool hasData);

 protected:
  // We override these methods to emit inFocus as appropriate to enable/disable the header button
  virtual void focusInEvent(QFocusEvent* e) override;
  virtual void focusOutEvent(QFocusEvent* e) override;

 private slots:

  void onToggled(bool checked);

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  bool defaulted() const;
  void updateStyle();

  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<BoolGetter> m_get;
  boost::optional<BoolSetter> m_set;
  boost::optional<BoolSetterBoolReturn> m_setBoolReturn;
  boost::optional<NoFailAction> m_reset;
  boost::optional<BasicQuery> m_isDefaulted;

  bool m_hasClickFocus = false;
  bool m_focused = false;
  bool m_locked = false;
};

class OSCheckBox2
  : public QPushButton
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit OSCheckBox2(QWidget* parent = nullptr);

  virtual ~OSCheckBox2() {}

  void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);

  void unbind();

 signals:

  void inFocus(bool inFocus, bool hasData);

 protected:
  // We override these methods to emit inFocus as appropriate to enable/disable the header button
  virtual void focusInEvent(QFocusEvent* e) override;
  virtual void focusOutEvent(QFocusEvent* e) override;

 private slots:

  void onToggled(bool checked);

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  bool defaulted() const;
  void updateStyle();

  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<BoolGetter> m_get;
  boost::optional<BoolSetter> m_set;
  boost::optional<NoFailAction> m_reset;
  boost::optional<BasicQuery> m_isDefaulted;

  bool m_hasClickFocus = false;
  bool m_focused = false;
  bool m_locked = false;
};

class OSGreyCheckBox2
  : public QPushButton
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit OSGreyCheckBox2(QWidget* parent = nullptr);

  virtual ~OSGreyCheckBox2() {}

  void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none);

  void unbind();

 private slots:

  void onToggled(bool checked);

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<BoolGetter> m_get;
  boost::optional<BoolSetter> m_set;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSCHECKBOX_HPP
