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

#include "SpacesSurfacesGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridController.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
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
#define SURFACENAME "Surface Name"
#define SURFACETYPE "Surface Type"
#define CONSTRUCTION "Construction"
#define OUTSIDEBOUNDARYCONDITION "Outside Boundary Condition"               // Dan note: cannot chose Surface if not already surface
#define OUTSIDEBOUNDARYCONDITIONOBJECT "Outside Boundary Condition Object"  // read only
#define SUNEXPOSURE "Sun Exposure"
#define WINDEXPOSURE "Wind Exposure"
#define SHADINGSURFACENAME "Shading Surface Name"  // read only

namespace openstudio {

SpacesSurfacesGridView::SpacesSurfacesGridView(bool isIP, const model::Model& model, QWidget* parent) : SpacesSubtabGridView(isIP, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSurfaceTypeFilter();
  showOutsideBoundaryConditionFilter();
  showSunExposureFilter();
  showWindExposureFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesSurfacesGridController(isIP, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  onClearSelection();
}

void SpacesSurfacesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesSurfacesGridView::onClearSelection() {
  m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

SpacesSurfacesGridController::SpacesSurfacesGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                           const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void SpacesSurfacesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      SURFACENAME, SURFACETYPE, CONSTRUCTION, OUTSIDEBOUNDARYCONDITION, OUTSIDEBOUNDARYCONDITIONOBJECT, SUNEXPOSURE, WINDEXPOSURE,
      //  SHADINGSURFACENAME, // UNDESIRABLE TO SHOW THIS VECTOR IN THIS VIEW
    };
    addCategoryAndFields(QString("General"), std::move(fields));
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesSurfacesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesSurfacesGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else {

      std::function<std::vector<model::ModelObject>(const model::Space&)> allSurfaces([](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        auto surfaces = t_space.surfaces();
        allModelObjects.insert(allModelObjects.end(), surfaces.begin(), surfaces.end());
        return allModelObjects;
      });

      if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpacesSurfacesGridController::onSelectAllStateChanged);
        connect(this, &SpacesSurfacesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row", DataSource(allSurfaces, true));
      } else if (field == SURFACENAME) {
        addNameLineEditColumn(
          Heading(QString(NAME), true, false), false, false, CastNullAdapter<model::Surface>(&model::Surface::name),
          CastNullAdapter<model::Surface>(&model::Surface::setName),
          boost::optional<std::function<void(model::Surface*)>>(std::function<void(model::Surface*)>([](model::Surface* t_s) { t_s->remove(); })),
          boost::optional<std::function<bool(model::Surface*)>>(), DataSource(allSurfaces, true));
      } else if (field == SURFACETYPE) {
        addComboBoxColumn(Heading(QString(SURFACETYPE)),
                          std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
                          std::function<std::vector<std::string>()>(&model::Surface::validSurfaceTypeValues),
                          CastNullAdapter<model::Surface>(&model::Surface::surfaceType),
                          CastNullAdapter<model::Surface>(&model::Surface::setSurfaceType), boost::optional<std::function<void(model::Surface*)>>(),
                          boost::optional<std::function<bool(model::Surface*)>>(), DataSource(allSurfaces, true));
      } else if (field == CONSTRUCTION) {
        setConstructionColumn(4);
        addDropZoneColumn(Heading(QString(CONSTRUCTION)), CastNullAdapter<model::Surface>(&model::Surface::construction),
                          CastNullAdapter<model::Surface>(&model::Surface::setConstruction),
                          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetConstruction)),
                          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::Surface*)>>(), DataSource(allSurfaces, true));
      } else if (field == OUTSIDEBOUNDARYCONDITION) {
        addComboBoxColumn(Heading(QString(OUTSIDEBOUNDARYCONDITION)),
                          std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
                          std::function<std::vector<std::string>()>(&model::Surface::validOutsideBoundaryConditionValues),
                          CastNullAdapter<model::Surface>(&model::Surface::outsideBoundaryCondition),
                          CastNullAdapter<model::Surface>(&model::Surface::setOutsideBoundaryCondition),
                          boost::optional<std::function<void(model::Surface*)>>(), boost::optional<std::function<bool(model::Surface*)>>(),
                          DataSource(allSurfaces, true));
      } else if (field == OUTSIDEBOUNDARYCONDITIONOBJECT) {
        std::function<bool(model::Surface*, const model::Surface&)> setter([](model::Surface* t_surface, const model::Surface& t_arg) {
          auto copy = t_arg;
          return t_surface->setAdjacentSurface(copy);
        });

        addDropZoneColumn(Heading(QString(OUTSIDEBOUNDARYCONDITIONOBJECT), true, false),
                          CastNullAdapter<model::Surface>(&model::Surface::adjacentSurface), setter,
                          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetAdjacentSurface)),
                          boost::optional<std::function<bool(model::Surface*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::Surface*)>>(), DataSource(allSurfaces, true));
      } else if (field == SUNEXPOSURE) {
        addComboBoxColumn(
          Heading(QString(SUNEXPOSURE)),
          std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
          std::function<std::vector<std::string>()>(&model::Surface::validSunExposureValues),
          CastNullAdapter<model::Surface>(&model::Surface::sunExposure), CastNullAdapter<model::Surface>(&model::Surface::setSunExposure),
          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetSunExposure)),
          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isSunExposureDefaulted)), DataSource(allSurfaces, true));
      } else if (field == WINDEXPOSURE) {
        addComboBoxColumn(Heading(QString(WINDEXPOSURE)),
                          std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
                          std::function<std::vector<std::string>()>(&model::Surface::validWindExposureValues),
                          CastNullAdapter<model::Surface>(&model::Surface::windExposure),
                          CastNullAdapter<model::Surface>(&model::Surface::setWindExposure),
                          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetWindExposure)),
                          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isWindExposureDefaulted)),
                          DataSource(allSurfaces, true));
      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString SpacesSurfacesGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesSurfacesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) return;

  OSGridController::checkSelectedFields();
}

void SpacesSurfacesGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesSurfacesGridController::refreshModelObjects() {
  std::vector<model::Space> spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
