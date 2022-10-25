/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSIntegerEdit.hpp"

#include <openstudio/model/ModelObject_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Containers.hpp>

#include <QFocusEvent>
#include <QIntValidator>
#include <QStyle>

#include <bitset>
#include <iomanip>

using openstudio::model::ModelObject;

namespace openstudio {

OSIntegerEdit2::OSIntegerEdit2(QWidget* parent) : QLineEdit(parent), m_isScientific(false) {
  this->setFixedWidth(90);
  this->setAcceptDrops(false);
  setEnabled(false);

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet("QLineEdit[style=\"0000\"] { color:black; background:white;   } "  // Locked=0, Focused=0, Auto=0, Defaulted=0
                      "QLineEdit[style=\"0001\"] { color:green; background:white;   } "  // Locked=0, Focused=0, Auto=0, Defaulted=1
                      "QLineEdit[style=\"0010\"] { color:grey;  background:white;   } "  // Locked=0, Focused=0, Auto=1, Defaulted=0
                      "QLineEdit[style=\"0011\"] { color:grey;  background:white;   } "  // Locked=0, Focused=0, Auto=1, Defaulted=1
                      "QLineEdit[style=\"0100\"] { color:black; background:#ffc627; } "  // Locked=0, Focused=1, Auto=0, Defaulted=0
                      "QLineEdit[style=\"0101\"] { color:green; background:#ffc627; } "  // Locked=0, Focused=1, Auto=0, Defaulted=1
                      "QLineEdit[style=\"0110\"] { color:grey;  background:#ffc627; } "  // Locked=0, Focused=1, Auto=1, Defaulted=0
                      "QLineEdit[style=\"0111\"] { color:grey;  background:#ffc627; } "  // Locked=0, Focused=1, Auto=1, Defaulted=1
                      "QLineEdit[style=\"1000\"] { color:black; background:#e6e6e6; } "  // Locked=1, Focused=0, Auto=0, Defaulted=0
                      "QLineEdit[style=\"1001\"] { color:green; background:#e6e6e6; } "  // Locked=1, Focused=0, Auto=0, Defaulted=1
                      "QLineEdit[style=\"1010\"] { color:grey;  background:#e6e6e6; } "  // Locked=1, Focused=0, Auto=1, Defaulted=0
                      "QLineEdit[style=\"1011\"] { color:grey;  background:#e6e6e6; } "  // Locked=1, Focused=0, Auto=1, Defaulted=1
                      "QLineEdit[style=\"1100\"] { color:black; background:#cc9a00; } "  // Locked=1, Focused=1, Auto=0, Defaulted=0
                      "QLineEdit[style=\"1101\"] { color:green; background:#cc9a00; } "  // Locked=1, Focused=1, Auto=0, Defaulted=1
                      "QLineEdit[style=\"1110\"] { color:grey;  background:#cc9a00; } "  // Locked=1, Focused=1, Auto=1, Defaulted=0
                      "QLineEdit[style=\"1111\"] { color:grey;  background:#cc9a00; } "  // Locked=1, Focused=1, Auto=1, Defaulted=1
  );

  m_intValidator = new QIntValidator();
  //this->setValidator(m_intValidator);
}

OSIntegerEdit2::~OSIntegerEdit2() = default;

void OSIntegerEdit2::enableClickFocus() {
  this->m_hasClickFocus = true;
}

void OSIntegerEdit2::disableClickFocus() {
  this->m_hasClickFocus = false;
  if (m_focused) {
    m_focused = false;
    clearFocus();
    updateStyle();
    emit inFocus(m_focused, false);
  }
}

bool OSIntegerEdit2::hasData() {
  return !this->text().isEmpty();
}

bool OSIntegerEdit2::locked() const {
  return m_locked;
}

void OSIntegerEdit2::setLocked(bool locked) {
  if (m_locked != locked) {
    m_locked = locked;
    setEnabled(!locked);

    if (locked) {
      disableClickFocus();
    }
    updateStyle();
  }
}

QIntValidator* OSIntegerEdit2::intValidator() {
  return m_intValidator;
}

void OSIntegerEdit2::bind(const model::ModelObject& modelObject, IntGetter get, boost::optional<IntSetter> set, boost::optional<NoFailAction> reset,
                          boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                          boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                          boost::optional<BasicQuery> isAutocalculated) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSIntegerEdit2::bind(const model::ModelObject& modelObject, OptionalIntGetter get, boost::optional<IntSetter> set,
                          boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                          boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                          boost::optional<BasicQuery> isAutocalculated) {
  m_modelObject = modelObject;
  m_getOptional = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSIntegerEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup, IntGetter get, boost::optional<IntSetter> set,
                          boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                          boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                          boost::optional<BasicQuery> isAutocalculated) {
  m_modelExtensibleGroup = modelExtensibleGroup;
  m_modelObject = modelExtensibleGroup.getOptionalObject<model::ModelObject>();
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSIntegerEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup, OptionalIntGetter get, boost::optional<IntSetter> set,
                          boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                          boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                          boost::optional<BasicQuery> isAutocalculated) {
  m_modelExtensibleGroup = modelExtensibleGroup;
  m_modelObject = modelExtensibleGroup.getOptionalObject<model::ModelObject>();
  m_getOptional = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSIntegerEdit2::completeBind() {

  // only let one of autosize/autocalculate
  if (autosized() && autocalculated()) {
    LOG_AND_THROW("A field can only be autosized or autocalculated, it cannot be both.");
  }

  setEnabled(true);

  connect(this, &OSIntegerEdit2::editingFinished, this, &OSIntegerEdit2::onEditingFinished);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSIntegerEdit2, &OSIntegerEdit2::onModelObjectChange>(this);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    ->onRemoveFromWorkspace.connect<OSIntegerEdit2, &OSIntegerEdit2::onModelObjectRemove>(this);

  refreshTextAndLabel();
}

void OSIntegerEdit2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSIntegerEdit2, &OSIntegerEdit2::onModelObjectChange>(
      this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSIntegerEdit2, &OSIntegerEdit2::onModelObjectRemove>(this);

    m_modelObject.reset();
    m_modelExtensibleGroup.reset();
    m_get.reset();
    m_getOptional.reset();
    m_set.reset();
    m_reset.reset();
    m_autosize.reset();
    m_autocalculate.reset();
    m_isDefaulted.reset();
    m_isAutosized.reset();
    m_isAutocalculated.reset();
    setLocked(true);
  }
}

void OSIntegerEdit2::onEditingFinished() {

  emit inFocus(m_focused, hasData());

  QString text = this->text();
  if (m_text == text) {
    return;
  }

  int pos = 0;
  QValidator::State state = m_intValidator->validate(text, pos);
  bool isAuto = false;
  if (state != QValidator::Acceptable) {
    if (text.isEmpty()) {
      // ok
    } else {
      boost::regex autore("[aA][uU][tT][oO]");
      isAuto = boost::regex_search(text.toStdString(), autore);
      if (isAuto) {
        // ok
      } else {
        // not ok
        refreshTextAndLabel();
        return;
      }
    }
  }

  if (m_modelObject) {
    std::string str = text.toStdString();
    ModelObject modelObject = m_modelObject.get();

    if (str.empty()) {
      if (m_reset) {
        (*m_reset)();
      } else {
        refreshTextAndLabel();
      }
    } else if (isAuto) {
      if (m_isAutosized) {
        if (m_autosize) {
          (*m_autosize)();
        } else if (m_reset) {
          (*m_reset)();
        } else {
          refreshTextAndLabel();
        }
      } else if (m_isAutocalculated) {
        if (m_autocalculate) {
          (*m_autocalculate)();
        } else if (m_reset) {
          (*m_reset)();
        } else {
          refreshTextAndLabel();
        }
      } else {
        refreshTextAndLabel();
      }
    } else {
      try {
        int value = boost::lexical_cast<int>(str);
        setPrecision(str);
        if (m_set) {
          bool result = (*m_set)(value);
          if (!result) {
            //restore
            refreshTextAndLabel();
          }
        }
      } catch (...) {
        // restore
        refreshTextAndLabel();
      }
    }
  }
}

void OSIntegerEdit2::onModelObjectChange() {
  if (m_modelExtensibleGroup) {
    if (m_modelExtensibleGroup->empty()) {
      // this is equivalent to onModelObjectRemove for the extensible group
      unbind();
      return;
    }
  }
  refreshTextAndLabel();
}

void OSIntegerEdit2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

void OSIntegerEdit2::updateStyle() {

  // Locked, Focused, Auto, Defaulted
  std::bitset<4> style;
  style[0] = defaulted();
  style[1] = autosized() || autocalculated();
  style[2] = m_focused;
  style[3] = m_locked;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

bool OSIntegerEdit2::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

bool OSIntegerEdit2::autosized() const {
  bool result = false;
  if (m_isAutosized) {
    result = (*m_isAutosized)();
  }
  return result;
}

bool OSIntegerEdit2::autocalculated() const {
  bool result = false;
  if (m_isAutocalculated) {
    result = (*m_isAutocalculated)();
  }
  return result;
}

void OSIntegerEdit2::refreshTextAndLabel() {

  QString text = this->text();

  //if (m_text == text) return;

  if (m_modelObject) {
    QString textValue;
    std::stringstream ss;

    OptionalInt oi;
    if (m_get) {
      oi = (*m_get)();
    } else {
      OS_ASSERT(m_getOptional);
      oi = (*m_getOptional)();
    }
    if (oi) {
      int value = *oi;
      if (m_isScientific) {
        ss << std::scientific;
      } else {
        ss << std::fixed;
      }
      if (m_precision) {
        ss << std::setprecision(*m_precision);
      }
      ss << value;
      textValue = toQString(ss.str());
      ss.str("");
    }

    if (m_text != textValue || text != textValue) {
      m_text = textValue;
      this->blockSignals(true);
      this->setText(m_text);
      this->updateStyle();
      this->blockSignals(false);
    }
  }
}

void OSIntegerEdit2::setPrecision(const std::string& str) {
  boost::regex rgx("-?([[:digit:]]*)(\\.)?([[:digit:]]+)([EDed][-\\+]?[[:digit:]]+)?");
  boost::smatch m;
  if (boost::regex_match(str, m, rgx)) {
    std::string sci, prefix, postfix;
    if (m[1].matched) {
      prefix = std::string(m[1].first, m[1].second);
    }
    if (m[3].matched) {
      postfix = std::string(m[3].first, m[3].second);
    }
    if (m[4].matched) {
      sci = std::string(m[4].first, m[4].second);
    }
    m_isScientific = !sci.empty();

    if (m_isScientific) {
      m_precision = prefix.size() + postfix.size() - 1;
    } else {
      m_precision = 0;
    }
  } else {
    m_isScientific = false;
    m_precision.reset();
  }
}

void OSIntegerEdit2::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, hasData());
  }

  QLineEdit::focusInEvent(e);
}

void OSIntegerEdit2::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);
  }

  QLineEdit::focusOutEvent(e);
}

}  // namespace openstudio
