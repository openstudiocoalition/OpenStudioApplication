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

#include "PeopleInspectorView.hpp"

#include "../shared_gui_components/OSSwitch.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSDoubleEdit.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/ModelExtensibleGroup.hpp>
#include <openstudio/model/PeopleDefinition.hpp>
#include <openstudio/model/PeopleDefinition_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/SpaceLoadInstance.hpp>
#include <openstudio/utilities/idf/IdfExtensibleGroup.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

namespace openstudio {

PeopleDefinitionInspectorView::PeopleDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  m_isIP = isIP;

  //QWidget* hiddenWidget = new QWidget();
  //this->stackedWidget()->insertWidget(0, hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  //this->stackedWidget()->setCurrentIndex(0);

  m_mainGridLayout = new QGridLayout();
  m_mainGridLayout->setContentsMargins(7, 7, 7, 7);
  m_mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(m_mainGridLayout);

  // int row = m_mainGridLayout->rowCount(); // Starts at 1, I actually want 0
  int row = 0;

  // name
  auto* vLayout = new QVBoxLayout();

  auto* label = new QLabel("Name: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_nameEdit = new OSLineEdit2();
  vLayout->addWidget(m_nameEdit);

  // Spans 1 row and 3 columns
  m_mainGridLayout->addLayout(vLayout, row, 0, 1, 3, Qt::AlignTop);
  ++row;

  // number of people, people per area, and area per person
  vLayout = new QVBoxLayout();

  label = new QLabel("Number of People: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_numberofPeopleEdit = new OSDoubleEdit2();
  vLayout->addWidget(m_numberofPeopleEdit);

  m_mainGridLayout->addLayout(vLayout, row, 0, Qt::AlignTop | Qt::AlignLeft);

  vLayout = new QVBoxLayout();

  label = new QLabel("People per Space Floor Area: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_peopleperSpaceFloorAreaEdit = new OSQuantityEdit2("people/m^2", "people/m^2", "people/ft^2", m_isIP);
  connect(this, &PeopleDefinitionInspectorView::toggleUnitsClicked, m_peopleperSpaceFloorAreaEdit, &OSQuantityEdit2::onUnitSystemChange);
  vLayout->addWidget(m_peopleperSpaceFloorAreaEdit);

  m_mainGridLayout->addLayout(vLayout, row, 1, Qt::AlignTop | Qt::AlignLeft);

  vLayout = new QVBoxLayout();

  label = new QLabel("Space Floor Area per Person: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_spaceFloorAreaperPersonEdit = new OSQuantityEdit2("m^2/person", "m^2/person", "ft^2/person", m_isIP);
  connect(this, &PeopleDefinitionInspectorView::toggleUnitsClicked, m_spaceFloorAreaperPersonEdit, &OSQuantityEdit2::onUnitSystemChange);
  vLayout->addWidget(m_spaceFloorAreaperPersonEdit);

  m_mainGridLayout->addLayout(vLayout, row, 2, Qt::AlignTop | Qt::AlignLeft);

  // New Row: fraction radiance, sensible heat fraction, carbon dioxide rate
  ++row;
  vLayout = new QVBoxLayout();

  label = new QLabel("Fraction Radiant: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_fractionRadiantEdit = new OSDoubleEdit2();
  vLayout->addWidget(m_fractionRadiantEdit);

  m_mainGridLayout->addLayout(vLayout, row, 0, Qt::AlignTop | Qt::AlignLeft);

  vLayout = new QVBoxLayout();

  label = new QLabel("Sensible Heat Fraction: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_sensibleHeatFractionEdit = new OSDoubleEdit2();
  vLayout->addWidget(m_sensibleHeatFractionEdit);

  m_mainGridLayout->addLayout(vLayout, row, 1, Qt::AlignTop | Qt::AlignLeft);

  vLayout = new QVBoxLayout();

  label = new QLabel("Carbon Dioxide Generation Rate: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_carbonDioxideGenerationRateEdit = new OSQuantityEdit2("m^3/s*W", "L/s*W", "ft^3*hr/min*Btu", m_isIP);
  connect(this, &PeopleDefinitionInspectorView::toggleUnitsClicked, m_carbonDioxideGenerationRateEdit, &OSQuantityEdit2::onUnitSystemChange);
  vLayout->addWidget(m_carbonDioxideGenerationRateEdit);

  m_mainGridLayout->addLayout(vLayout, row, 2, Qt::AlignTop | Qt::AlignLeft);

  // Separator
  ++row;
  // LOG(Debug, "Adding first separator at row=" << row);
  auto* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  // Important not to specific AlignTop & AlignLeft  or it's not displayed
  m_mainGridLayout->addWidget(line, row, 0, 1, 3);  // , Qt::AlignTop|Qt::AlignLeft);

  // Confort stuff
  ++row;

  vLayout = new QVBoxLayout();
  label = new QLabel("Enable ASHRAE 55 Comfort Warnings:");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_enableASHRAE55ComfortWarningsSwitch = new OSSwitch2();
  vLayout->addWidget(m_enableASHRAE55ComfortWarningsSwitch);
  m_mainGridLayout->addLayout(vLayout, row, 0, Qt::AlignTop | Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  label = new QLabel("Mean Radiant Temperature Calculation Type:");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_meanRadiantTemperatureCalculationTypeComboBox = new OSComboBox2();
  // Populate combobox choices
  for (const std::string& mrtType : openstudio::model::PeopleDefinition::meanRadiantTemperatureCalculationTypeValues()) {
    m_meanRadiantTemperatureCalculationTypeComboBox->addItem(QString::fromStdString(mrtType));
  }
  vLayout->addWidget(m_meanRadiantTemperatureCalculationTypeComboBox);
  m_mainGridLayout->addLayout(vLayout, row, 1, Qt::AlignTop | Qt::AlignLeft);

  // Extensible Toolbar
  ++row;
  auto* hbox = new QHBoxLayout();
  hbox->setSpacing(0);
  hbox->setContentsMargins(0, 0, 0, 0);

  label = new QLabel(tr("Add/Remove Extensible Groups"));
  label->setObjectName("H2");

  addBtn = new QPushButton();
  QIcon ico(":images/edit_add.png");
  addBtn->setIcon(ico);
  addBtn->setStyleSheet(" margin: 0px; border: 0px;");

  removeBtn = new QPushButton();
  QIcon ico2(":images/edit_remove.png");
  removeBtn->setIcon(ico2);
  removeBtn->setStyleSheet(" margin: 0px; border: 0px;");

  connect(addBtn, &QPushButton::clicked, this, &PeopleDefinitionInspectorView::addExtensible);
  connect(removeBtn, &QPushButton::clicked, this, &PeopleDefinitionInspectorView::removeExtensible);

  // Not attached yet! checkRemoveBtn( subBtn );
  removeBtn->setEnabled(false);

  // hbox->addSpacing(10);//because this row doesn't have a vbox, we need to move the text over some to get it to line up
  hbox->addWidget(label);
  hbox->addWidget(addBtn);
  hbox->addWidget(removeBtn);
  m_mainGridLayout->addLayout(hbox, row, 0, 1, 3, Qt::AlignTop | Qt::AlignLeft);

  // Separator
  ++row;
  // LOG(Debug, "Adding second separator at row=" << row);
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  m_mainGridLayout->addWidget(line, row, 0, 1, 3);  // , Qt::AlignTop|Qt::AlignLeft);

  // Save position
  lastRow = row;
  lastRowNonExtensible = lastRow;

  // Set row / col minimum dimensions
  m_mainGridLayout->setColumnMinimumWidth(0, 80);
  m_mainGridLayout->setColumnMinimumWidth(1, 80);
  m_mainGridLayout->setColumnMinimumWidth(2, 80);

  for (int i = 0; i <= lastRow; ++i) {
    m_mainGridLayout->setRowMinimumHeight(0, 30);
  }

  // The Idea here is to set the Strech factor of the row / column **after** the last one used to 1
  // So that we basically push everything in the top left corner.
  // Remember that row/cols are 0-indexed. We use 3 columns, so we set the column strech factor of the 4th column below
  m_mainGridLayout->setColumnStretch(3, 1);
  m_mainGridLayout->setRowStretch(lastRow + 1, 1);
}

void PeopleDefinitionInspectorView::adjustRowStretch() {

  // int row = m_mainGridLayout->rowCount();
  for (int i = lastRowNonExtensible + 1; i <= lastRow; ++i) {
    // set Row Stretch to the default of 0
    m_mainGridLayout->setRowStretch(i, 0);
    m_mainGridLayout->setRowMinimumHeight(i, 30);
  }
  // row after the last used: set a strech of 1 to push everything up
  m_mainGridLayout->setRowStretch(lastRow + 1, 1);
}

void PeopleDefinitionInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void PeopleDefinitionInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto peopleDefinition = modelObject.cast<model::PeopleDefinition>();
  attach(peopleDefinition);
  refresh();
}

void PeopleDefinitionInspectorView::onUpdate() {
  refresh();
}

void PeopleDefinitionInspectorView::attach(const openstudio::model::PeopleDefinition& peopleDefinition) {
  m_peopleDefinition = peopleDefinition;
  m_nameEdit->bind(*m_peopleDefinition, OptionalStringGetter(std::bind(&model::PeopleDefinition::name, m_peopleDefinition.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::PeopleDefinition::setName, m_peopleDefinition.get_ptr(), std::placeholders::_1)));

  // bind to PeopleDefinition methods
  m_numberofPeopleEdit->bind(
    *m_peopleDefinition, OptionalDoubleGetter(std::bind(&model::PeopleDefinition::numberofPeople, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::PeopleDefinition::setNumberofPeople, m_peopleDefinition.get_ptr(), std::placeholders::_1)));

  m_peopleperSpaceFloorAreaEdit->bind(
    m_isIP, *m_peopleDefinition, OptionalDoubleGetter(std::bind(&model::PeopleDefinition::peopleperSpaceFloorArea, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::PeopleDefinition::setPeopleperSpaceFloorArea, m_peopleDefinition.get_ptr(), std::placeholders::_1)));

  m_spaceFloorAreaperPersonEdit->bind(
    m_isIP, *m_peopleDefinition, OptionalDoubleGetter(std::bind(&model::PeopleDefinition::spaceFloorAreaperPerson, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::PeopleDefinition::setSpaceFloorAreaperPerson, m_peopleDefinition.get_ptr(), std::placeholders::_1)));

  // ETH: Note that this is overkill for this dimensionless value. Should switch to OSDoubleEdit(2).
  m_fractionRadiantEdit->bind(
    *m_peopleDefinition, DoubleGetter(std::bind(&model::PeopleDefinition::fractionRadiant, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::PeopleDefinition::setFractionRadiant, m_peopleDefinition.get_ptr(), std::placeholders::_1)));

  m_sensibleHeatFractionEdit->bind(
    *m_peopleDefinition, OptionalDoubleGetter(std::bind(&model::PeopleDefinition::sensibleHeatFraction, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(std::bind(&model::PeopleDefinition::setSensibleHeatFraction, m_peopleDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::PeopleDefinition::resetSensibleHeatFraction, m_peopleDefinition.get_ptr())), boost::none,
    boost::optional<NoFailAction>(std::bind(&model::PeopleDefinition::autocalculateSensibleHeatFraction, m_peopleDefinition.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::PeopleDefinition::isSensibleHeatFractionDefaulted, m_peopleDefinition.get_ptr())), boost::none,
    boost::optional<BasicQuery>(std::bind(&model::PeopleDefinition::isSensibleHeatFractionAutocalculated, m_peopleDefinition.get_ptr())));

  m_carbonDioxideGenerationRateEdit->bind(
    m_isIP, *m_peopleDefinition, DoubleGetter(std::bind(&model::PeopleDefinition::carbonDioxideGenerationRate, m_peopleDefinition.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::PeopleDefinition::setCarbonDioxideGenerationRate, m_peopleDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::PeopleDefinition::resetCarbonDioxideGenerationRate, m_peopleDefinition.get_ptr())), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::PeopleDefinition::isCarbonDioxideGenerationRateDefaulted, m_peopleDefinition.get_ptr())));

  m_enableASHRAE55ComfortWarningsSwitch->bind(
    *m_peopleDefinition, std::bind(&model::PeopleDefinition::enableASHRAE55ComfortWarnings, m_peopleDefinition.get_ptr()),
    BoolSetter(std::bind(&model::PeopleDefinition::setEnableASHRAE55ComfortWarnings, m_peopleDefinition.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::PeopleDefinition::resetEnableASHRAE55ComfortWarnings, m_peopleDefinition.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::PeopleDefinition::isEnableASHRAE55ComfortWarningsDefaulted, m_peopleDefinition.get_ptr())));

  m_meanRadiantTemperatureCalculationTypeComboBox->bind<std::string>(
    *m_peopleDefinition, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    &model::PeopleDefinition::meanRadiantTemperatureCalculationTypeValues,
    StringGetter(std::bind(&model::PeopleDefinition::meanRadiantTemperatureCalculationType, m_peopleDefinition.get_ptr())),
    std::bind(&model::PeopleDefinition::setMeanRadiantTemperatureCalculationType, m_peopleDefinition.get_ptr(), std::placeholders::_1),
    boost::optional<NoFailAction>(std::bind(&model::PeopleDefinition::resetMeanRadiantTemperatureCalculationType, m_peopleDefinition.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::PeopleDefinition::isMeanRadiantTemperatureCalculationTypeDefaulted, m_peopleDefinition.get_ptr())));

  // Extensible groups
  for (const IdfExtensibleGroup& i_eg : m_peopleDefinition->extensibleGroups()) {
    addThermalComfortModelTypeComboBox(i_eg.groupIndex());
  }

  checkButtons();

  this->stackedWidget()->setCurrentIndex(1);
}

OSComboBox2* PeopleDefinitionInspectorView::addThermalComfortModelTypeComboBox(int groupIndex) {

  // Increment last row
  ++lastRow;

  // Put it in a widget so we can delete and hide
  lastRowWidget = new QWidget();
  m_rowWidgets.push_back(lastRowWidget);
  lastHBoxLayout = new QHBoxLayout(lastRowWidget);
  m_HBoxLayouts.push_back(lastHBoxLayout);
  // groupIndex is 0-indexed
  auto* label = new QLabel("Thermal Comfort Model Type " + QString::number(groupIndex + 1));
  label->setObjectName("H2");
  lastHBoxLayout->addWidget(label);

  auto* thermalComfortModelTypeComboBox = new OSComboBox2();

  for (const std::string& val : model::PeopleDefinition::thermalComfortModelTypeValues()) {
    thermalComfortModelTypeComboBox->addItem(QString::fromStdString(val));
  }
  //thermalComfortModelTypeComboBox->addItem("Fanger");
  //thermalComfortModelTypeComboBox->addItem("Pierce");
  //thermalComfortModelTypeComboBox->addItem("KSU");
  //thermalComfortModelTypeComboBox->addItem("AdaptiveASH55");
  //thermalComfortModelTypeComboBox->addItem("AdaptiveCEN15251");

  lastHBoxLayout->addWidget(thermalComfortModelTypeComboBox);
  // Spans 1 row and 3 columns
  m_mainGridLayout->addWidget(lastRowWidget, lastRow, 0, 1, 3, Qt::AlignTop | Qt::AlignLeft);

  thermalComfortModelTypeComboBox->bind<std::string>(
    *m_peopleDefinition,
    static_cast<std::string (*)(const std::string&)>(&openstudio::toString),  // toString
    &model::PeopleDefinition::thermalComfortModelTypeValues,                  // choices: This returns an empty array...
    // getter, passing groupIndex as 1st and only parameter
    OptionalStringGetter(std::bind(&model::PeopleDefinition::getThermalComfortModelType, m_peopleDefinition.get_ptr(), groupIndex)),
    // setter, passing groupIndex as first param, then the placeholder (string)
    std::bind(&model::PeopleDefinition::setThermalComfortModelType, m_peopleDefinition.get_ptr(), groupIndex,
              std::placeholders::_1));  // No reset nor isDefaulted

  // Store a ref for unbind in detach
  m_thermalComfortModelTypeComboBoxes.push_back(thermalComfortModelTypeComboBox);

  // Adjust stretch
  adjustRowStretch();

  return thermalComfortModelTypeComboBox;
}

void PeopleDefinitionInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_numberofPeopleEdit->unbind();
  m_peopleperSpaceFloorAreaEdit->unbind();
  m_spaceFloorAreaperPersonEdit->unbind();
  m_fractionRadiantEdit->unbind();
  m_sensibleHeatFractionEdit->unbind();
  m_carbonDioxideGenerationRateEdit->unbind();
  m_enableASHRAE55ComfortWarningsSwitch->unbind();
  m_meanRadiantTemperatureCalculationTypeComboBox->unbind();

  for (OSComboBox2* thermalComfortModelTypeComboBox : m_thermalComfortModelTypeComboBoxes) {
    thermalComfortModelTypeComboBox->unbind();
  }

  m_peopleDefinition.reset();
}

void PeopleDefinitionInspectorView::refresh() {}

void PeopleDefinitionInspectorView::toggleUnits(bool displayIP) {}

void PeopleDefinitionInspectorView::checkButtons() {
  if (m_peopleDefinition) {
    unsigned int numFields = m_peopleDefinition->numFields();

    unsigned int numNonEx = m_peopleDefinition->numNonextensibleFields();
    if (numFields <= numNonEx) {
      removeBtn->setEnabled(false);
    } else {
      removeBtn->setEnabled(true);
    }

    unsigned int numMax = m_peopleDefinition->maxFields().get();
    if (numFields == numMax) {
      addBtn->setEnabled(false);
    } else {
      addBtn->setEnabled(true);
    }
  }
}

void PeopleDefinitionInspectorView::addExtensible() {
  IdfExtensibleGroup eg = m_peopleDefinition->pushExtensibleGroup();
  // Default to Fanger so we have a value...
  // eg.setString(0, "Fanger");
  addThermalComfortModelTypeComboBox(eg.groupIndex());

  checkButtons();
}

void PeopleDefinitionInspectorView::removeExtensible() {
  //LOG(Debug, "Start of removeExtensible, size of m_thermalComfortModelTypeComboBoxes=" << m_thermalComfortModelTypeComboBoxes.size()
  //    << ", m_HBoxLayouts=" << m_HBoxLayouts.size());

  OSComboBox2* thermalComfortModelTypeComboBox = m_thermalComfortModelTypeComboBoxes.back();
  thermalComfortModelTypeComboBox->unbind();
  m_thermalComfortModelTypeComboBoxes.pop_back();

  m_peopleDefinition->popExtensibleGroup();

  // LOG(Debug, "removeExtensible, rowCount=" << m_mainGridLayout->rowCount() << ", lastRow=" << lastRow);

  // true
  // LOG(Debug, "Pointing to same? " << std::boolalpha << (m_HBoxLayouts.back() == lastHBoxLayout));

  // LOG(Debug, "m_rowWidgets.back() == lastRowWidget : " << std::boolalpha << (m_rowWidgets.back() == lastRowWidget));
  // LOG(Debug, "lastHBoxLayout->widget() == lastRowWidget : " << std::boolalpha << (lastHBoxLayout->widget() == lastRowWidget));
  // LOG(Debug, "m_mainGridLayout->itemAtPosition(lastRow, 0)->widget() == lastRowWidget: " << std::boolalpha << (m_mainGridLayout->itemAtPosition(lastRow, 0)->widget() == lastRowWidget));
  // Output:
  // lastHBoxLayout->widget() == lastRowWidget : false
  // m_mainGridLayout->itemAtPosition(lastRow, 0)->widget() == lastRowWidget: true
  // m_rowWidgets.back() == lastRowWidget : true

  m_mainGridLayout->removeWidget(m_mainGridLayout->itemAtPosition(lastRow, 0)->widget());

  lastRowWidget->setVisible(false);

  // Then delete
  QLayoutItem* child;
  while ((child = lastHBoxLayout->takeAt(0)) != 0) {
    delete child;
  }
  delete lastHBoxLayout;
  delete lastRowWidget;

  // Decrement last row used
  --lastRow;

  // Remove from vector and Set pointer to new last
  m_rowWidgets.pop_back();
  if (m_rowWidgets.empty()) {
    lastRowWidget = nullptr;
  } else {
    lastRowWidget = m_rowWidgets.back();
  }

  m_HBoxLayouts.pop_back();
  if (m_HBoxLayouts.empty()) {
    lastHBoxLayout = nullptr;
  } else {
    lastHBoxLayout = m_HBoxLayouts.back();
  }

  checkButtons();
}

}  // namespace openstudio
