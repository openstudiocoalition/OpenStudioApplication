/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
#define DISPLAYNAME "Display Name"
#define CADOBJECTID "CAD Object ID"

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

SpacesSurfacesGridView::SpacesSurfacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, displayAdditionalProps, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSurfaceTypeFilter();
  showOutsideBoundaryConditionFilter();
  showSunExposureFilter();
  showWindExposureFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesSurfacesGridController(isIP, displayAdditionalProps, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  clearSelection();
}

void SpacesSurfacesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesSurfacesGridView::onClearSelection() {
  clearSelection();
}

void SpacesSurfacesGridView::clearSelection() {
  m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

SpacesSurfacesGridController::SpacesSurfacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                           IddObjectType iddObjectType, const model::Model& model,
                                                           const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void SpacesSurfacesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      SURFACENAME, SURFACETYPE, CONSTRUCTION, OUTSIDEBOUNDARYCONDITION, OUTSIDEBOUNDARYCONDITIONOBJECT, SUNEXPOSURE, WINDEXPOSURE,
      //SHADINGSURFACENAME, // UNDESIRABLE TO SHOW THIS VECTOR IN THIS VIEW
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesSurfacesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesSurfacesGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {DISPLAYNAME, CADOBJECTID});
  }
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else if (field == DISPLAYNAME) {
      addNameLineEditColumn(Heading(QString(DISPLAYNAME), false, false),                     // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::displayName),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setDisplayName)  // setter
      );
    } else if (field == CADOBJECTID) {
      addNameLineEditColumn(Heading(QString(CADOBJECTID), false, false),                     // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::cadObjectId),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setCADObjectId)  // setter
      );
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

        addComboBoxColumn<std::string, model::Surface>(  //
          Heading(QString(SURFACETYPE)),
          static_cast<std::string (*)(const std::string&)>(&openstudio::toString),             // toString
          std::function<std::vector<std::string>()>(&model::Surface::validSurfaceTypeValues),  // choices
          CastNullAdapter<model::Surface>(&model::Surface::surfaceType),                       // getter
          CastNullAdapter<model::Surface>(&model::Surface::setSurfaceType),                    // setter
          boost::none,                                                                         // reset
          boost::none,                                                                         // isDefaulted
          DataSource(allSurfaces, true)                                                        // source
        );

      } else if (field == CONSTRUCTION) {
        setConstructionColumn(4);
        addDropZoneColumn(Heading(QString(CONSTRUCTION)), CastNullAdapter<model::Surface>(&model::Surface::construction),
                          CastNullAdapter<model::Surface>(&model::Surface::setConstruction),
                          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetConstruction)),
                          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::Surface*)>>(), DataSource(allSurfaces, true));
      } else if (field == OUTSIDEBOUNDARYCONDITION) {

        addComboBoxColumn<std::string, model::Surface>(  //
          Heading(QString(OUTSIDEBOUNDARYCONDITION)),
          static_cast<std::string (*)(const std::string&)>(&openstudio::toString),                          // toString
          std::function<std::vector<std::string>()>(&model::Surface::validOutsideBoundaryConditionValues),  // choices
          CastNullAdapter<model::Surface>(&model::Surface::outsideBoundaryCondition),                       // getter
          CastNullAdapter<model::Surface>(&model::Surface::setOutsideBoundaryCondition),                    // setter
          boost::none,                                                                                      // reset
          boost::none,                                                                                      // isDefaulted
          DataSource(allSurfaces, true)                                                                     // source
        );

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

        addComboBoxColumn<std::string, model::Surface>(  //
          Heading(QString(SUNEXPOSURE)),
          static_cast<std::string (*)(const std::string&)>(&openstudio::toString),                                      // toString
          std::function<std::vector<std::string>()>(&model::Surface::validSunExposureValues),                           // choices
          CastNullAdapter<model::Surface>(&model::Surface::sunExposure),                                                // getter
          CastNullAdapter<model::Surface>(&model::Surface::setSunExposure),                                             // setter
          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetSunExposure)),        // reset
          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isSunExposureDefaulted)),  // isDefaulted
          DataSource(allSurfaces, true)                                                                                 // source
        );

      } else if (field == WINDEXPOSURE) {

        addComboBoxColumn<std::string, model::Surface>(  //
          Heading(QString(WINDEXPOSURE)),
          static_cast<std::string (*)(const std::string&)>(&openstudio::toString),                                       // toString
          std::function<std::vector<std::string>()>(&model::Surface::validWindExposureValues),                           // choices
          CastNullAdapter<model::Surface>(&model::Surface::windExposure),                                                // getter
          CastNullAdapter<model::Surface>(&model::Surface::setWindExposure),                                             // setter
          boost::optional<std::function<void(model::Surface*)>>(NullAdapter(&model::Surface::resetWindExposure)),        // reset
          boost::optional<std::function<bool(model::Surface*)>>(NullAdapter(&model::Surface::isWindExposureDefaulted)),  // isDefaulted
          DataSource(allSurfaces, true)                                                                                  // source
        );

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
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesSurfacesGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesSurfacesGridController::refreshModelObjects() {
  std::vector<model::Space> spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
