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

#include "ConstructionObjectVectorController.hpp"

#include "ModelObjectItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include <openstudio/model/Construction.hpp>
#include <openstudio/model/Construction_Impl.hpp>
#include <openstudio/model/Material.hpp>
#include <openstudio/model/Material_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/OS_Construction_FieldEnums.hxx>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QMessageBox>
#include <QTimer>
#include <QMutex>

namespace openstudio {

// ConstructionObjectVectorController

ConstructionObjectVectorController::ConstructionObjectVectorController(QWidget* parentWidget)
  : ModelObjectVectorController(), m_reportScheduled(false), m_reportItemsMutex(new QMutex()), m_parentWidget(parentWidget) {}

ConstructionObjectVectorController::~ConstructionObjectVectorController() {
  delete m_reportItemsMutex;
}

void ConstructionObjectVectorController::reportItemsLater() {
  m_reportScheduled = true;

  QTimer::singleShot(0, this, &ConstructionObjectVectorController::reportItems);
}

void ConstructionObjectVectorController::reportItems() {
  if (!m_reportItemsMutex->tryLock()) {
    return;
  }

  if (m_reportScheduled) {
    m_reportScheduled = false;

    ModelObjectVectorController::reportItems();
  }

  m_reportItemsMutex->unlock();
}

void ConstructionObjectVectorController::onChangeRelationship(const model::ModelObject& /*modelObject*/, int /*index*/, Handle /*newHandle*/,
                                                              Handle /*oldHandle*/) {
  reportItemsLater();
}

void ConstructionObjectVectorController::onDataChange(const model::ModelObject& /*modelObject*/) {
  reportItemsLater();
}

void ConstructionObjectVectorController::onChange(const model::ModelObject& /*modelObject*/) {
  reportItemsLater();
}

std::vector<OSItemId> ConstructionObjectVectorController::makeVector() {
  std::vector<OSItemId> result;
  if (m_modelObject) {
    auto construction = m_modelObject->cast<model::LayeredConstruction>();
    std::vector<model::Material> layers = construction.layers();
    for (const model::Material& layer : layers) {
      result.push_back(modelObjectToItemId(layer, false));
    }
  }
  return result;
}

void ConstructionObjectVectorController::onRemoveItem(OSItem* item) {
  if (m_modelObject) {
    auto construction = m_modelObject->cast<model::LayeredConstruction>();
    std::vector<model::Material> layers = construction.layers();
    OSAppBase* app = OSAppBase::instance();
    unsigned idx = 0;
    for (const model::Material& layer : layers) {
      boost::optional<model::ModelObject> modelObject = app->currentDocument()->getModelObject(item->itemId());
      if (modelObject) {
        if (modelObject->handle() == layer.handle()) {
          construction.eraseLayer(idx);
          break;
        }
        idx++;
      }
    }
  }
}

void ConstructionObjectVectorController::insert(const OSItemId& itemId, int insertPosition, boost::optional<int> erasePosition_) {
  if (m_modelObject) {
    boost::optional<model::Material> material_ = this->addToModel<model::Material>(itemId);
    if (!material_) {
      return;
    }

    auto construction = m_modelObject->cast<model::LayeredConstruction>();
    std::vector<model::Material> layers = construction.layers();
    if (!layers.empty()) {

      IddObjectType existingIddObjectType = layers.at(0).iddObjectType();
      IddObjectType newIddObjectType = material_.get().iddObjectType();

      LayerType existingLayerType = getLayerType(existingIddObjectType);
      LayerType newLayerType = getLayerType(newIddObjectType);

      // Need a valid widget to hang the msgbox on
      OS_ASSERT(this->parentWidget());

      if (newLayerType != existingLayerType) {
        // New layer type must match existing layer type
        QMessageBox::warning(this->parentWidget(), "Error Adding Layer", "New layer type must match existing layer type.", QMessageBox::Ok);
        return;
      }
    }

    if (insertPosition < 0) {
      insertPosition = construction.numLayers();
    }

    if (construction.insertLayer(insertPosition, material_.get()) && erasePosition_) {
      construction.eraseLayer(erasePosition_.get());
    }
  }
}

void ConstructionObjectVectorController::onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) {
  if (m_modelObject) {

    // currentItem position is set by OSDropZone::setItemIds
    boost::optional<int> toPosition_ = currentItem->position();
    boost::optional<int> fromPosition_ = replacementItemId.position();

    // If we drag from the library onto an existing, we want clone, then add at the position of the one existing
    // It will shift all other layers forward, and the user will be able to delete the one he dragged onto if he wants
    // If not from library, we want to **move** the item instead.
    if (this->fromComponentLibrary(replacementItemId)) {
      fromPosition_.reset();
    }

    if (toPosition_ && fromPosition_) {
      if (toPosition_.get() == fromPosition_.get()) {
        // to and from position are same, no-op
        return;
      } else if (toPosition_.get() < fromPosition_.get()) {
        // position to delete will be one higher after doing the insert
        fromPosition_ = fromPosition_.get() + 1;
      }
    }

    if (!toPosition_) {
      // insert at the end
      toPosition_ = -1;
    }

    insert(replacementItemId, toPosition_.get(), fromPosition_);
  }
}

void ConstructionObjectVectorController::onDrop(const OSItemId& itemId) {

  boost::optional<int> fromPosition = itemId.position();

  // If we drag from the library onto an existing, we want clone, then add at the position of the one existing
  // It will shift all other layers forward, and the user will be able to delete the one he dragged onto if he wants
  // If not from library, we want to **move** the item instead.
  if (this->fromComponentLibrary(itemId)) {
    fromPosition.reset();
  }

  insert(itemId, -1, fromPosition);
}

QWidget* ConstructionObjectVectorController::parentWidget() {
  return m_parentWidget;
}

void ConstructionObjectVectorController::setParentWidget(QWidget* parentWidget) {
  m_parentWidget = parentWidget;
}

ConstructionObjectVectorController::LayerType ConstructionObjectVectorController::getLayerType(IddObjectType iddObjectType) {
  if (iddObjectType == IddObjectType::OS_WindowMaterial_Blind || iddObjectType == IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice
      || iddObjectType == IddObjectType::OS_WindowMaterial_Gas || iddObjectType == IddObjectType::OS_WindowMaterial_GasMixture
      || iddObjectType == IddObjectType::OS_WindowMaterial_Glazing
      || iddObjectType == IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod
      || iddObjectType == IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic || iddObjectType == IddObjectType::OS_WindowMaterial_Screen
      || iddObjectType == IddObjectType::OS_WindowMaterial_Shade || iddObjectType == IddObjectType::OS_WindowMaterial_SimpleGlazingSystem) {
    return ConstructionObjectVectorController::FENESTRATION;
  } else if (iddObjectType == IddObjectType::OS_Material || iddObjectType == IddObjectType::OS_Material_AirGap
             || iddObjectType == IddObjectType::OS_Material_InfraredTransparent || iddObjectType == IddObjectType::OS_Material_NoMass
             || iddObjectType == IddObjectType::OS_Material_RoofVegetation) {
    return ConstructionObjectVectorController::OPAQUE;
  } else {
    // Should never get here
    OS_ASSERT(false);
    return ConstructionObjectVectorController::UNKNOWN;
  }
}

}  // namespace openstudio
