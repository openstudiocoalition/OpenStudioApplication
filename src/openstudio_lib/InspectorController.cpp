/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "InspectorController.hpp"

#include "InspectorView.hpp"
#include "MainWindow.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include <openstudio/model/AirLoopHVAC.hpp>
#include <openstudio/model/AirLoopHVACReturnPlenum.hpp>
#include <openstudio/model/AirLoopHVACReturnPlenum_Impl.hpp>
#include <openstudio/model/AirLoopHVACSupplyPlenum.hpp>
#include <openstudio/model/AirLoopHVACSupplyPlenum_Impl.hpp>
#include <openstudio/model/AirLoopHVACZoneMixer.hpp>
#include <openstudio/model/AirLoopHVACZoneMixer_Impl.hpp>
#include <openstudio/model/AirLoopHVACZoneSplitter.hpp>
#include <openstudio/model/AirLoopHVACZoneSplitter_Impl.hpp>
#include <openstudio/model/AirLoopHVAC_Impl.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/PlantLoop.hpp>
#include <openstudio/model/PlantLoop_Impl.hpp>
#include <openstudio/model/ScheduleCompact.hpp>
#include <openstudio/model/ScheduleCompact_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/WaterToAirComponent.hpp>
#include <openstudio/model/WaterToAirComponent_Impl.hpp>
#include <openstudio/model/WaterHeaterMixed.hpp>
#include <openstudio/model/WaterHeaterMixed_Impl.hpp>
#include <openstudio/model/ZoneHVACFourPipeFanCoil.hpp>
#include <openstudio/model/ZoneHVACFourPipeFanCoil_Impl.hpp>
#include <openstudio/model/ZoneHVACPackagedTerminalAirConditioner.hpp>
#include <openstudio/model/ZoneHVACPackagedTerminalAirConditioner_Impl.hpp>
#include <openstudio/model/ZoneHVACUnitHeater.hpp>
#include <openstudio/model/ZoneHVACUnitHeater_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

InspectorController::InspectorController() : QObject() {
  m_inspectorView = new InspectorView();

  //auto isConnected = connect(m_inspectorView, SIGNAL(itemRemoveClicked(OSItem *)), this, SIGNAL(itemRemoveClicked(OSItem *))); TODO these should all be deleted from this area of the code: cruft
  //OS_ASSERT(isConnected);

  connect(m_inspectorView, &InspectorView::removeButtonClicked, this, &InspectorController::removeButtonClicked);

  connect(m_inspectorView, &InspectorView::workspaceObjectRemoved, this, &InspectorController::workspaceObjectRemoved);

  connect(m_inspectorView, &InspectorView::addZoneClicked, this, &InspectorController::addBranchForZone);

  connect(this, &InspectorController::toggleUnitsClicked, m_inspectorView, &InspectorView::toggleUnitsClicked);

  connect(this, &InspectorController::toggleDisplayAdditionalPropsClicked, m_inspectorView, &InspectorView::toggleDisplayAdditionalPropsClicked);
  connect(m_inspectorView, &InspectorView::removeZoneClicked, this, &InspectorController::removeBranchForZone);

  connect(m_inspectorView, &InspectorView::addToLoopClicked, this, &InspectorController::addToLoop);

  connect(m_inspectorView, &InspectorView::removeFromLoopClicked, this, &InspectorController::removeFromLoop);

  connect(m_inspectorView, &InspectorView::destroyed, this, &InspectorController::onViewDestroyed);

  connect(m_inspectorView, &InspectorView::moveBranchForZoneSupplySelected, this, &InspectorController::moveBranchForZoneSupply);

  connect(m_inspectorView, &InspectorView::moveBranchForZoneReturnSelected, this, &InspectorController::moveBranchForZoneReturn);
}

InspectorController::~InspectorController() {
  if (m_inspectorView != nullptr) {
    delete m_inspectorView;

    m_inspectorView = nullptr;
  }
}

InspectorView* InspectorController::inspectorView() {
  return m_inspectorView;
}

void InspectorController::layoutModelObject(model::OptionalModelObject& modelObject, bool readOnly) {
  bool displayIP = OSAppBase::instance()->currentDocument()->mainWindow()->displayIP();
  bool displayAdditionalProps = OSAppBase::instance()->currentDocument()->mainWindow()->displayAdditionalProps();

  m_inspectorView->layoutModelObject(modelObject, readOnly, displayIP, displayAdditionalProps);

  m_modelObject = modelObject;
}

void InspectorController::addBranchForZone(model::ThermalZone& zone) {
  if (model::OptionalHVACComponent hvacComponent = m_modelObject->optionalCast<model::HVACComponent>()) {
    if (model::OptionalAirLoopHVAC airLoop = hvacComponent->airLoopHVAC()) {
      airLoop->multiAddBranchForZone(zone);
    }
  }
}

void InspectorController::removeBranchForZone(model::ThermalZone& zone) {
  if (model::OptionalAirLoopHVAC airLoop = zone.airLoopHVAC()) {
    airLoop->removeBranchForZone(zone);
  }
}

