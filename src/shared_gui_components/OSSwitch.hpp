/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSSWITCH_HPP
#define SHAREDGUICOMPONENTS_OSSWITCH_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include "FieldMethodTypedefs.hpp"

#include <openstudio/model/Model.hpp>
#include <QPushButton>

namespace openstudio {

class OSSwitch2
  : public QPushButton
  , public Nano::Observer
{
  Q_OBJECT
 public:
  explicit OSSwitch2(QWidget* parent = nullptr);

  virtual ~OSSwitch2();

  // Note: default
  void makeOnOff();

  void makeTrueFalse();

  void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);

  void unbind();

 private slots:

  void onClicked(bool checked);

  void onModelObjectChange();

  void onModelObjectRemove(const Handle& handle);

 private:
  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<BoolGetter> m_get;
  boost::optional<BoolSetter> m_set;
  boost::optional<NoFailAction> m_reset;
  boost::optional<BasicQuery> m_isDefaulted;
};

// class OSSwitch2 : public QPushButton, public Nano::Observer
// {
//   Q_OBJECT

//   public:

//  OSSwitch2(QWidget * parent = nullptr);

//   virtual ~OSSwitch2() {}

//   void bind(model::ModelObject & modelObject, const char * property);

//   void unbind();

//   private slots:

//   void onClicked(bool checked);

//   void onModelObjectChange();

//   void onModelObjectRemove(const Handle& handle);

//   private:

//   boost::optional<model::ModelObject> m_modelObject;

//   std::string m_property;
// };

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSSWITCH_HPP
