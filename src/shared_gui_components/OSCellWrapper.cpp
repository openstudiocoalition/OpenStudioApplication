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
#include <openstudio/model/SpaceLoadDefinition.hpp>
#include <openstudio/model/SpaceLoadDefinition_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/ZoneHVACComponent.hpp>
#include <openstudio/model/ZoneHVACComponent_Impl.hpp>

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

OSCellWrapper::OSCellWrapper(OSGridView* gridView, QSharedPointer<BaseConcept> baseConcept, OSObjectSelector* objectSelector, int modelRow,
                             int gridRow, int column)
  : QWidget(gridView),
    m_gridView(gridView),
    m_baseConcept(baseConcept),
    m_objectSelector(objectSelector),
    m_modelRow(modelRow),
    m_gridRow(gridRow),
    m_column(column) {
  m_layout = new QGridLayout();
  m_layout->setSpacing(0);
  m_layout->setVerticalSpacing(0);
  m_layout->setHorizontalSpacing(0);
  m_layout->setContentsMargins(0, 0, 1, 1);
  this->setLayout(m_layout);
  this->setAttribute(Qt::WA_StyledBackground);
  this->setObjectName("OSCellWrapper");
  setStyleSheet("QWidget#OSCellWrapper { border: none; border-right: 1px solid gray; border-bottom: 1px solid gray; }"
                "QWidget#OSCellWrapper[header=\"true\"]{ border: none; border-top: 1px solid black; border-right: 1px solid gray; border-bottom: 1px "
                "solid black; }");

  connect(this, &OSCellWrapper::rowNeedsStyle, objectSelector, &OSObjectSelector::onRowNeedsStyle);
}

OSCellWrapper::~OSCellWrapper() {
  disconnectModelSignals();
}

void OSCellWrapper::setGridController(OSGridController* gridController) {
  m_gridController = gridController;
}

void OSCellWrapper::setModelObject(const model::ModelObject& modelObject) {
  m_modelObject = modelObject;
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

    if (m_refreshCount == 0) {
      connectModelSignals();
    }

    size_t subrowCounter = 0;
    for (auto& item : items) {
      if (item) {
        auto mo = item->cast<model::ModelObject>();
        auto innerConcept = dataSource->innerConcept();
        bool isThisSelector = (m_baseConcept->isSelector() || innerConcept->isSelector());
        bool isThisParent = (m_baseConcept->isParent() || innerConcept->isParent());

        addOSWidget(createOSWidget(mo, innerConcept), mo, isThisSelector, isThisParent);
      } else {
        addOSWidget(new QWidget(nullptr), boost::none, false, false);
      }
      subrowCounter++;
    }

    if (dataSource->source().wantsPlaceholder()) {
      // use this space to put in a blank placeholder of some kind to make sure the
      // widget is evenly laid out relative to its friends in the adjacent columns
      addOSWidget(new QWidget(nullptr), boost::none, false, false);
    }

    if (dataSource->source().dropZoneConcept()) {
      // it makes sense to me that the drop zone would need a reference to the parent containing object
      // not an object the rest in the list was derived from
      // this should also be working and doing what you want
      addOSWidget(createOSWidget(m_modelObject.get(), dataSource->source().dropZoneConcept()), boost::none, false, false);
    }

    // right here you probably want some kind of container that's smart enough to know how to grow
    // and shrink as the contained items change. But I don't know enough about the model
    // to know how you'd want to do that. For now we make a fixed list that's got a VBoxLayout
    //
    // And think about this.
  } else {
    // This case is exactly what it used to do before the DataSource idea was added.

    // just the one
    addOSWidget(createOSWidget(m_modelObject.get(), m_baseConcept), m_modelObject.get(), m_baseConcept->isSelector(), m_baseConcept->isParent());
  }

  if (m_hasSubRows) {
    // You need a holder, either by specifying a column which has a placeholder,
    // or by specifying a column which has a DataSource DropZoneConcept
    OS_ASSERT(!m_holders.empty());
    //m_holders.back()->setObjectName("InnerCellBottom");
  }

  if (m_refreshCount > 0) {
    emit rowNeedsStyle(m_modelRow, m_gridRow);
  }

  ++m_refreshCount;
}

