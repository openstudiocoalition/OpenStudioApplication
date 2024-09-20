/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesSpacesGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/BuildingStory.hpp>
#include <openstudio/model/BuildingStory_Impl.hpp>
#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/DefaultScheduleSet_Impl.hpp>
#include <openstudio/model/DesignSpecificationOutdoorAir.hpp>
#include <openstudio/model/DesignSpecificationOutdoorAir_Impl.hpp>
#include <openstudio/model/ParentObject.hpp>
#include <openstudio/model/ParentObject_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate.hpp>
#include <openstudio/model/SpaceInfiltrationDesignFlowRate_Impl.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea.hpp>
#include <openstudio/model/SpaceInfiltrationEffectiveLeakageArea_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QCheckBox>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Space Name"
#define DISPLAYNAME "Display Name"
#define CADOBJECTID "CAD Object ID"
#define SELECTED "All"

// GENERAL
#define STORY "Story"
#define THERMALZONE "Thermal Zone"
#define SPACETYPE "Space Type"
#define DEFAULTCONSTRUCTIONSET "Default Construction Set"
#define DEFAULTSCHEDULESET "Default Schedule Set"
#define PARTOFTOTALFLOORAREA "Part of Total Floor Area"

// AIRFLOW
#define SPACEINFILTRATIONDESIGNFLOWRATES "Space Infiltration Design Flow Rates"
#define SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS "Space Infiltration Effective Leakage Areas"
#define DESIGNSPECIFICATIONOUTDOORAIROBJECTNAME "Design Specification Outdoor Air Object Name"

namespace openstudio {

SpacesSpacesGridView::SpacesSpacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, displayAdditionalProps, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesSpacesGridController(isIP, displayAdditionalProps, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  clearSelection();
}

void SpacesSpacesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  // m_itemSelectorButtons->enablePurgeButton();
}

void SpacesSpacesGridView::onClearSelection() {
  clearSelection();
}

void SpacesSpacesGridView::clearSelection() {
  //m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  // m_itemSelectorButtons->disablePurgeButton();
}

