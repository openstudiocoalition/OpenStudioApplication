/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
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

#include <openstudio/model/Component.hpp>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/ComponentData_Impl.hpp>
#include <openstudio/model/Component_Impl.hpp>
#include <openstudio/model/ElectricEquipment.hpp>
#include <openstudio/model/ElectricEquipmentDefinition.hpp>
#include <openstudio/model/ElectricEquipmentDefinition_Impl.hpp>
#include <openstudio/model/ElectricEquipment_Impl.hpp>
#include <openstudio/model/GasEquipment.hpp>
#include <openstudio/model/GasEquipmentDefinition.hpp>
#include <openstudio/model/GasEquipmentDefinition_Impl.hpp>
#include <openstudio/model/GasEquipment_Impl.hpp>
#include <openstudio/model/InternalMass.hpp>
#include <openstudio/model/InternalMassDefinition.hpp>
#include <openstudio/model/InternalMassDefinition_Impl.hpp>
#include <openstudio/model/InternalMass_Impl.hpp>
#include <openstudio/model/Lights.hpp>
#include <openstudio/model/LightsDefinition.hpp>
#include <openstudio/model/LightsDefinition_Impl.hpp>
#include <openstudio/model/Lights_Impl.hpp>
#include <openstudio/model/Luminaire.hpp>
#include <openstudio/model/LuminaireDefinition.hpp>
#include <openstudio/model/LuminaireDefinition_Impl.hpp>
#include <openstudio/model/Luminaire_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/OtherEquipment.hpp>
#include <openstudio/model/OtherEquipmentDefinition.hpp>
#include <openstudio/model/OtherEquipmentDefinition_Impl.hpp>
#include <openstudio/model/OtherEquipment_Impl.hpp>
#include <openstudio/model/People.hpp>
#include <openstudio/model/PeopleDefinition.hpp>
#include <openstudio/model/PeopleDefinition_Impl.hpp>
#include <openstudio/model/People_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/SpaceLoad.hpp>
#include <openstudio/model/SpaceLoadDefinition.hpp>
#include <openstudio/model/SpaceLoadDefinition_Impl.hpp>
#include <openstudio/model/SpaceLoadInstance.hpp>
#include <openstudio/model/SpaceLoad_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/SteamEquipment.hpp>
#include <openstudio/model/SteamEquipmentDefinition.hpp>
#include <openstudio/model/SteamEquipmentDefinition_Impl.hpp>
#include <openstudio/model/SteamEquipment_Impl.hpp>
#include <openstudio/model/WaterUseEquipment.hpp>
#include <openstudio/model/WaterUseEquipmentDefinition.hpp>
#include <openstudio/model/WaterUseEquipmentDefinition_Impl.hpp>
#include <openstudio/model/WaterUseEquipment_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

SpaceTypesController::SpaceTypesController(bool isIP, const model::Model& model) : ModelSubTabController(new SpaceTypesView(isIP, model), model) {}

void SpaceTypesController::onAddObject(const openstudio::IddObjectType& iddObjectType) {
  OS_ASSERT(IddObjectType::OS_SpaceType == iddObjectType.value());
  openstudio::model::SpaceType(this->model());
}

void SpaceTypesController::onAddObject(const openstudio::model::ModelObject& modelObject) {
  if (IddObjectType::OS_SpaceType == modelObject.iddObjectType().value()) {
    openstudio::model::SpaceType(this->model());
    return;
  }

  boost::optional<model::ParentObject> parent = modelObject.parent();
  OS_ASSERT(parent);

  // Expect a load from the gridview loads tab
  switch (modelObject.iddObjectType().value()) {
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

void SpaceTypesController::onCopyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->model());
}

void SpaceTypesController::onRemoveObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

void SpaceTypesController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) {
  // not yet implemented
}

void SpaceTypesController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType) {
  for (model::SpaceType spaceType : this->model().getConcreteModelObjects<model::SpaceType>()) {
    if (spaceType.spaces().empty()) {
      spaceType.remove();
    }
  }
}

void SpaceTypesController::onDrop(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::SpaceType>()) {
      if (this->fromComponentLibrary(itemId)) {
        modelObject->clone(this->model());
      }
    }
  } else {
    boost::optional<model::Component> component = this->getComponent(itemId);
    if (component) {
      if (component->primaryObject().optionalCast<model::SpaceType>()) {
        // boost::optional<model::ComponentData> componentData
        this->model().insertComponent(*component);
      }
    }
  }
}

void SpaceTypesController::onInspectItem(OSItem* item) {}

}  // namespace openstudio
