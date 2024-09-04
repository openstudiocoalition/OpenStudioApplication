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

#include "ScheduleOthersView.hpp"
#include "ModelObjectTypeListView.hpp"

#include "ScheduleConstantInspectorView.hpp"
#include "ScheduleCompactInspectorView.hpp"
#include "ScheduleFileInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QStackedWidget>

namespace openstudio {

ScheduleOthersView::ScheduleOthersView(const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(
    new ModelObjectTypeListView(ScheduleOthersView::modelObjectTypesAndNames(), model, true, OSItemType::CollapsibleListHeader, false, parent),
    new ScheduleOthersInspectorView(model, parent), false, parent) {}

std::vector<std::pair<IddObjectType, std::string>> ScheduleOthersView::modelObjectTypesAndNames() {
  return {{
    {IddObjectType::OS_Schedule_Constant, "Schedule Constant"},
    {IddObjectType::OS_Schedule_Compact, "Schedule Compact"},
    {IddObjectType::OS_Schedule_File, "Schedule File"},
  }};
}

ScheduleOthersInspectorView::ScheduleOthersInspectorView(const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, false, parent) {}

void ScheduleOthersInspectorView::onClearSelection() {
  QWidget* widget = this->stackedWidget()->currentWidget();
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  if (modelObjectInspectorView) {
    modelObjectInspectorView->clearSelection();
  }

  this->stackedWidget()->setCurrentIndex(0);
}

void ScheduleOthersInspectorView::onUpdate() {}

void ScheduleOthersInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_Schedule_Constant:
      this->showScheduleConstantView(modelObject);
      break;
    case IddObjectType::OS_Schedule_Compact:
      this->showScheduleCompactView(modelObject);
      break;
    case IddObjectType::OS_Schedule_File:
      this->showScheduleFileView(modelObject);
      break;
    default:
      showDefaultView();
  }
}

void ScheduleOthersInspectorView::showScheduleConstantView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleConstantInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showScheduleCompactView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleCompactInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showScheduleFileView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new ScheduleFileInspectorView(m_model);
  view->selectModelObject(modelObject);
  this->showInspector(view);
}

void ScheduleOthersInspectorView::showInspector(QWidget* widget) {
  if (QWidget* _widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(_widget);

    delete _widget;
  }

  this->stackedWidget()->addWidget(widget);
}

void ScheduleOthersInspectorView::showDefaultView() {
  if (QWidget* widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(widget);

    delete widget;
  }
}

}  // namespace openstudio