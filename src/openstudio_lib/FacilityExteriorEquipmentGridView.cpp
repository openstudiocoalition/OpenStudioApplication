/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "FacilityExteriorEquipmentGridView.hpp"

#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/ExteriorLights.hpp>
#include <openstudio/model/ExteriorLights_Impl.hpp>
#include <openstudio/model/ExteriorLightsDefinition.hpp>
#include <openstudio/model/ExteriorLightsDefinition_Impl.hpp>
#include <openstudio/model/ExteriorFuelEquipment.hpp>
#include <openstudio/model/ExteriorFuelEquipment_Impl.hpp>
#include <openstudio/model/ExteriorFuelEquipmentDefinition.hpp>
#include <openstudio/model/ExteriorFuelEquipmentDefinition_Impl.hpp>
#include <openstudio/model/ExteriorWaterEquipment.hpp>
#include <openstudio/model/ExteriorWaterEquipment_Impl.hpp>
#include <openstudio/model/ExteriorWaterEquipmentDefinition.hpp>
#include <openstudio/model/ExteriorWaterEquipmentDefinition_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Exterior_Lights_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QLabel>
#include <QScrollArea>

namespace openstudio {

FacilityExteriorEquipmentGridView::FacilityExteriorEquipmentGridView(bool isIP, bool displayAdditionalProps, const model::Model& model,
                                                                     QWidget* parent)
  : GridViewSubTab(isIP, displayAdditionalProps, model, parent) {
  auto modelObjects = subsetCastVector<model::ModelObject>(model.getConcreteModelObjects<model::ExteriorLights>());
  std::sort(modelObjects.begin(), modelObjects.end(), openstudio::WorkspaceObjectNameLess());

  m_gridController = new FacilityExteriorEquipmentGridController(isIP, displayAdditionalProps, tr("Exterior Equipment"),
                                                                 IddObjectType::OS_Exterior_Lights, model, modelObjects);
  auto* gridView = new OSGridView(m_gridController, tr("Exterior Equipment"), tr("Drop\nExterior Equipment"), false, parent);

  setGridController(m_gridController);
  setGridView(gridView);

  clearSelection();
}

void FacilityExteriorEquipmentGridView::addObject(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_Exterior_Lights == iddObjectType.value()) {
    model::ExteriorLights(model::ExteriorLightsDefinition(this->m_model));
  } else if (IddObjectType::OS_Exterior_FuelEquipment == iddObjectType.value()) {
    model::ExteriorFuelEquipment(model::ExteriorFuelEquipmentDefinition(this->m_model));
  } else if (IddObjectType::OS_Exterior_WaterEquipment == iddObjectType.value()) {
    model::ExteriorWaterEquipment(model::ExteriorWaterEquipmentDefinition(this->m_model));
  } else {
    // unhandled
    OS_ASSERT(false);
  }
}

void FacilityExteriorEquipmentGridView::purgeObjects(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_Exterior_Lights == iddObjectType.value()) {
    for (auto& mo : this->m_model.getConcreteModelObjects<model::ExteriorLightsDefinition>()) {
      if (mo.instances().empty()) {
        mo.remove();
      }
    }
  } else if (IddObjectType::OS_Exterior_FuelEquipment == iddObjectType.value()) {
    for (auto& mo : this->m_model.getConcreteModelObjects<model::ExteriorFuelEquipmentDefinition>()) {
      if (mo.instances().empty()) {
        mo.remove();
      }
    }
  } else if (IddObjectType::OS_Exterior_WaterEquipment == iddObjectType.value()) {
    for (auto& mo : this->m_model.getConcreteModelObjects<model::ExteriorWaterEquipmentDefinition>()) {
      if (mo.instances().empty()) {
        mo.remove();
      }
    }
  } else {
    // unhandled
    OS_ASSERT(false);
  }
}

void FacilityExteriorEquipmentGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void FacilityExteriorEquipmentGridView::onClearSelection() {
  clearSelection();
}

void FacilityExteriorEquipmentGridView::clearSelection() {
  //m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  //m_itemSelectorButtons->disablePurgeButton();
}

