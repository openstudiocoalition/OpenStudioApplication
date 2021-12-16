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

#include "SpacesSubtabGridView.hpp"

#include "../shared_gui_components/OSGridView.hpp"

#include <openstudio/model/BuildingStory.hpp>
#include <openstudio/model/BuildingStory_Impl.hpp>
#include <openstudio/model/ElectricEquipment.hpp>
#include <openstudio/model/ElectricEquipment_Impl.hpp>
#include <openstudio/model/GasEquipment.hpp>
#include <openstudio/model/GasEquipment_Impl.hpp>
#include <openstudio/model/HotWaterEquipment.hpp>
#include <openstudio/model/HotWaterEquipment_Impl.hpp>
#include <openstudio/model/InteriorPartitionSurface.hpp>
#include <openstudio/model/InteriorPartitionSurface_Impl.hpp>
#include <openstudio/model/InteriorPartitionSurfaceGroup.hpp>
#include <openstudio/model/InteriorPartitionSurfaceGroup_Impl.hpp>
#include <openstudio/model/InternalMass.hpp>
#include <openstudio/model/InternalMass_Impl.hpp>
#include <openstudio/model/Lights.hpp>
#include <openstudio/model/Lights_Impl.hpp>
#include <openstudio/model/Luminaire.hpp>
#include <openstudio/model/Luminaire_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/OtherEquipment.hpp>
#include <openstudio/model/OtherEquipment_Impl.hpp>
#include <openstudio/model/People.hpp>
#include <openstudio/model/People_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea_Impl.hpp>
#include <openstudio/model/SpaceLoadInstance.hpp>
#include <openstudio/model/SpaceLoadInstance_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/SteamEquipment.hpp>
#include <openstudio/model/SteamEquipment_Impl.hpp>
#include <openstudio/model/SubSurface.hpp>
#include <openstudio/model/SubSurface_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>

// FILTERS
#define STORY "Story"
#define THERMALZONE "Thermal Zone"
#define THERMALZONENAME "Thermal Zone Name"
#define SPACETYPE "Space Type"
#define SUBSURFACETYPE "SubSurface Type"
#define SPACENAME "Space Name"
#define LOADTYPE "Load Type"
#define WINDEXPOSURE "Wind Exposure"
#define SUNEXPOSURE "Sun Exposure"
#define OUTSIDEBOUNDARYCONDITION "Outside Boundary Condition"
#define SURFACETYPE "Surface Type"
#define INTERIORPARTITIONGROUP "Interior Partition Group"

#define ALL "All"
#define UNASSIGNED "Unassigned"

// LOAD TYPES
//#define SHOWALLLOADS "Show all loads"
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

SpacesSubtabGridView::SpacesSubtabGridView(bool isIP, const model::Model& model, QWidget* parent)
  : GridViewSubTab(isIP, model, parent), m_spacesModelObjects(subsetCastVector<model::ModelObject>(model.getConcreteModelObjects<model::Space>())) {

  // Filters

  QLabel* label = nullptr;

  m_filterGridLayout = new QGridLayout();
  m_filterGridLayout->setContentsMargins(7, 4, 0, 8);
  m_filterGridLayout->setSpacing(5);

  label = new QLabel();
  label->setText("Filters:");
  label->setObjectName("H2");
  m_filterGridLayout->addWidget(label, m_filterGridLayout->rowCount(), m_filterGridLayout->columnCount(), Qt::AlignTop | Qt::AlignLeft);
}

