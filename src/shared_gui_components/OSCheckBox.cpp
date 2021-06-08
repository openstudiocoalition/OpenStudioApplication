/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2020, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "OSCheckBox.hpp"

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <QString>
#include <QFocusEvent>
#include <QStyle>
#include <QTimer>

#include <bitset>

namespace openstudio {

OSSelectAllCheckBox::OSSelectAllCheckBox(QWidget* parent) : QCheckBox(parent) {
  setTristate(true);
}

OSSelectAllCheckBox::~OSSelectAllCheckBox() {}

void OSSelectAllCheckBox::onGridRowSelectionChanged(int numSelected, int numSelectable) {
  blockSignals(true);
  if (numSelected == 0) {
    setCheckState(Qt::Unchecked);
  } else if (numSelected == numSelectable) {
    setCheckState(Qt::Checked);
  } else {
    setCheckState(Qt::PartiallyChecked);
  }
  blockSignals(false);
}

OSCheckBox3::OSCheckBox3(QWidget* parent) : QCheckBox(parent) {
  this->setCheckable(true);

  setFocusPolicy(Qt::NoFocus);

  setEnabled(false);

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet(
    "QCheckBox::indicator:checked[style=\"000\"]   { image: url(:/shared_gui_components/images/checked_checkbox.png); }"  // Locked=0, Focused=0, Defaulted=0
    "QCheckBox::indicator:unchecked[style=\"000\"] { image: url(:/shared_gui_components/images/unchecked_checkbox.png); }"  // Locked=0, Focused=0, Defaulted=0
    "QCheckBox::indicator:checked[style=\"001\"]   { image: url(:/shared_gui_components/images/checked_checkbox_green.png); }"  // Locked=0, Focused=0, Defaulted=1
    "QCheckBox::indicator:unchecked[style=\"001\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_green.png); }"  // Locked=0, Focused=0, Defaulted=1
    "QCheckBox::indicator:checked[style=\"010\"]   { image: url(:/shared_gui_components/images/checked_checkbox_focused.png); }"  // Locked=0, Focused=1, Defaulted=0
    "QCheckBox::indicator:unchecked[style=\"010\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_focused.png); }"  // Locked=0, Focused=1, Defaulted=0
    "QCheckBox::indicator:checked[style=\"011\"]   { image: url(:/shared_gui_components/images/checked_checkbox_focused.png); }"  // Locked=0, Focused=1, Defaulted=1
    "QCheckBox::indicator:unchecked[style=\"011\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_focused.png); }"  // Locked=0, Focused=1, Defaulted=1
    "QCheckBox::indicator:checked[style=\"100\"]   { image: url(:/shared_gui_components/images/checked_checkbox_locked.png); }"  // Locked=0, Focused=0, Defaulted=0
    "QCheckBox::indicator:unchecked[style=\"100\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_locked.png); }"  // Locked=0, Focused=0, Defaulted=0
    "QCheckBox::indicator:checked[style=\"101\"]   { image: url(:/shared_gui_components/images/checked_checkbox_locked.png); }"  // Locked=0, Focused=0, Defaulted=1
    "QCheckBox::indicator:unchecked[style=\"101\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_locked.png); }"  // Locked=0, Focused=0, Defaulted=1
    "QCheckBox::indicator:checked[style=\"110\"]   { image: url(:/shared_gui_components/images/checked_checkbox_locked.png); }"  // Locked=0, Focused=1, Defaulted=0
    "QCheckBox::indicator:unchecked[style=\"110\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_locked.png); }"  // Locked=0, Focused=1, Defaulted=0
    "QCheckBox::indicator:checked[style=\"111\"]   { image: url(:/shared_gui_components/images/checked_checkbox_locked.png); }"  // Locked=0, Focused=1, Defaulted=1
    "QCheckBox::indicator:unchecked[style=\"111\"] { image: url(:/shared_gui_components/images/unchecked_checkbox_locked.png); }"  // Locked=0, Focused=1, Defaulted=1
  );
}

OSCheckBox3::~OSCheckBox3() {}

void OSCheckBox3::bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set, boost::optional<NoFailAction> reset,
                       boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  setEnabled(true);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->openstudio::model::detail::ModelObject_Impl::onChange.connect<OSCheckBox3, &OSCheckBox3::onModelObjectChange>(this);
  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->openstudio::model::detail::ModelObject_Impl::onRemoveFromWorkspace.connect<OSCheckBox3, &OSCheckBox3::onModelObjectRemove>(this);

