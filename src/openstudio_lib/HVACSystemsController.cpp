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

#include "HVACSystemsController.hpp"
#include "RefrigerationController.hpp"
#include "RefrigerationGraphicsItems.hpp"
#include "RefrigerationGridController.hpp"
#include "RefrigerationGridView.hpp"
#include "VRFController.hpp"
#include "VRFGraphicsItems.hpp"
#include "LoopLibraryDialog.hpp"
#include "HVACSystemsView.hpp"
#include "LoopScene.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "RefrigerationScene.hpp"
#include "../shared_gui_components/OSSwitch.hpp"
#include "ServiceWaterScene.hpp"
#include "HorizontalTabWidget.hpp"
#include "MainRightColumnController.hpp"
#include "../shared_gui_components/OSViewSwitcher.hpp"
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/WaterToAirComponent.hpp>
#include <openstudio/model/WaterToAirComponent_Impl.hpp>
#include <openstudio/model/WaterToWaterComponent.hpp>
#include <openstudio/model/WaterToWaterComponent_Impl.hpp>
#include <openstudio/model/WaterUseConnections.hpp>
#include <openstudio/model/WaterUseConnections_Impl.hpp>
#include <openstudio/model/WaterUseEquipment.hpp>
#include <openstudio/model/WaterUseEquipment_Impl.hpp>
#include <openstudio/model/WaterUseEquipmentDefinition.hpp>
#include <openstudio/model/WaterUseEquipmentDefinition_Impl.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem_Impl.hpp>
#include <openstudio/model/AirLoopHVACUnitaryHeatPumpAirToAir.hpp>
#include <openstudio/model/AirLoopHVACUnitaryHeatPumpAirToAir_Impl.hpp>
#include <openstudio/model/AvailabilityManager.hpp>
#include <openstudio/model/AvailabilityManager_Impl.hpp>
#include <openstudio/model/AvailabilityManagerAssignmentList.hpp>
#include <openstudio/model/AvailabilityManagerAssignmentList_Impl.hpp>

#include <openstudio/model/ChillerElectricEIR.hpp>
#include <openstudio/model/BoilerHotWater.hpp>

#include <openstudio/model/ControllerMechanicalVentilation.hpp>
#include <openstudio/model/ControllerMechanicalVentilation_Impl.hpp>
#include <openstudio/model/ControllerOutdoorAir.hpp>
#include <openstudio/model/ControllerOutdoorAir_Impl.hpp>
#include <openstudio/model/SetpointManager.hpp>
#include <openstudio/model/SetpointManagerSingleZoneReheat.hpp>
#include <openstudio/model/SetpointManagerSingleZoneReheat_Impl.hpp>
#include <openstudio/model/SetpointManagerScheduled.hpp>
#include <openstudio/model/SetpointManagerScheduled_Impl.hpp>
#include <openstudio/model/SetpointManagerFollowOutdoorAirTemperature.hpp>
#include <openstudio/model/SetpointManagerFollowOutdoorAirTemperature_Impl.hpp>
#include <openstudio/model/SetpointManagerOutdoorAirReset.hpp>
#include <openstudio/model/SetpointManagerOutdoorAirReset_Impl.hpp>
#include <openstudio/model/CoilCoolingDXSingleSpeed.hpp>
#include <openstudio/model/CoilCoolingDXSingleSpeed_Impl.hpp>
#include <openstudio/model/CoilCoolingDXTwoSpeed.hpp>
#include <openstudio/model/CoilCoolingDXTwoSpeed_Impl.hpp>
#include <openstudio/model/CoilCoolingWater.hpp>
#include <openstudio/model/CoilCoolingWater_Impl.hpp>
#include <openstudio/model/CoilHeatingGas.hpp>
#include <openstudio/model/CoilHeatingGas_Impl.hpp>
#include <openstudio/model/CoilHeatingElectric.hpp>
#include <openstudio/model/CoilHeatingElectric_Impl.hpp>
#include <openstudio/model/CoilHeatingWater.hpp>
#include <openstudio/model/CoilHeatingWater_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Node_Impl.hpp>
#include <openstudio/model/Loop.hpp>
#include <openstudio/model/Loop_Impl.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Splitter_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/Mixer.hpp>
#include <openstudio/model/Mixer_Impl.hpp>
#include <openstudio/model/PlantLoop.hpp>
#include <openstudio/model/PlantLoop_Impl.hpp>
#include <openstudio/model/AirLoopHVAC.hpp>
#include <openstudio/model/AirLoopHVAC_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/HVACTemplates.hpp>
#include <openstudio/model/Component.hpp>
#include <openstudio/model/Component_Impl.hpp>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/ComponentData_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/energyplus/ForwardTranslator/ForwardTranslatePlantEquipmentOperationSchemes.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/OS_ComponentData_FieldEnums.hxx>

#include <QMessageBox>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QMutex>

// Switch log Level in one go
#define LOGLEVEL Trace

