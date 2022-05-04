/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSQuantityEdit.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Containers.hpp>
#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/StringHelpers.hpp>

#include <QBoxLayout>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QLocale>
#include <QStyle>

#include <bitset>
#include <iomanip>

using openstudio::model::ModelObject;

namespace openstudio {

OSQuantityEdit2::OSQuantityEdit2(const std::string& modelUnits, const std::string& siUnits, const std::string& ipUnits, bool isIP, QWidget* parent)
  : QWidget(parent),
    m_lineEdit(new QuantityLineEdit()),
    m_units(new QLabel()),
    m_isIP(isIP),
    m_modelUnits(modelUnits),
    m_siUnits(siUnits),
    m_ipUnits(ipUnits),
    m_isScientific(false) {
  connect(m_lineEdit, &QuantityLineEdit::inFocus, this, &OSQuantityEdit2::inFocus);

  // do a test conversion to make sure units are ok
  boost::optional<double> test = convert(1.0, modelUnits, ipUnits);
  OS_ASSERT(test);
  test = convert(1.0, modelUnits, siUnits);
  OS_ASSERT(test);

  this->setAcceptDrops(false);
  m_lineEdit->setAcceptDrops(false);
  setEnabled(false);

  auto hLayout = new QHBoxLayout();
  setLayout(hLayout);
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->addWidget(m_lineEdit);
  hLayout->addWidget(m_units);

  m_doubleValidator = new QDoubleValidator();
  // Set the Locale to C, so that "1234.56" is accepted, but not "1234,56", no matter the user's system locale
  QLocale lo(QLocale::C);
  m_doubleValidator->setLocale(lo);
  //m_lineEdit->setValidator(m_doubleValidator);

  m_lineEdit->setMinimumWidth(60);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_units->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_lineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

OSQuantityEdit2::~OSQuantityEdit2() {}

void OSQuantityEdit2::enableClickFocus() {
  m_lineEdit->enableClickFocus();
}

bool OSQuantityEdit2::locked() const {
  return m_lineEdit->locked();
}

void OSQuantityEdit2::setLocked(bool locked) {
  m_lineEdit->setLocked(locked);
}

QDoubleValidator* OSQuantityEdit2::doubleValidator() {
  return m_doubleValidator;
}

void OSQuantityEdit2::bind(bool isIP, const model::ModelObject& modelObject, DoubleGetter get, boost::optional<DoubleSetter> set,
                           boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                           boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                           boost::optional<BasicQuery> isAutocalculated) {
  m_get = get;
  m_optionalGet.reset();
  m_set = set;
  m_setVoidReturn.reset();

  completeBind(isIP, modelObject, reset, autosize, autocalculate, isDefaulted, isAutosized, isAutocalculated);
}

void OSQuantityEdit2::bind(bool isIP, const model::ModelObject& modelObject, OptionalDoubleGetter optionalGet, boost::optional<DoubleSetter> set,
                           boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                           boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                           boost::optional<BasicQuery> isAutocalculated) {
  m_get.reset();
  m_optionalGet = optionalGet;
  m_set = set;
  m_setVoidReturn.reset();

  completeBind(isIP, modelObject, reset, autosize, autocalculate, isDefaulted, isAutosized, isAutocalculated);
}

void OSQuantityEdit2::bind(bool isIP, const model::ModelObject& modelObject, DoubleGetter get, DoubleSetterVoidReturn set,
                           boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                           boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                           boost::optional<BasicQuery> isAutocalculated) {
  m_get = get;
  m_optionalGet.reset();
  m_set.reset();
  m_setVoidReturn = set;

  completeBind(isIP, modelObject, reset, autosize, autocalculate, isDefaulted, isAutosized, isAutocalculated);
}

void OSQuantityEdit2::bind(bool isIP, const model::ModelObject& modelObject, OptionalDoubleGetter optionalGet, DoubleSetterVoidReturn set,
                           boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                           boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                           boost::optional<BasicQuery> isAutocalculated) {
  m_get.reset();
  m_optionalGet = optionalGet;
  m_set.reset();
  m_setVoidReturn = set;

  completeBind(isIP, modelObject, reset, autosize, autocalculate, isDefaulted, isAutosized, isAutocalculated);
}

void OSQuantityEdit2::completeBind(bool isIP, const model::ModelObject& modelObject, boost::optional<NoFailAction> reset,
                                   boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
                                   boost::optional<BasicQuery> isDefaulted, boost::optional<BasicQuery> isAutosized,
                                   boost::optional<BasicQuery> isAutocalculated) {
  // only let one of autosize/autocalculate
  if ((isAutosized && isAutocalculated) || (isAutosized && autocalculate) || (isAutocalculated && autosize)) {
    LOG_AND_THROW("A field can only be autosized or autocalculated, it cannot be both.");
  }

  m_isIP = isIP;
  m_modelObject = modelObject;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  setEnabled(true);

  connect(m_lineEdit, &QLineEdit::editingFinished, this,
          &OSQuantityEdit2::onEditingFinished);  // Evan note: would behaviors improve with "textChanged"?

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onChange.connect<OSQuantityEdit2, &OSQuantityEdit2::onModelObjectChange>(this);
  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onRemoveFromWorkspace.connect<OSQuantityEdit2, &OSQuantityEdit2::onModelObjectRemove>(this);

  refreshTextAndLabel();
}

void OSQuantityEdit2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onChange.disconnect<OSQuantityEdit2, &OSQuantityEdit2::onModelObjectChange>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onRemoveFromWorkspace.disconnect<OSQuantityEdit2, &OSQuantityEdit2::onModelObjectRemove>(this);
    m_modelObject.reset();
    m_get.reset();
    m_optionalGet.reset();
    m_set.reset();
    m_setVoidReturn.reset();
    m_reset.reset();
    m_autosize.reset();
    m_autocalculate.reset();
    m_isDefaulted.reset();
    m_isAutosized.reset();
    m_isAutocalculated.reset();
    setLocked(true);
  }
}

