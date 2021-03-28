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

#include "OSCellWrapper.hpp"

#include "OSCheckBox.hpp"
#include "OSComboBox.hpp"
#include "OSDoubleEdit.hpp"
#include "OSGridView.hpp"
#include "OSIntegerEdit.hpp"
#include "OSLineEdit.hpp"
#include "OSLoadNamePixmapLineEdit.hpp"
#include "OSObjectSelector.hpp"
#include "OSQuantityEdit.hpp"
#include "OSUnsignedEdit.hpp"
#include "OSWidgetHolder.hpp"

#include "../openstudio_lib/OSDropZone.hpp"
#include "../openstudio_lib/RenderingColorWidget.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QWidget>

#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <iterator>

namespace openstudio {

OSCellWrapper::OSCellWrapper(OSGridView* gridView) : QWidget(gridView), m_gridView(gridView) {
  this->setObjectName("TableCell");

  m_layout = new QGridLayout();
  m_layout->setSpacing(0);
  m_layout->setVerticalSpacing(0);
  m_layout->setHorizontalSpacing(0);
  m_layout->setContentsMargins(5, 5, 5, 5);
  this->setLayout(m_layout);


  this->setStyleSheet("QWidget#TableCell[selected=\"true\"]{ border: none; background-color: #94b3de; border-top: 1px solid black;  "
                      "border-right: 1px solid black; border-bottom: 1px solid black;}"
                      "QWidget#TableCell[selected=\"false\"][even=\"true\"] { border: none; background-color: #ededed; border-top: 1px "
                      "solid black; border-right: 1px solid black; border-bottom: 1px solid black;}"
                      "QWidget#TableCell[selected=\"false\"][even=\"false\"] { border: none; background-color: #cecece; border-top: 1px "
                      "solid black; border-right: 1px solid black; border-bottom: 1px solid black;}"
                      "QWidget#TableCell { border: none; background-color: #ff0000; border-top: 1px "
                      "solid black; border-right: 1px solid black; border-bottom: 1px solid black;}");
}

OSCellWrapper::~OSCellWrapper() {}

void OSCellWrapper::addWidgetLambda(QWidget* t_widget, const boost::optional<model::ModelObject>& t_obj, const bool t_isSelector) {
  OS_ASSERT(m_objectSelector);

  const int widgetHeight = 30;

  auto holder = new OSWidgetHolder(m_gridView);
  holder->setMinimumHeight(widgetHeight);
  auto l = new QVBoxLayout();
  l->setAlignment(Qt::AlignCenter);
  l->setSpacing(0);
  l->setContentsMargins(0, 0, 0, 0);
  l->addWidget(t_widget);
  holder->widget = t_widget;
  holder->setLayout(l);

  m_holders.push_back(holder);

  int subrow = m_holders.size() - 1;
  m_layout->addWidget(holder, subrow, 0);

  if (OSComboBox2* comboBox = qobject_cast<OSComboBox2*>(t_widget)) {
    connect(comboBox, &OSComboBox2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSDoubleEdit2* doubleEdit = qobject_cast<OSDoubleEdit2*>(t_widget)) {
    connect(doubleEdit, &OSDoubleEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSIntegerEdit2* integerEdit = qobject_cast<OSIntegerEdit2*>(t_widget)) {
    connect(integerEdit, &OSIntegerEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSQuantityEdit2* quantityEdit = qobject_cast<OSQuantityEdit2*>(t_widget)) {
    connect(quantityEdit, &OSQuantityEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSLineEdit2* lineEdit = qobject_cast<OSLineEdit2*>(t_widget)) {
    connect(lineEdit, &OSLineEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSUnsignedEdit2* unsignedEdit = qobject_cast<OSUnsignedEdit2*>(t_widget)) {
    connect(unsignedEdit, &OSUnsignedEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSDropZone2* dropZone = qobject_cast<OSDropZone2*>(t_widget)) {
    connect(dropZone, &OSDropZone2::inFocus, holder, &OSWidgetHolder::inFocus);
    // Is this widget's subrow a surface with a defaulted construction?
    //if (t_obj) {
    //  if (auto planarSurface = t_obj->optionalCast<model::PlanarSurface>()) {
    //    if (planarSurface->isConstructionDefaulted()) {
    //      // Is this column a construction?
    //      if (column == m_constructionColumn) {
    //        dropZone->setIsDefaulted(true);
    //      }
    //    }
    //  }
    //}
  } else if (HorizontalHeaderWidget* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(t_widget)) {
    connect(horizontalHeaderWidget, &HorizontalHeaderWidget::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (OSCheckBox3* checkBox = qobject_cast<OSCheckBox3*>(t_widget)) {
    connect(checkBox, &OSCheckBox3::inFocus, holder, &OSWidgetHolder::inFocus);
  }

  m_objectSelector->addObject(t_obj, holder, m_modelRow, m_gridRow, m_column, m_hasSubRows ? subrow : boost::optional<int>(), t_isSelector);
}

void OSCellWrapper::setGridController(OSGridController* gridController) {
  m_gridController = gridController;
}

void OSCellWrapper::setObjectSelector(OSObjectSelector* objectSelector, int modelRow, int gridRow, int column) {
  m_objectSelector = objectSelector;
  m_modelRow = modelRow;
  m_gridRow = gridRow;
  m_column = column;
}

void OSCellWrapper::setModelObject(const boost::optional<model::ModelObject>& modelObject) {
  m_modelObject = modelObject;
}

void OSCellWrapper::setBaseConcept(QSharedPointer<BaseConcept> baseConcept) {
  m_baseConcept = baseConcept;
}

void OSCellWrapper::refresh() {
  OS_ASSERT(m_baseConcept);
  OS_ASSERT(m_modelObject);
  OS_ASSERT(m_objectSelector);
  OS_ASSERT(m_gridController);
  
  m_holders.clear();

  m_hasSubRows = false;
  if (QSharedPointer<DataSourceAdapter> dataSource = m_baseConcept.dynamicCast<DataSourceAdapter>()) {
    m_hasSubRows = true;
    // here we magically create a multi-row column of any type that was constructed
    //
    // The details need to be fleshed out. The ideas all work, and it's rendering as expected,
    // however the placeHolder isn't doing its job, it might need to be a QSpacer of some kind.
    // The spacing around the list is a little awkward. The padding might need to be set to 0
    // all the way around.
    auto items = dataSource->source().items(m_modelObject.get());

    size_t subrowCounter = 0;
    for (auto& item : items) {
      if (item) {
        auto mo = item->cast<model::ModelObject>();
        auto innerConcept = dataSource->innerConcept();
        bool isThisSelector = (m_baseConcept->isSelector() || innerConcept->isSelector());

        if (isThisSelector) {
          bool isThisLocked = innerConcept->isLocked(mo);
          innerConcept->setBaseLocked(isThisLocked);
        }

        addWidgetLambda(m_gridController->makeWidget(mo, innerConcept, m_gridView), mo, isThisSelector);
      } else {
        addWidgetLambda(new QWidget(m_gridView), boost::none, false);
      }
      subrowCounter++;
    }

    if (dataSource->source().wantsPlaceholder()) {
      // use this space to put in a blank placeholder of some kind to make sure the
      // widget is evenly laid out relative to its friends in the adjacent columns
      addWidgetLambda(new QWidget(m_gridView), boost::none, false);
    }

    if (dataSource->source().dropZoneConcept()) {
      // it makes sense to me that the drop zone would need a reference to the parent containing object
      // not an object the rest in the list was derived from
      // this should also be working and doing what you want
      addWidgetLambda(m_gridController->makeWidget(m_modelObject.get(), dataSource->source().dropZoneConcept(), m_gridView), boost::none, false);
    }

    // right here you probably want some kind of container that's smart enough to know how to grow
    // and shrink as the contained items change. But I don't know enough about the model
    // to know how you'd want to do that. For now we make a fixed list that's got a VBoxLayout
    //
    // And think about this.
  } else {
    // This case is exactly what it used to do before the DataSource idea was added.

    // just the one
    addWidgetLambda(m_gridController->makeWidget(m_modelObject.get(), m_baseConcept, m_gridView), m_modelObject.get(), m_baseConcept->isSelector());
  }

  if (m_hasSubRows) {
    // You need a holder, either by specifying a column which has a placeholder,
    // or by specifying a column which has a DataSource DropZoneConcept
    OS_ASSERT(!m_holders.empty());
    m_holders.back()->setObjectName("InnerCellBottom");
  }
}

void OSCellWrapper::setCellProperties(bool isSelector, int row, int column, boost::optional<int> subrow, bool isVisible, bool isSelected,
                                      bool isLocked) {
  bool isEven = ((row % 2) == 0);
  bool isChanged = false;

  this->setVisible(isVisible);

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

}  // namespace openstudio