SpacesSpacesGridController::SpacesSpacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                                       const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void SpacesSpacesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      STORY, THERMALZONE, SPACETYPE, DEFAULTCONSTRUCTIONSET, DEFAULTSCHEDULESET, PARTOFTOTALFLOORAREA,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      DESIGNSPECIFICATIONOUTDOORAIROBJECTNAME,
      SPACEINFILTRATIONDESIGNFLOWRATES,
      SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Airflow"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesSpacesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesSpacesGridController::addColumns(const QString& category, std::vector<QString>& fields) {

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
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpacesSpacesGridController::onSelectAllStateChanged);
      connect(this, &SpacesSpacesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == STORY) {
      addDropZoneColumn(Heading(QString(STORY)), CastNullAdapter<model::Space>(&model::Space::buildingStory),
                        CastNullAdapter<model::Space>(&model::Space::setBuildingStory),
                        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetBuildingStory)));
    } else if (field == THERMALZONE) {
      std::function<bool(model::Space*, const model::ThermalZone&)> set([](model::Space* s, const model::ThermalZone& tz) {
        model::ThermalZone copy = tz;
        return s->setThermalZone(copy);
      });

      addDropZoneColumn(Heading(QString(THERMALZONE)), CastNullAdapter<model::Space>(&model::Space::thermalZone), set,
                        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetThermalZone)));
    } else if (field == SPACETYPE) {
      addDropZoneColumn(Heading(QString(SPACETYPE)), CastNullAdapter<model::Space>(&model::Space::spaceType),
                        CastNullAdapter<model::Space>(&model::Space::setSpaceType),
                        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetSpaceType)),
                        boost::optional<std::function<bool(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::isSpaceTypeDefaulted)),
                        boost::optional<std::function<std::vector<model::ModelObject>(const model::Space*)>>(), boost::optional<DataSource>());
    } else if (field == DEFAULTCONSTRUCTIONSET) {
      addDropZoneColumn(
        Heading(QString(DEFAULTCONSTRUCTIONSET)), CastNullAdapter<model::Space>(&model::Space::defaultConstructionSet),
        CastNullAdapter<model::Space>(&model::Space::setDefaultConstructionSet),
        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetDefaultConstructionSet)));
    } else if (field == DEFAULTSCHEDULESET) {
      addDropZoneColumn(Heading(QString(DEFAULTSCHEDULESET)), CastNullAdapter<model::Space>(&model::Space::defaultScheduleSet),
                        CastNullAdapter<model::Space>(&model::Space::setDefaultScheduleSet),
                        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetDefaultScheduleSet)));
    } else if (field == PARTOFTOTALFLOORAREA) {
      // We add the "Apply Selected" button to this column by passing 3rd arg, t_showColumnButton=true
      addCheckBoxColumn(Heading(QString(PARTOFTOTALFLOORAREA), true, true), std::string("Check to enable part of total floor area."),
                        NullAdapter(&model::Space::partofTotalFloorArea), NullAdapter(&model::Space::setPartofTotalFloorArea));
    } else if (field == SPACEINFILTRATIONDESIGNFLOWRATES) {
      std::function<boost::optional<model::SpaceInfiltrationDesignFlowRate>(model::Space*)> getter;

      std::function<bool(model::Space*, const model::SpaceInfiltrationDesignFlowRate&)> setter(
        [](model::Space* t_space, model::SpaceInfiltrationDesignFlowRate t_rate) {
          if (t_rate.space()) {
            boost::optional<model::ModelObject> clone_rate = t_rate.clone(t_rate.model());
            OS_ASSERT(clone_rate);
            auto success = clone_rate->cast<model::SpaceInfiltrationDesignFlowRate>().setSpace(*t_space);
            OS_ASSERT(success);
            return success;
          } else {
            auto success = t_rate.setSpace(*t_space);
            OS_ASSERT(success);
            return success;
          }
        });

      std::function<std::vector<model::ModelObject>(const model::Space&)> flowRates([](const model::Space& s) {
        auto rates = s.spaceInfiltrationDesignFlowRates();
        return std::vector<model::ModelObject>(rates.begin(), rates.end());
      });

      addNameLineEditColumn(
        Heading(QString(SPACEINFILTRATIONDESIGNFLOWRATES)), true, false,
        CastNullAdapter<model::SpaceInfiltrationDesignFlowRate>(&model::SpaceInfiltrationDesignFlowRate::name),
        CastNullAdapter<model::SpaceInfiltrationDesignFlowRate>(&model::SpaceInfiltrationDesignFlowRate::setName),
        boost::optional<std::function<void(model::SpaceInfiltrationDesignFlowRate*)>>(
          std::function<void(model::SpaceInfiltrationDesignFlowRate*)>([](model::SpaceInfiltrationDesignFlowRate* t_fr) { t_fr->remove(); })),
        boost::optional<std::function<bool(model::SpaceInfiltrationDesignFlowRate*)>>(std::function<bool(model::SpaceInfiltrationDesignFlowRate*)>(
          [](model::SpaceInfiltrationDesignFlowRate* t_fr) { return t_fr->spaceType().is_initialized(); })),
        DataSource(flowRates, false,
                   QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::SpaceInfiltrationDesignFlowRate, model::Space>(
                     Heading(SPACEINFILTRATIONDESIGNFLOWRATES), getter, setter, boost::none, boost::none, boost::none))));
    } else if (field == SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS) {
      std::function<boost::optional<model::SpaceInfiltrationEffectiveLeakageArea>(model::Space*)> getter;

      std::function<bool(model::Space*, const model::SpaceInfiltrationEffectiveLeakageArea&)> setter(
        [](model::Space* t_space, model::SpaceInfiltrationEffectiveLeakageArea t_area) {
          if (t_area.space()) {
            boost::optional<model::ModelObject> clone_area = t_area.clone(t_area.model());
            OS_ASSERT(clone_area);
            auto success = clone_area->cast<model::SpaceInfiltrationEffectiveLeakageArea>().setSpace(*t_space);
            OS_ASSERT(success);
            return success;
          } else {
            auto success = t_area.setSpace(*t_space);
            OS_ASSERT(success);
            return success;
          }
        });

      std::function<std::vector<model::ModelObject>(const model::Space&)> leakageAreas([](const model::Space& s) {
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
        boost::optional<std::function<bool(model::SpaceInfiltrationEffectiveLeakageArea*)>>(
          std::function<bool(model::SpaceInfiltrationEffectiveLeakageArea*)>(
            [](model::SpaceInfiltrationEffectiveLeakageArea* t_la) { return t_la->spaceType().is_initialized(); })),
        DataSource(leakageAreas, false,
                   QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::SpaceInfiltrationEffectiveLeakageArea, model::Space>(
                     Heading(SPACEINFILTRATIONEFFECTIVELEAKAGEAREAS), getter, setter, boost::none, boost::none, boost::none))));
    } else if (field == DESIGNSPECIFICATIONOUTDOORAIROBJECTNAME) {
      addDropZoneColumn(
        Heading(QString(DESIGNSPECIFICATIONOUTDOORAIROBJECTNAME)), CastNullAdapter<model::Space>(&model::Space::designSpecificationOutdoorAir),
        CastNullAdapter<model::Space>(&model::Space::setDesignSpecificationOutdoorAir),
        boost::optional<std::function<void(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::resetDesignSpecificationOutdoorAir)),
        boost::optional<std::function<bool(model::Space*)>>(CastNullAdapter<model::Space>(&model::Space::isDesignSpecificationOutdoorAirDefaulted)));
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString SpacesSpacesGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesSpacesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesSpacesGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesSpacesGridController::refreshModelObjects() {
  auto spaces = model().getModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
