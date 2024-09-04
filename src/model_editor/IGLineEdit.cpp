/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "IGLineEdit.hpp"
#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Optional.hpp>
#include <sstream>
#include <string>
#include <limits>

#include <QWidget>
#include <QString>
#include <QVariant>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QRegularExpressionValidator>

using std::ios;
using std::string;
using std::stringstream;

using namespace openstudio;

IGLineEdit::IGLineEdit(const QString& val, InspectorGadget* ig, QWidget* parent)
  : QLineEdit(val, parent), m_ig(ig), m_min(-std::numeric_limits<double>::max()), m_max(std::numeric_limits<double>::max()), m_default(0.0) {
  setCursorPosition(0);
  setToolTip(val);
}

IGLineEdit::~IGLineEdit() = default;

void IGLineEdit::setMin(double min) {
  m_min = min;
}

void IGLineEdit::setMax(double max) {
  m_max = max;
}

void IGLineEdit::setDefault(double def) {
  m_default = def;
}

bool IGLineEdit::checkValue(QString& txt) {
  const QValidator* v = validator();
  QString textVal = text();
  if (!v || (textVal.compare("autosize", Qt::CaseInsensitive) == 0)) {
    txt = textVal;
    return true;
  }

  const auto* dv = qobject_cast<const QRegularExpressionValidator*>(v);
  if (dv) {
    bool ok;
    double val = textVal.toDouble(&ok);
    if (!ok) {
      val = m_default;
    }
    if (val < m_min) {
      val = m_min;
    } else if (val > m_max) {
      val = m_max;
    }
    txt = doubleToQString(val);
    int dummy = 0;
    bool state = (dv->validate(txt, dummy) != QValidator::Invalid);
    OS_ASSERT(state);
    return state;
  } else if (const auto* iv = qobject_cast<const QIntValidator*>(v)) {
    int min = iv->bottom();
    int max = iv->top();
    bool ok;
    int val = textVal.toInt(&ok);
    if (!ok) {
      val = (int)m_default;
    }
    if (val < min) {
      val = min;
    } else if (val > max) {
      val = max;
    }
    txt = QString::number(val);
    int dummy = 0;
    bool state = (iv->validate(txt, dummy) != QValidator::Invalid);
    OS_ASSERT(state);
    return state;
  }

  // shouldn't get here
  OS_ASSERT(false);
  return false;
}

QString IGLineEdit::doubleToQString(double v) {
  return QString::number(v, 'g', 14);
}

void IGLineEdit::editDone() {
  QString val;
  if (checkValue(val)) {
    setText(val);  // emits textChanged but not textEdited
    setToolTip(val);
    emit newValue(val);
  }
}

void IGLineEdit::hardsizeClicked(bool checked) {
  if (!checked) {
    return;
  }

  QString val;
  if (!checkValue(val) || val.isEmpty()) {
    // do not allow empty string when hard sizing
    val = doubleToQString(m_default);
  }

  setText(val);
  setToolTip(val);
  // emit textEdited(val);
  // Call newValue like when editDone, otherwise the zero doesn't stick when you don't enter the field
  // See issue #1286
  emit newValue(val);
}
