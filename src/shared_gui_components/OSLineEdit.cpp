/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSLineEdit.hpp"

#include "../openstudio_lib/InspectorController.hpp"
#include "../openstudio_lib/InspectorView.hpp"
#include "../openstudio_lib/MainRightColumnController.hpp"
#include "../openstudio_lib/ModelObjectItem.hpp"
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"
#include "../openstudio_lib/OSItem.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <boost/optional.hpp>

#include <QFocusEvent>
#include <QMouseEvent>
#include <QString>
#include <QStyle>

#include <bitset>

#if !(_DEBUG || (__GNUC__ && !NDEBUG))
#  define TIMEOUT_INTERVAL 500
#else
#  define TIMEOUT_INTERVAL 2000
#endif

namespace openstudio {

OSLineEdit2::OSLineEdit2(QWidget* parent) : QLineEdit(parent) {
  this->setAcceptDrops(false);
  setEnabled(false);

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet("QLineEdit[style=\"000\"] { color:black; background:white;   } "  // Locked=0, Focused=0, Defaulted=0
                      "QLineEdit[style=\"001\"] { color:green; background:white;   } "  // Locked=0, Focused=0, Defaulted=1
                      "QLineEdit[style=\"010\"] { color:black; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=0
                      "QLineEdit[style=\"011\"] { color:green; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=1
                      "QLineEdit[style=\"100\"] { color:black; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=0
                      "QLineEdit[style=\"101\"] { color:green; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=1
                      "QLineEdit[style=\"110\"] { color:black; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=0
                      "QLineEdit[style=\"111\"] { color:green; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=1
  );
}

OSLineEdit2::~OSLineEdit2() = default;

void OSLineEdit2::enableClickFocus() {
  this->m_hasClickFocus = true;
}

bool OSLineEdit2::hasData() {
  return !this->text().isEmpty();
}

bool OSLineEdit2::locked() const {
  return m_locked;
}

void OSLineEdit2::setLocked(bool locked) {
  m_locked = locked;
  setReadOnly(locked);
  updateStyle();
}

boost::optional<model::ModelObject> OSLineEdit2::modelObject() const {
  return m_modelObject;
}

void OSLineEdit2::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetter> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  completeBind();
}

void OSLineEdit2::bind(const model::ModelObject& modelObject, OptionalStringGetter get, boost::optional<StringSetter> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_getOptional = get;
  m_set = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  completeBind();
}

void OSLineEdit2::bind(const model::ModelObject& modelObject, OptionalStringGetter get, boost::optional<StringSetterOptionalStringReturn> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_getOptional = get;
  m_setOptionalStringReturn = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  completeBind();
}

void OSLineEdit2::bind(const model::ModelObject& modelObject, OptionalStringGetterBoolArg get, boost::optional<StringSetterOptionalStringReturn> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_getOptionalBoolArg = get;
  m_setOptionalStringReturn = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  completeBind();
}

void OSLineEdit2::bind(const model::ModelObject& modelObject, StringGetter get, boost::optional<StringSetterVoidReturn> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_setVoidReturn = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  completeBind();
}

void OSLineEdit2::completeBind() {
  setEnabled(true);

  if (!m_set && !m_setOptionalStringReturn && !m_setVoidReturn) {
    setLocked(true);
  }

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSLineEdit2, &OSLineEdit2::onModelObjectChange>(this);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    ->onRemoveFromWorkspace.connect<OSLineEdit2, &OSLineEdit2::onModelObjectRemove>(this);

  connect(this, &OSLineEdit2::editingFinished, this, &OSLineEdit2::onEditingFinished);

  m_timer.setSingleShot(true);
  connect(&m_timer, &QTimer::timeout, this, &OSLineEdit2::emitItemClicked);

  onModelObjectChangeInternal(true);
}

void OSLineEdit2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSLineEdit2, &OSLineEdit2::onModelObjectChange>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSLineEdit2, &OSLineEdit2::onModelObjectRemove>(this);

    m_modelObject.reset();
    m_get.reset();
    m_getOptional.reset();
    m_getOptionalBoolArg.reset();
    m_set.reset();
    m_setOptionalStringReturn.reset();
    m_setVoidReturn.reset();
    m_reset.reset();
    m_isDefaulted.reset();
    m_item = nullptr;
    m_text = "";

    this->blockSignals(true);
    this->setText("");
    this->setToolTip("");
    this->blockSignals(false);

    m_hasClickFocus = false;
    m_focused = false;
    setLocked(true);
  }
}

QWidget* OSLineEdit2::qwidget() {
  return this;
}

void OSLineEdit2::onEditingFinished() {
  if (m_modelObject && (m_set || m_setOptionalStringReturn || m_setVoidReturn)) {
    if (m_text != this->text().toStdString()) {
      m_text = this->text().toStdString();
      auto result = false;
      if (m_set) {
        result = (*m_set)(m_text);
      } else if (m_setOptionalStringReturn) {
        // TODO: should check the optionalStringReturn
        (*m_setOptionalStringReturn)(m_text);
        // auto optionalStringReturn = (*m_setOptionalStringReturn)(m_text);
        //if (optionalStringReturn) {
        //result = true;
        //}
        result = true;
      } else if (m_setVoidReturn) {
        (*m_setVoidReturn)(m_text);
        result = true;
      }
      if (!result) {
        //restore
        onModelObjectChange();
      } else {
        emit inFocus(m_focused, hasData());
        adjustWidth();
      }
    }
  }
}