  connect(this, &OSCheckBox3::toggled, this, &OSCheckBox3::onToggled);
  bool checked = (*m_get)();

  this->setChecked(checked);

  // if we call update style here the set enabled above may not be in effect
  QTimer::singleShot(0, this, &OSCheckBox3::updateStyle);
}

void OSCheckBox3::bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetterBoolReturn> set,
                       boost::optional<NoFailAction> reset, boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_setBoolReturn = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  setEnabled(true);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.connect<OSCheckBox3, &OSCheckBox3::onModelObjectChange>(this);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onRemoveFromWorkspace.connect<OSCheckBox3, &OSCheckBox3::onModelObjectRemove>(this);

  connect(this, &OSCheckBox3::toggled, this, &OSCheckBox3::onToggled);
  bool checked = (*m_get)();

  this->setChecked(checked);

  // if we call update style here the set enabled above may not be in effect
  QTimer::singleShot(0, this, &OSCheckBox3::updateStyle);
}

void OSCheckBox3::enableClickFocus() {
  m_hasClickFocus = true;
  this->setFocusPolicy(Qt::ClickFocus);
}

void OSCheckBox3::disableClickFocus() {
  m_hasClickFocus = true;
  this->setFocusPolicy(Qt::NoFocus);
  clearFocus();
}

bool OSCheckBox3::locked() const {
  return m_locked;
}

void OSCheckBox3::setLocked(bool locked) {
  if (m_locked != locked) {
    m_locked = locked;
    setEnabled(!locked);
    updateStyle();
  }
}

void OSCheckBox3::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.disconnect<OSCheckBox3, &OSCheckBox3::onModelObjectChange>(
      this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onRemoveFromWorkspace.disconnect<OSCheckBox3, &OSCheckBox3::onModelObjectRemove>(this);

    m_get.reset();
    m_set.reset();
    m_setBoolReturn.reset();
    m_reset.reset();
    m_isDefaulted.reset();
    setEnabled(false);
    updateStyle();
  }
}

void OSCheckBox3::onToggled(bool checked) {
  emit inFocus(m_focused, true);  // fake that is has data
  if (m_modelObject && m_set) {
    if ((*m_get)() != checked) {
      (*m_set)(checked);
    }
  } else if (m_modelObject && m_setBoolReturn) {
    if ((*m_get)() != checked) {
      (*m_setBoolReturn)(checked);
    }
  }
}

void OSCheckBox3::onModelObjectChange() {
  if (m_modelObject) {
    if ((*m_get)() != this->isChecked()) {
      this->blockSignals(true);
      this->setChecked((*m_get)());
      updateStyle();
      this->blockSignals(false);
    }
  }
}

void OSCheckBox3::onModelObjectRemove(const Handle& handle) {
  unbind();
}

bool OSCheckBox3::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

void OSCheckBox3::updateStyle() {
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

void OSCheckBox3::focusInEvent(QFocusEvent* e) {
  if ((e->reason() == Qt::MouseFocusReason) && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, true);
  }
  QWidget::focusInEvent(e);
}

void OSCheckBox3::focusOutEvent(QFocusEvent* e) {
  if ((e->reason() == Qt::MouseFocusReason) && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);
  }
  // Pass it on for further processing
  QWidget::focusOutEvent(e);
}

OSCheckBox2::OSCheckBox2(QWidget* parent) : QPushButton(parent) {
  this->setAcceptDrops(false);

  this->setCheckable(true);

  setFocusPolicy(Qt::NoFocus);

  setEnabled(false);

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet("QPushButton[style=\"000\"] { border: none; background-color:white;   } "  // Locked=0, Focused=0, Defaulted=0
                      "QPushButton[style=\"001\"] { border: none; background-color:white;   } "  // Locked=0, Focused=0, Defaulted=1
                      "QPushButton[style=\"010\"] { border: none; background-color:#ffc627; } "  // Locked=0, Focused=1, Defaulted=0
                      "QPushButton[style=\"011\"] { border: none; background-color:#ffc627; } "  // Locked=0, Focused=1, Defaulted=1
                      "QPushButton[style=\"100\"] { border: none; background-color:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=0
                      "QPushButton[style=\"101\"] { border: none; background-color:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=1
                      "QPushButton[style=\"110\"] { border: none; background-color:#e6e6e6; } "  // Locked=1, Focused=1, Defaulted=0
                      "QPushButton[style=\"111\"] { border: none; background-color:#e6e6e6; } "  // Locked=1, Focused=1, Defaulted=1
  );
}