namespace openstudio {

const QString SHW = "SHW";
const QString REFRIGERATION = "REFRIGERATION";
const QString VRF = "VRF";

HVACSystemsController::HVACSystemsController(bool isIP, const model::Model& model)
  : m_hvacSystemsView(new HVACSystemsView()), m_updateMutex(new QMutex()), m_model(model), m_isIP(isIP) {

  m_hvacLayoutController = std::make_shared<HVACLayoutController>(this);

  m_hvacControlsController = std::make_shared<HVACControlsController>(this);

  m_model.getImpl<model::detail::Model_Impl>()->addWorkspaceObject.connect<HVACSystemsController, &HVACSystemsController::onObjectAdded>(this);
  //connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAdded, this, &HVACSystemsController::onObjectAdded, Qt::QueuedConnection);

  m_model.getImpl<model::detail::Model_Impl>()->removeWorkspaceObject.connect<HVACSystemsController, &HVACSystemsController::onObjectRemoved>(this);
  //connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemoved, this, &HVACSystemsController::onObjectRemoved, Qt::QueuedConnection);

  connect(m_hvacSystemsView->hvacToolbarView->addButton, &QPushButton::clicked, this, &HVACSystemsController::onAddSystemClicked);

  connect(m_hvacSystemsView->hvacToolbarView->copyButton, &QPushButton::clicked, this, &HVACSystemsController::onCopySystemClicked);

  connect(m_hvacSystemsView->hvacToolbarView->deleteButton, &QPushButton::clicked, this, &HVACSystemsController::onRemoveLoopClicked);

  connect(m_hvacSystemsView->hvacToolbarView->systemComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &HVACSystemsController::onSystemComboBoxIndexChanged);

  connect(m_hvacSystemsView->hvacToolbarView->topologyViewButton, &QPushButton::clicked, this, &HVACSystemsController::onShowTopologyClicked);

  connect(m_hvacSystemsView->hvacToolbarView->controlsViewButton, &QPushButton::clicked, this, &HVACSystemsController::onShowControlsClicked);

  connect(m_hvacSystemsView->hvacToolbarView->gridViewButton, &QPushButton::clicked, this, &HVACSystemsController::onShowGridClicked);

  // connect initial loops to trigger a refresh of the system combo box
  auto airloops = m_model.getModelObjects<model::AirLoopHVAC>();
  std::sort(airloops.begin(), airloops.end(), WorkspaceObjectNameLess());
  for (const auto& loop : airloops) {
    // Trigger a full refresh if the airLoop name changes
    LOG(LOGLEVEL, "HVACSystemsController Ctor: Attaching name change for AirLoopHVAC " << loop.nameString());
    loop.getImpl<detail::IdfObject_Impl>()
      ->detail::IdfObject_Impl::onNameChange.connect<HVACSystemsController, &HVACSystemsController::repopulateSystemComboBox>(this);
  }

  auto plantloops = m_model.getModelObjects<model::PlantLoop>();
  std::sort(plantloops.begin(), plantloops.end(), WorkspaceObjectNameLess());
  for (const auto& loop : plantloops) {
    LOG(LOGLEVEL, "HVACSystemsController Ctor: Attaching name change for PlantLoop " << loop.nameString());
    loop.getImpl<detail::IdfObject_Impl>()
      ->detail::IdfObject_Impl::onNameChange.connect<HVACSystemsController, &HVACSystemsController::repopulateSystemComboBox>(this);
  }

  updateLater();
}

HVACSystemsController::~HVACSystemsController() {
  delete m_hvacSystemsView;

  delete m_updateMutex;
}

void HVACSystemsController::clearSceneSelection() {
  if (m_hvacLayoutController) {
    m_hvacLayoutController->clearSceneSelection();
  }
}

QString HVACSystemsController::currentHandle() const {
  return m_currentHandle;
}

void HVACSystemsController::setCurrentHandle(const QString& handle) {
  m_currentHandle = handle;

  updateLater();
}

model::Model HVACSystemsController::model() const {
  return m_model;
}

void HVACSystemsController::repopulateSystemComboBox() {

  LOG(LOGLEVEL, "repopulateSystemComboBox() called");

  QComboBox* systemComboBox = m_hvacSystemsView->hvacToolbarView->systemComboBox;
  bool signalsAlreadyBlocked = systemComboBox->signalsBlocked();

  // If not already blocked, we do it
  if (!signalsAlreadyBlocked) {
    // we want to avoid onSystemComboBoxIndexChanged that triggers setCurrentHandle With triggers update
    // which will retrigger this very function...
    systemComboBox->blockSignals(true);
  }

  // Repopulate
  systemComboBox->clear();

  // Populate system combo box
  auto airloops = m_model.getModelObjects<model::AirLoopHVAC>();
  std::sort(airloops.begin(), airloops.end(), WorkspaceObjectNameLess());
  for (const auto& loop : airloops) {

    systemComboBox->addItem(QString::fromStdString(loop.nameString()), toQString(loop.handle()));
  }

  auto plantloops = m_model.getModelObjects<model::PlantLoop>();
  std::sort(plantloops.begin(), plantloops.end(), WorkspaceObjectNameLess());
  for (const auto& loop : plantloops) {
    systemComboBox->addItem(QString::fromStdString(loop.nameString()), toQString(loop.handle()));
  }

  // TODO: When addressing issue #961 - HVAC Toolbar review, that's where you start
  systemComboBox->addItem("Service Hot Water", SHW);
  systemComboBox->addItem("Refrigeration", REFRIGERATION);
  systemComboBox->addItem("VRF", VRF);

  // Set system combo box current index
  QString handle = currentHandle();
  if (handle == SHW || m_model.getModelObject<model::WaterUseConnections>(toUUID(handle))) {
    int index = systemComboBox->findData(SHW);

    OS_ASSERT(index >= 0);

    systemComboBox->setCurrentIndex(index);
  } else if (handle == REFRIGERATION) {
    int index = systemComboBox->findData(REFRIGERATION);

    OS_ASSERT(index >= 0);

    systemComboBox->setCurrentIndex(index);
  } else if (handle == VRF) {
    int index = systemComboBox->findData(VRF);

    OS_ASSERT(index >= 0);

    systemComboBox->setCurrentIndex(index);
  } else {
    int index = systemComboBox->findData(handle);

    if (index >= 0) {
      systemComboBox->setCurrentIndex(index);
    } else {
      systemComboBox->setCurrentIndex(systemComboBox->findData(SHW));
    }
  }

  // Release if needed
  if (!signalsAlreadyBlocked) {
    // Don't forget to renable both the combobox AND the HVACSystemsController!
    systemComboBox->blockSignals(false);
  }
}

void HVACSystemsController::update() {
  if (!m_updateMutex->tryLock()) {
    return;
  }

  if (m_dirty) {
    model::OptionalModelObject mo;
    OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(mo, false);

    // Remove old stuff

    QComboBox* systemComboBox = m_hvacSystemsView->hvacToolbarView->systemComboBox;

    LOG(LOGLEVEL, "update() called");
    systemComboBox->blockSignals(true);

    // Repopulate
    repopulateSystemComboBox();

    // Show layout
    QString handle = currentHandle();

    m_hvacSystemsView->hvacToolbarView->zoomInButton->show();
    m_hvacSystemsView->hvacToolbarView->zoomOutButton->show();

    m_hvacSystemsView->hvacToolbarView->addButton->show();
    m_hvacSystemsView->hvacToolbarView->copyButton->show();
    m_hvacSystemsView->hvacToolbarView->deleteButton->show();

    // Show Controls, to avoid still displaying the name of a previously selected "Water Use Connection" object for eg
    m_hvacSystemsView->hvacToolbarView->showControls(true);

    m_hvacLayoutController.reset();
    m_hvacControlsController.reset();
    m_refrigerationController.reset();
    m_vrfController.reset();

    OSAppBase::instance()->processEvents();

    if (handle == REFRIGERATION) {
      m_hvacSystemsView->hvacToolbarView->zoomInButton->setEnabled(false);
      m_hvacSystemsView->hvacToolbarView->zoomOutButton->setEnabled(false);

      if (m_hvacSystemsView->hvacToolbarView->topologyViewButton->isChecked()) {
        m_refrigerationController = std::make_shared<RefrigerationController>();

        m_hvacSystemsView->mainViewSwitcher->setView(m_refrigerationController->refrigerationView());
      } else if (m_hvacSystemsView->hvacToolbarView->gridViewButton->isChecked()) {
        // TODO

        m_hvacSystemsView->hvacToolbarView->zoomInButton->hide();
        m_hvacSystemsView->hvacToolbarView->zoomOutButton->hide();

        m_hvacSystemsView->hvacToolbarView->addButton->hide();
        m_hvacSystemsView->hvacToolbarView->copyButton->hide();
        m_hvacSystemsView->hvacToolbarView->deleteButton->hide();

        m_refrigerationGridController = std::make_shared<RefrigerationGridController>(m_isIP, m_model);

        connect(this, &HVACSystemsController::toggleUnitsClicked, m_refrigerationGridController->refrigerationGridView(),
                &RefrigerationGridView::toggleUnitsClicked);

        connect(this, &HVACSystemsController::toggleUnitsClicked, this, &HVACSystemsController::toggleUnits);

        m_hvacSystemsView->mainViewSwitcher->setView(m_refrigerationGridController->refrigerationGridView());
      } else {
        // Not allowed
        m_hvacControlsController = std::make_shared<HVACControlsController>(this);

        m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->noControlsView());
      }
    } else if (handle == VRF) {
      m_hvacSystemsView->hvacToolbarView->zoomInButton->setEnabled(false);
      m_hvacSystemsView->hvacToolbarView->zoomOutButton->setEnabled(false);

      if (m_hvacSystemsView->hvacToolbarView->topologyViewButton->isChecked()) {
        m_vrfController = std::make_shared<VRFController>();

        m_hvacSystemsView->mainViewSwitcher->setView(m_vrfController->vrfView());
      } else if (m_hvacSystemsView->hvacToolbarView->gridViewButton->isChecked()) {
        // Not allowed: Refrigeration only on Refrigeration tab
        m_refrigerationController = std::make_shared<RefrigerationController>();

        m_hvacSystemsView->mainViewSwitcher->setView(m_refrigerationController->noRefrigerationView());
      } else {
        m_hvacControlsController = std::make_shared<HVACControlsController>(this);

        m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->noControlsView());
      }
    } else  // NOT VRF NOR REFRIGERATION
    {
      if (m_hvacSystemsView->hvacToolbarView->topologyViewButton->isChecked()) {
        m_hvacLayoutController = std::make_shared<HVACLayoutController>(this);
        m_hvacSystemsView->mainViewSwitcher->setView(m_hvacLayoutController->hvacGraphicsView());

        m_hvacSystemsView->hvacToolbarView->zoomInButton->setEnabled(true);
        m_hvacSystemsView->hvacToolbarView->zoomOutButton->setEnabled(true);

        OSAppBase::instance()->currentDocument()->mainRightColumnController()->mainRightColumnView()->setCurrentId(
          MainRightColumnController::LIBRARY);
      } else if (m_hvacSystemsView->hvacToolbarView->gridViewButton->isChecked()) {
        // Not allowed: Refrigeration only on Refrigeration tab
        m_refrigerationController = std::make_shared<RefrigerationController>();

        m_hvacSystemsView->mainViewSwitcher->setView(m_refrigerationController->noRefrigerationView());
      } else {
        m_hvacControlsController = std::make_shared<HVACControlsController>(this);

        if (currentLoop()) {
          // If an AirLoopHVAC, set the view to the HVACAirLoopControlsView
          if ((currentLoop()->optionalCast<model::AirLoopHVAC>())) {
            m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->hvacAirLoopControlsView());

            m_hvacSystemsView->hvacToolbarView->zoomInButton->setEnabled(false);
            m_hvacSystemsView->hvacToolbarView->zoomOutButton->setEnabled(false);

            OSAppBase::instance()->currentDocument()->mainRightColumnController()->mainRightColumnView()->setCurrentId(
              MainRightColumnController::MY_MODEL);
          }
          // If a PlantLoop, HVACPlantLoopControlsView
          else if ((currentLoop()->optionalCast<model::PlantLoop>())) {
            m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->hvacPlantLoopControlsView());

            m_hvacSystemsView->hvacToolbarView->zoomInButton->setEnabled(false);
            m_hvacSystemsView->hvacToolbarView->zoomOutButton->setEnabled(false);

            OSAppBase::instance()->currentDocument()->mainRightColumnController()->mainRightColumnView()->setCurrentId(
              MainRightColumnController::MY_MODEL);
          } else {
            m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->noControlsView());
          }
        } else {
          m_hvacSystemsView->mainViewSwitcher->setView(m_hvacControlsController->noControlsView());
        }
      }
    }

    systemComboBox->blockSignals(false);

    m_dirty = false;
  }

  m_updateMutex->unlock();
}

void HVACSystemsController::updateLater() {
  m_dirty = true;

  QTimer::singleShot(0, this, &HVACSystemsController::update);
}

HVACSystemsView* HVACSystemsController::hvacSystemsView() const {
  return m_hvacSystemsView;
}

std::shared_ptr<HVACLayoutController> HVACSystemsController::hvacLayoutController() const {
  return m_hvacLayoutController;
}

std::shared_ptr<HVACControlsController> HVACSystemsController::hvacControlsController() const {
  return m_hvacControlsController;
}

std::vector<IddObjectType> HVACSystemsController::systemComboBoxTypes() const {
  std::vector<IddObjectType> types;

  types.push_back(model::AirLoopHVAC::iddObjectType());
  types.push_back(model::PlantLoop::iddObjectType());

  return types;
}

void HVACSystemsController::onObjectAdded(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& /*type*/,
                                          const openstudio::UUID& /*uuid*/) {
  std::vector<IddObjectType> types = systemComboBoxTypes();

  auto newObjectIddType = workspaceObject.cast<model::ModelObject>().iddObjectType();
  if (std::find(types.begin(), types.end(), newObjectIddType) != types.end()) {
    updateLater();
  }

  // If it's a Loop, we trigger a repopulation of the System Combobox upon name change
  if ((newObjectIddType == model::PlantLoop::iddObjectType()) || (newObjectIddType == model::AirLoopHVAC::iddObjectType())) {
    LOG(LOGLEVEL, "onObjectAdded: Attaching name change for " << workspaceObject.briefDescription());
    workspaceObject.getImpl<detail::IdfObject_Impl>()
      ->detail::IdfObject_Impl::onNameChange.connect<HVACSystemsController, &HVACSystemsController::repopulateSystemComboBox>(this);
  }
}

void HVACSystemsController::onObjectRemoved(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& /*type*/,
                                            const openstudio::UUID& /*uuid*/) {
  std::vector<IddObjectType> types = systemComboBoxTypes();

  if (std::find(types.begin(), types.end(), workspaceObject.cast<model::ModelObject>().iddObjectType()) != types.end()) {
    updateLater();
  }
}

