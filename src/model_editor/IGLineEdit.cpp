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

using std::ios;
using std::string;
using std::stringstream;

using namespace openstudio;

IGLineEdit::IGLineEdit(const QString& val, InspectorGadget* ig, QWidget* parent)
  : QLineEdit(val, parent), m_ig(ig), m_min(-std::numeric_limits<double>::max()), m_max(std::numeric_limits<double>::max()), m_default(0.0) {}

IGLineEdit::~IGLineEdit() {}

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

  const QRegExpValidator* dv = qobject_cast<const QRegExpValidator*>(v);
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
  } else if (const QIntValidator* iv = qobject_cast<const QIntValidator*>(v)) {
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
  // emit textEdited(val);
  // Call newValue like when editDone, otherwise the zero doesn't stick when you don't enter the field
  // See issue #1286
  emit newValue(val);
}
