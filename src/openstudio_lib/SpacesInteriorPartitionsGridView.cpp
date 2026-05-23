/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesInteriorPartitionsGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridController.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/DaylightingDeviceShelf.hpp>
#include <openstudio/model/DaylightingDeviceShelf_Impl.hpp>
#include <openstudio/model/InteriorPartitionSurface.hpp>
#include <openstudio/model/InteriorPartitionSurface_Impl.hpp>
#include <openstudio/model/InteriorPartitionSurfaceGroup.hpp>
#include <openstudio/model/InteriorPartitionSurfaceGroup_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QCheckBox>
#include <QCoreApplication>

namespace openstudio {

SpacesInteriorPartitionsGridView::SpacesInteriorPartitionsGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, displayAdditionalProps, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showInteriorPartitionGroupFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController =
    new SpacesInteriorPartitionsGridController(isIP, displayAdditionalProps, tr("Space"), IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, tr("Space"), tr("Drop\nSpace"), false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  clearSelection();
}

void SpacesInteriorPartitionsGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesInteriorPartitionsGridView::onClearSelection() {
  clearSelection();
}

void SpacesInteriorPartitionsGridView::clearSelection() {
  m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  //m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

SpacesInteriorPartitionsGridController::SpacesInteriorPartitionsGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                                               IddObjectType iddObjectType, const model::Model& model,
                                                                               const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void SpacesInteriorPartitionsGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      tr("Interior Partition Name"), tr("Interior Partition Group Name"), tr("Construction Name"), tr("Convert to Internal Mass"),
      //tr("Surface Area"),
      //tr("Daylighting Shelf Name"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesInteriorPartitionsGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesInteriorPartitionsGridController::addColumns(const QString& category, std::vector<QString>& fields) {

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

      std::function<std::vector<model::ModelObject>(const model::Space&)> allInteriorPartitionSurfaceGroups([](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        auto interiorPartitionSurfaceGroups = t_space.interiorPartitionSurfaceGroups();
        allModelObjects.insert(allModelObjects.end(), interiorPartitionSurfaceGroups.begin(), interiorPartitionSurfaceGroups.end());
        return allModelObjects;
      });

      std::function<std::vector<model::ModelObject>(const model::Space&)> allInteriorPartitionSurfaces(
        [allInteriorPartitionSurfaceGroups](const model::Space& t_space) {
          std::vector<model::ModelObject> allModelObjects;
          for (const auto& interiorPartitionSurfaceGroup : allInteriorPartitionSurfaceGroups(t_space)) {
            auto interiorPartitionSurfaces = interiorPartitionSurfaceGroup.cast<model::InteriorPartitionSurfaceGroup>().interiorPartitionSurfaces();
            for (const auto& interiorPartitionSurface : interiorPartitionSurfaces) {
              allModelObjects.push_back(interiorPartitionSurface);
            }
          }
          return allModelObjects;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allInteriorPartitionSurfaceInteriorPartitionSurfaceGroups(
        [allInteriorPartitionSurfaceGroups](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          for (const auto& interiorPartitionSurfaceGroup : allInteriorPartitionSurfaceGroups(t_space)) {
            auto interiorPartitionSurfaces = interiorPartitionSurfaceGroup.cast<model::InteriorPartitionSurfaceGroup>().interiorPartitionSurfaces();
            for (const auto& interiorPartitionSurface : interiorPartitionSurfaces) {
              auto group = interiorPartitionSurface.interiorPartitionSurfaceGroup();
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
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::checkStateChanged, this, &SpacesInteriorPartitionsGridController::onSelectAllStateChanged);
        connect(this, &SpacesInteriorPartitionsGridController::gridRowSelectionChanged, checkbox.data(),
                &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(tr("All"), false, false, checkbox), "Check to select this row", DataSource(allInteriorPartitionSurfaces, true));
      } else if (field == tr("Interior Partition Group Name")) {
        addNameLineEditColumn(Heading(tr("Interior Partition Group Name"), true, false), false, false,
                              CastNullAdapter<model::InteriorPartitionSurfaceGroup>(&model::InteriorPartitionSurfaceGroup::name),
                              CastNullAdapter<model::InteriorPartitionSurfaceGroup>(&model::InteriorPartitionSurfaceGroup::setName),
                              boost::optional<std::function<void(model::InteriorPartitionSurfaceGroup*)>>(),
                              boost::optional<std::function<bool(model::InteriorPartitionSurfaceGroup*)>>(),
                              DataSource(allInteriorPartitionSurfaceInteriorPartitionSurfaceGroups, true));
      } else if (field == tr("Interior Partition Name")) {
        addNameLineEditColumn(Heading(tr("Interior Partition Name"), true, false), false, false,
                              CastNullAdapter<model::InteriorPartitionSurface>(&model::InteriorPartitionSurface::name),
                              CastNullAdapter<model::InteriorPartitionSurface>(&model::InteriorPartitionSurface::setName),
                              boost::optional<std::function<void(model::InteriorPartitionSurface*)>>(),
                              boost::optional<std::function<bool(model::InteriorPartitionSurface*)>>(),
                              DataSource(allInteriorPartitionSurfaces, true));
      } else if (field == tr("Construction Name")) {
        setConstructionColumn(4);
        addDropZoneColumn(
          Heading(tr("Construction Name"), true, false),
          CastNullAdapter<model::InteriorPartitionSurface>(&model::InteriorPartitionSurface::construction),
          CastNullAdapter<model::InteriorPartitionSurface>(&model::InteriorPartitionSurface::setConstruction),
          boost::optional<std::function<void(model::InteriorPartitionSurface*)>>(NullAdapter(&model::InteriorPartitionSurface::resetConstruction)),
          boost::optional<std::function<bool(model::InteriorPartitionSurface*)>>(
            NullAdapter(&model::InteriorPartitionSurface::isConstructionDefaulted)),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::InteriorPartitionSurface*)>>(),
          DataSource(allInteriorPartitionSurfaces, true));
      } else if (field == tr("Convert to Internal Mass")) {
        // We add the "Apply Selected" button to this column by passing 3rd arg, t_showColumnButton=true
        addCheckBoxColumn(Heading(tr("Convert to Internal Mass"), true, true), std::string("Check to enable convert to InternalMass."),
                          NullAdapter(&model::InteriorPartitionSurface::converttoInternalMass),
                          NullAdapter(&model::InteriorPartitionSurface::setConverttoInternalMass), DataSource(allInteriorPartitionSurfaces, true));
      } else if (field == tr("Surface Area")) {
        std::function<bool(model::InteriorPartitionSurface*, double)> setter(
          [](model::InteriorPartitionSurface* t_interiorPartitionSurface, double t_arg) {
            return t_interiorPartitionSurface->setSurfaceArea(t_arg);
          });

        addValueEditColumn(Heading(tr("Surface Area")),
                           CastNullAdapter<model::InteriorPartitionSurface>(&model::InteriorPartitionSurface::surfaceArea), setter
                           // boost::optional<std::function<void(model::ModelObject *)>>(),
                           // boost::optional<std::function<bool(model::ModelObject *)>>()//,
                           // DataSource(
                           //  allInteriorPartitionSurfaces,
                           //  true
                           // )
        );

        //boost::optional<double> surfaceArea() const; // TODO this optional is causing troubles
        //bool setSurfaceArea(boost::optional<double> surfaceArea);
        //bool setSurfaceArea(double surfaceArea);
        //void resetSurfaceArea();

      } else if (field == tr("Daylighting Shelf Name")) {
        //boost::optional<DaylightingDeviceShelf> daylightingDeviceShelf() const;

      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString SpacesInteriorPartitionsGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesInteriorPartitionsGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesInteriorPartitionsGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesInteriorPartitionsGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
