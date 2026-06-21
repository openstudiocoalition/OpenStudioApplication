/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesShadingGridView.hpp"

#include "../shared_gui_components/OSDropZone.hpp"
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
#include <QCoreApplication>

namespace openstudio {

SpacesShadingGridView::SpacesShadingGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, displayAdditionalProps, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSpaceNameFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesShadingGridController(isIP, displayAdditionalProps, tr("Space"), IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, tr("Space"), tr("Drop\nSpace"), false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  clearSelection();
}

void SpacesShadingGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesShadingGridView::onClearSelection() {
  clearSelection();
}

void SpacesShadingGridView::clearSelection() {
  m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  //m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

SpacesShadingGridController::SpacesShadingGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                         IddObjectType iddObjectType, const model::Model& model,
                                                         const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void SpacesShadingGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      tr("Shading Surface Name"), tr("Shading Surface Group"), tr("Construction"), tr("Transmittance Schedule"),
      //tr("Daylighting Shelf Name"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesShadingGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesShadingGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {tr("Display Name"), tr("CAD Object ID")});
  }
  // always show name and selected columns
  fields.insert(fields.begin(), {tr("Space Name"), tr("All")});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == tr("Space Name")) {
      addParentNameLineEditColumn(Heading(tr("Space Name"), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else if (field == tr("Display Name")) {
      addNameLineEditColumn(Heading(tr("Display Name"), false, false),                       // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::displayName),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setDisplayName)  // setter
      );
    } else if (field == tr("CAD Object ID")) {
      addNameLineEditColumn(Heading(tr("CAD Object ID"), false, false),                      // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::cadObjectId),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setCADObjectId)  // setter
      );
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

      if (field == tr("All")) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip(tr("Check to select all rows"));
        connect(checkbox.data(), &OSSelectAllCheckBox::checkStateChanged, this, &SpacesShadingGridController::onSelectAllStateChanged);
        connect(this, &SpacesShadingGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(tr("All"), false, false, checkbox), tr("Check to select this row").toStdString(),
                        DataSource(allShadingSurfaceGroups, true));
      } else if (field == tr("Shading Surface Name")) {
        addNameLineEditColumn(
          Heading(tr("Shading Surface Name"), true, false), false, false, CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::name),
          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setName), boost::optional<std::function<void(model::ShadingSurface*)>>(),
          boost::optional<std::function<bool(model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == tr("Shading Surface Group")) {
        addNameLineEditColumn(Heading(tr("Shading Surface Group"), true, false), false, false,
                              CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::name),
                              CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::setName),
                              boost::optional<std::function<void(model::ShadingSurfaceGroup*)>>(),
                              boost::optional<std::function<bool(model::ShadingSurfaceGroup*)>>(),
                              DataSource(allShadingSurfaceShadingSurfaceGroups, true));
      } else if (field == tr("Construction")) {
        setConstructionColumn(4);
        addDropZoneColumn(Heading(tr("Construction")), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::construction),
                          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setConstruction),
                          boost::optional<std::function<void(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::resetConstruction)),
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(),
                          DataSource(allShadingSurfaces, true));
      } else if (field == tr("Transmittance Schedule")) {
        std::function<bool(model::ShadingSurface*, const model::Schedule&)> setter(
          [](model::ShadingSurface* t_shadingSurface, const model::Schedule& t_arg) {
            auto copy = t_arg;
            return t_shadingSurface->setTransmittanceSchedule(copy);
          });

        addDropZoneColumn(
          Heading(tr("Transmittance Schedule")), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::transmittanceSchedule), setter,
          boost::optional<std::function<void(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::resetTransmittanceSchedule)),
          boost::optional<std::function<bool(model::ShadingSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == tr("Shading Surface Name")) {
        //ShadingSurfaceGroup
        //boost::optional<Surface> shadedSurface() const;
        //bool setShadedSurface(const Surface& surface);

      } else if (field == tr("Daylighting Shelf Name")) {
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
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesShadingGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesShadingGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