void OSQuantityEdit2::onEditingFinished() {

  emit inFocus(m_lineEdit->focused(), m_lineEdit->hasData());

  QString text = m_lineEdit->text();
  if (m_text == text) return;

  int pos = 0;
  QValidator::State state = m_doubleValidator->validate(text, pos);
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
        double value = boost::lexical_cast<double>(str);
        setPrecision(str);

        std::string units;
        if (m_isIP) {
          units = m_ipUnits;
        } else {
          units = m_siUnits;
        }
        m_unitsStr = units;

        boost::optional<double> modelValue = convert(value, units, m_modelUnits);
        OS_ASSERT(modelValue);

        if (m_set) {
          bool result = (*m_set)(*modelValue);
          if (!result) {
            // restore
            refreshTextAndLabel();
          }
        } else if (m_setVoidReturn) {
          (*m_setVoidReturn)(*modelValue);
        }
      } catch (...) {
        // restore
        refreshTextAndLabel();
      }
    }
  }
}

void OSQuantityEdit2::onModelObjectChange() {
  //if (m_modelExtensibleGroup){
  //  if (m_modelExtensibleGroup->empty()){
  //    // this is equivalent to onModelObjectRemove for the extensible group
  //    unbind();
  //    return;
  //  }
  //}
  refreshTextAndLabel();
}

void OSQuantityEdit2::onUnitSystemChange(bool isIP) {
  m_isIP = isIP;
  refreshTextAndLabel();
}

void OSQuantityEdit2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

void OSQuantityEdit2::updateStyle() {
  // will also call m_lineEdit->updateStyle()
  m_lineEdit->setDefaultedAndAuto(defaulted(), (autosized() || autocalculated()));
}

bool OSQuantityEdit2::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

