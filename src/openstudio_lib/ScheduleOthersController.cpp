/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