void SpacesSubtabGridView::showStoryFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(STORY);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_storyFilter = new QComboBox();
  initializeStoryFilter();
  m_storyFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_storyFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::storyFilterChanged);

  layout->addWidget(m_storyFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showThermalZoneFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(THERMALZONE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_thermalZoneFilter = new QComboBox();
  initializeThermalZoneFilter();
  m_thermalZoneFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_thermalZoneFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::thermalZoneFilterChanged);

  layout->addWidget(m_thermalZoneFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showSpaceTypeFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(SPACETYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_spaceTypeFilter = new QComboBox();
  initializeSpaceTypeFilter();
  m_spaceTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_spaceTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::spaceTypeFilterChanged);

  layout->addWidget(m_spaceTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showSubSurfaceTypeFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(SUBSURFACETYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_subSurfaceTypeFilter = new QComboBox();
  initializeSubSurfaceTypeFilter();
  m_subSurfaceTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_subSurfaceTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::subSurfaceTypeFilterChanged);

  layout->addWidget(m_subSurfaceTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showSpaceNameFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(SPACENAME);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_spaceNameFilter = new QLineEdit();
  m_spaceNameFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_spaceNameFilter, &QLineEdit::editingFinished, this, &openstudio::SpacesSubtabGridView::spaceNameFilterChanged);
  // Evan note: there are issues with using the signal below (as well as textChanged), related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  //connect(m_spaceNameFilter, &QLineEdit::textEdited, this, &openstudio::SpacesSubtabGridView::spaceNameFilterChanged);

  layout->addWidget(m_spaceNameFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showWindExposureFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(WINDEXPOSURE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_windExposureFilter = new QComboBox();
  initializeWindExposureFilter();
  m_windExposureFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_windExposureFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::windExposureFilterChanged);

  layout->addWidget(m_windExposureFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showSunExposureFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(SUNEXPOSURE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_sunExposureFilter = new QComboBox();
  initializeSunExposureFilter();
  m_sunExposureFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_sunExposureFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::sunExposureFilterChanged);

  layout->addWidget(m_sunExposureFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showOutsideBoundaryConditionFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(OUTSIDEBOUNDARYCONDITION);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_outsideBoundaryConditionFilter = new QComboBox();
  initializeOutsideBoundaryConditionFilter();
  m_outsideBoundaryConditionFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_outsideBoundaryConditionFilter, &QComboBox::currentTextChanged, this,
          &openstudio::SpacesSubtabGridView::outsideBoundaryConditionFilterChanged);

  layout->addWidget(m_outsideBoundaryConditionFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showSurfaceTypeFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(SURFACETYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_surfaceTypeFilter = new QComboBox();
  initializeSurfaceTypeFilter();
  m_surfaceTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_surfaceTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::surfaceTypeFilterChanged);

  layout->addWidget(m_surfaceTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showInteriorPartitionGroupFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(INTERIORPARTITIONGROUP);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_interiorPartitionGroupFilter = new QComboBox();
  initializeInteriorPartitionGroupFilter();
  m_interiorPartitionGroupFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_interiorPartitionGroupFilter, &QComboBox::currentTextChanged, this,
          &openstudio::SpacesSubtabGridView::interiorPartitionGroupFilterChanged);

  layout->addWidget(m_interiorPartitionGroupFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::showLoadTypeFilter() {
  auto layout = new QVBoxLayout();

  auto label = new QLabel();
  label->setText(LOADTYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_loadTypeFilter = new QComboBox();
  initializeLoadTypeFilter();
  m_loadTypeFilter->setFixedWidth(1.5 * OSItem::ITEM_WIDTH);
  connect(m_loadTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::SpacesSubtabGridView::loadTypeFilterChanged);

  layout->addWidget(m_loadTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  m_filterGridLayout->addLayout(layout, m_filterGridLayout->rowCount() - 1, m_filterGridLayout->columnCount());
}

void SpacesSubtabGridView::initializeStoryFilter() {
  m_storyFilter->clear();
  m_storyFilter->addItem(ALL);
  m_storyFilter->addItem(UNASSIGNED);
  auto buildingStories = this->m_model.getConcreteModelObjects<model::BuildingStory>();
  std::sort(buildingStories.begin(), buildingStories.end(), openstudio::WorkspaceObjectNameLess());
  for (auto bd : buildingStories) {
    QString temp("unnamed");
    if (bd.name()) {
      temp = bd.name().get().c_str();
    }
    m_storyFilter->addItem(temp);
  }
}

void SpacesSubtabGridView::initializeThermalZoneFilter() {
  m_thermalZoneFilter->clear();
  m_thermalZoneFilter->addItem(ALL);
  m_thermalZoneFilter->addItem(UNASSIGNED);
  auto thermalZones = this->m_model.getConcreteModelObjects<model::ThermalZone>();
  std::sort(thermalZones.begin(), thermalZones.end(), openstudio::WorkspaceObjectNameLess());
  for (auto tz : thermalZones) {
    QString temp("unnamed");
    if (tz.name()) {
      temp = tz.name().get().c_str();
    }
    m_thermalZoneFilter->addItem(temp);
  }
}

void SpacesSubtabGridView::initializeSpaceTypeFilter() {
  m_spaceTypeFilter->clear();
  m_spaceTypeFilter->addItem(ALL);
  m_spaceTypeFilter->addItem(UNASSIGNED);
  auto spacetypes = this->m_model.getConcreteModelObjects<model::SpaceType>();
  std::sort(spacetypes.begin(), spacetypes.end(), openstudio::WorkspaceObjectNameLess());
  for (auto st : spacetypes) {
    QString temp("unnamed");
    if (st.name()) {
      temp = st.name().get().c_str();
    }
    m_spaceTypeFilter->addItem(temp);
  }
}

void SpacesSubtabGridView::initializeSubSurfaceTypeFilter() {
  m_subSurfaceTypeFilter->clear();
  m_subSurfaceTypeFilter->addItem(ALL);
  auto subSurfacetypes = model::SubSurface::validSubSurfaceTypeValues();
  for (auto sst : subSurfacetypes) {
    m_subSurfaceTypeFilter->addItem(sst.c_str());
  }
}

void SpacesSubtabGridView::initializeLoadTypeFilter() {

  { m_loadTypeFilter->addItem(ALL); }

  {
    auto pixMap = new QPixmap(":images/mini_icons/internal_mass.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, INTERNALMASS);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/people.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, PEOPLE);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/lights.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, LIGHTS);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/luminaire.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, LUMINAIRE);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/electric_equipment.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, ELECTRICEQUIPMENT);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/gas_equipment.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, GASEQUIPMENT);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/steam_equipment.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, HOTWATEREQUIPMENT);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/steam_equipment.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, STEAMEQUIPMENT);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/other_equipment.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, OTHEREQUIPMENT);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/infiltration.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, SPACEINFILTRATIONDESIGNFLOWRATE);
  }

  {
    auto pixMap = new QPixmap(":images/mini_icons/mini_infiltration_leak.png");
    OS_ASSERT(pixMap);
    m_loadTypeFilter->addItem(*pixMap, SPACEINFILTRATIONEFFECTIVELEAKAGEAREA);
  }
}

void SpacesSubtabGridView::initializeWindExposureFilter() {
  m_windExposureFilter->addItem(ALL);

  for (auto str : model::Surface::validWindExposureValues()) {
    m_windExposureFilter->addItem(str.c_str());
  }
}

void SpacesSubtabGridView::initializeSunExposureFilter() {
  m_sunExposureFilter->addItem(ALL);

  for (auto str : model::Surface::validSunExposureValues()) {
    m_sunExposureFilter->addItem(str.c_str());
  }
}

void SpacesSubtabGridView::initializeOutsideBoundaryConditionFilter() {
  m_outsideBoundaryConditionFilter->addItem(ALL);

  for (auto str : model::Surface::validOutsideBoundaryConditionValues()) {
    m_outsideBoundaryConditionFilter->addItem(str.c_str());
  }
}

void SpacesSubtabGridView::initializeSurfaceTypeFilter() {
  m_surfaceTypeFilter->addItem(ALL);

  for (auto str : model::Surface::validSurfaceTypeValues()) {
    m_surfaceTypeFilter->addItem(str.c_str());
  }
}

void SpacesSubtabGridView::initializeInteriorPartitionGroupFilter() {
  m_interiorPartitionGroupFilter->clear();
  m_interiorPartitionGroupFilter->addItem(ALL);
  auto interiorPartitions = this->m_model.getConcreteModelObjects<model::InteriorPartitionSurface>();
  std::sort(interiorPartitions.begin(), interiorPartitions.end(), openstudio::WorkspaceObjectNameLess());
  for (auto ip : interiorPartitions) {
    QString temp("unnamed");
    if (ip.name()) {
      temp = ip.name().get().c_str();
    }
    m_interiorPartitionGroupFilter->addItem(temp);
  }
}

void SpacesSubtabGridView::storyFilterChanged(const QString& text) {
  m_objectsFilteredByStory.clear();

  if (text == ALL) {
    // nothing to filter
  } else if (text == UNASSIGNED) {
    for (auto obj : this->m_gridController->modelObjects()) {
      if (obj.cast<model::Space>().buildingStory()) {
        m_objectsFilteredByStory.insert(obj);
      }
    }
  } else {
    // OSGridController::m_modelObjects returns the Spaces
    for (auto obj : this->m_gridController->modelObjects()) {
      auto buildingStory = obj.cast<model::Space>().buildingStory();
      if (!buildingStory || !buildingStory->name() || (buildingStory && buildingStory->name() && buildingStory->name().get().c_str() != text)) {
        m_objectsFilteredByStory.insert(obj);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::thermalZoneFilterChanged(const QString& text) {
  m_objectsFilteredByThermalZone.clear();

  if (text == ALL) {
    // nothing to filter
  } else if (text == UNASSIGNED) {
    for (auto obj : this->m_gridController->modelObjects()) {
      if (obj.cast<model::Space>().thermalZone()) {
        m_objectsFilteredByThermalZone.insert(obj);
      }
    }
  } else {
    // OSGridController::m_modelObjects returns the Spaces
    for (auto obj : this->m_gridController->modelObjects()) {
      auto thermalZone = obj.cast<model::Space>().thermalZone();
      if (!thermalZone || !thermalZone->name() || (thermalZone && thermalZone->name() && thermalZone->name().get().c_str() != text)) {
        m_objectsFilteredByThermalZone.insert(obj);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::spaceTypeFilterChanged(const QString& text) {
  m_objectsFilteredBySpaceType.clear();

  if (text == ALL) {
    // nothing to filter
  } else if (text == UNASSIGNED) {
    for (auto obj : this->m_gridController->modelObjects()) {
      if (obj.cast<model::Space>().spaceType()) {
        m_objectsFilteredBySpaceType.insert(obj);
      }
    }
  } else {
    // OSGridController::m_modelObjects returns the Spaces
    for (auto obj : this->m_gridController->modelObjects()) {
      // LOG(Debug, "spaceTypeFilterChanged, obj=" << obj.briefDescription());
      auto spaceType = obj.cast<model::Space>().spaceType();
      if (!spaceType || !spaceType->name() || (spaceType && spaceType->name() && spaceType->name().get().c_str() != text)) {
        m_objectsFilteredBySpaceType.insert(obj);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::subSurfaceTypeFilterChanged(const QString& text) {
  m_objectsFilteredBySubSurfaceType.clear();

  if (text == ALL) {
    // nothing to filter
  } else {

    // It's possible that "fixedwindow" might be returned when querying later, instead of "FixedWindow" returned
    // by SubSurface::validSubSurfaceTypes(), so we'll do a case-insensitive string comparison later
    std::string subSurfaceTypeToKeep = openstudio::toString(text);

    // ObjectSelector::m_selectableObjects returns SubSurfaces directly
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      // LOG(Debug, "subSurfaceTypeFilterChanged, obj=" << obj.briefDescription());
      const auto& subsurface = obj.optionalCast<model::SubSurface>();
      if (subsurface) {
        const auto& subsurfaceType = subsurface->subSurfaceType();
        // Case insensitive
        if (!openstudio::istringEqual(subsurfaceType, subSurfaceTypeToKeep)) {
          // LOG(Debug, "Adding obj=" << obj.briefDescription());
          m_objectsFilteredBySubSurfaceType.insert(obj);
        }
      }
    }

    // Alternatively, we could have done this:
    //for (auto obj : this->m_gridController->m_modelObjects) {
    //LOG(Debug, "subSurfaceTypeFilterChanged, obj=" << obj.briefDescription());
    //auto surfaces = obj.cast<model::Space>().surfaces();
    //for (auto surface : surfaces) {
    //auto subsurfaces = surface.subSurfaces();
    //for (auto subsurface : subsurfaces) {
    //auto subsurfaceType = subsurface.subSurfaceType();
    //if (!openstudio::istringEqual(subsurfaceType, subSurfaceTypeToKeep)) {
    //LOG(Debug, "Adding obj=" << obj.briefDescription());
    //m_objectsFilteredBySubSurfaceType.insert(obj);
    //}
    //}
    //}
    //}
  }

  filterChanged();
}

void SpacesSubtabGridView::spaceNameFilterChanged() {
  m_objectsFilteredBySpaceName.clear();

  if (m_spaceNameFilter->text().isEmpty()) {
    // nothing to filter
  } else {
    for (auto obj : this->m_gridController->modelObjects()) {
      QString objName(obj.name().get().c_str());
      if (!objName.contains(m_spaceNameFilter->text(), Qt::CaseInsensitive)) {
        m_objectsFilteredBySpaceName.insert(obj);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::loadTypeFilterChanged(const QString& text) {
  m_objectsFilteredByLoadType.clear();

  if (text == ALL) {
    // nothing to filter
  } else {

    // ObjectSelector::m_selectableObjects returns Load objects directly
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      if (text == INTERNALMASS) {
        if (!obj.optionalCast<model::InternalMass>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == PEOPLE) {
        if (!obj.optionalCast<model::People>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == LIGHTS) {
        if (!obj.optionalCast<model::Lights>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == LUMINAIRE) {
        if (!obj.optionalCast<model::Luminaire>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == ELECTRICEQUIPMENT) {
        if (!obj.optionalCast<model::ElectricEquipment>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == GASEQUIPMENT) {
        if (!obj.optionalCast<model::GasEquipment>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == HOTWATEREQUIPMENT) {
        if (!obj.optionalCast<model::HotWaterEquipment>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == STEAMEQUIPMENT) {
        if (!obj.optionalCast<model::SteamEquipment>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == OTHEREQUIPMENT) {
        if (!obj.optionalCast<model::OtherEquipment>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == SPACEINFILTRATIONDESIGNFLOWRATE) {
        if (!obj.optionalCast<model::SpaceInfiltrationDesignFlowRate>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else if (text == SPACEINFILTRATIONEFFECTIVELEAKAGEAREA) {
        if (!obj.optionalCast<model::SpaceInfiltrationEffectiveLeakageArea>()) {
          m_objectsFilteredByLoadType.insert(obj);
        }
      } else {
        // Should never get here
        OS_ASSERT(false);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::windExposureFilterChanged(const QString& text) {
  m_objectsFilteredByWindExposure.clear();

  if (text == ALL) {
    // nothing to filter
  } else {
    // For a case-insensitive comparison later
    std::string windExposureToKeep = openstudio::toString(text);

    // ObjectSelector::m_selectableObjects returns Surfaces directly
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      // LOG(Debug, "windExposureFilterChanged, obj=" << obj.briefDescription());
      const auto& surface = obj.optionalCast<model::Surface>();
      if (surface) {
        const auto& windExposure = surface->windExposure();
        if (windExposure.empty() || !openstudio::istringEqual(windExposure, windExposureToKeep)) {
          m_objectsFilteredByWindExposure.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::sunExposureFilterChanged(const QString& text) {
  m_objectsFilteredBySunExposure.clear();

  if (text == ALL) {
    // nothing to filter
  } else {
    // For a case-insensitive comparison later
    std::string sunExposureToKeep = openstudio::toString(text);

    // ObjectSelector::m_selectableObjects returns Surfaces directly
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      // LOG(Debug, "sunExposureFilterChanged, obj=" << obj.briefDescription());
      const auto& surface = obj.optionalCast<model::Surface>();
      if (surface) {
        const auto& sunExposure = surface->sunExposure();
        if (sunExposure.empty() || !openstudio::istringEqual(sunExposure, sunExposureToKeep)) {
          m_objectsFilteredBySunExposure.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::outsideBoundaryConditionFilterChanged(const QString& text) {
  m_objectsFilteredByOutsideBoundaryCondition.clear();

  if (text == ALL) {
    // nothing to filter
  } else {
    // For a case-insensitive comparison later
    std::string outsideBoundCondToKeep = openstudio::toString(text);

    // ObjectSelector::m_selectableObjects returns either Surfaces or SubSurfaces directly, depending on the subtab it's on
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      // LOG(Debug, "outsideBoundaryConditionFilterChanged, obj=" << obj.briefDescription());
      std::string outsideBoundaryCondition;
      if (const auto& surface = obj.optionalCast<model::Surface>()) {
        outsideBoundaryCondition = surface->outsideBoundaryCondition();
      } else if (const auto& subSurface = obj.optionalCast<model::SubSurface>()) {
        outsideBoundaryCondition = subSurface->outsideBoundaryCondition();
      }
      if (outsideBoundaryCondition.empty() || !openstudio::istringEqual(outsideBoundaryCondition, outsideBoundCondToKeep)) {
        m_objectsFilteredByOutsideBoundaryCondition.insert(obj);
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::surfaceTypeFilterChanged(const QString& text) {
  m_objectsFilteredBySurfaceType.clear();

  if (text == ALL) {
    // nothing to filter
  } else {
    // For a case-insensitive comparison later
    std::string surfaceTypeToKeep = openstudio::toString(text);

    // ObjectSelector::m_selectableObjects Surfaces directly
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      // LOG(Debug, "surfaceTypeFilterChanged, obj=" << obj.briefDescription());
      const auto& surface = obj.optionalCast<model::Surface>();
      if (surface) {
        const auto& surfaceType = surface->surfaceType();
        if (!openstudio::istringEqual(surfaceType, surfaceTypeToKeep)) {
          m_objectsFilteredBySurfaceType.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::interiorPartitionGroupFilterChanged(const QString& text) {
  m_objectsFilteredByInteriorPartitionGroup.clear();

  if (m_interiorPartitionGroupFilter->currentText() == ALL) {
    // nothing to filter
  } else {
    for (const auto& obj : this->m_gridController->selectorObjects()) {
      const auto& interiorPartitionSurfaceGroup = obj.optionalCast<model::InteriorPartitionSurfaceGroup>();
      if (interiorPartitionSurfaceGroup) {
        if (!interiorPartitionSurfaceGroup->name() || (interiorPartitionSurfaceGroup->name().get().c_str() != text)) {
          m_objectsFilteredByInteriorPartitionGroup.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void SpacesSubtabGridView::filterChanged() {
  // Note: JM 2018-08-21
  // The distinction between these two is especially needed for the "Loads" Subtab
  // because it can't match SpaceLoadInstances to a Space if the load is inherited from a SpaceType rather than the space

  /****************************************************************************
     * Filters that should apply at the ROW level because they are Space-related
     ***************************************************************************/
  std::set<openstudio::model::ModelObject> spaceFilteredObjects = m_objectsFilteredByStory;

  spaceFilteredObjects.insert(m_objectsFilteredByThermalZone.begin(), m_objectsFilteredByThermalZone.end());
  spaceFilteredObjects.insert(m_objectsFilteredBySpaceType.begin(), m_objectsFilteredBySpaceType.end());
  spaceFilteredObjects.insert(m_objectsFilteredBySpaceName.begin(), m_objectsFilteredBySpaceName.end());

  /***********************************************************************************
     * Filters that should apply at the SUBROW level because they are DataObject-related
     ***********************************************************************************/
  std::set<openstudio::model::ModelObject> allFilteredObjects;

  allFilteredObjects.insert(m_objectsFilteredBySubSurfaceType.begin(), m_objectsFilteredBySubSurfaceType.end());
  allFilteredObjects.insert(m_objectsFilteredByWindExposure.begin(), m_objectsFilteredByWindExposure.end());
  allFilteredObjects.insert(m_objectsFilteredBySunExposure.begin(), m_objectsFilteredBySunExposure.end());
  allFilteredObjects.insert(m_objectsFilteredByOutsideBoundaryCondition.begin(), m_objectsFilteredByOutsideBoundaryCondition.end());
  allFilteredObjects.insert(m_objectsFilteredBySurfaceType.begin(), m_objectsFilteredBySurfaceType.end());
  allFilteredObjects.insert(m_objectsFilteredByInteriorPartitionGroup.begin(), m_objectsFilteredByInteriorPartitionGroup.end());
  allFilteredObjects.insert(m_objectsFilteredByLoadType.begin(), m_objectsFilteredByLoadType.end());

  // add space filters to all filters
  allFilteredObjects.insert(spaceFilteredObjects.begin(), spaceFilteredObjects.end());

  this->m_gridController->setObjectFilter([allFilteredObjects](const model::ModelObject& obj) -> bool {
    // return false if object in allFilteredObjects
    return allFilteredObjects.count(obj) == 0;
  });
}

void SpacesSubtabGridView::addObject(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_Space == iddObjectType.value()) {
    model::Space(this->m_model);
  }
}

void SpacesSubtabGridView::purgeObjects(const IddObjectType& iddObjectType) {
  // Purge Spaces. The logic is to remove spaces that don't have a thermalZone nor surfaces
  for (auto mo : this->m_model.getConcreteModelObjects<model::Space>()) {
    if ((!mo.thermalZone()) && (mo.surfaces().empty())) {
      mo.remove();
    }
  }
}

}  // namespace openstudio
