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

#include "SpaceTypesGridView.hpp"

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelSubTabView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"
#include "../shared_gui_components/OSWidgetHolder.hpp"

#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/DefaultScheduleSet_Impl.hpp>
#include <openstudio/model/DesignSpecificationOutdoorAir.hpp>
#include <openstudio/model/DesignSpecificationOutdoorAir_Impl.hpp>
#include <openstudio/model/ElectricEquipment.hpp>
#include <openstudio/model/ElectricEquipmentDefinition.hpp>
#include <openstudio/model/ElectricEquipmentDefinition_Impl.hpp>
#include <openstudio/model/ElectricEquipment_Impl.hpp>
#include <openstudio/model/GasEquipment.hpp>
#include <openstudio/model/GasEquipmentDefinition.hpp>
#include <openstudio/model/GasEquipmentDefinition_Impl.hpp>
#include <openstudio/model/GasEquipment_Impl.hpp>
#include <openstudio/model/HotWaterEquipment.hpp>
#include <openstudio/model/HotWaterEquipmentDefinition.hpp>
#include <openstudio/model/HotWaterEquipmentDefinition_Impl.hpp>
#include <openstudio/model/HotWaterEquipment_Impl.hpp>
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
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/OtherEquipment.hpp>
#include <openstudio/model/OtherEquipmentDefinition.hpp>
#include <openstudio/model/OtherEquipmentDefinition_Impl.hpp>
#include <openstudio/model/OtherEquipment_Impl.hpp>
#include <openstudio/model/People.hpp>
#include <openstudio/model/PeopleDefinition.hpp>
#include <openstudio/model/PeopleDefinition_Impl.hpp>
#include <openstudio/model/People_Impl.hpp>
#include <openstudio/model/RenderingColor.hpp>
#include <openstudio/model/RenderingColor_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea_Impl.hpp>
#include <openstudio/model/SpaceLoad.hpp>
#include <openstudio/model/SpaceLoadDefinition.hpp>
#include <openstudio/model/SpaceLoadDefinition_Impl.hpp>
#include <openstudio/model/SpaceLoadInstance.hpp>
#include <openstudio/model/SpaceLoadInstance_Impl.hpp>
#include <openstudio/model/SpaceLoad_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/SteamEquipment.hpp>
#include <openstudio/model/SteamEquipmentDefinition.hpp>
#include <openstudio/model/SteamEquipmentDefinition_Impl.hpp>
#include <openstudio/model/SteamEquipment_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_SpaceType_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Space Type Name"
#define SELECTED "All"

// GENERAL
#define RENDERINGCOLOR "Rendering Color"
#define DEFAULTCONSTRUCTIONSET "Default Construction Set"
#define DEFAULTSCHEDULESET "Default Schedule Set"
#define DESIGNSPECIFICATIONOUTDOORAIR "Design Specification Outdoor Air"
#define SPACEINFILTRATIONDESIGNFLOWRATES "Space Infiltration Design Flow Rates"
#define SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS "Space Infiltration Effective Leakage Areas"

// LOADS
#define LOADNAME "Load Name"
#define MULTIPLIER "Multiplier"
#define DEFINITION "Definition"
#define SCHEDULE "Schedule"
#define ACTIVITYSCHEDULE "Activity Schedule\n(People Only)"

// MEASURE TAGS
#define STANDARDSTEMPLATE "Standards Template (Optional)"
#define STANDARDSBUILDINGTYPE "Standards Building Type\n(Optional)"
#define STANDARDSSPACETYPE "Standards Space Type\n(Optional)"

// LOAD TYPES
#define SHOWALLLOADS "Show all loads"
#define INTERNALMASS "Internal Mass"
#define PEOPLE "People"
#define LIGHTS "Lights"
#define LUMINAIRE "Luminaire"
#define ELECTRICEQUIPMENT "Electric Equipment"
#define GASEQUIPMENT "Gas Equipment"
#define HOTWATEREQUIPMENT "Hot Water Equipment"
#define STEAMEQUIPMENT "Steam Equipment"
#define OTHEREQUIPMENT "Other Equipment"
#define SPACEINFILTRATIONDESIGNFLOWRATE "Space Infiltration Design Flow Rate"
#define SPACEINFILTRATIONEFFECTIVELEAKAGEAREA "Space Infiltration Effective Leakage Area"