void OSLineEdit2::adjustWidth() {
  if (m_modelObject) {
    // Adjust the width to accommodate the text
    //QFont myFont;
    //QFontMetrics fm(myFont);
    //auto width = fm.horizontalAdvance(toQString(m_text));
    //if (width < 80) width = 80;
    //setFixedWidth(width + 10);
  }
}

void OSLineEdit2::updateStyle() {
  // Locked, Focused, Defaulted
  std::bitset<3> style;
  style[0] = defaulted();
  style[1] = m_focused;
  style[2] = m_locked;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

bool OSLineEdit2::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

void OSLineEdit2::onModelObjectChange() {
  onModelObjectChangeInternal(false);
}

void OSLineEdit2::onModelObjectChangeInternal(bool startingup) {
  if (m_modelObject) {
    OptionalString value;
    if (m_get) {
      value = (*m_get)();
    } else if (m_getOptional) {
      value = (*m_getOptional)();
    } else if (m_getOptionalBoolArg) {
      value = (*m_getOptionalBoolArg)(true);  // TODO may want to pass a variable
    } else {
      // unhandled
      OS_ASSERT(false);
    }

    std::string text;
    if (value) {
      text = *value;
      if (m_text != text) {
        m_text = text;
        this->blockSignals(true);
        QString qtext = QString::fromStdString(m_text);
        this->setText(qtext);
        this->setToolTip(qtext);
        updateStyle();
        this->blockSignals(false);
        if (startingup) {
          this->setCursorPosition(0);
        } else {
          m_timer.start(TIMEOUT_INTERVAL);
        }
      }
    }
  }
}

void OSLineEdit2::emitItemClicked() {
  if (!m_item && m_modelObject) {
    bool isDefaulted = defaulted();

    // override isDefaulted to prevent deleting in the inspector
    if (m_locked) {
      isDefaulted = true;
    }

    m_item = OSItem::makeItem(modelObjectToItemId(*m_modelObject, isDefaulted));
    OS_ASSERT(m_item);
    m_item->setParent(this);

    if (!isDefaulted && m_reset) {
      m_item->setRemoveable(true);
    } else {
      m_item->setRemoveable(false);
    }
    connect(m_item, &OSItem::itemRemoveClicked, this, &OSLineEdit2::onItemRemoveClicked);
  }

  if (m_item) {
    // Tell EditView to display this object
    emit itemClicked(m_item);
  }
}

void OSLineEdit2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

void OSLineEdit2::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    event->accept();

    m_timer.start(TIMEOUT_INTERVAL);
  }
}

void OSLineEdit2::onItemRemoveClicked() {
  if (m_reset) {
    // unbind will get called in onModelObjectRemove if model object is removed
    (*m_reset)();
  }
}

void OSLineEdit2::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, hasData());
  }

  QLineEdit::focusInEvent(e);
}

void OSLineEdit2::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);

    auto mouseOverInspectorView =
      OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectorController()->inspectorView()->mouseOverInspectorView();
    if (!mouseOverInspectorView) {
      emit itemClicked(nullptr);
    }
  }

  QLineEdit::focusOutEvent(e);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OSLineEdit::OSLineEdit( QWidget * parent )
//   : QLineEdit(parent)
// {
//   this->setAcceptDrops(false);
//   setEnabled(false);
// }

// void OSLineEdit::bind(model::ModelObject & modelObject, const char * property)
// {
//   m_modelObject = modelObject;

//   m_property = property;

//   setEnabled(true);

//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.connect<OSLineEdit, &OSLineEdit::onModelObjectChange>(this);

//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onRemoveFromWorkspace.connect<OSLineEdit, &OSLineEdit::onModelObjectRemove>(this);

//   connect(this, &OSLineEdit::editingFinished, this, &OSLineEdit::onEditingFinished);

//   onModelObjectChange();
// }

// void OSLineEdit::unbind()
// {
//   if (m_modelObject){
//     m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.disconnect<OSLineEdit, &OSLineEdit::onModelObjectChange>(this);
//     m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onRemoveFromWorkspace.disconnect<OSLineEdit, &OSLineEdit::onModelObjectRemove>(this);

//     m_modelObject.reset();
//     m_property = "";
//     setEnabled(false);
//   }
// }

// void OSLineEdit::onEditingFinished()
// {
//   if( m_modelObject )
//   {
//     std::string stdstring = this->text().toStdString();

//     QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());

//     if( variant.canConvert<std::string>() ) {
//       QVariant var;
//       var.setValue<std::string>(stdstring);

//       m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->setProperty(m_property.c_str(), var);
//     } else if( variant.canConvert<boost::optional<std::string> >() ) {
//       boost::optional<std::string> ostring(stdstring);

//       QVariant ovar;
//       ovar.setValue<boost::optional<std::string> >(ostring);

//       m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->setProperty(m_property.c_str(),ovar);
//     }

//   }
// }

// void OSLineEdit::onModelObjectChange()
// {
//   if( m_modelObject )
//   {
//     QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());

//     if( variant.canConvert<std::string>() )
//     {
//       setText(QString::fromStdString(variant.value<std::string>()));
//     }
//     else if( variant.canConvert<boost::optional<std::string> >() )
//     {
//       if( boost::optional<std::string> string = variant.value<boost::optional<std::string> >() )
//       {
//         if( string )
//         {
//           setText(QString::fromStdString(string.get()));
//         }
//       }
//     }
//   }
// }

// void OSLineEdit::onModelObjectRemove(const Handle& handle)
// {
//   m_modelObject.reset();
//   m_property = "";
//   setEnabled(false);
// }

}  // namespace openstudio
