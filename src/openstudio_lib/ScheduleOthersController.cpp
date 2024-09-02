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

#include "ScheduleOthersController.hpp"
#include "ScheduleOthersView.hpp"

#include <openstudio/model/ScheduleConstant.hpp>
#include <openstudio/model/ScheduleConstant_Impl.hpp>
#include <openstudio/model/ExternalFile.hpp>
#include <openstudio/model/ExternalFile_Impl.hpp>
#include <openstudio/model/ScheduleFile.hpp>

#include <openstudio/utilities/core/Logger.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

namespace openstudio {

ScheduleOthersController::ScheduleOthersController(const model::Model& model) : ModelSubTabController(new ScheduleOthersView(model), model) {}

void ScheduleOthersController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  switch (iddObjectType.value()) {
    case IddObjectType::OS_Schedule_Constant: {
      openstudio::model::ScheduleConstant(this->model());
      break;
    }
    case IddObjectType::OS_Schedule_Compact: {
      QMessageBox message(subTabView());
      message.setText("Creation of Schedule:Compact is not supported, you should use a ScheduleRuleset instead");
      message.exec();
      return;
    }
    case IddObjectType::OS_Schedule_File: {
      QString selfilter = tr("CSV Files(*.csv)");
      QString fileName = QFileDialog::getOpenFileName(this->subTabView(), tr("Select External File"), QDir::homePath(),
                                                      tr("All files (*.*);;CSV Files(*.csv);;TSV Files(*.tsv)"), &selfilter);
      if (!fileName.isNull()) {
        boost::optional<model::ExternalFile> e_ = model::ExternalFile::getExternalFile(this->model(), fileName.toStdString());
        if (e_) {
          model::ScheduleFile(*e_, 1, 1);
        }
      }
      break;
    }
    default:
      LOG_FREE_AND_THROW("ScheduleOthersController", "Unknown IddObjectType '" << iddObjectType.valueName() << "'");
  }
}

void ScheduleOthersController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->model());
}

void ScheduleOthersController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void ScheduleOthersController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void ScheduleOthersController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  this->model().purgeUnusedResourceObjects(iddObjectType);
}

void ScheduleOthersController::onDrop(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::ScheduleConstant>()) {
      if (this->fromComponentLibrary(itemId)) {
        modelObject->clone(this->model());
      }
    }
  }
}

void ScheduleOthersController::onInspectItem(OSItem* item) {}

}  // namespace openstudio