void HVACSystemsController::onObjectChanged() {}

void HVACSystemsController::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

void HVACLayoutController::addLibraryObjectToTopLevel(const OSItemId& itemId) {
  model::OptionalModelObject object = OSAppBase::instance()->currentDocument()->getModelObject(itemId);

  if (object) {
    if (OSAppBase::instance()->currentDocument()->fromComponentLibrary(itemId)) {
      if (boost::optional<model::WaterUseConnections> waterUseConnections = object->optionalCast<model::WaterUseConnections>()) {
        waterUseConnections->clone(m_hvacSystemsController->model());

        return;
      }
    }
  }

  QMessageBox message(m_hvacSystemsController->hvacSystemsView());

  message.setText("The selected component is not allowed at this location.");

  message.exec();
}

void HVACLayoutController::addLibraryObjectToModelNode(const OSItemId& itemId, model::HVACComponent& comp) {
  model::OptionalModelObject object;
  bool remove = false;
  auto doc = OSAppBase::instance()->currentDocument();

  object = doc->getModelObject(itemId);
  if (object) {
    if (!doc->fromModel(itemId)) {
      object = object->clone(comp.model());
      remove = true;
    }
  }

  if (auto component = doc->getComponent(itemId)) {
    // Ugly hack to avoid the component being treated as a resource.
    component->componentData().setString(OS_ComponentDataFields::UUID, toString(createUUID()));
    if (auto componentData = comp.model().insertComponent(component.get())) {
      object = componentData->primaryComponentObject();
      remove = true;
    }
  }

  if (object) {

    bool added = false;

    if (boost::optional<model::HVACComponent> hvacComponent = object->optionalCast<model::HVACComponent>()) {
      if (boost::optional<model::Node> node = comp.optionalCast<model::Node>()) {
        // Note: Julien Marrec, 2018-01-04
        // When you have a WaterToWaterComponent that has a tertiaryPlantLoop, you should override the addToNode method
        // to call addToTertiaryNode when needed. This will work with addSupplyBranchForComponent (etc) too
        // Take a look at CentralHeatPumpSystem::addToNode (and CentralHeatPumpSystem::addToTertiaryNode) for an actual example
        auto zone = hvacComponent->optionalCast<model::ThermalZone>();
        if (zone) {
          added = zone->multiAddToNode(node.get());
        } else {
          added = hvacComponent->addToNode(node.get());
        }
      } else if (boost::optional<model::Splitter> splitter = comp.optionalCast<model::Splitter>()) {
        if (boost::optional<model::PlantLoop> plant = splitter->plantLoop()) {
          if (plant->supplyComponent(splitter->handle())) {
            added = plant->addSupplyBranchForComponent(hvacComponent.get());
          } else if (plant->demandComponent(splitter->handle())) {
            added = plant->addDemandBranchForComponent(hvacComponent.get());
          }
        } else if (boost::optional<model::AirLoopHVAC> airLoop = splitter->airLoopHVAC()) {
          if (boost::optional<model::ThermalZone> zone = object->optionalCast<model::ThermalZone>()) {
            added = airLoop->multiAddBranchForZone(zone.get());
          }
        }
      }
    } else if (boost::optional<model::WaterUseEquipment> waterUseEquipment = object->optionalCast<model::WaterUseEquipment>()) {
      if (boost::optional<model::WaterUseConnections> waterUseConnections = comp.optionalCast<model::WaterUseConnections>()) {
        added = waterUseConnections->addWaterUseEquipment(waterUseEquipment.get());
      }
    } else if (boost::optional<model::WaterUseEquipmentDefinition> waterUseEquipmentDefinition =
                 object->optionalCast<model::WaterUseEquipmentDefinition>()) {
      if (boost::optional<model::WaterUseConnections> waterUseConnections = comp.optionalCast<model::WaterUseConnections>()) {
        model::WaterUseEquipment waterUseEquipment(waterUseEquipmentDefinition.get());
        waterUseEquipment.resetFlowRateFractionSchedule();

        added = waterUseConnections->addWaterUseEquipment(waterUseEquipment);
      }
    }

    if (!added) {
      if (remove) {
        object->remove();
      }

      QMessageBox message(m_hvacSystemsController->hvacSystemsView());

      message.setText("The selected component is not allowed at this location.");

      message.exec();
    }
  }
}

boost::optional<model::Loop> HVACSystemsController::currentLoop() const {
  return m_model.getModelObject<model::Loop>(toUUID(m_currentHandle));
}

void HVACLayoutController::removeModelObject(model::ModelObject& modelObject) {
  if (modelObject.handle().isNull()) {
    return;
  }

  model::OptionalModelObject mo;

  OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(mo, false);

  boost::optional<model::Loop> loop = m_hvacSystemsController->currentLoop();

  if (loop) {
    if (boost::optional<model::WaterToWaterComponent> comp = modelObject.optionalCast<model::WaterToWaterComponent>()) {
      boost::optional<model::PlantLoop> plant = comp->plantLoop();
      boost::optional<model::PlantLoop> secondaryPlant = comp->secondaryPlantLoop();
      boost::optional<model::PlantLoop> tertiaryPlant = comp->tertiaryPlantLoop();

      // If it's on at least two plantLoops, we disconnect it from the current loop only (instead of deleting) and return
      if (plant ? (secondaryPlant || tertiaryPlant) : (secondaryPlant && tertiaryPlant)) {
        if (plant) {
          if (plant.get() == loop.get()) {
            comp->removeFromPlantLoop();
            return;
          }
        }
        if (secondaryPlant) {
          if (secondaryPlant.get() == loop.get()) {
            comp->removeFromSecondaryPlantLoop();
            return;
          }
        }
        if (tertiaryPlant) {
          if (tertiaryPlant.get() == loop.get()) {
            comp->removeFromTertiaryPlantLoop();
            return;
          }
        }
      }
    } else if (boost::optional<model::WaterToAirComponent> comp = modelObject.optionalCast<model::WaterToAirComponent>()) {
      boost::optional<model::PlantLoop> plant = comp->plantLoop();
      boost::optional<model::AirLoopHVAC> airLoop = comp->airLoopHVAC();

      if (plant && airLoop) {
        if (plant.get() == loop.get()) {
          comp->removeFromPlantLoop();

          return;
        }
        if (airLoop.get() == loop.get()) {
          comp->removeFromAirLoopHVAC();

          return;
        }
      }
    } else if (boost::optional<model::ThermalZone> comp = modelObject.optionalCast<model::ThermalZone>()) {
      boost::optional<model::AirLoopHVAC> airLoop = comp->airLoopHVAC();

      if (airLoop) {
        airLoop->removeBranchForZone(comp.get());

        return;
      }
    }
  }

  modelObject.remove();
}

void HVACLayoutController::goToOtherLoop(model::ModelObject& modelObject) {
  if (boost::optional<model::WaterUseConnections> waterUseConnections = modelObject.optionalCast<model::WaterUseConnections>()) {
    if (boost::optional<model::Loop> loop = waterUseConnections->loop()) {
      goToOtherLoop(loop.get());
    }

    return;
  }

  if (boost::optional<model::Loop> loop = modelObject.optionalCast<model::Loop>()) {
    m_hvacSystemsController->setCurrentHandle(toQString(loop->handle()));

    return;
  }

  boost::optional<model::Loop> t_currentLoop = m_hvacSystemsController->currentLoop();

  if (t_currentLoop) {
    if (boost::optional<model::WaterToAirComponent> comp = modelObject.optionalCast<model::WaterToAirComponent>()) {
      if (t_currentLoop->optionalCast<model::AirLoopHVAC>()) {
        if (boost::optional<model::PlantLoop> plantLoop = comp->plantLoop()) {
          m_hvacSystemsController->setCurrentHandle(toQString(plantLoop->handle()));
        }

        return;
      } else {  // always true if not an AirLoopHVAC:  if (t_currentLoop->optionalCast<model::PlantLoop>())
        if (boost::optional<model::AirLoopHVAC> airLoopHVAC = comp->airLoopHVAC()) {
          m_hvacSystemsController->setCurrentHandle(toQString(airLoopHVAC->handle()));
        }

        return;
      }
    }

    if (boost::optional<model::WaterToWaterComponent> comp = modelObject.optionalCast<model::WaterToWaterComponent>()) {
      boost::optional<model::PlantLoop> primaryPlantLoop = comp->plantLoop();
      boost::optional<model::PlantLoop> secondaryPlantLoop = comp->secondaryPlantLoop();

      // From primary, go to secondary
      if (primaryPlantLoop) {
        if (t_currentLoop.get() == primaryPlantLoop.get()) {
          if (secondaryPlantLoop) {
            m_hvacSystemsController->setCurrentHandle(toQString(secondaryPlantLoop->handle()));

            return;
          }
        }
      }

      // From secondary, go to primary
      if (secondaryPlantLoop) {
        if (t_currentLoop.get() == secondaryPlantLoop.get()) {
          if (primaryPlantLoop) {
            m_hvacSystemsController->setCurrentHandle(toQString(primaryPlantLoop->handle()));

            return;
          }
        }
      }

      // From tertiary, go to either primary or secondary, whichever is on the other side
      // eg if tertiary = supply => go to secondary (demand)
      //    if tertiary = demand => go to primary (supply)
      boost::optional<model::PlantLoop> tertiaryPlantLoop = comp->tertiaryPlantLoop();

      if (tertiaryPlantLoop) {

        if (t_currentLoop.get() == tertiaryPlantLoop.get()) {

          // If the comp is on the supply side of the current=tertiary loop
          // Jump to the demand side of the other loop, that is secondaryLoop
          if (tertiaryPlantLoop->supplyComponent(comp->handle())) {
            if (secondaryPlantLoop) {
              m_hvacSystemsController->setCurrentHandle(toQString(secondaryPlantLoop->handle()));
              return;
            }
          }

          // Else it's on the demand side, so jump to primary=supply
          else {
            if (primaryPlantLoop) {
              m_hvacSystemsController->setCurrentHandle(toQString(primaryPlantLoop->handle()));
              return;
            }
          }
        }  // End if currentLoop = loop

      }  // End if tertiaryPlantLoop()
    }
  }
}