void OSCheckBox2::bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set, boost::optional<NoFailAction> reset,
                       boost::optional<BasicQuery> isDefaulted) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_isDefaulted = isDefaulted;

  setEnabled(true);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.connect<OSCheckBox2, &OSCheckBox2::onModelObjectChange>(this);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onRemoveFromWorkspace.connect<OSCheckBox2, &OSCheckBox2::onModelObjectRemove>(this);

  connect(this, &OSCheckBox2::toggled, this, &OSCheckBox2::onToggled);
  bool checked = (*m_get)();

  this->setChecked(checked);
  updateStyle();
}

void OSCheckBox2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.disconnect<OSCheckBox2, &OSCheckBox2::onModelObjectChange>(
      this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onRemoveFromWorkspace.disconnect<OSCheckBox2, &OSCheckBox2::onModelObjectRemove>(this);

    m_get.reset();
    m_set.reset();
    m_reset.reset();
    m_isDefaulted.reset();
    setEnabled(false);
    updateStyle();
  }
}

void OSCheckBox2::focusInEvent(QFocusEvent* e) {
  if ((e->reason() == Qt::MouseFocusReason) && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, true);
  }
  QWidget::focusInEvent(e);
}

void OSCheckBox2::focusOutEvent(QFocusEvent* e) {
  if ((e->reason() == Qt::MouseFocusReason) && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);
  }
  // Pass it on for further processing
  QWidget::focusOutEvent(e);
}

void OSCheckBox2::onToggled(bool checked) {
  if (m_modelObject && m_set) {
    if ((*m_get)() != checked) {
      (*m_set)(checked);
    }
  }
}

void OSCheckBox2::onModelObjectChange() {
  if (m_modelObject) {
    this->setChecked((*m_get)());
  }
}

void OSCheckBox2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

bool OSCheckBox2::defaulted() const {
  bool result = false;
  if (m_isDefaulted) {
    result = (*m_isDefaulted)();
  }
  return result;
}

void OSCheckBox2::updateStyle() {
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

OSGreyCheckBox2::OSGreyCheckBox2(QWidget* parent) : QPushButton(parent) {
  setObjectName("StandardGrayButton");
  this->setAcceptDrops(false);
  this->setCheckable(true);
  setEnabled(false);
}

void OSGreyCheckBox2::bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set) {
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;

  setEnabled(true);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onChange.connect<OSGreyCheckBox2, &OSGreyCheckBox2::onModelObjectChange>(this);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    .get()
    ->onRemoveFromWorkspace.connect<OSGreyCheckBox2, &OSGreyCheckBox2::onModelObjectRemove>(this);

  connect(this, &OSGreyCheckBox2::toggled, this, &OSGreyCheckBox2::onToggled);
  bool checked = (*m_get)();

  this->setChecked(checked);
}

void OSGreyCheckBox2::unbind() {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onChange.disconnect<OSGreyCheckBox2, &OSGreyCheckBox2::onModelObjectChange>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      .get()
      ->onRemoveFromWorkspace.disconnect<OSGreyCheckBox2, &OSGreyCheckBox2::onModelObjectRemove>(this);

    m_get.reset();
    m_set.reset();
    setEnabled(false);
  }
}

void OSGreyCheckBox2::onToggled(bool checked) {
  if (m_modelObject && m_set) {
    if ((*m_get)() != checked) {
      (*m_set)(checked);
    }
  }
}

void OSGreyCheckBox2::onModelObjectChange() {
  if (m_modelObject) {
    this->setChecked((*m_get)());
  }
}

void OSGreyCheckBox2::onModelObjectRemove(const Handle& handle) {
  unbind();
}

}  // namespace openstudio