namespace openstudio {

SpaceTypesGridView::SpaceTypesGridView(bool isIP, const model::Model& model, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(mainLayout);

  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();
  auto spaceTypeModelObjects = subsetCastVector<model::ModelObject>(spaceTypes);

  m_gridController = new SpaceTypesGridController(m_isIP, "Space Types", IddObjectType::OS_SpaceType, model, spaceTypeModelObjects);
  auto* gridView = new OSGridView(m_gridController, "Space Types", "Drop\nSpace Type", false, parent);

  // Load Filter

  QLabel* label = nullptr;

  QVBoxLayout* layout = nullptr;

  auto* filterGridLayout = new QGridLayout();
  filterGridLayout->setContentsMargins(7, 4, 0, 8);
  filterGridLayout->setSpacing(5);

  label = new QLabel();
  label->setText("Filter:");
  label->setObjectName("H2");
  filterGridLayout->addWidget(label, filterGridLayout->rowCount(), 0, Qt::AlignTop | Qt::AlignLeft);

  layout = new QVBoxLayout();

  m_filterLabel = new QLabel();
  m_filterLabel->setText("Load Type");
  m_filterLabel->setObjectName("H3");
  layout->addWidget(m_filterLabel, Qt::AlignTop | Qt::AlignLeft);

  m_filters = new QComboBox();
  m_filters->setFixedWidth(1.5 * OSItem::ITEM_WIDTH);

  { m_filters->addItem(SHOWALLLOADS); }

  {
    const QPixmap* pixMap = new QPixmap(":images/mini_icons/internal_mass.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, INTERNALMASS);
  }

  {
    const QPixmap* pixMap = new QPixmap(":images/mini_icons/people.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, PEOPLE);
  }

  {
    const QPixmap* pixMap = new QPixmap(":images/mini_icons/lights.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, LIGHTS);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/luminaire.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, LUMINAIRE);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/electric_equipment.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, ELECTRICEQUIPMENT);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/gas_equipment.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, GASEQUIPMENT);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/steam_equipment.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, HOTWATEREQUIPMENT);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/steam_equipment.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, STEAMEQUIPMENT);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/other_equipment.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, OTHEREQUIPMENT);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/infiltration.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, SPACEINFILTRATIONDESIGNFLOWRATE);
  }

  {
    const auto* pixMap = new QPixmap(":images/mini_icons/mini_infiltration_leak.png");
    OS_ASSERT(pixMap);
    m_filters->addItem(*pixMap, SPACEINFILTRATIONEFFECTIVELEAKAGEAREA);
  }

  disableFilter();
  layout->addWidget(m_filters, Qt::AlignTop | Qt::AlignLeft);

  layout->addStretch();

  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, 1);

  filterGridLayout->setRowStretch(filterGridLayout->rowCount(), 100);
  filterGridLayout->setColumnStretch(filterGridLayout->columnCount(), 100);

  gridView->addLayoutToContentLayout(filterGridLayout);
  gridView->addSpacingToContentLayout(7);

  mainLayout->addWidget(gridView, 0, Qt::AlignTop);

  mainLayout->addStretch(1);

  // GridController

  OS_ASSERT(m_gridController);

  connect(m_filters, &QComboBox::currentTextChanged, m_gridController, &SpaceTypesGridController::filterChanged);

  connect(gridView, &OSGridView::dropZoneItemClicked, this, &SpaceTypesGridView::dropZoneItemClicked);

  connect(this, &SpaceTypesGridView::selectionCleared, m_gridController, &SpaceTypesGridController::onSelectionCleared);

  connect(this, &SpaceTypesGridView::toggleUnitsClicked, m_gridController, &SpaceTypesGridController::toggleUnitsClicked);

  connect(this, &SpaceTypesGridView::toggleUnitsClicked, m_gridController, &SpaceTypesGridController::onToggleUnits);

  // std::vector<model::SpaceType> spaceType = model.getConcreteModelObjects<model::SpaceType>();  // NOTE for horizontal system lists
}

std::set<model::ModelObject> SpaceTypesGridView::selectedObjects() const {
  return m_gridController->selectedObjects();
}

void SpaceTypesGridView::enableFilter() {
  m_filterLabel->setEnabled(true);
  m_filters->setEnabled(true);
}

void SpaceTypesGridView::disableFilter() {
  m_filterLabel->setEnabled(false);
  m_filters->setEnabled(false);
}

SpaceTypesGridController::SpaceTypesGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                                   const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void SpaceTypesGridController::setCategoriesAndFields() {

  {
    std::vector<QString> fields{
      RENDERINGCOLOR,
      DEFAULTCONSTRUCTIONSET,
      DEFAULTSCHEDULESET,
      DESIGNSPECIFICATIONOUTDOORAIR,
      SPACEINFILTRATIONDESIGNFLOWRATES,
      SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS,
    };
    addCategoryAndFields(QString("General"), std::move(fields));
  }

  {
    std::vector<QString> fields{
      LOADNAME, MULTIPLIER, DEFINITION, SCHEDULE, ACTIVITYSCHEDULE,
    };
    addCategoryAndFields(QString("Loads"), std::move(fields));
  }

  {
    std::vector<QString> fields{
      STANDARDSTEMPLATE,
      STANDARDSBUILDINGTYPE,
      STANDARDSSPACETYPE,
    };
    addCategoryAndFields(QString("Measure\nTags"), std::move(fields));
  }

  OSGridController::setCategoriesAndFields();
}

void SpaceTypesGridController::filterChanged(const QString& text) {
  std::set<openstudio::model::ModelObject> allFilteredObjects;

  if (text == SHOWALLLOADS) {
    // nothing to filter
  } else {

    // ObjectSelector::m_selectableObjects returns Load objects directly
    for (const auto& obj : this->selectorObjects()) {
      if (text == INTERNALMASS) {
        if (!obj.optionalCast<model::InternalMass>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == PEOPLE) {
        if (!obj.optionalCast<model::People>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == LIGHTS) {
        if (!obj.optionalCast<model::Lights>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == LUMINAIRE) {
        if (!obj.optionalCast<model::Luminaire>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == ELECTRICEQUIPMENT) {
        if (!obj.optionalCast<model::ElectricEquipment>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == GASEQUIPMENT) {
        if (!obj.optionalCast<model::GasEquipment>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == HOTWATEREQUIPMENT) {
        if (!obj.optionalCast<model::HotWaterEquipment>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == STEAMEQUIPMENT) {
        if (!obj.optionalCast<model::SteamEquipment>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == OTHEREQUIPMENT) {
        if (!obj.optionalCast<model::OtherEquipment>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == SPACEINFILTRATIONDESIGNFLOWRATE) {
        if (!obj.optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          allFilteredObjects.insert(obj);
        }
      } else if (text == SPACEINFILTRATIONEFFECTIVELEAKAGEAREA) {
        if (!obj.optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          allFilteredObjects.insert(obj);
        }
      } else {
        // Should never get here
        OS_ASSERT(false);
      }
    }
  }

  this->setObjectFilter([allFilteredObjects](const model::ModelObject& obj) -> bool {
    // return false if object in allFilteredObjects
    return allFilteredObjects.count(obj) == 0;
  });
}

SpaceTypesGridView* SpaceTypesGridController::spaceTypesGridView() {
  auto* gridView = qobject_cast<OSGridView*>(this->parent());
  OS_ASSERT(gridView);

  auto* spaceTypesGridView = qobject_cast<SpaceTypesGridView*>(gridView->parent());

  return spaceTypesGridView;
}

void SpaceTypesGridController::onCategorySelected(int index) {
  auto* gridView = this->spaceTypesGridView();
  if (gridView) {
    if (gridView->m_filters) {
      gridView->m_filters->setCurrentIndex(0);
    }
  }

  if (spaceTypesGridView()) {
    // Enable filter when viewing loads tab...
    if (index == 1) {
      spaceTypesGridView()->enableFilter();
    }
    // ...or if the custom tab has a loads column
    else if (index == 3) {
      auto categoriesAndFields = this->categoriesAndFields();
      auto fields = categoriesAndFields.at(index);
      for (const auto& field : fields.second) {
        if (field == LOADNAME) {
          spaceTypesGridView()->enableFilter();
          break;
        }
      }
    } else {
      spaceTypesGridView()->disableFilter();
    }
  }

  OSGridController::onCategorySelected(index);
}

void SpaceTypesGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      auto getter = CastNullAdapter<model::SpaceType>(&model::SpaceType::name);
      auto setter = CastNullAdapter<model::SpaceType>(&model::SpaceType::setNameProtected);

      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, getter, setter);

    } else if (field == SELECTED && category != "Loads") {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpaceTypesGridController::onSelectAllStateChanged);
      connect(this, &SpaceTypesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);

      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == LOADNAME || field == MULTIPLIER || field == DEFINITION || field == SCHEDULE || field == ACTIVITYSCHEDULE
               || field == SELECTED) {
      // Create a lambda function that collates all of the loads in a space type
      // and returns them as an std::vector
      std::function<std::vector<model::ModelObject>(const model::SpaceType&)> allLoads([](const model::SpaceType& t_spaceType) {
        std::vector<model::ModelObject> loads;
        auto InternalMass = t_spaceType.internalMass();
        auto People = t_spaceType.people();
        auto Lights = t_spaceType.lights();
        auto Luminaire = t_spaceType.luminaires();
        auto ElectricEquipment = t_spaceType.electricEquipment();
        auto GasEquipment = t_spaceType.gasEquipment();
        auto HotWaterEquipment = t_spaceType.hotWaterEquipment();
        auto SteamEquipment = t_spaceType.steamEquipment();
        auto OtherEquipment = t_spaceType.otherEquipment();
        auto SpaceInfiltrationDesignFlowRate = t_spaceType.spaceInfiltrationDesignFlowRates();
        auto SpaceInfiltrationEffectiveLeakageArea = t_spaceType.spaceInfiltrationEffectiveLeakageAreas();

        loads.insert(loads.end(), InternalMass.begin(), InternalMass.end());
        loads.insert(loads.end(), People.begin(), People.end());
        loads.insert(loads.end(), Lights.begin(), Lights.end());
        loads.insert(loads.end(), Luminaire.begin(), Luminaire.end());
        loads.insert(loads.end(), ElectricEquipment.begin(), ElectricEquipment.end());
        loads.insert(loads.end(), GasEquipment.begin(), GasEquipment.end());
        loads.insert(loads.end(), HotWaterEquipment.begin(), HotWaterEquipment.end());
        loads.insert(loads.end(), SteamEquipment.begin(), SteamEquipment.end());
        loads.insert(loads.end(), OtherEquipment.begin(), OtherEquipment.end());
        loads.insert(loads.end(), SpaceInfiltrationDesignFlowRate.begin(), SpaceInfiltrationDesignFlowRate.end());
        loads.insert(loads.end(), SpaceInfiltrationEffectiveLeakageArea.begin(), SpaceInfiltrationEffectiveLeakageArea.end());

        return loads;
      });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> allLoadInstances(
        [allLoads](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> loadInstances;
          for (const auto& l : allLoads(t_spaceType)) {
            loadInstances.emplace_back(l.optionalCast<model::SpaceLoadInstance>());
          }
          return loadInstances;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> allLoadsWithSchedules(
        [allLoads](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> retval;
          for (auto& l : allLoads(t_spaceType)) {
            // internal mass does not have a schedule
            if (!l.optionalCast<model::InternalMass>()) {
              retval.emplace_back(std::move(l));
            } else {
              retval.emplace_back();
            }
          }
          return retval;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> allLoadsWithActivityLevelSchedules(
        [allLoads](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> retval;
          for (auto&& l : allLoads(t_spaceType)) {
            // only people have activity schedules, so this effectively gives us only
            // the People objects while inserting blanks for those which are not people,
            // which is what we want
            if (l.optionalCast<model::People>()) {
              retval.emplace_back(std::move(l));
            } else {
              retval.emplace_back();
            }
          }
          return retval;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> allDefinitions(
        [allLoadInstances](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> definitions;
          for (const auto& l : allLoadInstances(t_spaceType)) {
            if (l) {
              definitions.emplace_back(l->cast<model::SpaceLoadInstance>().definition());
            } else {
              definitions.emplace_back();
            }
          }
          return definitions;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> allLoadsWithMultipliers(
        [](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> loads;
          auto InternalMass = t_spaceType.internalMass();
          auto People = t_spaceType.people();
          auto Lights = t_spaceType.lights();
          auto Luminaire = t_spaceType.luminaires();
          auto ElectricEquipment = t_spaceType.electricEquipment();
          auto GasEquipment = t_spaceType.gasEquipment();
          auto HotWaterEquipment = t_spaceType.hotWaterEquipment();
          auto SteamEquipment = t_spaceType.steamEquipment();
          auto OtherEquipment = t_spaceType.otherEquipment();
          auto SpaceInfiltrationDesignFlowRate = t_spaceType.spaceInfiltrationDesignFlowRates();
          auto SpaceInfiltrationEffectiveLeakageArea = t_spaceType.spaceInfiltrationEffectiveLeakageAreas();

          loads.insert(loads.end(), InternalMass.begin(), InternalMass.end());
          loads.insert(loads.end(), People.begin(), People.end());
          loads.insert(loads.end(), Lights.begin(), Lights.end());
          loads.insert(loads.end(), Luminaire.begin(), Luminaire.end());
          loads.insert(loads.end(), ElectricEquipment.begin(), ElectricEquipment.end());
          loads.insert(loads.end(), GasEquipment.begin(), GasEquipment.end());
          loads.insert(loads.end(), HotWaterEquipment.begin(), HotWaterEquipment.end());
          loads.insert(loads.end(), SteamEquipment.begin(), SteamEquipment.end());
          loads.insert(loads.end(), OtherEquipment.begin(), OtherEquipment.end());
          //loads.insert(loads.end(), SpaceInfiltrationDesignFlowRate.begin(), SpaceInfiltrationDesignFlowRate.end());
          //loads.insert(loads.end(), SpaceInfiltrationEffectiveLeakageArea.begin(), SpaceInfiltrationEffectiveLeakageArea.end());

          for (unsigned i = 0; i < SpaceInfiltrationDesignFlowRate.size(); ++i) {
            loads.emplace_back();
          }

          for (unsigned i = 0; i < SpaceInfiltrationEffectiveLeakageArea.size(); ++i) {
            loads.emplace_back();
          }

          return loads;
        });

      std::function<double(model::ModelObject*)> multiplier([allLoads](model::ModelObject* t_modelObject) {
        double retval = 0;

        boost::optional<model::InternalMass> im = t_modelObject->optionalCast<model::InternalMass>();
        if (im) {
          retval = im->multiplier();
          return retval;
        }

        boost::optional<model::People> p = t_modelObject->optionalCast<model::People>();
        if (p) {
          retval = p->multiplier();
          return retval;
        }

        boost::optional<model::Lights> light = t_modelObject->optionalCast<model::Lights>();
        if (light) {
          retval = light->multiplier();
          return retval;
        }

        boost::optional<model::Luminaire> lum = t_modelObject->optionalCast<model::Luminaire>();
        if (lum) {
          retval = lum->multiplier();
          return retval;
        }

        boost::optional<model::ElectricEquipment> e = t_modelObject->optionalCast<model::ElectricEquipment>();
        if (e) {
          retval = e->multiplier();
          return retval;
        }

        boost::optional<model::GasEquipment> g = t_modelObject->optionalCast<model::GasEquipment>();
        if (g) {
          retval = g->multiplier();
          return retval;
        }

        boost::optional<model::HotWaterEquipment> h = t_modelObject->optionalCast<model::HotWaterEquipment>();
        if (h) {
          retval = h->multiplier();
          return retval;
        }

        boost::optional<model::SteamEquipment> se = t_modelObject->optionalCast<model::SteamEquipment>();
        if (se) {
          retval = se->multiplier();
          return retval;
        }

        boost::optional<model::OtherEquipment> o = t_modelObject->optionalCast<model::OtherEquipment>();
        if (o) {
          retval = o->multiplier();
          return retval;
        }

        // Should never get here
        OS_ASSERT(false);
        return retval;
      });

      std::function<void(model::ModelObject*, double)> setMultiplier([](model::ModelObject* t_modelObject, double multiplier) {
        boost::optional<model::InternalMass> im = t_modelObject->optionalCast<model::InternalMass>();
        if (im) {
          im->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::People> p = t_modelObject->optionalCast<model::People>();
        if (p) {
          p->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::Lights> light = t_modelObject->optionalCast<model::Lights>();
        if (light) {
          light->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::Luminaire> lum = t_modelObject->optionalCast<model::Luminaire>();
        if (lum) {
          lum->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::ElectricEquipment> e = t_modelObject->optionalCast<model::ElectricEquipment>();
        if (e) {
          e->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::GasEquipment> g = t_modelObject->optionalCast<model::GasEquipment>();
        if (g) {
          g->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::HotWaterEquipment> h = t_modelObject->optionalCast<model::HotWaterEquipment>();
        if (h) {
          h->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::SteamEquipment> se = t_modelObject->optionalCast<model::SteamEquipment>();
        if (se) {
          se->setMultiplier(multiplier);
          return;
        }

        boost::optional<model::OtherEquipment> o = t_modelObject->optionalCast<model::OtherEquipment>();
        if (o) {
          o->setMultiplier(multiplier);
          return;
        }

        // Should never get here
        OS_ASSERT(false);
      });

      const boost::optional<std::function<void(model::ModelObject*)>> resetMultiplier([](model::ModelObject* t_modelObject) {
        boost::optional<model::InternalMass> im = t_modelObject->optionalCast<model::InternalMass>();
        if (im) {
          im->resetMultiplier();
          return;
        }

        boost::optional<model::People> p = t_modelObject->optionalCast<model::People>();
        if (p) {
          p->resetMultiplier();
          return;
        }

        boost::optional<model::Lights> light = t_modelObject->optionalCast<model::Lights>();
        if (light) {
          light->resetMultiplier();
          return;
        }

        boost::optional<model::Luminaire> lum = t_modelObject->optionalCast<model::Luminaire>();
        if (lum) {
          lum->resetMultiplier();
          return;
        }

        boost::optional<model::ElectricEquipment> e = t_modelObject->optionalCast<model::ElectricEquipment>();
        if (e) {
          e->resetMultiplier();
          return;
        }

        boost::optional<model::GasEquipment> g = t_modelObject->optionalCast<model::GasEquipment>();
        if (g) {
          g->resetMultiplier();
          return;
        }

        boost::optional<model::HotWaterEquipment> h = t_modelObject->optionalCast<model::HotWaterEquipment>();
        if (h) {
          h->resetMultiplier();
          return;
        }

        boost::optional<model::SteamEquipment> se = t_modelObject->optionalCast<model::SteamEquipment>();
        if (se) {
          se->resetMultiplier();
          return;
        }

        boost::optional<model::OtherEquipment> o = t_modelObject->optionalCast<model::OtherEquipment>();
        if (o) {
          o->resetMultiplier();
          return;
        }

        // Should never get here
        OS_ASSERT(false);
      });

      const boost::optional<std::function<bool(model::ModelObject*)>> isMultiplierDefaulted([](model::ModelObject* t_modelObject) {
        boost::optional<model::InternalMass> im = t_modelObject->optionalCast<model::InternalMass>();
        if (im) {
          return im->isMultiplierDefaulted();
        }

        boost::optional<model::People> p = t_modelObject->optionalCast<model::People>();
        if (p) {
          return p->isMultiplierDefaulted();
        }

        boost::optional<model::Lights> light = t_modelObject->optionalCast<model::Lights>();
        if (light) {
          return light->isMultiplierDefaulted();
        }

        boost::optional<model::Luminaire> lum = t_modelObject->optionalCast<model::Luminaire>();
        if (lum) {
          return lum->isMultiplierDefaulted();
        }

        boost::optional<model::ElectricEquipment> e = t_modelObject->optionalCast<model::ElectricEquipment>();
        if (e) {
          return e->isMultiplierDefaulted();
        }

        boost::optional<model::GasEquipment> g = t_modelObject->optionalCast<model::GasEquipment>();
        if (g) {
          return g->isMultiplierDefaulted();
        }

        boost::optional<model::HotWaterEquipment> h = t_modelObject->optionalCast<model::HotWaterEquipment>();
        if (h) {
          return h->isMultiplierDefaulted();
        }

        boost::optional<model::SteamEquipment> se = t_modelObject->optionalCast<model::SteamEquipment>();
        if (se) {
          return se->isMultiplierDefaulted();
        }

        boost::optional<model::OtherEquipment> o = t_modelObject->optionalCast<model::OtherEquipment>();
        if (o) {
          return o->isMultiplierDefaulted();
        }

        // Should never get here
        OS_ASSERT(false);
        return false;
      });

      std::function<bool(model::ModelObject*, const model::Schedule&)> setActivityLevelSchedule([](model::ModelObject* l, model::Schedule t_s) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->setActivityLevelSchedule(t_s);
        }

        OS_ASSERT(false);
        return false;
      });

      boost::optional<std::function<void(model::ModelObject*)>> resetActivityLevelSchedule([](model::ModelObject* l) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          p->resetActivityLevelSchedule();
        } else {
          //OS_ASSERT(false); TODO
        }
      });

      boost::optional<std::function<bool(model::ModelObject*)>> isActivityLevelScheduleDefaulted([](model::ModelObject* l) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->isActivityLevelScheduleDefaulted();
        }

        OS_ASSERT(false);
        return false;
      });

      std::function<bool(model::ModelObject*, const model::Schedule&)> setSchedule([](model::ModelObject* l, model::Schedule t_s) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->setNumberofPeopleSchedule(t_s);
        }

        if (boost::optional<model::Lights> light = l->optionalCast<model::Lights>()) {
          return light->setSchedule(t_s);
        }

        if (boost::optional<model::Luminaire> lum = l->optionalCast<model::Luminaire>()) {
          return lum->setSchedule(t_s);
        }

        if (boost::optional<model::ElectricEquipment> e = l->optionalCast<model::ElectricEquipment>()) {
          return e->setSchedule(t_s);
        }

        if (boost::optional<model::GasEquipment> g = l->optionalCast<model::GasEquipment>()) {
          return g->setSchedule(t_s);
        }

        if (boost::optional<model::HotWaterEquipment> h = l->optionalCast<model::HotWaterEquipment>()) {
          return h->setSchedule(t_s);
        }

        if (boost::optional<model::SteamEquipment> se = l->optionalCast<model::SteamEquipment>()) {
          return se->setSchedule(t_s);
        }

        if (boost::optional<model::OtherEquipment> o = l->optionalCast<model::OtherEquipment>()) {
          return o->setSchedule(t_s);
        }

        if (boost::optional<model::SpaceInfiltrationDesignFlowRate> f = l->optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          return f->setSchedule(t_s);
        }

        if (boost::optional<model::SpaceInfiltrationEffectiveLeakageArea> la = l->optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          return la->setSchedule(t_s);
        }

        OS_ASSERT(false);
        return false;
      });

      boost::optional<std::function<void(model::ModelObject*)>> resetSchedule([](model::ModelObject* l) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          p->resetNumberofPeopleSchedule();
        } else if (boost::optional<model::Lights> light = l->optionalCast<model::Lights>()) {
          light->resetSchedule();
        } else if (boost::optional<model::Luminaire> lum = l->optionalCast<model::Luminaire>()) {
          lum->resetSchedule();
        } else if (boost::optional<model::ElectricEquipment> e = l->optionalCast<model::ElectricEquipment>()) {
          e->resetSchedule();
        } else if (boost::optional<model::GasEquipment> g = l->optionalCast<model::GasEquipment>()) {
          g->resetSchedule();
        } else if (boost::optional<model::HotWaterEquipment> h = l->optionalCast<model::HotWaterEquipment>()) {
          h->resetSchedule();
        } else if (boost::optional<model::SteamEquipment> se = l->optionalCast<model::SteamEquipment>()) {
          se->resetSchedule();
        } else if (boost::optional<model::OtherEquipment> o = l->optionalCast<model::OtherEquipment>()) {
          o->resetSchedule();
        } else if (boost::optional<model::SpaceInfiltrationDesignFlowRate> f = l->optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          f->resetSchedule();
        } else if (boost::optional<model::SpaceInfiltrationEffectiveLeakageArea> la =
                     l->optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          la->resetSchedule();
        } else if (boost::optional<model::InternalMass> im = l->optionalCast<model::InternalMass>()) {
          // Note: InternalMass does not have a schedule
        } else {
          OS_ASSERT(false);
        }
      });

      boost::optional<std::function<bool(model::ModelObject*)>> isScheduleDefaulted([](model::ModelObject* l) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->isNumberofPeopleScheduleDefaulted();
        } else if (boost::optional<model::Lights> light = l->optionalCast<model::Lights>()) {
          return light->isScheduleDefaulted();
        } else if (boost::optional<model::Luminaire> lum = l->optionalCast<model::Luminaire>()) {
          return lum->isScheduleDefaulted();
        } else if (boost::optional<model::ElectricEquipment> e = l->optionalCast<model::ElectricEquipment>()) {
          return e->isScheduleDefaulted();
        } else if (boost::optional<model::GasEquipment> g = l->optionalCast<model::GasEquipment>()) {
          return g->isScheduleDefaulted();
        } else if (boost::optional<model::HotWaterEquipment> h = l->optionalCast<model::HotWaterEquipment>()) {
          return h->isScheduleDefaulted();
        } else if (boost::optional<model::SteamEquipment> se = l->optionalCast<model::SteamEquipment>()) {
          return se->isScheduleDefaulted();
        } else if (boost::optional<model::OtherEquipment> o = l->optionalCast<model::OtherEquipment>()) {
          return o->isScheduleDefaulted();
        } else if (boost::optional<model::SpaceInfiltrationDesignFlowRate> f = l->optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          return f->isScheduleDefaulted();
        } else if (boost::optional<model::SpaceInfiltrationEffectiveLeakageArea> la =
                     l->optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          return la->isScheduleDefaulted();
        } else if (boost::optional<model::InternalMass> im = l->optionalCast<model::InternalMass>()) {
          // Note: InternalMass does not have a schedule
          return false;
        } else {
          OS_ASSERT(false);
          return false;
        }
      });

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ModelObject*)>> scheduleOtherObjects;

      std::function<boost::optional<model::Schedule>(model::ModelObject*)> activityLevelSchedule([](model::ModelObject* l) {
        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->activityLevelSchedule();
        }

        // should be impossible to get here
        OS_ASSERT(false);
        return boost::optional<model::Schedule>();
      });

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ModelObject*)>> activityLevelScheduleOtherObjects;

      std::function<boost::optional<model::Schedule>(model::ModelObject*)> schedule([](model::ModelObject* l) {
        if (boost::optional<model::InternalMass> im = l->optionalCast<model::InternalMass>()) {
          // Note: InternalMass does not have a schedule
          return boost::optional<model::Schedule>();
        }

        if (boost::optional<model::People> p = l->optionalCast<model::People>()) {
          return p->numberofPeopleSchedule();
        }

        if (boost::optional<model::Lights> light = l->optionalCast<model::Lights>()) {
          return light->schedule();
        }

        if (boost::optional<model::Luminaire> lum = l->optionalCast<model::Luminaire>()) {
          return lum->schedule();
        }

        if (boost::optional<model::ElectricEquipment> e = l->optionalCast<model::ElectricEquipment>()) {
          return e->schedule();
        }

        if (boost::optional<model::GasEquipment> g = l->optionalCast<model::GasEquipment>()) {
          return g->schedule();
        }

        if (boost::optional<model::HotWaterEquipment> h = l->optionalCast<model::HotWaterEquipment>()) {
          return h->schedule();
        }

        if (boost::optional<model::SteamEquipment> se = l->optionalCast<model::SteamEquipment>()) {
          return se->schedule();
        }

        if (boost::optional<model::OtherEquipment> o = l->optionalCast<model::OtherEquipment>()) {
          return o->schedule();
        }

        if (boost::optional<model::SpaceInfiltrationDesignFlowRate> f = l->optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          return f->schedule();
        }

        if (boost::optional<model::SpaceInfiltrationEffectiveLeakageArea> la = l->optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          return la->schedule();
        }

        // should be impossible to get here
        OS_ASSERT(false);
        return boost::optional<model::Schedule>();
      });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> schedules(
        [allLoads, schedule](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> retval;

          for (auto& l : allLoads(t_spaceType)) {
            retval.emplace_back(schedule(&l));
          }

          return retval;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::SpaceType&)> activityLevelSchedules(
        [allLoads](const model::SpaceType& t_spaceType) {
          std::vector<boost::optional<model::ModelObject>> retval;

          for (const auto& l : allLoads(t_spaceType)) {
            boost::optional<model::People> p = l.optionalCast<model::People>();
            if (p) {
              retval.emplace_back(p->activityLevelSchedule());
            } else {
              retval.emplace_back();
            }
          }

          return retval;
        });

      if (field == LOADNAME) {

        // Here we create a NameLineEdit column, but this one includes a "DataSource" object
        // The DataSource object is used in OSGridController::widgetAt to make a list of NameLineEdit widgets
        // for each SpaceType that is passed in.
        //
        // Notice that it takes the "allLoads" functor from above.
        //
        // Just as an implementation note, it would be possible to use the DataSource as an alternative
        // to the ProxyAdapter function, if the DataSource were to return a vector of 1.
        //
        // The final argument to DataSource tells the system that we want an additional widget to be displayed
        // at the bottom of each list. In this case, it's a dropZone. Any type of BaseConcept would work.

        addLoadNameColumn(Heading(QString(LOADNAME), true, false), CastNullAdapter<model::SpaceLoad>(&model::SpaceLoad::name),
                          CastNullAdapter<model::SpaceLoad>(&model::SpaceLoad::setName),
                          boost::optional<std::function<void(model::SpaceLoad*)>>(
                            std::function<void(model::SpaceLoad*)>([](model::SpaceLoad* t_sl) { t_sl->remove(); })),
                          boost::optional<std::function<bool(model::SpaceLoad*)>>(), DataSource(allLoads, true));

      } else if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpaceTypesGridController::onSelectAllStateChanged);
        connect(this, &SpaceTypesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row", DataSource(allLoads, true));
      } else if (field == MULTIPLIER) {

        addValueEditColumn(
          Heading(QString(MULTIPLIER)), multiplier, setMultiplier, resetMultiplier, isMultiplierDefaulted,
          DataSource(allLoadsWithMultipliers, true
                     //QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<ValueType, DataSourceType>(headingLabel, getter, setter)
                     ));

      } else if (field == DEFINITION) {
        std::function<boost::optional<model::SpaceLoadDefinition>(model::SpaceType*)> getter;

        std::function<bool(model::SpaceType*, const model::SpaceLoadDefinition&)> setter(
          [](model::SpaceType* t_spaceType, const model::SpaceLoadDefinition& t_definition) {
            boost::optional<model::InternalMassDefinition> im = t_definition.optionalCast<model::InternalMassDefinition>();
            if (im) {
              model::InternalMass(*im).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::PeopleDefinition> p = t_definition.optionalCast<model::PeopleDefinition>();
            if (p) {
              model::People(*p).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::LightsDefinition> light = t_definition.optionalCast<model::LightsDefinition>();
            if (light) {
              model::Lights(*light).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::LuminaireDefinition> lum = t_definition.optionalCast<model::LuminaireDefinition>();
            if (lum) {
              model::Luminaire(*lum).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::ElectricEquipmentDefinition> e = t_definition.optionalCast<model::ElectricEquipmentDefinition>();
            if (e) {
              model::ElectricEquipment(*e).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::GasEquipmentDefinition> g = t_definition.optionalCast<model::GasEquipmentDefinition>();
            if (g) {
              model::GasEquipment(*g).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::HotWaterEquipmentDefinition> h = t_definition.optionalCast<model::HotWaterEquipmentDefinition>();
            if (h) {
              model::HotWaterEquipment(*h).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::SteamEquipmentDefinition> se = t_definition.optionalCast<model::SteamEquipmentDefinition>();
            if (se) {
              model::SteamEquipment(*se).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::OtherEquipmentDefinition> o = t_definition.optionalCast<model::OtherEquipmentDefinition>();
            if (o) {
              model::OtherEquipment(*o).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::SpaceInfiltrationDesignFlowRate> dfr = t_definition.optionalCast<model::SpaceInfiltrationDesignFlowRate>();
            if (dfr) {
              model::SpaceInfiltrationDesignFlowRate(*dfr).setParent(*t_spaceType);
              return true;
            }

            boost::optional<model::SpaceInfiltrationEffectiveLeakageArea> ela =
              t_definition.optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>();
            if (ela) {
              model::SpaceInfiltrationEffectiveLeakageArea(*ela).setParent(*t_spaceType);
              return true;
            }

            return false;
          });

        addNameLineEditColumn(
          Heading(QString(DEFINITION), true, false), true, true, CastNullAdapter<model::SpaceLoadDefinition>(&model::SpaceLoadDefinition::name),
          CastNullAdapter<model::SpaceLoadDefinition>(&model::SpaceLoadDefinition::setName),
          boost::optional<std::function<void(model::SpaceLoadDefinition*)>>(), boost::optional<std::function<bool(model::SpaceLoadDefinition*)>>(),
          DataSource(allDefinitions, false,
                     QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::SpaceLoadDefinition, model::SpaceType>(
                       Heading(DEFINITION), getter, setter, boost::none, boost::none, boost::none))));

      } else if (field == SCHEDULE) {

        addDropZoneColumn(Heading(QString(SCHEDULE)), schedule, setSchedule, resetSchedule, isScheduleDefaulted, scheduleOtherObjects,
                          DataSource(allLoadsWithSchedules, true));

      } else if (field == ACTIVITYSCHEDULE) {

        addDropZoneColumn(Heading(QString(SCHEDULE)), activityLevelSchedule, setActivityLevelSchedule, resetActivityLevelSchedule,
                          isActivityLevelScheduleDefaulted, activityLevelScheduleOtherObjects, DataSource(allLoadsWithActivityLevelSchedules, true));
      }

    } else if (field == DEFAULTCONSTRUCTIONSET) {
      addDropZoneColumn(
        Heading(QString(DEFAULTCONSTRUCTIONSET)), CastNullAdapter<model::SpaceType>(&model::SpaceType::defaultConstructionSet),
        CastNullAdapter<model::SpaceType>(&model::SpaceType::setDefaultConstructionSet),
        boost::optional<std::function<void(model::SpaceType*)>>(CastNullAdapter<model::SpaceType>(&model::SpaceType::resetDefaultConstructionSet)));

    } else if (field == DEFAULTSCHEDULESET) {
      addDropZoneColumn(
        Heading(QString(DEFAULTSCHEDULESET)), CastNullAdapter<model::SpaceType>(&model::SpaceType::defaultScheduleSet),
        CastNullAdapter<model::SpaceType>(&model::SpaceType::setDefaultScheduleSet),
        boost::optional<std::function<void(model::SpaceType*)>>(CastNullAdapter<model::SpaceType>(&model::SpaceType::resetDefaultScheduleSet)));

    } else if (field == DESIGNSPECIFICATIONOUTDOORAIR) {
      addDropZoneColumn(Heading(QString(DESIGNSPECIFICATIONOUTDOORAIR)),
                        CastNullAdapter<model::SpaceType>(&model::SpaceType::designSpecificationOutdoorAir),
                        CastNullAdapter<model::SpaceType>(&model::SpaceType::setDesignSpecificationOutdoorAir),
                        boost::optional<std::function<void(model::SpaceType*)>>(
                          CastNullAdapter<model::SpaceType>(&model::SpaceType::resetDesignSpecificationOutdoorAir)),
                        boost::optional<std::function<bool(model::SpaceType*)>>(
                          CastNullAdapter<model::SpaceType>(&model::SpaceType::isDesignSpecificationOutdoorAirDefaulted)));

    } else if (field == RENDERINGCOLOR) {
      addRenderingColorColumn(Heading(QString(RENDERINGCOLOR), true, false), CastNullAdapter<model::SpaceType>(&model::SpaceType::renderingColor),
                              CastNullAdapter<model::SpaceType>(&model::SpaceType::setRenderingColor));

    } else if (field == SPACEINFILTRATIONDESIGNFLOWRATES) {
      std::function<boost::optional<model::SpaceInfiltrationDesignFlowRate>(model::SpaceType*)> getter;

      std::function<bool(model::SpaceType*, const model::SpaceInfiltrationDesignFlowRate&)> setter(
        [](model::SpaceType* t_type, model::SpaceInfiltrationDesignFlowRate t_rate) {
          if (t_rate.spaceType()) {
            boost::optional<model::ModelObject> clone_rate = t_rate.clone(t_rate.model());
            OS_ASSERT(clone_rate);
            auto success = clone_rate->cast<model::SpaceInfiltrationDesignFlowRate>().setSpaceType(*t_type);
            OS_ASSERT(success);
            return success;
          } else {
            auto success = t_rate.setSpaceType(*t_type);
            OS_ASSERT(success);
            return success;
          }
        });

      std::function<std::vector<model::ModelObject>(const model::SpaceType&)> flowRates([](const model::SpaceType& s) {
        auto rates = s.spaceInfiltrationDesignFlowRates();
        return std::vector<model::ModelObject>(rates.begin(), rates.end());
      });

      addNameLineEditColumn(
        Heading(QString(SPACEINFILTRATIONDESIGNFLOWRATES)), true, false,
        CastNullAdapter<model::SpaceInfiltrationDesignFlowRate>(&model::SpaceInfiltrationDesignFlowRate::name),
        CastNullAdapter<model::SpaceInfiltrationDesignFlowRate>(&model::SpaceInfiltrationDesignFlowRate::setName),
        boost::optional<std::function<void(model::SpaceInfiltrationDesignFlowRate*)>>(
          std::function<void(model::SpaceInfiltrationDesignFlowRate*)>([](model::SpaceInfiltrationDesignFlowRate* t_fr) { t_fr->remove(); })),
        boost::optional<std::function<bool(model::SpaceInfiltrationDesignFlowRate*)>>(),
        DataSource(flowRates, false,
                   QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::SpaceInfiltrationDesignFlowRate, model::SpaceType>(
                     Heading(SPACEINFILTRATIONDESIGNFLOWRATES), getter, setter, boost::none, boost::none, boost::none))));

    } else if (field == SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS) {
      std::function<boost::optional<model::SpaceInfiltrationEffectiveLeakageArea>(model::SpaceType*)> getter;

      std::function<bool(model::SpaceType*, const model::SpaceInfiltrationEffectiveLeakageArea&)> setter(
        [](model::SpaceType* t_type, model::SpaceInfiltrationEffectiveLeakageArea t_area) {
          if (t_area.spaceType()) {
            boost::optional<model::ModelObject> clone_area = t_area.clone(t_area.model());
            OS_ASSERT(clone_area);
            auto success = clone_area->cast<model::SpaceInfiltrationEffectiveLeakageArea>().setSpaceType(*t_type);
            OS_ASSERT(success);
            return success;
          } else {
            auto success = t_area.setSpaceType(*t_type);
            OS_ASSERT(success);
            return success;
          }
        });

      std::function<std::vector<model::ModelObject>(const model::SpaceType&)> leakageAreas([](const model::SpaceType& s) {
        auto areas = s.spaceInfiltrationEffectiveLeakageAreas();
        return std::vector<model::ModelObject>(areas.begin(), areas.end());
      });

      addNameLineEditColumn(
        Heading(QString(SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS)), true, false,
        CastNullAdapter<model::SpaceInfiltrationEffectiveLeakageArea>(&model::SpaceInfiltrationEffectiveLeakageArea::name),
        CastNullAdapter<model::SpaceInfiltrationEffectiveLeakageArea>(&model::SpaceInfiltrationEffectiveLeakageArea::setName),
        boost::optional<std::function<void(model::SpaceInfiltrationEffectiveLeakageArea*)>>(
          std::function<void(model::SpaceInfiltrationEffectiveLeakageArea*)>(
            [](model::SpaceInfiltrationEffectiveLeakageArea* t_la) { t_la->remove(); })),
        boost::optional<std::function<bool(model::SpaceInfiltrationEffectiveLeakageArea*)>>(),
        DataSource(leakageAreas, false,
                   QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::SpaceInfiltrationEffectiveLeakageArea, model::SpaceType>(
                     Heading(SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS), getter, setter, boost::none, boost::none, boost::none))));

    } else if (field == STANDARDSTEMPLATE) {

      std::function<std::string(const std::string&)> toStringNoOp = [](std::string t_s) { return t_s; };

      std::function<std::vector<std::string>(model::SpaceType*)> choices = [](model::SpaceType* t_spaceType) {
        std::vector<std::string> retval;

        const auto& types = t_spaceType->suggestedStandardsTemplates();

        retval.insert(retval.end(), types.begin(), types.end());
        return retval;
      };

      std::function<boost::optional<std::string>(model::SpaceType*)> getter = [](model::SpaceType* t_spaceType) {
        return t_spaceType->standardsTemplate();
      };

      std::function<bool(model::SpaceType*, std::string)> setter = [this](model::SpaceType* t_spaceType, std::string t_value) {
        // We start by resetting the Standards Building Type and SpaceType
        t_spaceType->resetStandardsSpaceType();
        t_spaceType->resetStandardsBuildingType();
        bool success = t_spaceType->setStandardsTemplate(t_value);

        // Note: JM 2018-08-23
        // Because we want **dependent** dropdown lists, we need to find a way to connect
        // a change in STANDARDSTEMPLATE to trigger a refresh of STANDARDSBUILDINGTYPE AND STANDARDSSPACETYPE
        // This is a hack (at best), but it works
        // Get the corresponding Standards Building Type Dropdown, and trigger repopulating

        int columnCount = this->columnCount();
        for (int i = 1; i < this->rowCount(); ++i) {
          if (this->modelObjectFromGridRow(i).handle() == t_spaceType->handle()) {
            auto* gridView = qobject_cast<OSGridView*>(this->parent());
            if (QLayoutItem* child = gridView->itemAtPosition(i, columnCount - 2)) {

              // Standards Building Type is penultimate
              QWidget* t_widgetStandardsBuildingType = child->widget();
              // 0 appears to be GridLayout, 1 is a Holder
              QObject* oBT = t_widgetStandardsBuildingType->children()[1];
              auto* holderBT = qobject_cast<OSWidgetHolder*>(oBT);
              if (holderBT) {
                auto* comboBoxBuildingType = qobject_cast<OSComboBox2*>(holderBT->widget());
                if (comboBoxBuildingType) {
                  comboBoxBuildingType->onChoicesRefreshTrigger();
                }
              }
            }

            // Standards Space Type is last
            if (QLayoutItem* child = gridView->itemAtPosition(i, columnCount - 1)) {

              // Standards Building Type is penultimate
              QWidget* t_widgetStandardsSpaceType = child->widget();
              // 0 appears to be GridLayout, 1 is a Holder
              QObject* oST = t_widgetStandardsSpaceType->children()[1];
              auto* holderST = qobject_cast<OSWidgetHolder*>(oST);
              if (holderST) {
                auto* comboBoxSpaceType = qobject_cast<OSComboBox2*>(holderST->widget());
                if (comboBoxSpaceType) {
                  comboBoxSpaceType->onChoicesRefreshTrigger();
                }
              }
            }
            break;
          }
        }

        return success;
      };

      boost::optional<std::function<void(model::SpaceType*)>> resetter([](model::SpaceType* t_spaceType) {
        // Reset all three standards info (dependent)
        t_spaceType->resetStandardsSpaceType();
        t_spaceType->resetStandardsBuildingType();
        t_spaceType->resetStandardsTemplate();
      });

      // Note: It will end up creating a ComboBoxOptionalChoiceImpl
      addComboBoxColumn(Heading(QString(STANDARDSTEMPLATE)), toStringNoOp, choices, getter, setter, resetter,
                        boost::none,  // No DataSource
                        // Make editable
                        true);
    } else if (field == STANDARDSBUILDINGTYPE) {

      std::function<std::string(const std::string&)> toStringNoOp = [](std::string t_s) { return t_s; };

      std::function<std::vector<std::string>(model::SpaceType*)> choices = [](model::SpaceType* t_spaceType) {
        std::vector<std::string> retval;

        const auto& types = t_spaceType->suggestedStandardsBuildingTypes();

        retval.insert(retval.end(), types.begin(), types.end());
        return retval;
      };

      std::function<boost::optional<std::string>(model::SpaceType*)> getter = [](model::SpaceType* t_spaceType) {
        return t_spaceType->standardsBuildingType();
      };

      std::function<bool(model::SpaceType*, std::string)> setter = [this](model::SpaceType* t_spaceType, std::string t_value) {
        // We start by resetting the Standards SpaceType
        t_spaceType->resetStandardsSpaceType();
        bool success = t_spaceType->setStandardsBuildingType(t_value);

        // Note: JM 2018-08-23
        // Because we want **dependent** dropdown lists, we need to find a way to connect
        // a change in STANDARDSBUILDINGTYPE to trigger a refresh of STANDARDSSPACETYPE
        // This is a hack (at best), but it works
        // Get the corresponding Standards Space Type Dropdown, and trigger repopulating

        int columnCount = this->columnCount();
        for (int i = 1; i < this->rowCount(); ++i) {
          if (this->modelObjectFromGridRow(i).handle() == t_spaceType->handle()) {

            auto* gridView = qobject_cast<OSGridView*>(this->parent());

            // Standards Space Type is last
            if (QLayoutItem* child = gridView->itemAtPosition(i, columnCount - 1)) {

              // Standards Building Type is penultimate
              QWidget* t_widgetStandardsSpaceType = child->widget();
              // 0 appears to be GridLayout, 1 is a Holder
              QObject* oST = t_widgetStandardsSpaceType->children()[1];
              auto* holderST = qobject_cast<OSWidgetHolder*>(oST);
              if (holderST) {
                auto* comboBoxSpaceType = qobject_cast<OSComboBox2*>(holderST->widget());
                if (comboBoxSpaceType) {
                  comboBoxSpaceType->onChoicesRefreshTrigger();
                }
              }
            }
            break;
          }
        }

        return success;
      };

      boost::optional<std::function<void(model::SpaceType*)>> resetter([](model::SpaceType* t_spaceType) {
        // Reset Building Type and SpaceType
        t_spaceType->resetStandardsSpaceType();
        t_spaceType->resetStandardsBuildingType();
      });

      // Note: It will end up creating a ComboBoxOptionalChoiceImpl
      addComboBoxColumn(Heading(QString(STANDARDSBUILDINGTYPE)), toStringNoOp, choices, getter, setter, resetter,
                        boost::none,  // No DataSource
                        // Make editable
                        true);
    } else if (field == STANDARDSSPACETYPE) {

      std::function<std::string(const std::string&)> toStringNoOp = [](std::string t_s) { return t_s; };

      std::function<std::vector<std::string>(model::SpaceType*)> choices = [](model::SpaceType* t_spaceType) {
        std::vector<std::string> retval;

        const auto& types = t_spaceType->suggestedStandardsSpaceTypes();

        retval.insert(retval.end(), types.begin(), types.end());
        return retval;
      };

      std::function<boost::optional<std::string>(model::SpaceType*)> getter = [](model::SpaceType* t_spaceType) {
        return t_spaceType->standardsSpaceType();
      };

      std::function<bool(model::SpaceType * t_spaceType, std::string)> setter = [](model::SpaceType* t_spaceType, std::string t_val) {
        return t_spaceType->setStandardsSpaceType(t_val);
      };

      boost::optional<std::function<void(model::SpaceType * t_spaceType)>> resetter(
        [](model::SpaceType* t_spaceType) { t_spaceType->resetStandardsSpaceType(); });

      addComboBoxColumn(Heading(QString(STANDARDSSPACETYPE)), toStringNoOp, choices, getter, setter, resetter, boost::none,
                        // Make editable
                        true);
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString SpaceTypesGridController::getColor(const model::ModelObject& /*modelObject*/) {
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  // TODO: The code below is currently commented out because a refresh crash bug is precluding rack color
  // updates due to rack assignments to cases and walk-ins.  No colors are better than wrong colors.

  //std::vector<model::RefrigerationSystem> refrigerationSystems = m_model.getConcreteModelObjects<model::RefrigerationSystem>();

  //boost::optional<model::SpaceType> refrigerationCase = modelObject.optionalCast<model::SpaceType>();
  //OS_ASSERT(refrigerationCase);

  //boost::optional<model::RefrigerationSystem> refrigerationSystem = refrigerationCase->system();
  //if(!refrigerationSystem){
  //  return color;
  //}

  //std::vector<model::RefrigerationSystem>::iterator it;
  //it = std::find(refrigerationSystems.begin(), refrigerationSystems.end(), refrigerationSystem.get());
  //if(it != refrigerationSystems.end()){
  //  int index = std::distance(refrigerationSystems.begin(), it);
  //  if(index >= static_cast<int>(m_colors.size())){
  //    index = m_colors.size() - 1; // similar to scheduleView's approach
  //  }
  //  color = this->m_colors.at(index).name();
  //}

  return color;
}

void SpaceTypesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpaceTypesGridController::onItemDropped(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::SpaceType>()) {
      modelObject->clone(model());
    }
  }
}

void SpaceTypesGridController::refreshModelObjects() {
  std::vector<model::SpaceType> spaceTypes = model().getConcreteModelObjects<model::SpaceType>();
  std::sort(spaceTypes.begin(), spaceTypes.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaceTypes));
}

void SpaceTypesGridController::onComboBoxIndexChanged(int index) {}

}  // namespace openstudio
