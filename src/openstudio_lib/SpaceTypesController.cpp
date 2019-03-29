/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2019, Alliance for Sustainable Energy, LLC, and other contributors. All rights reserved.
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

#include "SpaceTypesController.hpp"

#include "SpaceTypesView.hpp"

#include <openstudio/src/model/Component.hpp>
#include <openstudio/src/model/ComponentData.hpp>
#include <openstudio/src/model/ComponentData_Impl.hpp>
#include <openstudio/src/model/Component_Impl.hpp>
#include <openstudio/src/model/ElectricEquipment.hpp>
#include <openstudio/src/model/ElectricEquipmentDefinition.hpp>
#include <openstudio/src/model/ElectricEquipmentDefinition_Impl.hpp>
#include <openstudio/src/model/ElectricEquipment_Impl.hpp>
#include <openstudio/src/model/GasEquipment.hpp>
#include <openstudio/src/model/GasEquipmentDefinition.hpp>
#include <openstudio/src/model/GasEquipmentDefinition_Impl.hpp>
#include <openstudio/src/model/GasEquipment_Impl.hpp>
#include <openstudio/src/model/InternalMass.hpp>
#include <openstudio/src/model/InternalMassDefinition.hpp>
#include <openstudio/src/model/InternalMassDefinition_Impl.hpp>
#include <openstudio/src/model/InternalMass_Impl.hpp>
#include <openstudio/src/model/Lights.hpp>
#include <openstudio/src/model/LightsDefinition.hpp>
#include <openstudio/src/model/LightsDefinition_Impl.hpp>
#include <openstudio/src/model/Lights_Impl.hpp>
#include <openstudio/src/model/Luminaire.hpp>
#include <openstudio/src/model/LuminaireDefinition.hpp>
#include <openstudio/src/model/LuminaireDefinition_Impl.hpp>
#include <openstudio/src/model/Luminaire_Impl.hpp>
#include <openstudio/src/model/Model.hpp>
#include <openstudio/src/model/OtherEquipment.hpp>
#include <openstudio/src/model/OtherEquipmentDefinition.hpp>
#include <openstudio/src/model/OtherEquipmentDefinition_Impl.hpp>
#include <openstudio/src/model/OtherEquipment_Impl.hpp>
#include <openstudio/src/model/People.hpp>
#include <openstudio/src/model/PeopleDefinition.hpp>
#include <openstudio/src/model/PeopleDefinition_Impl.hpp>
#include <openstudio/src/model/People_Impl.hpp>
#include <openstudio/src/model/Space.hpp>
#include <openstudio/src/model/SpaceLoad.hpp>
#include <openstudio/src/model/SpaceLoadDefinition.hpp>
#include <openstudio/src/model/SpaceLoadDefinition_Impl.hpp>
#include <openstudio/src/model/SpaceLoadInstance.hpp>
#include <openstudio/src/model/SpaceLoad_Impl.hpp>
#include <openstudio/src/model/SpaceType.hpp>
#include <openstudio/src/model/SpaceType_Impl.hpp>
#include <openstudio/src/model/SteamEquipment.hpp>
#include <openstudio/src/model/SteamEquipmentDefinition.hpp>
#include <openstudio/src/model/SteamEquipmentDefinition_Impl.hpp>
#include <openstudio/src/model/SteamEquipment_Impl.hpp>
#include <openstudio/src/model/WaterUseEquipment.hpp>
#include <openstudio/src/model/WaterUseEquipmentDefinition.hpp>
#include <openstudio/src/model/WaterUseEquipmentDefinition_Impl.hpp>
#include <openstudio/src/model/WaterUseEquipment_Impl.hpp>

#include <openstudio/src/utilities/core/Assert.hpp>

#include <openstudio/src/utilities/idd/IddEnums.hpp>
#include <openstudio/src/utilities/idd/IddEnums.hxx>

namespace openstudio {

  SpaceTypesController::SpaceTypesController(bool isIP,
    const model::Model& model)
    : ModelSubTabController(new SpaceTypesView(isIP, model), model)
{
}

void SpaceTypesController::onAddObject(const openstudio::IddObjectType& iddObjectType)
{
  OS_ASSERT(IddObjectType::OS_SpaceType == iddObjectType.value());
  openstudio::model::SpaceType(this->model());
}

void SpaceTypesController::onAddObject(const openstudio::model::ModelObject& modelObject)
{
  if (IddObjectType::OS_SpaceType == modelObject.iddObjectType().value()) {
    openstudio::model::SpaceType(this->model());
    return;
  }

  boost::optional<model::ParentObject> parent = modelObject.parent();
  OS_ASSERT(parent);

  // Expect a load from the gridview loads tab
  switch (modelObject.iddObjectType().value()){
    case IddObjectType::OS_People:
      openstudio::model::People(openstudio::model::PeopleDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_InternalMass:
      openstudio::model::InternalMass(openstudio::model::InternalMassDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_Lights:
      openstudio::model::Lights(openstudio::model::LightsDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_Luminaire:
      openstudio::model::Luminaire(openstudio::model::LuminaireDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_ElectricEquipment:
      openstudio::model::ElectricEquipment(openstudio::model::ElectricEquipmentDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_GasEquipment:
      openstudio::model::GasEquipment(openstudio::model::GasEquipmentDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_SteamEquipment:
      openstudio::model::SteamEquipment(openstudio::model::SteamEquipmentDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_OtherEquipment:
      openstudio::model::OtherEquipment(openstudio::model::OtherEquipmentDefinition(this->model())).setParent(*parent);
      break;
    case IddObjectType::OS_WaterUse_Equipment:
      openstudio::model::WaterUseEquipment(openstudio::model::WaterUseEquipmentDefinition(this->model())).setParent(*parent);
      break;
    default:
      // Should not get here
      OS_ASSERT(false);
      LOG_FREE(Error, "LoadsController", "Unknown IddObjectType '" << modelObject.iddObjectType().valueName() << "'");
  }

}

void SpaceTypesController::onCopyObject(const openstudio::model::ModelObject& modelObject)
{
  modelObject.clone(this->model());
}

void SpaceTypesController::onRemoveObject(openstudio::model::ModelObject modelObject)
{
  modelObject.remove();
}

void SpaceTypesController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId)
{
  // not yet implemented
}

void SpaceTypesController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType)
{
  for (model::SpaceType spaceType : this->model().getConcreteModelObjects<model::SpaceType>()){
    if (spaceType.spaces().empty()){
      spaceType.remove();
    }
  }
}

void SpaceTypesController::onDrop(const OSItemId& itemId)
{
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject){
    if (modelObject->optionalCast<model::SpaceType>()){
      if (this->fromComponentLibrary(itemId)){
        modelObject = modelObject->clone(this->model());
      }
    }
  }else{
    boost::optional<model::Component> component = this->getComponent(itemId);
    if (component){
      if (component->primaryObject().optionalCast<model::SpaceType>()){
        boost::optional<model::ComponentData> componentData = this->model().insertComponent(*component);
      }
    }
  }
}

void SpaceTypesController::onInspectItem(OSItem* item)
{
}

} // openstudio
