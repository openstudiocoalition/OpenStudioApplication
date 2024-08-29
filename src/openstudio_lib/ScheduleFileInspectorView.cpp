/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "ScheduleFileInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSIntegerEdit.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSSwitch.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ScheduleFile.hpp>
#include <openstudio/model/ScheduleFile_Impl.hpp>
#include <openstudio/model/ExternalFile.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QLineEdit>

namespace openstudio {

// ScheduleFileInspectorView

ScheduleFileInspectorView::ScheduleFileInspectorView(const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  createLayout();
}

void ScheduleFileInspectorView::createLayout() {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();

  QLabel* label = nullptr;

  // Name

  label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_nameEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 3);

  // FilePath
  ++row;

  label = new QLabel("FilePath: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_filePath = new QLineEdit();
  m_filePath->setReadOnly(true);
  mainGridLayout->addWidget(m_filePath, row, 0, 1, 3);

  ++row;

  QVBoxLayout* vLayout = nullptr;

  // Column Number
  vLayout = new QVBoxLayout();

  label = new QLabel("Column Number: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_columnNumber = new OSIntegerEdit2();
  vLayout->addWidget(m_columnNumber);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Rows to Skip
  vLayout = new QVBoxLayout();

  label = new QLabel("Rows to Skip at Top: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_rowstoSkipatTop = new OSIntegerEdit2();
  vLayout->addWidget(m_rowstoSkipatTop);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Number of Hours of Data
  vLayout = new QVBoxLayout();

  label = new QLabel("Number of Hours of Data: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_numberofHoursofData = new OSIntegerEdit2();
  vLayout->addWidget(m_numberofHoursofData);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Column Separator
  vLayout = new QVBoxLayout();

  label = new QLabel("Column Separator: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_columnSeparator = new OSComboBox2();
  m_columnSeparator->addItem("Comma");
  m_columnSeparator->addItem("Tab");
  m_columnSeparator->addItem("Fixed");
  m_columnSeparator->addItem("Space");
  m_columnSeparator->addItem("Semicolon");
  vLayout->addWidget(m_columnSeparator);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Interpolate
  vLayout = new QVBoxLayout();

  label = new QLabel("Interpolate to Timestep: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_interpolatetoTimestep = new OSSwitch2();
  vLayout->addWidget(m_interpolatetoTimestep);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Minutes per Item
  vLayout = new QVBoxLayout();

  label = new QLabel("Minutes per Item: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_minutesperItem = new OSComboBox2();
  for (const auto& val : model::ScheduleFile::minutesperItemValues()) {
    m_minutesperItem->addItem(QString::fromStdString(val));
  }
  vLayout->addWidget(m_minutesperItem);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Adjust Schedule for Daylight Savings
  vLayout = new QVBoxLayout();

  label = new QLabel("Adjust Schedule for Daylight Savings: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_adjustScheduleforDaylightSavings = new OSSwitch2();
  vLayout->addWidget(m_adjustScheduleforDaylightSavings);

  mainGridLayout->addLayout(vLayout, row, 0);

  // Translate File With Relative Path
  vLayout = new QVBoxLayout();

  label = new QLabel("Translate File With Relative Path: ");
  label->setObjectName("H2");
  vLayout->addWidget(label);

  m_translateFileWithRelativePath = new OSSwitch2();
  vLayout->addWidget(m_translateFileWithRelativePath);

  mainGridLayout->addLayout(vLayout, row, 1);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void ScheduleFileInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void ScheduleFileInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto sch = modelObject.cast<model::ScheduleFile>();
  attach(sch);
  refresh();
}

void ScheduleFileInspectorView::onUpdate() {
  refresh();
}

void ScheduleFileInspectorView::attach(openstudio::model::ScheduleFile& sch) {
  m_sch = sch;

  // m_nameEdit->bind(sch,"name");
  m_nameEdit->bind(
    *m_sch, OptionalStringGetter(std::bind(&model::ScheduleFile::name, m_sch.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::ScheduleFile::setName, m_sch.get_ptr(), std::placeholders::_1)));

  // OSIntegerEdit2
  m_columnNumber->bind(*m_sch, IntGetter(std::bind(&model::ScheduleFile::columnNumber, m_sch.get_ptr())),
                       boost::optional<IntSetter>(std::bind(&model::ScheduleFile::setColumnNumber, m_sch.get_ptr(), std::placeholders::_1)));

  m_rowstoSkipatTop->bind(*m_sch, IntGetter(std::bind(&model::ScheduleFile::rowstoSkipatTop, m_sch.get_ptr())),
                          boost::optional<IntSetter>(std::bind(&model::ScheduleFile::setRowstoSkipatTop, m_sch.get_ptr(), std::placeholders::_1)));

  m_numberofHoursofData->bind(
    *m_sch, OptionalIntGetter(std::bind(&model::ScheduleFile::numberofHoursofData, m_sch.get_ptr())),
    boost::optional<IntSetter>(std::bind(&model::ScheduleFile::setNumberofHoursofData, m_sch.get_ptr(), std::placeholders::_1)));

  // OSComboBox2

  m_columnSeparator->bind<std::string>(
    *m_sch, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    // ScheduleFile::columnSeparatorValues does not exist: https://github.com/NREL/OpenStudio/issues/5246
    []() {
      return std::vector<std::string>{"Comma", "Tab", "Fixed", "Space", "Semicolon"};
    },
    std::bind(&model::ScheduleFile::columnSeparator, m_sch.get_ptr()),
    std::bind(&model::ScheduleFile::setColumnSeparator, m_sch.get_ptr(), std::placeholders::_1), boost::none, boost::none);

  /*
  m_minutesperItem->bind<std::string>(*m_sch, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
                                      &model::ScheduleFile::minutesperItemValues,
                                      OptionalStringGetter(std::bind(&model::ScheduleFile::minutesperItem, m_sch.get_ptr())),
                                      std::bind(&model::ScheduleFile::setColumnSeparator, m_sch.get_ptr(), std::placeholders::_1),
                                      boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetMinutesperItem, m_sch.get_ptr())),
                                      boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isMinutesperItemDefaulted, m_sch.get_ptr()))

  );
  */

  // OSSwitch2

  m_interpolatetoTimestep->bind(
    *m_sch, std::bind(&model::ScheduleFile::interpolatetoTimestep, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setInterpolatetoTimestep, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetInterpolatetoTimestep, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isInterpolatetoTimestepDefaulted, m_sch.get_ptr())));

  m_adjustScheduleforDaylightSavings->bind(
    *m_sch, std::bind(&model::ScheduleFile::adjustScheduleforDaylightSavings, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setAdjustScheduleforDaylightSavings, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetAdjustScheduleforDaylightSavings, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isAdjustScheduleforDaylightSavingsDefaulted, m_sch.get_ptr())));

  m_translateFileWithRelativePath->bind(
    *m_sch, std::bind(&model::ScheduleFile::translateFileWithRelativePath, m_sch.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::ScheduleFile::setTranslateFileWithRelativePath, m_sch.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ScheduleFile::resetTranslateFileWithRelativePath, m_sch.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::ScheduleFile::isTranslateFileWithRelativePathDefaulted, m_sch.get_ptr())));

  // QLineEdit
  m_filePath->setText(toQString(m_sch->externalFile().fileName()));

  this->stackedWidget()->setCurrentIndex(1);
}

void ScheduleFileInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_columnNumber->unbind();
  m_rowstoSkipatTop->unbind();
  m_numberofHoursofData->unbind();
  m_columnSeparator->unbind();
  m_interpolatetoTimestep->unbind();
  // m_minutesperItem->unbind();
  m_adjustScheduleforDaylightSavings->unbind();
  m_translateFileWithRelativePath->unbind();

  m_sch = boost::none;
}

void ScheduleFileInspectorView::refresh() {}

}  // namespace openstudio

