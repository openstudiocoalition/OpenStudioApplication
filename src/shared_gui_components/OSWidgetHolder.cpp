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

#include "OSWidgetHolder.hpp"

#include "OSCellWrapper.hpp"
#include "OSCheckBox.hpp"
#include "OSComboBox.hpp"
#include "OSDoubleEdit.hpp"
#include "OSIntegerEdit.hpp"
#include "OSLineEdit.hpp"
#include "OSLoadNamePixmapLineEdit.hpp"
#include "OSObjectSelector.hpp"
#include "OSQuantityEdit.hpp"
#include "OSUnsignedEdit.hpp"

#include "../openstudio_lib/OSDropZone.hpp"
#include "../openstudio_lib/RenderingColorWidget.hpp"

#include <QPainter>
#include <QStyleOption>

namespace openstudio {

OSWidgetHolder::OSWidgetHolder(OSCellWrapper* cellWrapper, QWidget* widget, bool isEven) : QWidget(cellWrapper), m_widget(widget), m_isEven(isEven) {
  this->setObjectName("OSWidgetHolder");

  const int widgetHeight = 40;

  this->setMinimumHeight(widgetHeight);
  auto* layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(5);
  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(widget);
  this->setLayout(layout);

  // set properties for style
  this->setProperty("selected", false);
  this->setProperty("even", m_isEven);

  this->setStyleSheet("QWidget#OSWidgetHolder[selected=\"true\"]{ border: none; background-color: #94b3de; }"
                      "QWidget#OSWidgetHolder[selected=\"false\"][even=\"true\"] { border: none; background-color: #ededed; }"
                      "QWidget#OSWidgetHolder[selected=\"false\"][even=\"false\"] { border: none; background-color: #cecece; }"
                      "QWidget#OSWidgetHolder { border: none; background-color: #ff0000; }");
}

OSWidgetHolder::~OSWidgetHolder() = default;

QWidget* OSWidgetHolder::widget() const {
  return m_widget;
}

void OSWidgetHolder::setCellProperties(const GridCellLocation& location, const GridCellInfo& info) {

  // set widget properties
  if (info.isSelector) {
    auto* check = qobject_cast<QCheckBox*>(m_widget);
    if (check) {
      check->blockSignals(true);
      check->setChecked(info.isSelected());
      check->blockSignals(false);
    }
  }

  // lock the widget if needed, probably a sign we need a base class with setLocked
  if (auto* comboBox = qobject_cast<OSComboBox2*>(m_widget)) {
    comboBox->setLocked(info.isLocked());
  } else if (auto* doubleEdit = qobject_cast<OSDoubleEdit2*>(m_widget)) {
    doubleEdit->setLocked(info.isLocked());
  } else if (auto* integerEdit = qobject_cast<OSIntegerEdit2*>(m_widget)) {
    integerEdit->setLocked(info.isLocked());
  } else if (auto* quantityEdit = qobject_cast<OSQuantityEdit2*>(m_widget)) {
    quantityEdit->setLocked(info.isLocked());
  } else if (auto* lineEdit = qobject_cast<OSLineEdit2*>(m_widget)) {
    lineEdit->setLocked(info.isLocked());
  } else if (auto* unsignedEdit = qobject_cast<OSUnsignedEdit2*>(m_widget)) {
    unsignedEdit->setLocked(info.isLocked());
  } else if (auto* dropZone = qobject_cast<OSDropZone2*>(m_widget)) {
    dropZone->setLocked(info.isLocked());
  } else if (auto* pixmap = qobject_cast<OSLoadNamePixmapLineEdit*>(m_widget)) {
    pixmap->setLocked(info.isLocked());
  } else if (auto* checkBox = qobject_cast<OSCheckBox3*>(m_widget)) {
    checkBox->setLocked(info.isLocked());
    if (info.isSelector) {
      checkBox->blockSignals(true);
      checkBox->setChecked(info.isSelected());
      checkBox->blockSignals(false);
    }
  }

  // apply style to this
  bool isSelected = info.isSelected();
  bool isEven = ((location.gridRow % 2) == 0);
  bool isChanged = false;

  this->setVisible(info.isVisible());
  QVariant currentSelected = this->property("selected");
  if (currentSelected.isNull() || currentSelected.toBool() != isSelected) {
    this->setProperty("selected", isSelected);
    isChanged = true;
  }

  QVariant currentEven = this->property("even");
  if (currentEven.isNull() || currentEven.toBool() != isEven) {
    this->setProperty("even", isEven);
    isChanged = true;
  }

  if (isChanged) {
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

void OSWidgetHolder::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