FacilityExteriorEquipmentGridController::FacilityExteriorEquipmentGridController(bool isIP, bool displayAdditionalProps, const QString& settingsText,
                                                                                 IddObjectType iddObjectType, const model::Model& model,
                                                                                 const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, settingsText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void FacilityExteriorEquipmentGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      tr("Exterior Lights Definition"), tr("Schedule"), tr("Control Option"), tr("Multiplier"), tr("End Use Subcategory"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Exterior Lights"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      tr("Exterior Fuel Equipment Definition"), tr("Schedule"), tr("Fuel Type"), tr("Multiplier"), tr("End Use Subcategory"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Exterior Fuel Equipment"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      tr("Exterior Water Equipment Definition"),
      tr("Schedule"),
      tr("Multiplier"),
      tr("End Use Subcategory"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Exterior Water Equipment"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void FacilityExteriorEquipmentGridController::onCategorySelected(int index) {
  if (index == 0) {
    setIddObjectType(IddObjectType::OS_Exterior_Lights);
  } else if (index == 1) {
    setIddObjectType(IddObjectType::OS_Exterior_FuelEquipment);
  } else if (index == 2) {
    setIddObjectType(IddObjectType::OS_Exterior_WaterEquipment);
  } else {
    // unhandled
    OS_ASSERT(false);
  }
  setModelObjects(std::vector<model::ModelObject>());
  OSGridController::onCategorySelected(index);
}

void FacilityExteriorEquipmentGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {tr("Display Name"), tr("CAD Object ID")});
  }
  // always show name and selected columns
  fields.insert(fields.begin(), {tr("Name"), tr("All")});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == tr("Name")) {

      addParentNameLineEditColumn(Heading(tr("Name"), false, false), false, CastNullAdapter<model::ModelObject>(&model::ModelObject::name),
                                  CastNullAdapter<model::ModelObject>(&model::ModelObject::setName));
    } else if (field == tr("Display Name")) {
      addNameLineEditColumn(Heading(tr("Display Name"), false, false),                                   // heading
                            false,                                                                       // isInspectable
                            false,                                                                       // isLocked
                            DisplayNameAdapter<model::ModelObject>(&model::ModelObject::displayName),    // getter
                            DisplayNameAdapter<model::ModelObject>(&model::ModelObject::setDisplayName)  // setter
      );
    } else if (field == tr("CAD Object ID")) {
      addNameLineEditColumn(Heading(tr("CAD Object ID"), false, false),                                  // heading
                            false,                                                                       // isInspectable
                            false,                                                                       // isLocked
                            DisplayNameAdapter<model::ModelObject>(&model::ModelObject::cadObjectId),    // getter
                            DisplayNameAdapter<model::ModelObject>(&model::ModelObject::setCADObjectId)  // setter
      );
    } else if (field == tr("All")) {

      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::checkStateChanged, this, &FacilityExteriorEquipmentGridController::onSelectAllStateChanged);
      connect(this, &FacilityExteriorEquipmentGridController::gridRowSelectionChanged, checkbox.data(),
              &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(tr("All"), false, false, checkbox), "Check to select this row");

      // Exterior Lights
    } else if (IddObjectType::OS_Exterior_Lights == iddObjectType().value()) {

      if (field == tr("Exterior Lights Definition")) {

        std::function<boost::optional<model::ExteriorLightsDefinition>(model::ExteriorLights*)> get([](model::ExteriorLights* el) {
          boost::optional<model::ExteriorLightsDefinition> result;
          if (!el->handle().isNull()) {
            result = el->exteriorLightsDefinition();
          }
          return result;
        });

        addDropZoneColumn(Heading(tr("Exterior Lights Definition")), get,
                          CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::setExteriorLightsDefinition),
                          boost::optional<std::function<void(model::ExteriorLights*)>>());

      } else if (field == tr("Schedule")) {

        std::function<bool(model::ExteriorLights*, const model::Schedule&)> set([](model::ExteriorLights* el, const model::Schedule& s) {
          model::Schedule copy = s;
          return el->setSchedule(copy);
        });

        addDropZoneColumn(Heading(tr("Schedule")), CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::schedule), set,
                          boost::optional<std::function<void(model::ExteriorLights*)>>(
                            CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::resetSchedule)));

      } else if (field == tr("Control Option")) {

        addComboBoxColumn<std::string, model::ExteriorLights>(
          Heading(tr("Control Option")), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ExteriorLights::controlOptionValues),
          CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::controlOption),
          CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::setControlOption),
          boost::optional<std::function<void(model::ExteriorLights*)>>(
            CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::resetControlOption)),
          boost::optional<std::function<bool(model::ExteriorLights*)>>(
            CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::isControlOptionDefaulted)),
          boost::optional<DataSource>());

      } else if (field == tr("Multiplier")) {

        addValueEditColumn(Heading(tr("Multiplier")), NullAdapter(&model::ExteriorLights::multiplier),
                           NullAdapter(&model::ExteriorLights::setMultiplier),
                           boost::optional<std::function<void(model::ExteriorLights*)>>(
                             CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::resetMultiplier)),
                           boost::optional<std::function<bool(model::ExteriorLights*)>>(
                             CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::isMultiplierDefaulted)),
                           boost::optional<DataSource>());

      } else if (field == tr("End Use Subcategory")) {

        addValueEditColumn(Heading(tr("End Use Subcategory")), CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::endUseSubcategory),
                           CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::setEndUseSubcategory),
                           boost::optional<std::function<void(model::ExteriorLights*)>>(
                             CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::resetEndUseSubcategory)),
                           boost::optional<std::function<bool(model::ExteriorLights*)>>(
                             CastNullAdapter<model::ExteriorLights>(&model::ExteriorLights::isEndUseSubcategoryDefaulted)),
                           boost::optional<DataSource>());

      } else {
        // unhandled
        OS_ASSERT(false);
      }

      // Exterior Fuel Equipment
    } else if (IddObjectType::OS_Exterior_FuelEquipment == iddObjectType().value()) {

      if (field == tr("Exterior Fuel Equipment Definition")) {

        std::function<boost::optional<model::ExteriorFuelEquipmentDefinition>(model::ExteriorFuelEquipment*)> get(
          [](model::ExteriorFuelEquipment* el) {
            boost::optional<model::ExteriorFuelEquipmentDefinition> result;
            if (!el->handle().isNull()) {
              result = el->exteriorFuelEquipmentDefinition();
            }
            return result;
          });

        addDropZoneColumn(Heading(tr("Exterior Fuel Equipment Definition")), get,
                          CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::setExteriorFuelEquipmentDefinition),
                          boost::optional<std::function<void(model::ExteriorFuelEquipment*)>>());

      } else if (field == tr("Schedule")) {

        std::function<boost::optional<model::Schedule>(model::ExteriorFuelEquipment*)> get(
          [](model::ExteriorFuelEquipment* el) { return el->schedule(); });

        std::function<bool(model::ExteriorFuelEquipment*, const model::Schedule&)> set(
          [](model::ExteriorFuelEquipment* el, const model::Schedule& s) {
            model::Schedule copy = s;
            return el->setSchedule(copy);
          });

        addDropZoneColumn(Heading(tr("Schedule")), get, set, boost::optional<std::function<void(model::ExteriorFuelEquipment*)>>());

      } else if (field == tr("Fuel Type")) {

        addComboBoxColumn<std::string, model::ExteriorFuelEquipment>(
          Heading(tr("Fuel Type")), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ExteriorFuelEquipment::fuelTypeValues),
          CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::fuelType),
          // Gotta help the compiler here, since there is a `bool setFuelType(const std::string&)` and a `bool setFuelType(const FuelType&)` overload
          CastNullAdapter<model::ExteriorFuelEquipment, bool, std::string>(&model::ExteriorFuelEquipment::setFuelType),
          boost::optional<std::function<void(model::ExteriorFuelEquipment*)>>(), boost::optional<DataSource>());

      } else if (field == tr("Multiplier")) {

        addValueEditColumn(Heading(tr("Multiplier")), NullAdapter(&model::ExteriorFuelEquipment::multiplier),
                           NullAdapter(&model::ExteriorFuelEquipment::setMultiplier),
                           boost::optional<std::function<void(model::ExteriorFuelEquipment*)>>(
                             CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::resetMultiplier)),
                           boost::optional<std::function<bool(model::ExteriorFuelEquipment*)>>(
                             CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::isMultiplierDefaulted)),
                           boost::optional<DataSource>());

      } else if (field == tr("End Use Subcategory")) {

        addValueEditColumn(Heading(tr("End Use Subcategory")),
                           CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::endUseSubcategory),
                           CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::setEndUseSubcategory),
                           boost::optional<std::function<void(model::ExteriorFuelEquipment*)>>(
                             CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::resetEndUseSubcategory)),
                           boost::optional<std::function<bool(model::ExteriorFuelEquipment*)>>(
                             CastNullAdapter<model::ExteriorFuelEquipment>(&model::ExteriorFuelEquipment::isEndUseSubcategoryDefaulted)),
                           boost::optional<DataSource>());

      } else {
        // unhandled
        OS_ASSERT(false);
      }

      // Exterior Water Equipment
    } else if (IddObjectType::OS_Exterior_WaterEquipment == iddObjectType().value()) {

      if (field == tr("Exterior Water Equipment Definition")) {

        std::function<boost::optional<model::ExteriorWaterEquipmentDefinition>(model::ExteriorWaterEquipment*)> get(
          [](model::ExteriorWaterEquipment* el) {
            boost::optional<model::ExteriorWaterEquipmentDefinition> result;
            if (!el->handle().isNull()) {
              result = el->exteriorWaterEquipmentDefinition();
            }
            return result;
          });

        addDropZoneColumn(Heading(tr("Exterior Water Equipment Definition")), get,
                          CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::setExteriorWaterEquipmentDefinition),
                          boost::optional<std::function<void(model::ExteriorWaterEquipment*)>>());

      } else if (field == tr("Schedule")) {

        std::function<boost::optional<model::Schedule>(model::ExteriorWaterEquipment*)> get(
          [](model::ExteriorWaterEquipment* el) { return el->schedule(); });

        std::function<bool(model::ExteriorWaterEquipment*, const model::Schedule&)> set(
          [](model::ExteriorWaterEquipment* el, const model::Schedule& s) {
            model::Schedule copy = s;
            return el->setSchedule(copy);
          });

        addDropZoneColumn(Heading(tr("Schedule")), get, set, boost::optional<std::function<void(model::ExteriorWaterEquipment*)>>());

      } else if (field == tr("Multiplier")) {

        addValueEditColumn(Heading(tr("Multiplier")), NullAdapter(&model::ExteriorWaterEquipment::multiplier),
                           NullAdapter(&model::ExteriorWaterEquipment::setMultiplier),
                           boost::optional<std::function<void(model::ExteriorWaterEquipment*)>>(
                             CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::resetMultiplier)),
                           boost::optional<std::function<bool(model::ExteriorWaterEquipment*)>>(
                             CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::isMultiplierDefaulted)),
                           boost::optional<DataSource>());

      } else if (field == tr("End Use Subcategory")) {

        addValueEditColumn(Heading(tr("End Use Subcategory")),
                           CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::endUseSubcategory),
                           CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::setEndUseSubcategory),
                           boost::optional<std::function<void(model::ExteriorWaterEquipment*)>>(
                             CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::resetEndUseSubcategory)),
                           boost::optional<std::function<bool(model::ExteriorWaterEquipment*)>>(
                             CastNullAdapter<model::ExteriorWaterEquipment>(&model::ExteriorWaterEquipment::isEndUseSubcategoryDefaulted)),
                           boost::optional<DataSource>());

      } else {
        // unhandled
        OS_ASSERT(false);
      }

    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString FacilityExteriorEquipmentGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void FacilityExteriorEquipmentGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void FacilityExteriorEquipmentGridController::onItemDropped(const OSItemId& itemId) {}

