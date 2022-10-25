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

#include "SpacesShadingGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridController.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/ShadingSurface.hpp>
#include <openstudio/model/ShadingSurface_Impl.hpp>
#include <openstudio/model/ShadingSurfaceGroup.hpp>
#include <openstudio/model/ShadingSurfaceGroup_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QCheckBox>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Space Name"
#define SELECTED "All"

// GENERAL
#define SHADINGSURFACEGROUP "Shading Surface Group"  // read only
#define CONSTRUCTION "Construction"
#define TRANSMITTANCESCHEDULE "Transmittance Schedule"
#define SHADEDSURFACENAME "Shading Surface Name"       // DAN note: need model method for suggestions
#define DAYLIGHTINGSHELFNAME "Daylighting Shelf Name"  // read only

namespace openstudio {

SpacesShadingGridView::SpacesShadingGridView(bool isIP, const model::Model& model, QWidget* parent) : SpacesSubtabGridView(isIP, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSpaceNameFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesShadingGridController(isIP, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  onClearSelection();
}

void SpacesShadingGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesShadingGridView::onClearSelection() {
  m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  //m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

SpacesShadingGridController::SpacesShadingGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                                         const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void SpacesShadingGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields;
    fields.push_back(SHADEDSURFACENAME);
    fields.push_back(SHADINGSURFACEGROUP);
    fields.push_back(CONSTRUCTION);
    fields.push_back(TRANSMITTANCESCHEDULE);
    //fields.push_back(DAYLIGHTINGSHELFNAME);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesShadingGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesShadingGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else {

      std::function<std::vector<model::ModelObject>(const model::Space&)> allShadingSurfaceGroups([](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        auto shadingSurfaceGroups = t_space.shadingSurfaceGroups();
        allModelObjects.insert(allModelObjects.end(), shadingSurfaceGroups.begin(), shadingSurfaceGroups.end());
        return allModelObjects;
      });

      std::function<std::vector<model::ModelObject>(const model::Space&)> allShadingSurfaces([allShadingSurfaceGroups](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        for (const auto& shadingSurfaceGroup : allShadingSurfaceGroups(t_space)) {
          auto shadingSurfaces = shadingSurfaceGroup.cast<model::ShadingSurfaceGroup>().shadingSurfaces();
          allModelObjects.insert(allModelObjects.end(), shadingSurfaces.begin(), shadingSurfaces.end());
        }
        return allModelObjects;
      });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allShadingSurfaceShadingSurfaceGroups(
        [allShadingSurfaceGroups](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          for (const auto& shadingSurfaceGroup : allShadingSurfaceGroups(t_space)) {
            auto shadingSurfaces = shadingSurfaceGroup.cast<model::ShadingSurfaceGroup>().shadingSurfaces();
            for (const auto& shadingSurface : shadingSurfaces) {
              auto group = shadingSurface.shadingSurfaceGroup();
              if (group) {
                allModelObjects.push_back(*group);
              } else {
                allModelObjects.emplace_back();
              }
            }
          }
          return allModelObjects;
        });

      if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpacesShadingGridController::onSelectAllStateChanged);
        connect(this, &SpacesShadingGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row", DataSource(allShadingSurfaceGroups, true));
      } else if (field == SHADEDSURFACENAME) {
        addNameLineEditColumn(
          Heading(QString(SHADEDSURFACENAME), true, false), false, false, CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::name),
          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setName), boost::optional<std::function<void(model::ShadingSurface*)>>(),
          boost::optional<std::function<bool(model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == SHADINGSURFACEGROUP) {
        addNameLineEditColumn(Heading(QString(SHADINGSURFACEGROUP), true, false), false, false,
                              CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::name),
                              CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::setName),
                              boost::optional<std::function<void(model::ShadingSurfaceGroup*)>>(),
                              boost::optional<std::function<bool(model::ShadingSurfaceGroup*)>>(),
                              DataSource(allShadingSurfaceShadingSurfaceGroups, true));
      } else if (field == CONSTRUCTION) {
        setConstructionColumn(4);
        addDropZoneColumn(Heading(QString(CONSTRUCTION)), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::construction),
                          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setConstruction),
                          boost::optional<std::function<void(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::resetConstruction)),
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(),
                          DataSource(allShadingSurfaces, true));
      } else if (field == TRANSMITTANCESCHEDULE) {
        std::function<bool(model::ShadingSurface*, const model::Schedule&)> setter(
          [](model::ShadingSurface* t_shadingSurface, const model::Schedule& t_arg) {
            auto copy = t_arg;
            return t_shadingSurface->setTransmittanceSchedule(copy);
          });

        addDropZoneColumn(
          Heading(QString(TRANSMITTANCESCHEDULE)), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::transmittanceSchedule), setter,
          boost::optional<std::function<void(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::resetTransmittanceSchedule)),
          boost::optional<std::function<bool(model::ShadingSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == SHADEDSURFACENAME) {
        //ShadingSurfaceGroup
        //boost::optional<Surface> shadedSurface() const;
        //bool setShadedSurface(const Surface& surface);

      } else if (field == DAYLIGHTINGSHELFNAME) {
        //ShadingSurface
        //boost::optional<DaylightingDeviceShelf> daylightingDeviceShelf() const;
      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString SpacesShadingGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesShadingGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) return;

  OSGridController::checkSelectedFields();
}

void SpacesShadingGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesShadingGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