void HVACSystemsController::addToModel(AddToModelEnum addToModelEnum) {
  boost::optional<model::Loop> loop;

  switch (addToModelEnum) {
    case ADDTOMODEL_AIRLOOPHVAC: {
      model::AirLoopHVAC airLoopHVAC(m_model);

      loop = airLoopHVAC;

      break;
    }
    case ADDTOMODEL_DUAL_AIRLOOPHVAC: {
      model::AirLoopHVAC airLoopHVAC(m_model, true);

      loop = airLoopHVAC;

      break;
    }
    case ADDTOMODEL_PLANTLOOP: {
      model::PlantLoop plantLoop(m_model);

      loop = plantLoop;

      break;
    }
    case ADDTOMODEL_SHWLOOP: {
      loop = model::addSHWLoop(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_3: {
      loop = model::addSystemType3(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_4: {
      loop = model::addSystemType4(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_5: {
      loop = model::addSystemType5(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_6: {
      loop = model::addSystemType6(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_7: {
      loop = model::addSystemType7(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_8: {
      loop = model::addSystemType8(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_9: {
      loop = model::addSystemType9(m_model);
      break;
    }
    case ADDTOMODEL_SYSTEM_TYPE_10: {
      loop = model::addSystemType10(m_model);
      break;
    }
    default:
      break;
  }

  if (loop) {
    m_currentHandle = toQString(loop->handle());
  }
}

void HVACSystemsController::onShowTopologyClicked() {
  updateLater();
}

void HVACSystemsController::onShowControlsClicked() {
  updateLater();
}

void HVACSystemsController::onShowGridClicked() {
  updateLater();
}

void HVACSystemsController::onAddSystemClicked() {
  LoopLibraryDialog loopLibraryDialog(m_hvacSystemsView);

  loopLibraryDialog.exec();

  if (loopLibraryDialog.result() == QDialog::Accepted) {
    addToModel(loopLibraryDialog.addToModelEnum().get());
  }

  // Temporary hack to test refrigeration grid view
  //m_hvacLayoutController->refrigerationController()->refrigerationSystemListController()->createNewSystem();
}

void HVACSystemsController::onSystemComboBoxIndexChanged(int i) {
  QString handle = m_hvacSystemsView->hvacToolbarView->systemComboBox->itemData(i).toString();

  setCurrentHandle(handle);
}

void HVACSystemsController::onCopySystemClicked() {
  auto loop = currentLoop();
  if (loop) {
    auto clone = loop->clone(loop->model());
    setCurrentHandle(toQString(clone.handle()));
  }
}

void HVACSystemsController::onRemoveLoopClicked() {
  if (boost::optional<model::Loop> loop = currentLoop()) {
    if (loop->handle().isNull()) {
      return;
    }

    QComboBox* chooser = m_hvacSystemsView->hvacToolbarView->systemComboBox;

    int i = chooser->findData(toQString(loop->handle()));

    if (i == 0) {
      setCurrentHandle(chooser->itemData(1).toString());
    } else {
      setCurrentHandle(chooser->itemData(i - 1).toString());
    }

    loop->remove();
  }
}

void HVACLayoutController::onModelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly) {
  if (modelObject) {
    if (boost::optional<model::WaterUseConnections> waterUseConnections = modelObject->optionalCast<model::WaterUseConnections>()) {
      m_hvacSystemsController->setCurrentHandle(toQString(waterUseConnections->handle()));
    } else {
      OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(modelObject, readOnly);
    }
  } else {
    OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(modelObject, readOnly);
  }
}

HVACControlsController::HVACControlsController(HVACSystemsController* hvacSystemsController)
  : m_hvacAirLoopControlsView(new HVACAirLoopControlsView()),
    m_hvacPlantLoopControlsView(new HVACPlantLoopControlsView()),
    m_noControlsView(new NoControlsView()),
    m_hvacSystemsController(hvacSystemsController) {
  updateLater();
}

HVACControlsController::~HVACControlsController() {
  delete m_hvacAirLoopControlsView;
  delete m_hvacPlantLoopControlsView;
  delete m_noControlsView;
  delete m_mechanicalVentilationView;
  delete m_singleZoneReheatSPMView;
  delete m_noSupplyAirTempControlView;
  delete m_noMechanicalVentilationView;
  delete m_systemAvailabilityDropZone;
  delete m_supplyAirTempScheduleDropZone;
  delete m_availabilityManagerDropZone;
  delete m_followOATempSPMView;
  delete m_oaResetSPMView;
  delete m_airLoopHVACUnitaryHeatPumpAirToAirControlView;
  delete m_scheduledSPMView;
}

boost::optional<model::AirLoopHVAC> HVACControlsController::airLoopHVAC() const {
  if (boost::optional<model::Loop> loop = m_hvacSystemsController->currentLoop()) {
    return loop->optionalCast<model::AirLoopHVAC>();
  }
  return boost::none;
}

boost::optional<model::PlantLoop> HVACControlsController::plantLoop() const {
  if (boost::optional<model::Loop> loop = m_hvacSystemsController->currentLoop()) {
    return loop->optionalCast<model::PlantLoop>();
  }
  return boost::none;
}

HVACAirLoopControlsView* HVACControlsController::hvacAirLoopControlsView() const {
  return m_hvacAirLoopControlsView;
}

HVACPlantLoopControlsView* HVACControlsController::hvacPlantLoopControlsView() const {
  return m_hvacPlantLoopControlsView;
}

NoControlsView* HVACControlsController::noControlsView() const {
  return m_noControlsView;
}

void HVACControlsController::update() {
  if (m_dirty) {

    // If we're looking at an AirLoopHVAC
    if (boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC()) {

      m_hvacAirLoopControlsView->setUpdatesEnabled(false);

      delete m_mechanicalVentilationView;
      delete m_singleZoneReheatSPMView;
      delete m_noSupplyAirTempControlView;
      delete m_noMechanicalVentilationView;
      delete m_systemAvailabilityDropZone;
      delete m_supplyAirTempScheduleDropZone;
      delete m_availabilityManagerDropZone;
      delete m_followOATempSPMView;
      delete m_oaResetSPMView;
      delete m_scheduledSPMView;
      delete m_airLoopHVACUnitaryHeatPumpAirToAirControlView;

      m_hvacAirLoopControlsView->nightCycleComboBox->setEnabled(false);

      QString title;
      title.append(QString::fromStdString(t_airLoopHVAC->nameString()));
      m_hvacAirLoopControlsView->systemNameLabel->setText(title);

      // Cooling Type

      m_hvacAirLoopControlsView->coolingTypeLabel->setText("Unclassified Cooling Type");

      std::vector<model::ModelObject> modelObjects = t_airLoopHVAC->supplyComponents(model::CoilCoolingDXSingleSpeed::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->coolingTypeLabel->setText("DX Cooling");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::CoilCoolingDXTwoSpeed::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->coolingTypeLabel->setText("DX Cooling");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::CoilCoolingWater::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->coolingTypeLabel->setText("Chilled Water");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::AirLoopHVACUnitaryHeatPumpAirToAir::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->coolingTypeLabel->setText("DX Cooling");
      }

      // Heating Type

      m_hvacAirLoopControlsView->heatingTypeLabel->setText("Unclassified Heating Type");

      modelObjects = t_airLoopHVAC->supplyComponents(model::CoilHeatingGas::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->heatingTypeLabel->setText("Gas Heating");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::CoilHeatingElectric::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->heatingTypeLabel->setText("Electric Heating");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::CoilHeatingWater::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->heatingTypeLabel->setText("Hot Water");
      }

      modelObjects = t_airLoopHVAC->supplyComponents(model::AirLoopHVACUnitaryHeatPumpAirToAir::iddObjectType());
      if (!modelObjects.empty()) {
        m_hvacAirLoopControlsView->heatingTypeLabel->setText("Air Source Heat Pump");
      }

      // HVAC Operation Schedule

      auto* systemAvailabilityVectorController = new SystemAvailabilityVectorController();
      systemAvailabilityVectorController->attach(t_airLoopHVAC.get());
      m_systemAvailabilityDropZone = new OSDropZone(systemAvailabilityVectorController);
      m_systemAvailabilityDropZone->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      m_systemAvailabilityDropZone->setMinItems(1);
      m_systemAvailabilityDropZone->setMaxItems(1);
      m_systemAvailabilityDropZone->setItemsRemoveable(false);
      m_systemAvailabilityDropZone->setAcceptDrops(true);
      m_systemAvailabilityDropZone->setItemsAcceptDrops(true);
      m_systemAvailabilityDropZone->setEnabled(true);
      m_hvacAirLoopControlsView->hvacOperationViewSwitcher->setView(m_systemAvailabilityDropZone);

      // Allow clicking on the Schedule to see it in the right column inspector
      connect(m_systemAvailabilityDropZone.data(), &OSDropZone::itemClicked, systemAvailabilityVectorController,
              &SystemAvailabilityVectorController::onDropZoneItemClicked);

      // Night Cycle Control

      m_hvacAirLoopControlsView->nightCycleComboBox->setEnabled(true);

      std::string nightCycleControlType = t_airLoopHVAC->nightCycleControlType();

      int nightCycleSelectorIndex = m_hvacAirLoopControlsView->nightCycleComboBox->findData(QString::fromStdString(nightCycleControlType));

      m_hvacAirLoopControlsView->nightCycleComboBox->setCurrentIndex(nightCycleSelectorIndex);

      connect(m_hvacAirLoopControlsView->nightCycleComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
              &HVACControlsController::onNightCycleComboBoxIndexChanged);

      // Mechanical Ventilation

      if (boost::optional<model::AirLoopHVACOutdoorAirSystem> oaSystem = t_airLoopHVAC->airLoopHVACOutdoorAirSystem()) {
        model::ControllerOutdoorAir controllerOutdoorAir = oaSystem->getControllerOutdoorAir();

        model::ControllerMechanicalVentilation controllerMechanicalVentilation = controllerOutdoorAir.controllerMechanicalVentilation();

        m_mechanicalVentilationView = new MechanicalVentilationView();

        m_hvacAirLoopControlsView->ventilationViewSwitcher->setView(m_mechanicalVentilationView);

        // Economizer Control Type

        connect(m_mechanicalVentilationView->economizerComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
                &HVACControlsController::onEconomizerComboBoxIndexChanged);

        std::string economizerControlType = controllerOutdoorAir.getEconomizerControlType();

        int economizerIndex = m_mechanicalVentilationView->economizerComboBox->findData(QString::fromStdString(economizerControlType));

        m_mechanicalVentilationView->economizerComboBox->setCurrentIndex(economizerIndex);

        // Ventilation Calculation Method

        connect(m_mechanicalVentilationView->ventilationCalcMethodComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &HVACControlsController::onVentilationCalcMethodComboBoxIndexChanged);

        std::string ventilationMethod = controllerMechanicalVentilation.systemOutdoorAirMethod();

        int ventilationMethodIndex = m_mechanicalVentilationView->ventilationCalcMethodComboBox->findData(QString::fromStdString(ventilationMethod));

        m_mechanicalVentilationView->ventilationCalcMethodComboBox->setCurrentIndex(ventilationMethodIndex);

        // Demand Controlled Ventilation

        // m_mechanicalVentilationView->dcvButton->bind(controllerMechanicalVentilation,"demandControlledVentilation");
        m_mechanicalVentilationView->dcvButton->bind(
          controllerMechanicalVentilation,
          std::bind(&model::ControllerMechanicalVentilation::demandControlledVentilation, controllerMechanicalVentilation),
          boost::optional<BoolSetter>(std::bind(&model::ControllerMechanicalVentilation::setDemandControlledVentilationNoFail,
                                                controllerMechanicalVentilation, std::placeholders::_1)),
          boost::optional<NoFailAction>(
            std::bind(&model::ControllerMechanicalVentilation::resetDemandControlledVentilation, controllerMechanicalVentilation)),
          boost::optional<BasicQuery>(
            std::bind(&model::ControllerMechanicalVentilation::isDemandControlledVentilationDefaulted, controllerMechanicalVentilation)));
      } else {
        m_noMechanicalVentilationView = new NoMechanicalVentilationView();

        m_hvacAirLoopControlsView->ventilationViewSwitcher->setView(m_noMechanicalVentilationView);
      }

      // Supply Air Temperature
      boost::optional<model::SetpointManager> _spm;
      for (auto& spm : t_airLoopHVAC->supplyOutletNode().setpointManagers()) {
        if (istringEqual("Temperature", spm.controlVariable())) {
          _spm = spm;
          break;
        }
      }

      boost::optional<model::SetpointManagerSingleZoneReheat> spmSZR;
      boost::optional<model::SetpointManagerScheduled> spmS;
      boost::optional<model::SetpointManagerFollowOutdoorAirTemperature> spmFOAT;
      boost::optional<model::SetpointManagerOutdoorAirReset> spmOAR;

      if (_spm && (spmSZR = _spm->optionalCast<model::SetpointManagerSingleZoneReheat>())) {
        m_singleZoneReheatSPMView = new SingleZoneReheatSPMView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_singleZoneReheatSPMView);

        for (auto& thermalZone : t_airLoopHVAC->thermalZones()) {
          m_singleZoneReheatSPMView->controlZoneComboBox->addItem(QString::fromStdString(thermalZone.nameString()), toQString(thermalZone.handle()));
        }

        m_singleZoneReheatSPMView->controlZoneComboBox->addItem("", toQString(UUID()));

        if (boost::optional<model::ThermalZone> tz = spmSZR->controlZone()) {
          int index = m_singleZoneReheatSPMView->controlZoneComboBox->findData(toQString(tz->handle()));

          if (index > -1) {
            m_singleZoneReheatSPMView->controlZoneComboBox->setCurrentIndex(index);
          } else {
            m_singleZoneReheatSPMView->controlZoneComboBox->addItem(QString::fromStdString(tz->nameString()), toQString(tz->handle()));

            int i = m_singleZoneReheatSPMView->controlZoneComboBox->count() - 1;

            m_singleZoneReheatSPMView->controlZoneComboBox->setCurrentIndex(i);
          }
        } else {
          int index = m_singleZoneReheatSPMView->controlZoneComboBox->findData(toQString(UUID()));

          OS_ASSERT(index > -1);

          m_singleZoneReheatSPMView->controlZoneComboBox->setCurrentIndex(index);
        }

        connect(m_singleZoneReheatSPMView->controlZoneComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
                &HVACControlsController::onControlZoneComboBoxChanged);
      } else if (_spm && (spmS = _spm->optionalCast<model::SetpointManagerScheduled>())) {
        m_scheduledSPMView = new ScheduledSPMView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_scheduledSPMView);

        auto* supplyAirTempScheduleVectorController = new SupplyAirTempScheduleVectorController();
        supplyAirTempScheduleVectorController->attach(spmS.get());
        m_supplyAirTempScheduleDropZone = new OSDropZone(supplyAirTempScheduleVectorController);
        m_supplyAirTempScheduleDropZone->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_supplyAirTempScheduleDropZone->setMinItems(1);
        m_supplyAirTempScheduleDropZone->setMaxItems(1);
        m_supplyAirTempScheduleDropZone->setItemsRemoveable(false);
        m_supplyAirTempScheduleDropZone->setAcceptDrops(true);
        m_supplyAirTempScheduleDropZone->setItemsAcceptDrops(true);
        m_supplyAirTempScheduleDropZone->setEnabled(true);
        m_scheduledSPMView->supplyAirTemperatureViewSwitcher->setView(m_supplyAirTempScheduleDropZone);

        // Allow clicking on the Schedule to see it in the right column inspector
        connect(m_supplyAirTempScheduleDropZone.data(), &OSDropZone::itemClicked, supplyAirTempScheduleVectorController,
                &SupplyAirTempScheduleVectorController::onDropZoneItemClicked);
      } else if (_spm && (_spm->optionalCast<model::SetpointManagerFollowOutdoorAirTemperature>())) {
        m_followOATempSPMView = new FollowOATempSPMView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_followOATempSPMView);
      } else if (_spm && (_spm->optionalCast<model::SetpointManagerOutdoorAirReset>())) {
        m_oaResetSPMView = new OAResetSPMView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_oaResetSPMView);
      } else if (!t_airLoopHVAC->supplyComponents(model::AirLoopHVACUnitaryHeatPumpAirToAir::iddObjectType()).empty()) {
        auto hp = t_airLoopHVAC->supplyComponents(model::AirLoopHVACUnitaryHeatPumpAirToAir::iddObjectType())
                    .back()
                    .cast<model::AirLoopHVACUnitaryHeatPumpAirToAir>();

        m_airLoopHVACUnitaryHeatPumpAirToAirControlView = new AirLoopHVACUnitaryHeatPumpAirToAirControlView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_airLoopHVACUnitaryHeatPumpAirToAirControlView);

        std::vector<model::ThermalZone> thermalZones = t_airLoopHVAC->thermalZones();

        for (auto& thermalZone : thermalZones) {
          m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->addItem(QString::fromStdString(thermalZone.nameString()),
                                                                                        toQString(thermalZone.handle()));
        }

        m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->addItem("", toQString(UUID()));

        if (boost::optional<model::ThermalZone> tz = hp.controllingZone()) {
          int index = m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->findData(toQString(tz->handle()));

          if (index > -1) {
            m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->setCurrentIndex(index);
          } else {
            m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->addItem(QString::fromStdString(tz->nameString()),
                                                                                          toQString(tz->handle()));

            int i = m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->count() - 1;

            m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->setCurrentIndex(i);
          }
        } else {
          int index = m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->findData(toQString(UUID()));

          OS_ASSERT(index > -1);

          m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->setCurrentIndex(index);
        }

        connect(m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox,
                static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
                &HVACControlsController::onUnitaryHeatPumpControlZoneChanged);
      } else {
        m_noSupplyAirTempControlView = new NoSupplyAirTempControlView();

        m_hvacAirLoopControlsView->supplyAirTemperatureViewSwitcher->setView(m_noSupplyAirTempControlView);
      }

      // AVM List
      auto* availabilityManagerObjectVectorController = new AvailabilityManagerObjectVectorController();
      availabilityManagerObjectVectorController->attach(t_airLoopHVAC.get());
      m_availabilityManagerDropZone = new OSDropZone(availabilityManagerObjectVectorController, "Drag From Library", QSize(0, 0), false);
      m_availabilityManagerDropZone->setFixedSize(QSize(OSItem::ITEM_WIDTH + 20, 10 * 50));
      m_availabilityManagerDropZone->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      m_availabilityManagerDropZone->setMinItems(0);
      m_availabilityManagerDropZone->setMaxItems(10);
      m_availabilityManagerDropZone->setItemsRemoveable(true);
      m_availabilityManagerDropZone->setAcceptDrops(true);
      m_availabilityManagerDropZone->setItemsAcceptDrops(true);
      m_availabilityManagerDropZone->setEnabled(true);
      m_availabilityManagerDropZone->setItemsDraggable(true);
      m_hvacAirLoopControlsView->availabilityManagerViewSwitcher->setView(m_availabilityManagerDropZone);

      // When clicking on the drop zone item, pull it on the MainRightColumnController
      connect(m_availabilityManagerDropZone.data(), &OSDropZone::itemClicked, availabilityManagerObjectVectorController,
              &AvailabilityManagerObjectVectorController::onDropZoneItemClicked);

      m_hvacAirLoopControlsView->setUpdatesEnabled(true);

    }

    // Else if a plantLoop
    else if (boost::optional<model::PlantLoop> t_plantLoop = plantLoop()) {

      m_hvacPlantLoopControlsView->setUpdatesEnabled(false);

      delete m_systemAvailabilityDropZone;

      QString title;
      title.append(QString::fromStdString(t_plantLoop->nameString()));
      m_hvacPlantLoopControlsView->systemNameLabel->setText(title);

      openstudio::energyplus::ComponentType plType = openstudio::energyplus::plantLoopType(t_plantLoop.get());

      if (plType == openstudio::energyplus::ComponentType::BOTH) {
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setText("Both");
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setStyleSheet("QLabel { color : orange; }");
      } else if (plType == openstudio::energyplus::ComponentType::HEATING) {
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setText("Heating");
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setStyleSheet("QLabel { color : red; }");

      } else if (plType == openstudio::energyplus::ComponentType::COOLING) {
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setText("Cooling");
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setStyleSheet("QLabel { color : blue; }");
      } else if (plType == openstudio::energyplus::ComponentType::NONE) {
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setText("None");
        m_hvacPlantLoopControlsView->plantLoopTypeLabel->setStyleSheet("QLabel { color : black; }");
      }

      // Heating Components
      QString heatingComps("<ul>");
      // std::vector<model::HVACComponent> heatingComponents = openstudio::energyplus::heatingComponents(t_plantLoop.get());
      for (const model::HVACComponent& hc : openstudio::energyplus::heatingComponents(t_plantLoop.get())) {
        heatingComps.append("<li>");
        heatingComps.append(QString::fromStdString(hc.nameString()));
        heatingComps.append("</li>");
      }
      heatingComps.append("</ul>");
      m_hvacPlantLoopControlsView->heatingComponentsLabel->setText(heatingComps);

      // Cooling Components
      QString coolingComps("<ul>");
      // std::vector<model::HVACComponent> coolingComponents = openstudio::energyplus::coolingComponents(t_plantLoop.get());
      for (const model::HVACComponent& hc : openstudio::energyplus::coolingComponents(t_plantLoop.get())) {
        coolingComps.append("<li>");
        coolingComps.append(QString::fromStdString(hc.nameString()));
        coolingComps.append("</li>");
      }
      coolingComps.append("</ul>");
      m_hvacPlantLoopControlsView->coolingComponentsLabel->setText(coolingComps);

      // Setpoint Components
      QString setpointComps("<ul>");
      // std::vector<model::HVACComponent> setpointComponents = openstudio::energyplus::setpointComponents(t_plantLoop.get());
      for (const model::HVACComponent& hc : openstudio::energyplus::setpointComponents(t_plantLoop.get())) {
        setpointComps.append("<li>");
        setpointComps.append(QString::fromStdString(hc.nameString()));
        setpointComps.append("</li>");
      }
      setpointComps.append("</ul>");
      m_hvacPlantLoopControlsView->setpointComponentsLabel->setText(setpointComps);

      // Uncontrolled Components
      QString uncontrolledComps("<ul>");
      // std::vector<model::HVACComponent> uncontrolledComponents = openstudio::energyplus::uncontrolledComponents(t_plantLoop.get());
      for (const model::HVACComponent& hc : openstudio::energyplus::uncontrolledComponents(t_plantLoop.get())) {
        uncontrolledComps.append("<li>");
        uncontrolledComps.append(QString::fromStdString(hc.nameString()));
        uncontrolledComps.append("</li>");
      }
      uncontrolledComps.append("</ul>");
      m_hvacPlantLoopControlsView->uncontrolledComponentsLabel->setText(uncontrolledComps);

      // AvailabilityManagers

      auto* availabilityManagerObjectVectorController = new AvailabilityManagerObjectVectorController();
      availabilityManagerObjectVectorController->attach(t_plantLoop.get());
      // m_availabilityManagerDropZone = new OSDropZone(availabilityManagerObjectVectorController);
      m_availabilityManagerDropZone = new OSDropZone(availabilityManagerObjectVectorController, "Drag From Library", QSize(0, 0), false);

      m_availabilityManagerDropZone->setFixedSize(QSize(OSItem::ITEM_WIDTH + 20, 10 * 50));
      m_availabilityManagerDropZone->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      m_availabilityManagerDropZone->setMinItems(0);
      m_availabilityManagerDropZone->setMaxItems(10);
      m_availabilityManagerDropZone->setItemsRemoveable(true);
      m_availabilityManagerDropZone->setAcceptDrops(true);
      m_availabilityManagerDropZone->setItemsAcceptDrops(true);
      m_availabilityManagerDropZone->setEnabled(true);
      m_availabilityManagerDropZone->setItemsDraggable(true);
      m_hvacPlantLoopControlsView->availabilityManagerViewSwitcher->setView(m_availabilityManagerDropZone);

      // When clicking on the drop zone item, pull it on the MainRightColumnController
      connect(m_availabilityManagerDropZone.data(), &OSDropZone::itemClicked, availabilityManagerObjectVectorController,
              &AvailabilityManagerObjectVectorController::onDropZoneItemClicked);

      m_hvacPlantLoopControlsView->setUpdatesEnabled(true);
    }

    m_dirty = false;
  }
}

void HVACControlsController::onEconomizerComboBoxIndexChanged(int index) {
  boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC();

  OS_ASSERT(t_airLoopHVAC);

  boost::optional<model::AirLoopHVACOutdoorAirSystem> oaSystem = t_airLoopHVAC->airLoopHVACOutdoorAirSystem();

  OS_ASSERT(oaSystem);

  model::ControllerOutdoorAir controllerOutdoorAir = oaSystem->getControllerOutdoorAir();

  OS_ASSERT(m_mechanicalVentilationView);

  QString data = m_mechanicalVentilationView->economizerComboBox->itemData(index).toString();

  controllerOutdoorAir.setEconomizerControlType(data.toStdString());
}

void HVACControlsController::onVentilationCalcMethodComboBoxIndexChanged(int index) {
  boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC();

  OS_ASSERT(t_airLoopHVAC);

  boost::optional<model::AirLoopHVACOutdoorAirSystem> oaSystem = t_airLoopHVAC->airLoopHVACOutdoorAirSystem();

  OS_ASSERT(oaSystem);

  model::ControllerOutdoorAir controllerOutdoorAir = oaSystem->getControllerOutdoorAir();

  model::ControllerMechanicalVentilation controllerMechanicalVentilation = controllerOutdoorAir.controllerMechanicalVentilation();

  OS_ASSERT(m_mechanicalVentilationView);

  QString data = m_mechanicalVentilationView->ventilationCalcMethodComboBox->itemData(index).toString();

  controllerMechanicalVentilation.setSystemOutdoorAirMethod(data.toStdString());
}

void HVACControlsController::onNightCycleComboBoxIndexChanged(int index) {
  boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC();

  OS_ASSERT(t_airLoopHVAC);

  QString data = m_hvacAirLoopControlsView->nightCycleComboBox->itemData(index).toString();

  t_airLoopHVAC->setNightCycleControlType(data.toStdString());
}

void HVACControlsController::onControlZoneComboBoxChanged(int index) {
  QString stringHandle = m_singleZoneReheatSPMView->controlZoneComboBox->itemData(index).toString();

  UUID handle(toUUID(stringHandle));

  model::Model t_model = m_hvacSystemsController->model();

  boost::optional<model::ThermalZone> tz = t_model.getModelObject<model::ThermalZone>(handle);

  boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC();

  OS_ASSERT(t_airLoopHVAC);

  std::vector<model::SetpointManagerSingleZoneReheat> setpointManagers =
    subsetCastVector<model::SetpointManagerSingleZoneReheat>(t_airLoopHVAC->supplyOutletNode().setpointManagers());
  if (!setpointManagers.empty()) {
    model::SetpointManagerSingleZoneReheat spm = setpointManagers.front();
    if (tz) {
      spm.setControlZone(*tz);
    } else {
      spm.resetControlZone();
    }
  }
}
void HVACControlsController::onUnitaryHeatPumpControlZoneChanged(int index) {
  QString stringHandle = m_airLoopHVACUnitaryHeatPumpAirToAirControlView->controlZoneComboBox->itemData(index).toString();

  UUID handle(toUUID(stringHandle));

  model::Model t_model = m_hvacSystemsController->model();

  boost::optional<model::ThermalZone> tz = t_model.getModelObject<model::ThermalZone>(handle);

  boost::optional<model::AirLoopHVAC> t_airLoopHVAC = airLoopHVAC();

  OS_ASSERT(t_airLoopHVAC);

  std::vector<model::ModelObject> modelObjects = t_airLoopHVAC->supplyComponents(model::AirLoopHVACUnitaryHeatPumpAirToAir::iddObjectType());

  OS_ASSERT(!modelObjects.empty());

  auto hp = modelObjects.back().cast<model::AirLoopHVACUnitaryHeatPumpAirToAir>();

  if (tz) {
    hp.setControllingZone(tz.get());
  } else {
    hp.resetControllingZone();
  }
}

void HVACControlsController::updateLater() {
  m_dirty = true;

  QTimer::singleShot(0, this, &HVACControlsController::update);
}

HVACLayoutController::HVACLayoutController(HVACSystemsController* hvacSystemsController)
  : m_hvacGraphicsView(new HVACGraphicsView()),
    m_hvacSystemsController(hvacSystemsController)
// m_refrigerationController(std::make_shared<RefrigerationController>())
{
  connect(m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->zoomOutButton, &QPushButton::clicked, m_hvacGraphicsView.data(),
          &HVACGraphicsView::zoomOut);

  connect(m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->zoomInButton, &QPushButton::clicked, m_hvacGraphicsView.data(),
          &HVACGraphicsView::zoomIn);

  updateLater();
}

HVACLayoutController::~HVACLayoutController() {
  delete m_hvacGraphicsView;
}

//std::shared_ptr<RefrigerationController> HVACLayoutController::refrigerationController() const
//{
//  return m_refrigerationController;
//}

HVACGraphicsView* HVACLayoutController::hvacGraphicsView() const {
  return m_hvacGraphicsView;
}

void HVACLayoutController::goToServiceWaterScene() {
  // A null UUID signals the service water scene

  m_hvacSystemsController->setCurrentHandle(toQString(UUID()));
}

void HVACLayoutController::clearSceneSelection() {
  if (QGraphicsScene* scene = m_hvacGraphicsView->scene()) {
    scene->clearSelection();
  }
}

void HVACLayoutController::update() {
  if (m_dirty) {
    model::Model t_model = m_hvacSystemsController->model();

    // Remove old stuff
    if (QGraphicsScene* oldScene = m_hvacGraphicsView->scene()) {
      oldScene->deleteLater();
    }

    QString handle = m_hvacSystemsController->currentHandle();

    boost::optional<model::ModelObject> mo = t_model.getModelObject<model::ModelObject>(toUUID(handle));

    if (mo) {
      if (boost::optional<model::Loop> loop = mo->optionalCast<model::Loop>()) {
        m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->showControls(true);

        auto* loopScene = new LoopScene(loop.get());

        m_hvacGraphicsView->setScene(loopScene);

        connect(loopScene, &LoopScene::modelObjectSelected, this, &HVACLayoutController::onModelObjectSelected);

        connect(loopScene, &LoopScene::removeModelObjectClicked, this, &HVACLayoutController::removeModelObject);

        connect(loopScene, &LoopScene::innerNodeClicked, this, &HVACLayoutController::goToOtherLoop);

        connect(loopScene, static_cast<void (LoopScene::*)(OSItemId, model::HVACComponent&)>(&LoopScene::hvacComponentDropped), this,
                &HVACLayoutController::addLibraryObjectToModelNode);
      } else if (boost::optional<model::WaterUseConnections> waterUseConnections = mo->optionalCast<model::WaterUseConnections>()) {
        m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->showControls(false);

        m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->label->setText(QString::fromStdString(waterUseConnections->nameString()));

        auto* waterUseConnectionsScene = new WaterUseConnectionsDetailScene(waterUseConnections.get());

        m_hvacGraphicsView->setScene(waterUseConnectionsScene);

        connect(waterUseConnectionsScene,
                static_cast<void (WaterUseConnectionsDetailScene::*)(OSItemId, model::HVACComponent&)>(
                  &WaterUseConnectionsDetailScene::hvacComponentDropped),
                this, &HVACLayoutController::addLibraryObjectToModelNode);

        connect(waterUseConnectionsScene, &WaterUseConnectionsDetailScene::modelObjectSelected, this, &HVACLayoutController::onModelObjectSelected);

        connect(waterUseConnectionsScene, &WaterUseConnectionsDetailScene::goToServiceWaterSceneClicked, this,
                &HVACLayoutController::goToServiceWaterScene);

        connect(waterUseConnectionsScene, &WaterUseConnectionsDetailScene::innerNodeClicked, this, &HVACLayoutController::goToOtherLoop);

        connect(waterUseConnectionsScene, &WaterUseConnectionsDetailScene::removeModelObjectClicked, this, &HVACLayoutController::removeModelObject);
      }
    }
    //else if(handle == REFRIGERATION)
    //{
    //  //RefrigerationScene * refrigerationScene = new RefrigerationScene();

    //  //m_hvacGraphicsView->setScene(m_refrigerationController->refrigerationScene().data());
    //}
    else {
      m_hvacSystemsController->hvacSystemsView()->hvacToolbarView->showControls(true);

      auto* serviceWaterScene = new ServiceWaterScene(t_model);

      m_hvacGraphicsView->setScene(serviceWaterScene);

      connect(serviceWaterScene, static_cast<void (ServiceWaterScene::*)(OSItemId)>(&ServiceWaterScene::hvacComponentDropped), this,
              &HVACLayoutController::addLibraryObjectToTopLevel);

      connect(serviceWaterScene, &ServiceWaterScene::modelObjectSelected, this, &HVACLayoutController::onModelObjectSelected);

      connect(serviceWaterScene, &ServiceWaterScene::removeModelObjectClicked, this, &HVACLayoutController::removeModelObject);
    }
  }

  m_dirty = false;
}

void HVACLayoutController::updateLater() {
  m_dirty = true;

  QTimer::singleShot(0, this, &HVACLayoutController::update);
}

SystemAvailabilityVectorController::SystemAvailabilityVectorController()
  : ModelObjectVectorController(), m_reportScheduled(false), m_reportItemsMutex(new QMutex()) {}

SystemAvailabilityVectorController::~SystemAvailabilityVectorController() {
  delete m_reportItemsMutex;
}

void SystemAvailabilityVectorController::attach(const model::ModelObject& modelObject) {
  detach();

  m_modelObject = modelObject;

  if (m_modelObject) {
    m_model = m_modelObject->model();

    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.connect<SystemAvailabilityVectorController, &SystemAvailabilityVectorController::reportItemsLater>(this);
  }

  reportItemsLater();
}

void SystemAvailabilityVectorController::detach() {
  if (m_modelObject) {
    m_modelObject.reset();
  }

  if (m_model) {
    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.disconnect<SystemAvailabilityVectorController, &SystemAvailabilityVectorController::reportItemsLater>(this);

    m_model.reset();
  }

  reportItemsLater();
}

void SystemAvailabilityVectorController::reportItemsLater() {
  m_reportScheduled = true;

  QTimer::singleShot(0, this, &SystemAvailabilityVectorController::reportItems);
}

void SystemAvailabilityVectorController::reportItems() {
  if (!m_reportItemsMutex->tryLock()) {
    return;
  }

  if (m_reportScheduled) {
    m_reportScheduled = false;

    ModelObjectVectorController::reportItems();
  }

  m_reportItemsMutex->unlock();
}

boost::optional<model::AirLoopHVAC> SystemAvailabilityVectorController::airLoopHVAC() {
  if (m_modelObject && !m_modelObject->handle().isNull()) {
    return m_modelObject->optionalCast<model::AirLoopHVAC>();
  } else {
    return boost::none;
  }
}

std::vector<OSItemId> SystemAvailabilityVectorController::makeVector() {
  std::vector<OSItemId> result;

  if (boost::optional<model::AirLoopHVAC> _airLoopHVAC = airLoopHVAC()) {
    result.push_back(modelObjectToItemId(_airLoopHVAC->availabilitySchedule(), false));
  }

  return result;
}

void SystemAvailabilityVectorController::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::AirLoopHVAC> _airLoopHVAC = airLoopHVAC()) {
    boost::optional<model::ModelObject> mo = this->getModelObject(itemId);

    if (mo) {
      if (mo->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          mo = mo->clone(_airLoopHVAC->model());
        }

        auto schedule = mo->cast<model::Schedule>();
        _airLoopHVAC->setAvailabilitySchedule(schedule);
      }
    }
  }
}

void SystemAvailabilityVectorController::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

SupplyAirTempScheduleVectorController::SupplyAirTempScheduleVectorController()
  : ModelObjectVectorController(), m_reportScheduled(false), m_reportItemsMutex(new QMutex()) {}

SupplyAirTempScheduleVectorController::~SupplyAirTempScheduleVectorController() {
  delete m_reportItemsMutex;
}

void SystemAvailabilityVectorController::onDropZoneItemClicked(OSItem* item) {
  OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObjectByItem(item, false);
}

void SupplyAirTempScheduleVectorController::attach(const model::ModelObject& modelObject) {
  detach();

  m_modelObject = modelObject;

  if (m_modelObject) {
    m_model = m_modelObject->model();

    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.connect<SupplyAirTempScheduleVectorController, &SupplyAirTempScheduleVectorController::reportItemsLater>(this);
  }

  reportItemsLater();
}

void SupplyAirTempScheduleVectorController::detach() {
  if (m_modelObject) {
    m_modelObject.reset();
  }

  if (m_model) {
    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.disconnect<SupplyAirTempScheduleVectorController, &SupplyAirTempScheduleVectorController::reportItemsLater>(this);

    m_model.reset();
  }

  reportItemsLater();
}

void SupplyAirTempScheduleVectorController::reportItemsLater() {
  m_reportScheduled = true;

  QTimer::singleShot(0, this, &SupplyAirTempScheduleVectorController::reportItems);
}

void SupplyAirTempScheduleVectorController::reportItems() {
  if (!m_reportItemsMutex->tryLock()) {
    return;
  }

  if (m_reportScheduled) {
    m_reportScheduled = false;

    ModelObjectVectorController::reportItems();
  }

  m_reportItemsMutex->unlock();
}

boost::optional<model::SetpointManagerScheduled> SupplyAirTempScheduleVectorController::setpointManagerScheduled() {
  if (m_modelObject && !m_modelObject->handle().isNull()) {
    return m_modelObject->optionalCast<model::SetpointManagerScheduled>();
  } else {
    return boost::none;
  }
}

std::vector<OSItemId> SupplyAirTempScheduleVectorController::makeVector() {
  std::vector<OSItemId> result;

  if (boost::optional<model::SetpointManagerScheduled> t_setpointManagerScheduled = setpointManagerScheduled()) {
    if (boost::optional<model::Schedule> schedule = t_setpointManagerScheduled->schedule()) {
      result.push_back(modelObjectToItemId(schedule.get(), false));
    }
  }

  return result;
}

void SupplyAirTempScheduleVectorController::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::SetpointManagerScheduled> t_setpointManagerScheduled = setpointManagerScheduled()) {
    boost::optional<model::ModelObject> mo = getModelObject(itemId);

    if (mo) {
      if (mo->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          mo = mo->clone(t_setpointManagerScheduled->model());
        }

        auto schedule = mo->cast<model::Schedule>();
        t_setpointManagerScheduled->setSchedule(schedule);
      }
    }
  }
}

void SupplyAirTempScheduleVectorController::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

void SupplyAirTempScheduleVectorController::onDropZoneItemClicked(OSItem* item) {
  OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObjectByItem(item, false);
}

/***********************************************************************************************************************
 *        A V A I L A B I L I T Y    M A N A G E R S - AvailabilityManagerObjectVectorController
 ***********************************************************************************************************************/

// CLASSIC ONES

// CTOR
AvailabilityManagerObjectVectorController::AvailabilityManagerObjectVectorController()
  : ModelObjectVectorController(), m_reportScheduled(false), m_reportItemsMutex(new QMutex()) {}

AvailabilityManagerObjectVectorController::~AvailabilityManagerObjectVectorController() {
  delete m_reportItemsMutex;
}

void AvailabilityManagerObjectVectorController::attach(const model::ModelObject& modelObject) {
  detach();

  m_modelObject = modelObject;

  if (m_modelObject) {
    m_model = m_modelObject->model();

    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.connect<AvailabilityManagerObjectVectorController, &AvailabilityManagerObjectVectorController::reportItemsLater>(this);
  }

  reportItemsLater();
}

void AvailabilityManagerObjectVectorController::detach() {
  if (m_modelObject) {
    m_modelObject.reset();
  }

  if (m_model) {
    m_model->getImpl<model::detail::Model_Impl>()
      ->onChange.disconnect<AvailabilityManagerObjectVectorController, &AvailabilityManagerObjectVectorController::reportItemsLater>(this);

    m_model.reset();
  }

  reportItemsLater();
}

void AvailabilityManagerObjectVectorController::reportItemsLater() {
  m_reportScheduled = true;

  QTimer::singleShot(0, this, &AvailabilityManagerObjectVectorController::reportItems);
}

void AvailabilityManagerObjectVectorController::reportItems() {
  if (!m_reportItemsMutex->tryLock()) {
    return;
  }

  if (m_reportScheduled) {
    m_reportScheduled = false;

    ModelObjectVectorController::reportItems();
  }

  m_reportItemsMutex->unlock();
}

// ACTUAL STUFF

boost::optional<model::PlantLoop> AvailabilityManagerObjectVectorController::currentLoop() {
  if (m_modelObject && !m_modelObject->handle().isNull()) {
    return m_modelObject->optionalCast<model::PlantLoop>();
  } else {
    return boost::none;
  }
}

boost::optional<model::AvailabilityManagerAssignmentList> AvailabilityManagerObjectVectorController::avmList() {
  if (m_modelObject && !m_modelObject->handle().isNull()) {
    if (auto mo = m_modelObject->optionalCast<model::Loop>()) {
      model::Loop loop = mo.get();
      return loop.getImpl<model::detail::Loop_Impl>()->availabilityManagerAssignmentList();
    }
    /*
     *if (auto mo = m_modelObject->optionalCast<model::PlantLoop>())
     *{
     *  model::PlantLoop p =  m_modelObject->cast<model::PlantLoop>();
     *  return p.getImpl<model::detail::PlantLoop_Impl>()->availabilityManagerAssignmentList();
     *}
     *else if (auto mo = m_modelObject->optionalCast<model::AirLoopHVAC>())
     *{
     *  model::AirLoopHVAC a =  m_modelObject->cast<model::AirLoopHVAC>();
     *  return a.getImpl<model::detail::AirLoopHVAC_Impl>()->availabilityManagerAssignmentList();
     *}
     */
  }

  return boost::none;
}

void AvailabilityManagerObjectVectorController::onChangeRelationship(const model::ModelObject& /*modelObject*/, int /*index*/, Handle /*newHandle*/,
                                                                     Handle /*oldHandle*/) {
  emit itemIds(makeVector());
}

void AvailabilityManagerObjectVectorController::onDataChange(const model::ModelObject& /*modelObject*/) {
  emit itemIds(makeVector());
}

std::vector<OSItemId> AvailabilityManagerObjectVectorController::makeVector() {
  std::vector<OSItemId> result;

  /*
   *if( boost::optional<model::PlantLoop> t_currentLoop = currentLoop() )
   *{
   *  for( const auto& avm: t_currentLoop->availabilityManagers() ) {
   *    result.push_back(modelObjectToItemId(avm, false));
   *  }
   *}
   */
  if (boost::optional<model::AvailabilityManagerAssignmentList> t_avmList = avmList()) {
    for (const auto& avm : t_avmList->availabilityManagers()) {
      result.push_back(modelObjectToItemId(avm, false));
    }
  }

  return result;
}

void AvailabilityManagerObjectVectorController::onRemoveItem(OSItem* item) {

  /**
  if( boost::optional<model::Loop> t_currentLoop = currentLoop() )

    model::LayeredConstruction construction = m_modelObject->cast<model::LayeredConstruction>();
    std::vector<model::Material> layers = construction.layers();
    OSAppBase * app = OSAppBase::instance();
    unsigned idx = 0;
    for (model::Material layer : layers){
      boost::optional<model::ModelObject> modelObject = app->currentDocument()->getModelObject(item->itemId());
      if(modelObject){
        if(modelObject->handle() == layer.handle()){
          construction.eraseLayer(idx);
          break;
        }
        idx++;
      }
    }
  }*/

  /*
   *if( boost::optional<model::PlantLoop> t_currentLoop = currentLoop() )
   *{
   *  // Start by getting the old one
   *  ModelObjectItem* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
   *  OS_ASSERT(modelObjectItem);
   *  model::ModelObject modelObject = modelObjectItem->modelObject();
   *  if (!modelObject.handle().isNull()) {
   *    // There is no reason we shouldn't enter this code block if the onDrop is fine...
   *    if (modelObject.optionalCast<model::AvailabilityManager>()) {
   *      model::AvailabilityManager c_avm = modelObject.cast<model::AvailabilityManager>();
   *      t_currentLoop->removeAvailabilityManager(c_avm);
   *    }
   *  }
   *}
   */

  if (boost::optional<model::AvailabilityManagerAssignmentList> t_avmList = avmList()) {
    // We get the avm
    auto* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
    OS_ASSERT(modelObjectItem);
    model::ModelObject modelObject = modelObjectItem->modelObject();
    if (!modelObject.handle().isNull()) {
      // There is no reason we shouldn't enter this code block if the onDrop is fine...
      if (auto avm_ = modelObject.optionalCast<model::AvailabilityManager>()) {
        // And we remove it
        t_avmList->removeAvailabilityManager(avm_.get());
      }
    }
  }
}

void AvailabilityManagerObjectVectorController::onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) {
  // Start by making sure the replacement object is indeed an AvailabilityManager
  if (boost::optional<model::AvailabilityManagerAssignmentList> t_avmList = avmList()) {
    boost::optional<model::ModelObject> mo = getModelObject(replacementItemId);
    if (mo) {
      if (mo->optionalCast<model::AvailabilityManager>()) {
        // If we drag from the library onto an existing, we want clone, then add at the priority of the one existing
        // It will shift all other AVMs forward, and the user will be able to delete the one he dragged onto if he wants
        bool fromLibrary = false;
        if (this->fromComponentLibrary(replacementItemId)) {
          fromLibrary = true;
          mo = mo->clone(t_avmList->model());
        }

        auto new_avm = mo->cast<model::AvailabilityManager>();

        // Now that we know it's an AVM, start by getting the one being dragged ONTO
        auto* modelObjectItem = qobject_cast<ModelObjectItem*>(currentItem);
        OS_ASSERT(modelObjectItem);
        model::ModelObject modelObject = modelObjectItem->modelObject();
        if (!modelObject.handle().isNull()) {
          // There is no reason we shouldn't enter this code block if the onDrop is fine...
          if (auto avm_ = modelObject.optionalCast<model::AvailabilityManager>()) {

            // Get the priority of the current avm
            unsigned priority = t_avmList->availabilityManagerPriority(avm_.get());

            if (fromLibrary) {
              // Insert the new avm at this priority
              t_avmList->addAvailabilityManager(new_avm, priority);
            } else {
              // We are dragging an existing AVM onto another existing one
              // We shift them instead, by setting the priority of the one being dragged to the one being dragger onto
              t_avmList->setAvailabilityManagerPriority(new_avm, priority);
            }
          }
        }
      }
    }
  }
}

/* On Drop, if it's an AvailabilityManager, call currentLoop->addAvailabilityManager */
void AvailabilityManagerObjectVectorController::onDrop(const OSItemId& itemId) {
  if (boost::optional<model::AvailabilityManagerAssignmentList> t_avmList = avmList()) {
    boost::optional<model::ModelObject> mo = getModelObject(itemId);

    if (mo) {
      if (mo->optionalCast<model::AvailabilityManager>()) {
        if (this->fromComponentLibrary(itemId)) {
          mo = mo->clone(t_avmList->model());
        }

        auto avm = mo->cast<model::AvailabilityManager>();
        t_avmList->addAvailabilityManager(avm);
      }
    }
  }
}

void AvailabilityManagerObjectVectorController::onDropZoneItemClicked(OSItem* item) {

  OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObjectByItem(item, false);

  /*
 *  if( boost::optional<model::AvailabilityManagerAssignmentList> t_avmList = avmList() )
 *  {
 *    // We get the avm
 *    ModelObjectItem* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
 *    OS_ASSERT(modelObjectItem);
 *    model::ModelObject modelObject = modelObjectItem->modelObject();
 *    if (!modelObject.handle().isNull()) {
 *      // There is no reason we shouldn't enter this code block if the onDrop is fine...
 *      if (modelObject.optionalCast<model::AvailabilityManager>()) {
 *        model::AvailabilityManager c_avm = modelObject.cast<model::AvailabilityManager>();
 *
 *        OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectModelObject(mo,false);
 *
 *        // inspectModelObjectByItem(OSItem * item, bool readOnly)
 *      }
 *    }
 *  }
 */
}

}  // namespace openstudio
