/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectListView.hpp"
#include "ModelObjectItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "BCLComponentItem.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/ZoneHVACComponent.hpp>
#include <openstudio/model/ZoneHVACComponent_Impl.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/UtilityBill.hpp>
#include <openstudio/model/UtilityBill_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <iostream>
#include <QMutex>
#include <QTimer>

namespace openstudio {

ModelObjectListController::ModelObjectListController(const openstudio::IddObjectType& iddObjectType, const model::Model& model, bool isLibrary)
  : m_iddObjectType(iddObjectType), m_model(model), m_isLibrary(isLibrary), m_reportScheduled(false), m_reportItemsMutex(new QMutex()) {

  if (!m_isLibrary) {

    // model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<ModelObjectListController, &ModelObjectListController::objectAdded>(this);
    connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectListController::objectAdded, Qt::QueuedConnection);

    //model.getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.connect<ModelObjectListController, &ModelObjectListController::objectRemoved>(this);
    connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectListController::objectRemoved, Qt::QueuedConnection);
  }
}

ModelObjectListController::~ModelObjectListController() {
  delete m_reportItemsMutex;
}

IddObjectType ModelObjectListController::iddObjectType() const {
  return m_iddObjectType;
}

void ModelObjectListController::objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                            const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  if (iddObjectType == m_iddObjectType) {
    m_reportItemsMutex->lock();

    m_selectedHandle = handle;
    if (!m_reportScheduled) {
      m_reportScheduled = true;
      QTimer::singleShot(0, this, &ModelObjectListController::reportItemsImpl);
    }

    m_reportItemsMutex->unlock();
  }
}

void ModelObjectListController::objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                              const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  if (iddObjectType == m_iddObjectType) {
    m_reportItemsMutex->lock();

    if (!m_reportScheduled) {
      m_reportScheduled = true;
      QTimer::singleShot(0, this, &ModelObjectListController::reportItemsImpl);
    }

    m_reportItemsMutex->unlock();
  }
}

void ModelObjectListController::reportItemsImpl() {
  m_reportItemsMutex->lock();

  if (m_reportScheduled) {
    m_reportScheduled = false;
    std::vector<OSItemId> ids = this->makeVector();
    emit itemIds(ids);

    if (m_selectedHandle) {
      for (const OSItemId& id : ids) {
        if (id.itemId() == toQString(*m_selectedHandle)) {
          emit selectedItemId(id);
          break;
        }
      }
      m_selectedHandle.reset();
    }
  }

  m_reportItemsMutex->unlock();
}

std::vector<OSItemId> ModelObjectListController::makeVector() {
  std::vector<OSItemId> result;

  if (m_isLibrary) {
    std::vector<std::pair<std::string, std::string>> pairs;
    pairs.push_back(std::make_pair<std::string, std::string>("OpenStudio Type", m_iddObjectType.valueDescription()));

    // get BCL results
    std::vector<BCLComponent> bclresults = OSAppBase::instance()->currentDocument()->componentAttributeSearch(pairs);

    // sort by name
    std::sort(bclresults.begin(), bclresults.end(), BCLComponentNameGreater());

    for (auto it = bclresults.begin(); it != bclresults.end(); ++it) {
      result.push_back(bclComponentToItemId(*it));
    }
  }

  // get objects by type
  std::vector<WorkspaceObject> workspaceObjects = m_model.getObjectsByType(m_iddObjectType);

  // sort by name
  std::sort(workspaceObjects.begin(), workspaceObjects.end(), WorkspaceObjectNameGreater());

  for (const WorkspaceObject& workspaceObject : workspaceObjects) {
    if (!workspaceObject.handle().isNull()) {
      auto modelObject = workspaceObject.cast<openstudio::model::ModelObject>();
      if (boost::optional<model::HVACComponent> hvacComponent = modelObject.optionalCast<model::HVACComponent>()) {
        if ((!hvacComponent->containingHVACComponent()) && (!hvacComponent->containingZoneHVACComponent())) {
          result.push_back(modelObjectToItemId(hvacComponent.get(), false));
        }
        // Special case when there is a containingZoneHVACComponent, it might be a tank for a HPWH that we DO want to be able
        // to drag and drop...
        else if (boost::optional<openstudio::model::ZoneHVACComponent> zComp = hvacComponent->containingZoneHVACComponent()) {

          openstudio::IddObjectType zCompType = zComp->iddObjectType();

          // Special case for a WaterHeaterMixed, can be part of a HeatPump(PumpedCondenser) or HeatPump:WrappedCondenser
          if ((m_iddObjectType == openstudio::IddObjectType::OS_WaterHeater_Stratified)
              && ((zCompType == openstudio::IddObjectType::OS_WaterHeater_HeatPump)
                  || (zCompType == openstudio::IddObjectType::OS_WaterHeater_HeatPump_WrappedCondenser))) {
            result.push_back(modelObjectToItemId(hvacComponent.get(), false));
          }
          // Special case for a WaterHeaterMixed, can be part of a HeatPump(PumpedCondenser) only
          else if ((m_iddObjectType == openstudio::IddObjectType::OS_WaterHeater_Mixed)
                   && (zCompType == openstudio::IddObjectType::OS_WaterHeater_HeatPump)) {
            result.push_back(modelObjectToItemId(hvacComponent.get(), false));
          }
        }

      } else {
        result.push_back(modelObjectToItemId(modelObject, false));
      }
    }
  }

  return result;
}

ModelObjectListView::ModelObjectListView(const openstudio::IddObjectType& iddObjectType, const model::Model& model, bool addScrollArea,
                                         bool isLibrary, QWidget* parent)
  : OSItemList(new ModelObjectListController(iddObjectType, model, isLibrary), addScrollArea, parent) {}

boost::optional<openstudio::model::ModelObject> ModelObjectListView::selectedModelObject() const {
  OSItem* selectedItem = this->selectedItem();
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(selectedItem);
  if (modelObjectItem) {
    return modelObjectItem->modelObject();
  }
  return boost::none;
}

IddObjectType ModelObjectListView::iddObjectType() const {
  OSVectorController* vectorController = this->vectorController();
  auto* modelObjectListController = qobject_cast<ModelObjectListController*>(vectorController);
  OS_ASSERT(modelObjectListController);
  return modelObjectListController->iddObjectType();
}

}  // namespace openstudio
