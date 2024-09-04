/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSUnsignedEdit.hpp"

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

OSUnsignedEdit2::OSUnsignedEdit2(QWidget* parent) : QLineEdit(parent), m_isScientific(false) {
  this->setFixedWidth(90);
  this->setAcceptDrops(false);
  setEnabled(false);

  m_intValidator = new QIntValidator();
  m_intValidator->setBottom(0);
  //this->setValidator(m_intValidator);

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
}

OSUnsignedEdit2::~OSUnsignedEdit2() = default;

void OSUnsignedEdit2::enableClickFocus() {
  this->m_hasClickFocus = true;
}

void OSUnsignedEdit2::disableClickFocus() {
  this->m_hasClickFocus = false;
  if (m_focused) {
    m_focused = false;
    clearFocus();
    updateStyle();
    emit inFocus(false, false);
  }
}

bool OSUnsignedEdit2::hasData() {
  return !this->text().isEmpty();
}

bool OSUnsignedEdit2::locked() const {
  return m_locked;
}

void OSUnsignedEdit2::setLocked(bool locked) {
  if (m_locked != locked) {
    m_locked = locked;
    setReadOnly(locked);
    if (locked) {
      disableClickFocus();
    }
    updateStyle();
  }
}

QIntValidator* OSUnsignedEdit2::intValidator() {
  return m_intValidator;
}

void OSUnsignedEdit2::bind(const model::ModelObject& modelObject, UnsignedGetter get, boost::optional<UnsignedSetter> set,
                           boost::optional<NoFailAction> reset, boost::optional<NoFailAction> autosize, boost::optional<NoFailAction> autocalculate,
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

void OSUnsignedEdit2::bind(const model::ModelObject& modelObject, OptionalUnsignedGetter get, boost::optional<UnsignedSetter> set,
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

void OSUnsignedEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup, UnsignedGetter get, boost::optional<UnsignedSetter> set,
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

void OSUnsignedEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup, OptionalUnsignedGetter get, boost::optional<UnsignedSetter> set,
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

void OSUnsignedEdit2::completeBind() {

  // only let one of autosize/autocalculate
  if ((m_isAutosized && m_isAutocalculated) || (m_isAutosized && m_autocalculate) || (m_isAutocalculated && m_autosize)) {
    LOG_AND_THROW("A field can only be autosized or autocalculated, it cannot be both.");
  }

  setEnabled(true);

  connect(this, &OSUnsignedEdit2::editingFinished, this, &OSUnsignedEdit2::onEditingFinished);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSUnsignedEdit2, &OSUnsignedEdit2::onModelObjectChange>(
    this);
  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    ->onRemoveFromWorkspace.connect<OSUnsignedEdit2, &OSUnsignedEdit2::onModelObjectRemove>(this);

  refreshTextAndLabel();
}

void OSUnsignedEdit2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onChange.disconnect<OSUnsignedEdit2, &OSUnsignedEdit2::onModelObjectChange>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSUnsignedEdit2, &OSUnsignedEdit2::onModelObjectRemove>(this);
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

void OSUnsignedEdit2::onEditingFinished() {

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
            // restore
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

void OSUnsignedEdit2::onModelObjectChange() {
  if (m_modelExtensibleGroup) {
    if (m_modelExtensibleGroup->empty()) {
      // this is equivalent to onModelObjectRemove for the extensible group
      unbind();
      return;
    }
  }
  refreshTextAndLabel();
}

void OSUnsignedEdit2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

bool OSUnsignedEdit2::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

bool OSUnsignedEdit2::autosized() const {
  bool result = false;
  if (m_isAutosized) {
    result = (*m_isAutosized)();
  }
  return result;
}

bool OSUnsignedEdit2::autocalculated() const {
  bool result = false;
  if (m_isAutocalculated) {
    result = (*m_isAutocalculated)();
  }
  return result;
}

void OSUnsignedEdit2::updateStyle() {
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

void OSUnsignedEdit2::refreshTextAndLabel() {

  QString text = this->text();

  //if (m_text == text) return;

  if (m_modelObject) {
    QString textValue;
    std::stringstream ss;

    if (autosized()) {
      textValue = QString("autosize");
    } else if (autocalculated()) {
      textValue = QString("autocalculate");
    }

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
      updateStyle();
      this->blockSignals(false);
    }
  }
}

void OSUnsignedEdit2::setPrecision(const std::string& str) {
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

void OSUnsignedEdit2::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, hasData());
  }

  QLineEdit::focusInEvent(e);
}

void OSUnsignedEdit2::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);
  }

  QLineEdit::focusOutEvent(e);
}

}  // namespace openstudio