void FacilityExteriorEquipmentGridController::refreshModelObjects() {
  if (IddObjectType::OS_Exterior_Lights == iddObjectType().value()) {
    auto lights = model().getConcreteModelObjects<model::ExteriorLights>();
    std::sort(lights.begin(), lights.end(), openstudio::WorkspaceObjectNameLess());
    setModelObjects(subsetCastVector<model::ModelObject>(lights));
  } else if (IddObjectType::OS_Exterior_FuelEquipment == iddObjectType().value()) {
    auto equip = model().getConcreteModelObjects<model::ExteriorFuelEquipment>();
    std::sort(equip.begin(), equip.end(), openstudio::WorkspaceObjectNameLess());
    setModelObjects(subsetCastVector<model::ModelObject>(equip));
  } else if (IddObjectType::OS_Exterior_WaterEquipment == iddObjectType().value()) {
    auto equip = model().getConcreteModelObjects<model::ExteriorWaterEquipment>();
    std::sort(equip.begin(), equip.end(), openstudio::WorkspaceObjectNameLess());
    setModelObjects(subsetCastVector<model::ModelObject>(equip));
  } else {
    // unhandled
    OS_ASSERT(false);
  }
}

void FacilityExteriorEquipmentGridController::onComboBoxIndexChanged(int index) {}

}  // namespace openstudio