void OSCellWrapper::setCellProperties(const GridCellLocation& location, const GridCellInfo& info) {

  if (location.subrow) {
    // apply to just one subrow
    OS_ASSERT(location.subrow.get() < m_holders.size());
    m_holders[location.subrow.get()]->setCellProperties(location, info);

  } else {
    // apply to all the subrows
    for (auto* holder : m_holders) {
      holder->setCellProperties(location, info);
    }
  }
}

QWidget* OSCellWrapper::createOSWidget(model::ModelObject t_mo, const QSharedPointer<BaseConcept>& t_baseConcept) {
  QWidget* widget = nullptr;

  OS_ASSERT(m_gridController);

  if (QSharedPointer<CheckBoxConcept> checkBoxConcept = t_baseConcept.dynamicCast<CheckBoxConcept>()) {

    // This is basically for a row in the "Select All" column
    auto* checkBox = new OSCheckBox3(nullptr);  // OSCheckBox3 is derived from QCheckBox, whereas OSCheckBox2 is derived from QPushButton
    if (checkBoxConcept->tooltip().size()) {
      checkBox->setToolTip(checkBoxConcept->tooltip().c_str());
    }

    checkBox->bind(t_mo, BoolGetter(std::bind(&CheckBoxConcept::get, checkBoxConcept.data(), t_mo)),
                   boost::optional<BoolSetter>(std::bind(&CheckBoxConcept::set, checkBoxConcept.data(), t_mo, std::placeholders::_1)));

    widget = checkBox;

  } else if (auto checkBoxConceptBoolReturn = t_baseConcept.dynamicCast<CheckBoxConceptBoolReturn>()) {
    // This is for a proper setter **that returns a bool**, such as Ideal Air Loads column
    auto* checkBoxBoolReturn = new OSCheckBox3(nullptr);
    if (checkBoxConceptBoolReturn->tooltip().size()) {
      checkBoxBoolReturn->setToolTip(checkBoxConceptBoolReturn->tooltip().c_str());
    }

    if (checkBoxConceptBoolReturn->hasClickFocus()) {
      checkBoxBoolReturn->enableClickFocus();
    }

    checkBoxBoolReturn->bind(t_mo, BoolGetter(std::bind(&CheckBoxConceptBoolReturn::get, checkBoxConceptBoolReturn.data(), t_mo)),
                             boost::optional<BoolSetterBoolReturn>(
                               std::bind(&CheckBoxConceptBoolReturn::set, checkBoxConceptBoolReturn.data(), t_mo, std::placeholders::_1)));

    widget = checkBoxBoolReturn;

  } else if (QSharedPointer<ComboBoxConcept> comboBoxConcept = t_baseConcept.dynamicCast<ComboBoxConcept>()) {

    auto choiceConcept = comboBoxConcept->choiceConcept(t_mo);

    auto* comboBox = new OSComboBox2(nullptr, choiceConcept->editable());
    if (comboBoxConcept->hasClickFocus()) {
      comboBox->enableClickFocus();
    }

    comboBox->bind(t_mo, choiceConcept);

    widget = comboBox;

  } else if (QSharedPointer<ValueEditConcept<double>> doubleEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<double>>()) {

    auto* doubleEdit = new OSDoubleEdit2(nullptr);
    if (doubleEditConcept->hasClickFocus()) {
      doubleEdit->enableClickFocus();
    }

    doubleEdit->bind(t_mo, DoubleGetter(std::bind(&ValueEditConcept<double>::get, doubleEditConcept.data(), t_mo)),
                     boost::optional<DoubleSetter>(std::bind(&ValueEditConcept<double>::set, doubleEditConcept.data(), t_mo, std::placeholders::_1)),
                     boost::optional<NoFailAction>(std::bind(&ValueEditConcept<double>::reset, doubleEditConcept.data(), t_mo)),
                     boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
                     boost::optional<BasicQuery>(std::bind(&ValueEditConcept<double>::isDefaulted, doubleEditConcept.data(), t_mo)));

    widget = doubleEdit;

  } else if (QSharedPointer<OptionalValueEditConcept<double>> optionalDoubleEditConcept =
               t_baseConcept.dynamicCast<OptionalValueEditConcept<double>>()) {

    auto* optionalDoubleEdit = new OSDoubleEdit2(nullptr);
    if (optionalDoubleEditConcept->hasClickFocus()) {
      optionalDoubleEdit->enableClickFocus();
    }

    optionalDoubleEdit->bind(t_mo, OptionalDoubleGetter(std::bind(&OptionalValueEditConcept<double>::get, optionalDoubleEditConcept.data(), t_mo)),
                             boost::optional<DoubleSetter>(
                               std::bind(&OptionalValueEditConcept<double>::set, optionalDoubleEditConcept.data(), t_mo, std::placeholders::_1)));

    widget = optionalDoubleEdit;

  } else if (QSharedPointer<ValueEditVoidReturnConcept<double>> doubleEditVoidReturnConcept =
               t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<double>>()) {

    auto* doubleEditVoidReturn = new OSDoubleEdit2(nullptr);
    if (doubleEditVoidReturnConcept->hasClickFocus()) {
      doubleEditVoidReturn->enableClickFocus();
    }

    doubleEditVoidReturn->bind(
      t_mo, DoubleGetter(std::bind(&ValueEditVoidReturnConcept<double>::get, doubleEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(std::bind(&ValueEditVoidReturnConcept<double>::set, doubleEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&ValueEditVoidReturnConcept<double>::reset, doubleEditVoidReturnConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&ValueEditVoidReturnConcept<double>::isDefaulted, doubleEditVoidReturnConcept.data(), t_mo)));

    widget = doubleEditVoidReturn;

  } else if (QSharedPointer<OptionalValueEditVoidReturnConcept<double>> optionalDoubleEditVoidReturnConcept =
               t_baseConcept.dynamicCast<OptionalValueEditVoidReturnConcept<double>>()) {

    auto* optionalDoubleEditVoidReturn = new OSDoubleEdit2(nullptr);
    if (optionalDoubleEditVoidReturnConcept->hasClickFocus()) {
      optionalDoubleEditVoidReturn->enableClickFocus();
    }

    optionalDoubleEditVoidReturn->bind(
      t_mo, OptionalDoubleGetter(std::bind(&OptionalValueEditVoidReturnConcept<double>::get, optionalDoubleEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&OptionalValueEditVoidReturnConcept<double>::set, optionalDoubleEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)));

    widget = optionalDoubleEditVoidReturn;

  } else if (QSharedPointer<ValueEditConcept<int>> integerEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<int>>()) {

    auto* integerEdit = new OSIntegerEdit2(nullptr);
    if (integerEditConcept->hasClickFocus()) {
      integerEdit->enableClickFocus();
    }

    integerEdit->bind(t_mo, IntGetter(std::bind(&ValueEditConcept<int>::get, integerEditConcept.data(), t_mo)),
                      boost::optional<IntSetter>(std::bind(&ValueEditConcept<int>::set, integerEditConcept.data(), t_mo, std::placeholders::_1)),
                      boost::optional<NoFailAction>(std::bind(&ValueEditConcept<int>::reset, integerEditConcept.data(), t_mo)),
                      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
                      boost::optional<BasicQuery>(std::bind(&ValueEditConcept<int>::isDefaulted, integerEditConcept.data(), t_mo)));

    widget = integerEdit;

  } else if (QSharedPointer<ValueEditConcept<std::string>> lineEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<std::string>>()) {

    auto* lineEdit = new OSLineEdit2(nullptr);
    if (lineEditConcept->hasClickFocus()) {
      lineEdit->enableClickFocus();
    }

    lineEdit->bind(t_mo, StringGetter(std::bind(&ValueEditConcept<std::string>::get, lineEditConcept.data(), t_mo)),
                   boost::optional<StringSetter>(std::bind(&ValueEditConcept<std::string>::set, lineEditConcept.data(), t_mo, std::placeholders::_1)),
                   boost::optional<NoFailAction>(std::bind(&ValueEditConcept<std::string>::reset, lineEditConcept.data(), t_mo)),
                   boost::optional<BasicQuery>(std::bind(&ValueEditConcept<std::string>::isDefaulted, lineEditConcept.data(), t_mo)));

    widget = lineEdit;

  } else if (QSharedPointer<ValueEditVoidReturnConcept<std::string>> lineEditConcept =
               t_baseConcept.dynamicCast<ValueEditVoidReturnConcept<std::string>>()) {

    auto* lineEdit = new OSLineEdit2(nullptr);
    if (lineEditConcept->hasClickFocus()) {
      lineEdit->enableClickFocus();
    }

    lineEdit->bind(t_mo, StringGetter(std::bind(&ValueEditVoidReturnConcept<std::string>::get, lineEditConcept.data(), t_mo)),
                   boost::optional<StringSetterVoidReturn>(
                     std::bind(&ValueEditVoidReturnConcept<std::string>::set, lineEditConcept.data(), t_mo, std::placeholders::_1)),
                   boost::optional<NoFailAction>(std::bind(&ValueEditVoidReturnConcept<std::string>::reset, lineEditConcept.data(), t_mo)),
                   boost::optional<BasicQuery>(std::bind(&ValueEditVoidReturnConcept<std::string>::isDefaulted, lineEditConcept.data(), t_mo)));

    widget = lineEdit;

  } else if (QSharedPointer<NameLineEditConcept> nameLineEditConcept = t_baseConcept.dynamicCast<NameLineEditConcept>()) {

    OSLineEdit2Interface* nameLineEdit = nameLineEditConcept->makeWidget(nullptr);
    if (nameLineEditConcept->hasClickFocus()) {
      nameLineEdit->enableClickFocus();
    }

    nameLineEdit->bind(
      t_mo, OptionalStringGetter(std::bind(&NameLineEditConcept::get, nameLineEditConcept.data(), t_mo, true)),
      boost::optional<StringSetter>(std::bind(&NameLineEditConcept::setReturnBool, nameLineEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&NameLineEditConcept::reset, nameLineEditConcept.data(), t_mo)),
      boost::optional<BasicQuery>(std::bind(&NameLineEditConcept::isInherited, nameLineEditConcept.data(), t_mo)));

    widget = nameLineEdit->qwidget();

    if (nameLineEditConcept->isInspectable()) {
      if (auto* normalLineEdit = qobject_cast<OSLineEdit2*>(widget)) {
        connect(normalLineEdit, &OSLineEdit2::itemClicked, m_gridView, &OSGridView::dropZoneItemClicked);
      } else if (auto* pixmapLineEdit = qobject_cast<OSLoadNamePixmapLineEdit*>(widget)) {
        connect(pixmapLineEdit, &OSLoadNamePixmapLineEdit::itemClicked, m_gridView, &OSGridView::dropZoneItemClicked);
      }
    }

    if (nameLineEditConcept->isLocked()) {
      nameLineEdit->setLocked(true);
    }

  } else if (QSharedPointer<QuantityEditConcept<double>> quantityEditConcept = t_baseConcept.dynamicCast<QuantityEditConcept<double>>()) {

    auto* quantityEdit =
      new OSQuantityEdit2(quantityEditConcept->modelUnits().toStdString().c_str(), quantityEditConcept->siUnits().toStdString().c_str(),
                          quantityEditConcept->ipUnits().toStdString().c_str(), quantityEditConcept->isIP(), nullptr);
    if (quantityEditConcept->hasClickFocus()) {
      quantityEdit->enableClickFocus();
    }

    quantityEdit->bind(
      m_gridController->isIP(), t_mo, DoubleGetter(std::bind(&QuantityEditConcept<double>::get, quantityEditConcept.data(), t_mo)),
      boost::optional<DoubleSetter>(std::bind(&QuantityEditConcept<double>::set, quantityEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&QuantityEditConcept<double>::reset, quantityEditConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&QuantityEditConcept<double>::isDefaulted, quantityEditConcept.data(), t_mo)));

    connect(m_gridController, &OSGridController::toggleUnitsClicked, quantityEdit, &OSQuantityEdit2::onUnitSystemChange);

    widget = quantityEdit;

  } else if (QSharedPointer<OptionalQuantityEditConcept<double>> optionalQuantityEditConcept =
               t_baseConcept.dynamicCast<OptionalQuantityEditConcept<double>>()) {

    auto* optionalQuantityEdit = new OSQuantityEdit2(
      optionalQuantityEditConcept->modelUnits().toStdString().c_str(), optionalQuantityEditConcept->siUnits().toStdString().c_str(),
      optionalQuantityEditConcept->ipUnits().toStdString().c_str(), optionalQuantityEditConcept->isIP(), nullptr);
    if (optionalQuantityEditConcept->hasClickFocus()) {
      optionalQuantityEdit->enableClickFocus();
    }

    optionalQuantityEdit->bind(m_gridController->isIP(), t_mo,
                               OptionalDoubleGetter(std::bind(&OptionalQuantityEditConcept<double>::get, optionalQuantityEditConcept.data(), t_mo)),
                               boost::optional<DoubleSetter>(std::bind(&OptionalQuantityEditConcept<double>::set, optionalQuantityEditConcept.data(),
                                                                       t_mo, std::placeholders::_1)));

    connect(m_gridController, &OSGridController::toggleUnitsClicked, optionalQuantityEdit, &OSQuantityEdit2::onUnitSystemChange);

    widget = optionalQuantityEdit;

  } else if (QSharedPointer<QuantityEditVoidReturnConcept<double>> quantityEditVoidReturnConcept =
               t_baseConcept.dynamicCast<QuantityEditVoidReturnConcept<double>>()) {

    auto* quantityEditVoidReturn = new OSQuantityEdit2(
      quantityEditVoidReturnConcept->modelUnits().toStdString().c_str(), quantityEditVoidReturnConcept->siUnits().toStdString().c_str(),
      quantityEditVoidReturnConcept->ipUnits().toStdString().c_str(), quantityEditVoidReturnConcept->isIP(), nullptr);
    if (quantityEditVoidReturnConcept->hasClickFocus()) {
      quantityEditVoidReturn->enableClickFocus();
    }

    quantityEditVoidReturn->bind(
      m_gridController->isIP(), t_mo,
      DoubleGetter(std::bind(&QuantityEditVoidReturnConcept<double>::get, quantityEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&QuantityEditVoidReturnConcept<double>::set, quantityEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&QuantityEditVoidReturnConcept<double>::reset, quantityEditVoidReturnConcept.data(), t_mo)),
      boost::optional<NoFailAction>(), boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&QuantityEditVoidReturnConcept<double>::isDefaulted, quantityEditVoidReturnConcept.data(), t_mo)));

    connect(m_gridController, &OSGridController::toggleUnitsClicked, quantityEditVoidReturn, &OSQuantityEdit2::onUnitSystemChange);

    widget = quantityEditVoidReturn;

  } else if (QSharedPointer<OptionalQuantityEditVoidReturnConcept<double>> optionalQuantityEditVoidReturnConcept =
               t_baseConcept.dynamicCast<OptionalQuantityEditVoidReturnConcept<double>>()) {

    auto* optionalQuantityEditVoidReturn = new OSQuantityEdit2(optionalQuantityEditVoidReturnConcept->modelUnits().toStdString().c_str(),
                                                               optionalQuantityEditVoidReturnConcept->siUnits().toStdString().c_str(),
                                                               optionalQuantityEditVoidReturnConcept->ipUnits().toStdString().c_str(),
                                                               optionalQuantityEditVoidReturnConcept->isIP(), nullptr);
    if (optionalQuantityEditVoidReturnConcept->hasClickFocus()) {
      optionalQuantityEditVoidReturn->enableClickFocus();
    }

    optionalQuantityEditVoidReturn->bind(
      m_gridController->isIP(), t_mo,
      OptionalDoubleGetter(std::bind(&OptionalQuantityEditVoidReturnConcept<double>::get, optionalQuantityEditVoidReturnConcept.data(), t_mo)),
      DoubleSetterVoidReturn(
        std::bind(&OptionalQuantityEditVoidReturnConcept<double>::set, optionalQuantityEditVoidReturnConcept.data(), t_mo, std::placeholders::_1)));

    connect(m_gridController, &OSGridController::toggleUnitsClicked, optionalQuantityEditVoidReturn, &OSQuantityEdit2::onUnitSystemChange);

    widget = optionalQuantityEditVoidReturn;

  } else if (QSharedPointer<ValueEditConcept<unsigned>> unsignedEditConcept = t_baseConcept.dynamicCast<ValueEditConcept<unsigned>>()) {

    auto* unsignedEdit = new OSUnsignedEdit2(nullptr);
    if (unsignedEditConcept->hasClickFocus()) {
      unsignedEdit->enableClickFocus();
    }

    unsignedEdit->bind(
      t_mo, UnsignedGetter(std::bind(&ValueEditConcept<unsigned>::get, unsignedEditConcept.data(), t_mo)),
      boost::optional<UnsignedSetter>(std::bind(&ValueEditConcept<unsigned>::set, unsignedEditConcept.data(), t_mo, std::placeholders::_1)),
      boost::optional<NoFailAction>(std::bind(&ValueEditConcept<unsigned>::reset, unsignedEditConcept.data(), t_mo)), boost::optional<NoFailAction>(),
      boost::optional<NoFailAction>(),
      boost::optional<BasicQuery>(std::bind(&ValueEditConcept<unsigned>::isDefaulted, unsignedEditConcept.data(), t_mo)));

    widget = unsignedEdit;

  } else if (QSharedPointer<DropZoneConcept> dropZoneConcept = t_baseConcept.dynamicCast<DropZoneConcept>()) {
    auto* dropZone = new OSDropZone2();
    if (dropZoneConcept->hasClickFocus()) {
      dropZone->enableClickFocus();
    }

    dropZone->bind(t_mo, OptionalModelObjectGetter(std::bind(&DropZoneConcept::get, dropZoneConcept.data(), t_mo)),
                   ModelObjectSetter(std::bind(&DropZoneConcept::set, dropZoneConcept.data(), t_mo, std::placeholders::_1)),
                   NoFailAction(std::bind(&DropZoneConcept::reset, dropZoneConcept.data(), t_mo)),
                   ModelObjectIsDefaulted(std::bind(&DropZoneConcept::isDefaulted, dropZoneConcept.data(), t_mo)),
                   OtherModelObjects(std::bind(&DropZoneConcept::otherObjects, dropZoneConcept.data(), t_mo)));

    //connect(dropZone, OSDropZone2::itemClicked, gridView(), OSGridView::dropZoneItemClicked);
    connect(dropZone, &OSDropZone2::itemClicked, m_gridView, &OSGridView::dropZoneItemClicked);

    widget = dropZone;

  } else if (QSharedPointer<RenderingColorConcept> renderingColorConcept = t_baseConcept.dynamicCast<RenderingColorConcept>()) {
    auto* renderingColorWidget = new RenderingColorWidget2(nullptr);

    renderingColorWidget->bind(t_mo, OptionalModelObjectGetter(std::bind(&RenderingColorConcept::get, renderingColorConcept.data(), t_mo)),
                               ModelObjectSetter(std::bind(&RenderingColorConcept::set, renderingColorConcept.data(), t_mo, std::placeholders::_1)));

    widget = renderingColorWidget;

  } else {
    // Unknown type
    OS_ASSERT(false);
  }

  return widget;
}