bool OSQuantityEdit2::autosized() const {
  bool result = false;
  if (m_isAutosized) {
    result = (*m_isAutosized)();
  }
  return result;
}

bool OSQuantityEdit2::autocalculated() const {
  bool result = false;
  if (m_isAutocalculated) {
    result = (*m_isAutocalculated)();
  }
  return result;
}

void OSQuantityEdit2::refreshTextAndLabel() {

  QString text = m_lineEdit->text();

  std::string units;
  if (m_isIP) {
    units = m_ipUnits;
  } else {
    units = m_siUnits;
  }

  //if (m_text == text && m_unitsStr == units) return;

  if (m_modelObject) {
    QString textValue;
    std::stringstream ss;
    boost::optional<double> value;

    if (m_get) {
      value = (*m_get)();
    } else if (m_optionalGet) {
      value = (*m_optionalGet)();
    }

    if (value) {
      boost::optional<double> displayValue = convert(*value, m_modelUnits, units);
      OS_ASSERT(displayValue);

      if (m_isScientific) {
        ss << std::scientific;
      } else {
        ss << std::fixed;
      }
      if (m_precision) {

        // check if precision is too small to display value
        int precision = *m_precision;
        double minValue = std::pow(10.0, -precision);
        if (*displayValue < minValue) {
          m_precision.reset();
        }

        if (m_precision) {
          ss << std::setprecision(*m_precision);
        }
      }
      ss << *displayValue;
      textValue = toQString(ss.str());
      ss.str("");
    }

    if (m_text != textValue || text != textValue || m_unitsStr != units) {
      m_text = textValue;
      m_unitsStr = units;
      m_lineEdit->blockSignals(true);
      m_lineEdit->setText(textValue);
      updateStyle();
      m_lineEdit->blockSignals(false);
    }

    ss << units;
    m_units->blockSignals(true);
    m_units->setTextFormat(Qt::RichText);
    m_units->setText(toQString(formatUnitString(ss.str(), DocumentFormat::XHTML)));
    m_units->blockSignals(false);
  }
}

void OSQuantityEdit2::setPrecision(const std::string& str) {
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
      if (m[2].matched) {
        m_precision = postfix.size();
      } else {
        m_precision = 0;
      }
    }
  } else {
    m_isScientific = false;
    m_precision.reset();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QuantityLineEdit::QuantityLineEdit(QWidget* parent) : QLineEdit(parent) {
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
}

void QuantityLineEdit::enableClickFocus() {
  m_hasClickFocus = true;
}

void QuantityLineEdit::disableClickFocus() {
  m_hasClickFocus = false;
  m_focused = false;
  updateStyle();
  clearFocus();
  emit inFocus(false, false);
}

bool QuantityLineEdit::hasData() const {
  return !(this->text().isEmpty());
}

bool QuantityLineEdit::focused() const {
  return m_focused;
}

void QuantityLineEdit::setDefaultedAndAuto(bool defaulted, bool isAuto) {
  m_defaulted = defaulted;
  m_auto = isAuto;
  updateStyle();
}

bool QuantityLineEdit::locked() const {
  return m_locked;
}

void QuantityLineEdit::setLocked(bool locked) {
  if (m_locked != locked) {
    m_locked = locked;
    setReadOnly(locked);
    updateStyle();
  }
}

void QuantityLineEdit::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = true;
    updateStyle();
    emit inFocus(m_focused, hasData());
  }

  QLineEdit::focusInEvent(e);
}

void QuantityLineEdit::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = false;
    updateStyle();
    emit inFocus(m_focused, false);
  }

  QLineEdit::focusOutEvent(e);
}

void QuantityLineEdit::updateStyle() {
  // Locked, Focused, Auto, Defaulted
  std::bitset<4> style;
  style[0] = m_defaulted;
  style[1] = m_auto;
  style[2] = m_focused;
  style[3] = m_locked;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = this->property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

}  // namespace openstudio