void InspectorController::moveBranchForZoneSupply(model::ThermalZone& zone, const Handle& newPlenumHandle) {
  model::Model model = zone.model();

  if (boost::optional<model::AirLoopHVACSupplyPlenum> supplyPlenum = model.getModelObject<model::AirLoopHVACSupplyPlenum>(newPlenumHandle)) {
    if (boost::optional<model::ThermalZone> plenumZone = supplyPlenum->thermalZone()) {
      zone.setSupplyPlenum(plenumZone.get());
    }
  } else if (boost::optional<model::ThermalZone> plenumZone = model.getModelObject<model::ThermalZone>(newPlenumHandle)) {
    zone.setSupplyPlenum(plenumZone.get());
  } else {
    zone.removeSupplyPlenum();
  }

  // This updates the plenum chooser combo box
  // Need to process events first because HVAC scene needs to redraw to supply colors to combo box
  QApplication::instance()->processEvents();
  OS_ASSERT(m_inspectorView);
  m_inspectorView->update();
}

void InspectorController::moveBranchForZoneReturn(model::ThermalZone& zone, const Handle& newPlenumHandle) {
  model::Model model = zone.model();

  if (boost::optional<model::AirLoopHVACReturnPlenum> returnPlenum = model.getModelObject<model::AirLoopHVACReturnPlenum>(newPlenumHandle)) {
    if (boost::optional<model::ThermalZone> plenumZone = returnPlenum->thermalZone()) {
      zone.setReturnPlenum(plenumZone.get());
    }
  } else if (boost::optional<model::ThermalZone> plenumZone = model.getModelObject<model::ThermalZone>(newPlenumHandle)) {
    zone.setReturnPlenum(plenumZone.get());
  } else {
    zone.removeReturnPlenum();
  }

  // This updates the plenum chooser combo box
  // Need to process events first because HVAC scene needs to redraw to supply colors to combo box
  QApplication::instance()->processEvents();
  OS_ASSERT(m_inspectorView);
  m_inspectorView->update();
}

void InspectorController::onViewDestroyed(QObject* object) {
  m_inspectorView = nullptr;
}

void InspectorController::addToLoop(model::Loop& loop, boost::optional<model::HVACComponent>& hvacComponent) {
  if (hvacComponent) {
    if (boost::optional<model::PlantLoop> plantLoop = loop.optionalCast<model::PlantLoop>()) {
      if (hvacComponent->optionalCast<model::WaterHeaterMixed>()) {
        plantLoop->addSupplyBranchForComponent(hvacComponent.get());
      } else {
        plantLoop->addDemandBranchForComponent(hvacComponent.get());
      }
    }
  }

  //if( m_modelObject )
  //{
  //  if( boost::optional<model::PlantLoop> plantLoop = loop.optionalCast<model::PlantLoop>() )
  //  {
  //    if( boost::optional<model::AirTerminalSingleDuctVAVReheat> comp =
  //          m_modelObject->optionalCast<model::AirTerminalSingleDuctVAVReheat>() )
  //    {
  //      if( boost::optional<model::HVACComponent> coil = comp->reheatCoil() )
  //      {
  //        if( boost::optional<model::WaterToAirComponent> waterCoil = coil->optionalCast<model::WaterToAirComponent>() )
  //        {
  //          plantLoop->addDemandBranchForComponent( waterCoil.get() );
  //        }
  //      }
  //    }
  //    else if( boost::optional<model::ZoneHVACPackagedTerminalAirConditioner> comp =
  //          m_modelObject->optionalCast<model::ZoneHVACPackagedTerminalAirConditioner>() )
  //    {
  //      if( boost::optional<model::HVACComponent> coil = comp->heatingCoil() )
  //      {
  //        if( boost::optional<model::WaterToAirComponent> waterCoil = coil->optionalCast<model::WaterToAirComponent>() )
  //        {
  //          plantLoop->addDemandBranchForComponent( waterCoil.get() );
  //        }
  //      }
  //    }
  //    else if( boost::optional<model::ZoneHVACFourPipeFanCoil> comp =
  //          m_modelObject->optionalCast<model::ZoneHVACFourPipeFanCoil>() )
  //    {
  //      if( waterToAirComponent )
  //      {
  //        if( boost::optional<model::HVACComponent> coil = comp->heatingCoil() )
  //        {
  //          if( waterToAirComponent->handle() == coil->handle() )
  //          {
  //            plantLoop->addDemandBranchForComponent( waterToAirComponent.get() );
  //          }
  //        }
  //        if( boost::optional<model::HVACComponent> coil = comp->coolingCoil() )
  //        {
  //          if( waterToAirComponent->handle() == coil->handle() )
  //          {
  //            plantLoop->addDemandBranchForComponent( waterToAirComponent.get() );
  //          }
  //        }
  //      }
  //    }
  //    else if( boost::optional<model::HVACComponent> comp = m_modelObject->optionalCast<model::HVACComponent>() )
  //    {
  //      plantLoop->addDemandBranchForComponent( comp.get() );
  //    }
  //  }
  //}
}

void InspectorController::removeFromLoop(model::Loop& loop, boost::optional<model::HVACComponent>& hvacComponent) {
  if (hvacComponent) {
    if (boost::optional<model::PlantLoop> plantLoop = loop.optionalCast<model::PlantLoop>()) {
      if (hvacComponent->optionalCast<model::WaterHeaterMixed>()) {
        plantLoop->removeSupplyBranchWithComponent(hvacComponent.get());
      } else {
        plantLoop->removeDemandBranchWithComponent(hvacComponent.get());
      }
    }
  }
}

void InspectorController::toggleUnits(bool displayIP) {}

}  // namespace openstudio