void OSCellWrapper::addOSWidget(QWidget* widget, const boost::optional<model::ModelObject>& obj, const bool isSelector, const bool isParent) {
  OS_ASSERT(m_objectSelector);

  bool isEven = ((m_gridRow % 2) == 0);

  // holder will take ownership of widget
  auto* holder = new OSWidgetHolder(this, widget, isEven);

  m_holders.push_back(holder);

  int subrow = m_holders.size() - 1;
  m_layout->addWidget(holder, subrow, 0);

  bool isLocked = false;

  if (auto* comboBox = qobject_cast<OSComboBox2*>(widget)) {
    isLocked = comboBox->locked();
    connect(comboBox, &OSComboBox2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* doubleEdit = qobject_cast<OSDoubleEdit2*>(widget)) {
    isLocked = doubleEdit->locked();
    connect(doubleEdit, &OSDoubleEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* integerEdit = qobject_cast<OSIntegerEdit2*>(widget)) {
    isLocked = integerEdit->locked();
    connect(integerEdit, &OSIntegerEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* quantityEdit = qobject_cast<OSQuantityEdit2*>(widget)) {
    isLocked = quantityEdit->locked();
    connect(quantityEdit, &OSQuantityEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* lineEdit = qobject_cast<OSLineEdit2*>(widget)) {
    isLocked = lineEdit->locked();
    connect(lineEdit, &OSLineEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* unsignedEdit = qobject_cast<OSUnsignedEdit2*>(widget)) {
    isLocked = unsignedEdit->locked();
    connect(unsignedEdit, &OSUnsignedEdit2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* pixmap = qobject_cast<OSLoadNamePixmapLineEdit*>(widget)) {
    isLocked = pixmap->locked();
    connect(pixmap, &OSLoadNamePixmapLineEdit::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* dropZone = qobject_cast<OSDropZone2*>(widget)) {
    isLocked = dropZone->locked();
    connect(dropZone, &OSDropZone2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(widget)) {
    // not locked
    connect(horizontalHeaderWidget, &HorizontalHeaderWidget::inFocus, holder, &OSWidgetHolder::inFocus);
    makeHeader();
  } else if (auto* checkBox = qobject_cast<OSCheckBox3*>(widget)) {
    isLocked = checkBox->locked();
    connect(checkBox, &OSCheckBox3::inFocus, holder, &OSWidgetHolder::inFocus);
  } else if (auto* renderingColor = qobject_cast<RenderingColorWidget2*>(widget)) {
    isLocked = renderingColor->locked();
    //connect(renderingColor, &RenderingColorWidget2::inFocus, holder, &OSWidgetHolder::inFocus);
  } else {
    // Unknown type
  }

  m_objectSelector->addObject(obj, holder, m_modelRow, m_gridRow, m_column, m_hasSubRows ? subrow : boost::optional<int>(), isSelector, isParent,
                              isLocked);
}

void OSCellWrapper::connectModelSignals() {
  if (m_modelObject && !m_connectedmodel) {
    m_connectedmodel = m_modelObject->model();

    // get signals if object is added or removed
    m_connectedmodel->getImpl<model::detail::Model_Impl>()->addWorkspaceObject.connect<OSCellWrapper, &OSCellWrapper::onAddWorkspaceObject>(this);
    m_connectedmodel->getImpl<model::detail::Model_Impl>()->removeWorkspaceObject.connect<OSCellWrapper, &OSCellWrapper::onRemoveWorkspaceObject>(
      this);
  }
}

void OSCellWrapper::disconnectModelSignals() {
  if (m_connectedmodel) {
    m_connectedmodel->getImpl<model::detail::Model_Impl>()->addWorkspaceObject.disconnect<OSCellWrapper, &OSCellWrapper::onAddWorkspaceObject>(this);
    m_connectedmodel->getImpl<model::detail::Model_Impl>()->removeWorkspaceObject.disconnect<OSCellWrapper, &OSCellWrapper::onRemoveWorkspaceObject>(
      this);

    m_connectedmodel.reset();
  }
}

void OSCellWrapper::makeHeader() {
  m_layout->setContentsMargins(0, 1, 1, 1);
  setProperty("header", true);
  this->style()->unpolish(this);
  this->style()->polish(this);
}

void OSCellWrapper::onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType,
                                            const openstudio::UUID& handle) {}

void OSCellWrapper::onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType,
                                         const openstudio::UUID& handle) {
  OS_ASSERT(m_baseConcept);
  OS_ASSERT(m_modelObject);
  OS_ASSERT(m_objectSelector);
  OS_ASSERT(m_gridController);

  // the object has been added to the model but is not fully constructed and does not yet have a parent
  m_newModelObjects.insert(object.cast<model::ModelObject>());
  QTimer::singleShot(0, this, &OSCellWrapper::processNewModelObjects);
}

void OSCellWrapper::processNewModelObjects() {
  OS_ASSERT(m_modelObject);
  OS_ASSERT(m_objectSelector);

  boost::optional<model::ThermalZone> thisThermalZone = m_modelObject->optionalCast<model::ThermalZone>();

  bool needsRefresh = false;

  for (const auto& newModelObject : m_newModelObjects) {
    boost::optional<model::ParentObject> p = newModelObject.parent();
    if (p) {
      if (p.get() == m_modelObject.get()) {
        needsRefresh = true;
        break;
      }
    }

    if (thisThermalZone) {
      boost::optional<model::ZoneHVACComponent> zoneHVACEquip = newModelObject.optionalCast<model::ZoneHVACComponent>();
      if (zoneHVACEquip) {
        boost::optional<model::ThermalZone> thermalZone = zoneHVACEquip->thermalZone();
        if (thermalZone && thermalZone.get() == thisThermalZone.get()) {
          needsRefresh = true;
          break;
        }
      }
    }
  }

  m_newModelObjects.clear();

  if (needsRefresh) {
    // clear current holders
    QLayoutItem* child;
    while ((child = m_layout->takeAt(0)) != nullptr) {
      delete child->widget();  // delete the widget
      delete child;            // delete the layout item
    }
    m_holders.clear();

    // tell object selector to clear this cell
    m_objectSelector->clearCell(m_modelRow, m_gridRow, m_column);

    // schedule refresh
    QTimer::singleShot(0, this, &OSCellWrapper::refresh);
  }
}

}  // namespace openstudio
