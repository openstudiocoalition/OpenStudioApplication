/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSSwitch.hpp"
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <boost/optional.hpp>
#include <QString>

namespace openstudio {

OSSwitch2::OSSwitch2(QWidget* parent) : QPushButton(parent) {
  this->setAcceptDrops(false);
  setFlat(true);
  setFixedSize(63, 21);

  makeOnOff();

  this->setCheckable(true);
}

OSSwitch2::~OSSwitch2() = default;

void OSSwitch2::makeOnOff() {
  setObjectName("OnOffSliderButton");
}

void OSSwitch2::makeTrueFalse() {
  setObjectName("TrueFalseSliderButton");
}

void OSSwitch2::bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set, boost::optional<NoFailAction> reset,
                     boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSSwitch2, &OSSwitch2::onModelObjectChange>(this);
  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onRemoveFromWorkspace.connect<OSSwitch2, &OSSwitch2::onModelObjectRemove>(
    this);

  connect(this, &OSSwitch2::clicked, this, &OSSwitch2::onClicked);

  onModelObjectChange();
}

void OSSwitch2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSSwitch2, &OSSwitch2::onModelObjectChange>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSSwitch2, &OSSwitch2::onModelObjectRemove>(this);
    m_modelObject.reset();
    m_get.reset();
    m_set.reset();
    m_reset.reset();
    m_isDefaulted.reset();
  }
}

void OSSwitch2::onClicked(bool checked) {
  if (m_modelObject && m_set) {
    if ((*m_get)() != checked) {
      (*m_set)(checked);
    }
  }
}

void OSSwitch2::onModelObjectChange() {
  if (m_modelObject) {
    if ((*m_get)() != this->isChecked()) {
      this->blockSignals(true);
      this->setChecked((*m_get)());
      this->blockSignals(false);
    }
  }
}

void OSSwitch2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

// OSSwitch::OSSwitch( QWidget * parent )
//   : QPushButton(parent)
// {
//   this->setAcceptDrops(false);
//   setFlat(true);
//   setFixedSize(63,21);

//   setObjectName("OnOffSliderButton");

//   this->setCheckable(true);
// }

// void OSSwitch::bind(model::ModelObject & modelObject, const char * property)
// {
//   m_modelObject = modelObject;

//   m_property = property;

//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.connect<OSSwitch, &OSSwitch::onModelObjectChange>(this);
//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onRemoveFromWorkspace.connect<OSSwitch, &OSSwitch::onModelObjectRemove>(this);

//   connect(this, &OSSwitch::clicked, this, &OSSwitch::onClicked);

//   onModelObjectChange();
// }

// void OSSwitch::unbind()
// {
//   if (m_modelObject){
//     m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.disconnect<OSSwitch, &OSSwitch::onModelObjectChange>(this);
//     m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onRemoveFromWorkspace.disconnect<OSSwitch, &OSSwitch::onModelObjectRemove>(this);

//     m_modelObject.reset();
//     m_property = "";
//   }
// }

// void OSSwitch::onClicked(bool checked)
// {
//   if( m_modelObject )
//   {
//     m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->setProperty(m_property.c_str(),checked);
//   }
// }

// void OSSwitch::onModelObjectChange()
// {
//   if( m_modelObject )
//   {
//     QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());

//     if( variant.canConvert<bool>() )
//     {
//       this->setChecked(variant.value<bool>());
//     }
//   }
// }

// void OSSwitch::onModelObjectRemove(const Handle& handle)
// {
//   m_modelObject.reset();
//   m_property = "";
// }

}  // namespace openstudio
