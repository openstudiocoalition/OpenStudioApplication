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

#include "ScheduleCompactInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSDoubleEdit.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/ScheduleCompact.hpp>
#include <openstudio/model/ScheduleCompact_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QPlainTextEdit>

#include <sstream>

namespace openstudio {

// ScheduleCompactInspectorView

ScheduleCompactInspectorView::ScheduleCompactInspectorView(const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  createLayout();
}

void ScheduleCompactInspectorView::createLayout() {
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
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 1);

  ++row;

  // Value

  label = new QLabel("Content: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row++, 0);

  m_content = new QPlainTextEdit();
  m_content->setReadOnly(true);
  QFont f("monospace");
  f.setStyleHint(QFont::Monospace);
  m_content->setFont(f);

  mainGridLayout->addWidget(m_content, row++, 0, 1, 2);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);
  mainGridLayout->setColumnStretch(0, 3);
  mainGridLayout->setColumnStretch(1, 1);
  mainGridLayout->setColumnStretch(2, 1);
}

void ScheduleCompactInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void ScheduleCompactInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto sch = modelObject.cast<model::ScheduleCompact>();
  attach(sch);
  refresh();
}

void ScheduleCompactInspectorView::onUpdate() {
  refresh();
}

void ScheduleCompactInspectorView::attach(openstudio::model::ScheduleCompact& sch) {
  m_sch = sch;

  m_nameEdit->bind(
    *m_sch, OptionalStringGetter(std::bind(&model::ScheduleCompact::name, m_sch.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::ScheduleCompact::setName, m_sch.get_ptr(), std::placeholders::_1)));

  std::stringstream ss;
  sch.print(ss);

  m_content->setPlainText(toQString(ss.str()));

  this->stackedWidget()->setCurrentIndex(1);
}

void ScheduleCompactInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();

  m_sch = boost::none;
}

void ScheduleCompactInspectorView::refresh() {}

}  // namespace openstudio

