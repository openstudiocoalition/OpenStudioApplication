/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ThermalZonesGridView.hpp"

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelSubTabView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/AirLoopHVAC.hpp>
#include <openstudio/model/AirLoopHVAC_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/RenderingColor.hpp>
#include <openstudio/model/RenderingColor_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/SizingZone.hpp>
#include <openstudio/model/SizingZone_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/Thermostat.hpp>
#include <openstudio/model/ThermostatSetpointDualSetpoint.hpp>
#include <openstudio/model/ThermostatSetpointDualSetpoint_Impl.hpp>
#include <openstudio/model/ZoneControlHumidistat.hpp>
#include <openstudio/model/ZoneControlHumidistat_Impl.hpp>
#include <openstudio/model/ZoneHVACComponent.hpp>
#include <openstudio/model/ZoneHVACComponent_Impl.hpp>

#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_ThermalZone_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QSettings>
#include <QTimer>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Name"
#define SELECTED "All"
#define DISPLAYNAME "Display Name"
#define CADOBJECTID "CAD Object ID"

//HVAC SYSTEMS
#define RENDERINGCOLOR "Rendering Color"
#define IDEALAIRLOADS "Turn On\nIdeal\nAir Loads"
#define AIRLOOPNAME "Air Loop Name"
#define ZONEEQUIPMENT "Zone Equipment"
#define COOLINGTHERMOSTATSCHEDULE "Cooling Thermostat\nSchedule"
#define HEATINGTHERMOSTATSCHEDULE "Heating Thermostat\nSchedule"
#define HUMIDIFYINGSETPOINTSCHEDULE "Humidifying Setpoint\nSchedule"
#define DEHUMIDIFYINGSETPOINTSCHEDULE "Dehumidifying Setpoint\nSchedule"
#define MULTIPLIER "Multiplier"

//COOLING SIZING PARAMETERS
#define ZONECOOLINGDESIGNSUPPLYAIRTEMPERATURE "Zone Cooling\nDesign Supply\nAir Temperature"
#define ZONECOOLINGDESIGNSUPPLYAIRHUMIDITYRATIO "Zone Cooling\nDesign Supply\nAir Humidity Ratio"
#define ZONECOOLINGSIZINGFACTOR "Zone Cooling\nSizing Factor"
#define COOLINGMINIMUMAIRFLOWPERZONEFLOORAREA "Cooling Minimum Air\nFlow per Zone\nFloor Area"
#define DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINCOOLINGMODE "Design Zone Air\nDistribution Effectiveness\nin Cooling Mode"
#define COOLINGMINIMUMAIRFLOWFRACTION "Cooling Minimum\nAir Flow Fraction"
#define COOLINGDESIGNAIRFLOWMETHOD "Cooling Design\nAir Flow Method"
#define COOLINGDESIGNAIRFLOWRATE "Cooling Design\nAir Flow Rate"
#define COOLINGMINIMUMAIRFLOW "Cooling Minimum\nAir Flow"

//HEATING SIZING PARAMETERS
#define ZONEHEATINGDESIGNSUPPLYAIRTEMPERATURE "Zone Heating\nDesign Supply\nAir Temperature"
#define ZONEHEATINGDESIGNSUPPLYAIRHUMIDITYRATIO "Zone Heating\nDesign Supply\nAir Humidity Ratio"
#define ZONEHEATINGSIZINGFACTOR "Zone Heating\nSizing Factor"
#define HEATINGMAXIMUMAIRFLOWPERZONEFLOORAREA "Heating Maximum Air\nFlow per Zone\nFloor Area"
#define DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINHEATINGMODE "Design Zone Air\nDistribution Effectiveness\nin Heating Mode"
#define HEATINGMAXIMUMAIRFLOWFRACTION "Heating Maximum\nAir Flow Fraction"
#define HEATINGDESIGNAIRFLOWMETHOD "Heating Design\nAir Flow Method"
#define HEATINGDESIGNAIRFLOWRATE "Heating Design\nAir Flow Rate"
#define HEATINGMAXIMUMAIRFLOW "Heating Maximum\nAir Flow"

namespace openstudio {

ThermalZonesGridView::ThermalZonesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : QWidget(parent), m_isIP(isIP), m_displayAdditionalProps(displayAdditionalProps) {
  QVBoxLayout* layout = nullptr;

  layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  std::vector<model::ThermalZone> thermalZones = model.getConcreteModelObjects<model::ThermalZone>();
  std::vector<model::ModelObject> thermalZoneModelObjects = subsetCastVector<model::ModelObject>(thermalZones);

  m_gridController =
    new ThermalZonesGridController(m_isIP, m_displayAdditionalProps, "Thermal Zones", IddObjectType::OS_ThermalZone, model, thermalZoneModelObjects);
  auto* gridView = new OSGridView(m_gridController, "Thermal Zones", "Drop\nZone", false, parent);

  connect(gridView, &OSGridView::dropZoneItemClicked, this, &ThermalZonesGridView::dropZoneItemClicked);

  connect(this, &ThermalZonesGridView::selectionCleared, m_gridController, &ThermalZonesGridController::onSelectionCleared);

  gridView->showDropZone(false);

  layout->addWidget(gridView, 0, Qt::AlignTop);

  layout->addStretch(1);

  connect(this, &ThermalZonesGridView::toggleUnitsClicked, m_gridController, &ThermalZonesGridController::toggleUnitsClicked);

  connect(this, &ThermalZonesGridView::toggleUnitsClicked, m_gridController, &ThermalZonesGridController::onToggleUnits);

  connect(this, &ThermalZonesGridView::toggleDisplayAdditionalPropsClicked, m_gridController,
          &ThermalZonesGridController::toggleDisplayAdditionalPropsClicked);

  connect(this, &ThermalZonesGridView::toggleDisplayAdditionalPropsClicked, m_gridController,
          &ThermalZonesGridController::onToggleDisplayAdditionalProps);

  // std::vector<model::ThermalZone> thermalZone = model.getConcreteModelObjects<model::ThermalZone>();  // NOTE for horizontal system lists
}

std::set<model::ModelObject> ThermalZonesGridView::selectedObjects() const {
  return m_gridController->selectedObjects();
}

ThermalZonesGridController::ThermalZonesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                                       const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void ThermalZonesGridController::setCategoriesAndFields() {

  {
    std::vector<QString> fields{
      RENDERINGCOLOR,
      IDEALAIRLOADS,
      AIRLOOPNAME,
      ZONEEQUIPMENT,
      COOLINGTHERMOSTATSCHEDULE,
      HEATINGTHERMOSTATSCHEDULE,
      HUMIDIFYINGSETPOINTSCHEDULE,
      DEHUMIDIFYINGSETPOINTSCHEDULE,
      MULTIPLIER,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("HVAC\nSystems"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      ZONECOOLINGDESIGNSUPPLYAIRTEMPERATURE,
      ZONECOOLINGDESIGNSUPPLYAIRHUMIDITYRATIO,
      ZONECOOLINGSIZINGFACTOR,
      COOLINGMINIMUMAIRFLOWPERZONEFLOORAREA,
      DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINCOOLINGMODE,
      COOLINGMINIMUMAIRFLOWFRACTION,
      COOLINGDESIGNAIRFLOWMETHOD,
      COOLINGDESIGNAIRFLOWRATE,
      COOLINGMINIMUMAIRFLOW,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Cooling\nSizing\nParameters"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      ZONEHEATINGDESIGNSUPPLYAIRTEMPERATURE,
      ZONEHEATINGDESIGNSUPPLYAIRHUMIDITYRATIO,
      ZONEHEATINGSIZINGFACTOR,
      HEATINGMAXIMUMAIRFLOWPERZONEFLOORAREA,
      DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINHEATINGMODE,
      HEATINGMAXIMUMAIRFLOWFRACTION,
      HEATINGDESIGNAIRFLOWMETHOD,
      HEATINGDESIGNAIRFLOWRATE,
      HEATINGMAXIMUMAIRFLOW,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Heating\nSizing\nParameters"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void ThermalZonesGridController::addColumns(const QString& /*category*/, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {DISPLAYNAME, CADOBJECTID});
  }

  // always show name column
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  // This is probably borderline unreadable, but it dries up the code. This lambda will: call a member function on the TZ's sizing zone, and will
  // trigger a nano_emit for the TZ
  // eg: `makeProxyAdapterForceRefresh(&model::SizingZone::setZoneCoolingDesignSupplyAirTemperature)` will return
  //   ```
  //   std::function<bool(model::ThermalZone*, double val)>([](model::ThermalZone* t_z, double val) {
  //     bool b = t_z->sizingZone().setZoneCoolingDesignSupplyAirTemperature(val);
  //     t_z->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.nano_emit();
  //     return b;
  //   })
  //   ```
  using SizingZoneMemberFn = bool (model::SizingZone::*)(double);
  auto makeProxyAdapterForceRefresh = [](SizingZoneMemberFn t_func) {
    return std::function<bool(model::ThermalZone*, double)>([t_func](model::ThermalZone* t_z, double val) {
      bool b = (t_z->sizingZone().*t_func)(val);
      t_z->getImpl<openstudio::model::detail::ModelObject_Impl>()->onDataChange.nano_emit();
      return b;
    });
  };

  for (const QString& field : fields) {
    if (field == IDEALAIRLOADS) {
      // We add the "Apply Selected" button to this column by passing 3rd arg, t_showColumnButton=true
      addCheckBoxColumn(Heading(QString(IDEALAIRLOADS), true, true), std::string("Check to enable ideal air loads."),
                        NullAdapter(&model::ThermalZone::useIdealAirLoads), NullAdapter(&model::ThermalZone::setUseIdealAirLoads));
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &ThermalZonesGridController::onSelectAllStateChanged);
      connect(this, &ThermalZonesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == RENDERINGCOLOR) {
      addRenderingColorColumn(Heading(QString(RENDERINGCOLOR), true, false), CastNullAdapter<model::ThermalZone>(&model::ThermalZone::renderingColor),
                              CastNullAdapter<model::ThermalZone>(&model::ThermalZone::setRenderingColor));

    } else if (field == ZONECOOLINGDESIGNSUPPLYAIRTEMPERATURE) {

      addQuantityEditColumn(Heading(QString(ZONECOOLINGDESIGNSUPPLYAIRTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneCoolingDesignSupplyAirTemperature, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneCoolingDesignSupplyAirTemperature));

    } else if (field == ZONEHEATINGDESIGNSUPPLYAIRTEMPERATURE) {
      addQuantityEditColumn(Heading(QString(ZONEHEATINGDESIGNSUPPLYAIRTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneHeatingDesignSupplyAirTemperature, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneHeatingDesignSupplyAirTemperature));
    } else if (field == ZONECOOLINGDESIGNSUPPLYAIRHUMIDITYRATIO) {
      addQuantityEditColumn(Heading(QString(ZONECOOLINGDESIGNSUPPLYAIRHUMIDITYRATIO)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneCoolingDesignSupplyAirHumidityRatio, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneCoolingDesignSupplyAirHumidityRatio));
    } else if (field == ZONEHEATINGDESIGNSUPPLYAIRHUMIDITYRATIO) {
      addQuantityEditColumn(Heading(QString(ZONEHEATINGDESIGNSUPPLYAIRHUMIDITYRATIO)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneHeatingDesignSupplyAirHumidityRatio, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneHeatingDesignSupplyAirHumidityRatio));
    } else if (field == ZONEHEATINGSIZINGFACTOR) {
      addQuantityEditColumn(Heading(QString(ZONEHEATINGSIZINGFACTOR)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneHeatingSizingFactor, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneHeatingSizingFactor));
    } else if (field == ZONECOOLINGSIZINGFACTOR) {
      addQuantityEditColumn(Heading(QString()), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::zoneCoolingSizingFactor, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setZoneCoolingSizingFactor));
    } else if (field == HEATINGDESIGNAIRFLOWRATE) {
      addQuantityEditColumn(Heading(QString(HEATINGDESIGNAIRFLOWRATE)), QString("m^3/s"), QString("m^3/s"), QString("ft^3/min"), isIP(),
                            ProxyAdapter(&model::SizingZone::heatingDesignAirFlowRate, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setHeatingDesignAirFlowRate));
    } else if (field == HEATINGMAXIMUMAIRFLOW) {
      addQuantityEditColumn(Heading(QString(HEATINGMAXIMUMAIRFLOW)), QString("m^3/s"), QString("m^3/s"), QString("ft^3/min"), isIP(),
                            ProxyAdapter(&model::SizingZone::heatingMaximumAirFlow, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setHeatingMaximumAirFlow));
    } else if (field == HEATINGDESIGNAIRFLOWMETHOD) {
      addComboBoxColumn<std::string, model::ThermalZone>(
        Heading(QString(HEATINGDESIGNAIRFLOWMETHOD)),
        std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
        std::function<std::vector<std::string>()>(&model::SizingZone::heatingDesignAirFlowMethodValues),
        std::function<std::string(model::ThermalZone*)>([](model::ThermalZone* t_z) {
          try {
            return t_z->sizingZone().heatingDesignAirFlowMethod();
          } catch (const std::exception& e) {
            // If this code is called there is no sizingZone currently set. This means
            // that the ThermalZone is probably in the process of being destructed. So,
            // we don't want to create a new sizingZone, we really just want to try to
            // continue gracefully, so let's pretend that the current heatingDesignAirFlowMethod
            // is the 0th one in the list of possible options.
            //
            // This might not be the best solution to this problem
            LOG(Debug, "Exception while obtaining heatingDesignAirflowMethod " << e.what());
            return model::SizingZone::heatingDesignAirFlowMethodValues().at(0);
          }
        }),
        std::function<bool(model::ThermalZone*, std::string)>([](model::ThermalZone* t_z, const std::string& t_val) {
          bool b = t_z->sizingZone().setHeatingDesignAirFlowMethod(t_val);
          t_z->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.nano_emit();
          return b;
        }),
        boost::optional<std::function<void(openstudio::model::ThermalZone*)>>(),
        boost::optional<std::function<bool(openstudio::model::ThermalZone*)>>(), boost::optional<openstudio::DataSource>());

    } else if (field == COOLINGDESIGNAIRFLOWRATE) {
      addQuantityEditColumn(Heading(QString(COOLINGDESIGNAIRFLOWRATE)), QString("m^3/s"), QString("m^3/s"), QString("ft^3/min"), isIP(),
                            ProxyAdapter(&model::SizingZone::coolingDesignAirFlowRate, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setCoolingDesignAirFlowRate));
    } else if (field == COOLINGMINIMUMAIRFLOWPERZONEFLOORAREA) {
      addQuantityEditColumn(Heading(QString(COOLINGMINIMUMAIRFLOWPERZONEFLOORAREA)), QString("m^3/s*m^2"), QString("m^3/s*m^2"),
                            QString("ft^3/min*ft^2"), isIP(),
                            ProxyAdapter(&model::SizingZone::coolingMinimumAirFlowperZoneFloorArea, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setCoolingMinimumAirFlowperZoneFloorArea));
    } else if (field == COOLINGMINIMUMAIRFLOW) {
      addQuantityEditColumn(Heading(QString(COOLINGMINIMUMAIRFLOW)), QString("m^3/s"), QString("m^3/s"), QString("ft^3/min"), isIP(),
                            ProxyAdapter(&model::SizingZone::coolingMinimumAirFlow, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setCoolingMinimumAirFlow));
    } else if (field == COOLINGMINIMUMAIRFLOWFRACTION) {
      addQuantityEditColumn(Heading(QString(COOLINGMINIMUMAIRFLOWFRACTION)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::coolingMinimumAirFlowFraction, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setCoolingMinimumAirFlowFraction));
    } else if (field == HEATINGMAXIMUMAIRFLOWPERZONEFLOORAREA) {
      addQuantityEditColumn(Heading(QString(HEATINGMAXIMUMAIRFLOWPERZONEFLOORAREA)), QString("m^3/s*m^2"), QString("m^3/s*m^2"),
                            QString("ft^3/min*ft^2"), isIP(),
                            ProxyAdapter(&model::SizingZone::heatingMaximumAirFlowperZoneFloorArea, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setHeatingMaximumAirFlowperZoneFloorArea));
    } else if (field == HEATINGMAXIMUMAIRFLOWFRACTION) {
      addQuantityEditColumn(Heading(QString(HEATINGMAXIMUMAIRFLOWFRACTION)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::heatingMaximumAirFlowFraction, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setHeatingMaximumAirFlowFraction));
    } else if (field == DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINCOOLINGMODE) {
      addQuantityEditColumn(Heading(QString(DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINCOOLINGMODE)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::designZoneAirDistributionEffectivenessinCoolingMode, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setDesignZoneAirDistributionEffectivenessinCoolingMode));
    } else if (field == DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINHEATINGMODE) {
      addQuantityEditColumn(Heading(QString(DESIGNZONEAIRDISTRIBUTIONEFFECTIVENESSINHEATINGMODE)), QString(""), QString(""), QString(""), isIP(),
                            ProxyAdapter(&model::SizingZone::designZoneAirDistributionEffectivenessinHeatingMode, &model::ThermalZone::sizingZone),
                            makeProxyAdapterForceRefresh(&model::SizingZone::setDesignZoneAirDistributionEffectivenessinHeatingMode));

    } else if (field == COOLINGDESIGNAIRFLOWMETHOD) {
      addComboBoxColumn<std::string, model::ThermalZone>(
        Heading(QString(COOLINGDESIGNAIRFLOWMETHOD)),
        std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
        std::function<std::vector<std::string>()>(&model::SizingZone::coolingDesignAirFlowMethodValues),
        std::function<std::string(model::ThermalZone*)>([](model::ThermalZone* t_z) {
          try {
            return t_z->sizingZone().coolingDesignAirFlowMethod();
          } catch (const std::exception& e) {
            // If this code is called there is no sizingZone currently set. This means
            // that the ThermalZone is probably in the process of being destructed. So,
            // we don't want to create a new sizingZone, we really just want to try to
            // continue gracefully, so let's pretend that the current coolingDesignAirFlowMethod
            // is the 0th one in the list of possible options.
            //
            // This might not be the best solution to this problem
            LOG(Debug, "Exception while obtaining coolingDesignAirflowMethod " << e.what());
            return model::SizingZone::coolingDesignAirFlowMethodValues().at(0);
          }
        }),
        std::function<bool(model::ThermalZone*, std::string)>([](model::ThermalZone* t_z, const std::string& t_val) {
          bool b = t_z->sizingZone().setCoolingDesignAirFlowMethod(t_val);
          t_z->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.nano_emit();
          return b;
        }),
        boost::optional<std::function<void(openstudio::model::ThermalZone*)>>(),
        boost::optional<std::function<bool(openstudio::model::ThermalZone*)>>(), boost::optional<openstudio::DataSource>());

    } else if (field == COOLINGTHERMOSTATSCHEDULE) {

      std::function<boost::optional<model::Schedule>(model::ThermalZone*)> coolingSchedule([](model::ThermalZone* z) {
        boost::optional<model::Schedule> schedule;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          schedule = thermostat->coolingSetpointTemperatureSchedule();
        }
        return schedule;
      });

      std::function<bool(model::ThermalZone*, const model::Schedule&)> setCoolingSchedule([](model::ThermalZone* z, model::Schedule t_s) {
        bool result = false;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          result = thermostat->setCoolingSetpointTemperatureSchedule(t_s);
        } else {
          model::ThermostatSetpointDualSetpoint t(z->model());
          result = t.setCoolingSetpointTemperatureSchedule(t_s);
          z->setThermostatSetpointDualSetpoint(t);
        }
        return result;
      });

      boost::optional<std::function<void(model::ThermalZone*)>> resetCoolingSchedule([](model::ThermalZone* z) {
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          thermostat->resetCoolingSetpointTemperatureSchedule();
          if (!thermostat->heatingSetpointTemperatureSchedule()) {
            z->resetThermostatSetpointDualSetpoint();
          }
        }
      });

      boost::optional<std::function<bool(model::ThermalZone*)>> isDefaulted;

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ThermalZone*)>> otherObjects([](const model::ThermalZone* z) {
        std::vector<model::ModelObject> result;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          result.push_back(*thermostat);
        }
        return result;
      });

      addDropZoneColumn(Heading(QString(COOLINGTHERMOSTATSCHEDULE)), coolingSchedule, setCoolingSchedule, resetCoolingSchedule, isDefaulted,
                        otherObjects);

    } else if (field == HEATINGTHERMOSTATSCHEDULE) {

      std::function<boost::optional<model::Schedule>(model::ThermalZone*)> heatingSchedule([](model::ThermalZone* z) {
        boost::optional<model::Schedule> schedule;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          schedule = thermostat->heatingSetpointTemperatureSchedule();
        }
        return schedule;
      });

      std::function<bool(model::ThermalZone*, const model::Schedule&)> setHeatingSchedule([](model::ThermalZone* z, model::Schedule t_s) {
        bool result = false;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          result = thermostat->setHeatingSetpointTemperatureSchedule(t_s);
        } else {
          model::ThermostatSetpointDualSetpoint t(z->model());
          result = t.setHeatingSetpointTemperatureSchedule(t_s);
          z->setThermostatSetpointDualSetpoint(t);
        }
        return result;
      });

      boost::optional<std::function<void(model::ThermalZone*)>> resetHeatingSchedule([](model::ThermalZone* z) {
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          thermostat->resetHeatingSetpointTemperatureSchedule();
          if (!thermostat->coolingSetpointTemperatureSchedule()) {
            z->resetThermostatSetpointDualSetpoint();
          }
        }
      });

      boost::optional<std::function<bool(model::ThermalZone*)>> isDefaulted;

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ThermalZone*)>> otherObjects([](const model::ThermalZone* z) {
        std::vector<model::ModelObject> result;
        if (boost::optional<model::ThermostatSetpointDualSetpoint> thermostat = z->thermostatSetpointDualSetpoint()) {
          result.push_back(*thermostat);
        }
        return result;
      });

      addDropZoneColumn(Heading(QString(HEATINGTHERMOSTATSCHEDULE)), heatingSchedule, setHeatingSchedule, resetHeatingSchedule, isDefaulted,
                        otherObjects);

    } else if (field == HUMIDIFYINGSETPOINTSCHEDULE) {

      std::function<boost::optional<model::Schedule>(model::ThermalZone*)> humidifyingSchedule([](model::ThermalZone* z) {
        boost::optional<model::Schedule> schedule;
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          schedule = humidistat->humidifyingRelativeHumiditySetpointSchedule();
        }
        return schedule;
      });

      std::function<bool(model::ThermalZone*, const model::Schedule&)> setHumidifyingSchedule([](model::ThermalZone* z, model::Schedule t_s) {
        bool result = false;
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          result = humidistat->setHumidifyingRelativeHumiditySetpointSchedule(t_s);
        } else {
          model::ZoneControlHumidistat h(z->model());
          result = h.setHumidifyingRelativeHumiditySetpointSchedule(t_s);
          z->setZoneControlHumidistat(h);
        }
        return result;
      });

      boost::optional<std::function<void(model::ThermalZone*)>> resetHumidifyingSchedule([](model::ThermalZone* z) {
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          humidistat->resetHumidifyingRelativeHumiditySetpointSchedule();
          if (!humidistat->dehumidifyingRelativeHumiditySetpointSchedule()) {
            z->resetZoneControlHumidistat();
          }
        }
      });

      boost::optional<std::function<bool(model::ThermalZone*)>> isDefaulted;

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ThermalZone*)>> otherObjects([](const model::ThermalZone* z) {
        std::vector<model::ModelObject> result;
        boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat();
        if (humidistat) {
          result.push_back(*humidistat);
        }
        return result;
      });

      addDropZoneColumn(Heading(QString(HUMIDIFYINGSETPOINTSCHEDULE)), humidifyingSchedule, setHumidifyingSchedule, resetHumidifyingSchedule,
                        isDefaulted, otherObjects);

    } else if (field == DEHUMIDIFYINGSETPOINTSCHEDULE) {

      std::function<boost::optional<model::Schedule>(model::ThermalZone*)> dehumidifyingSchedule([](model::ThermalZone* z) {
        boost::optional<model::Schedule> schedule;
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          schedule = humidistat->dehumidifyingRelativeHumiditySetpointSchedule();
        }
        return schedule;
      });

      std::function<bool(model::ThermalZone*, const model::Schedule&)> setDehumidifyingSchedule([](model::ThermalZone* z, model::Schedule t_s) {
        bool result = false;
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          result = humidistat->setDehumidifyingRelativeHumiditySetpointSchedule(t_s);
        } else {
          model::ZoneControlHumidistat h(z->model());
          result = h.setDehumidifyingRelativeHumiditySetpointSchedule(t_s);
          z->setZoneControlHumidistat(h);
        }
        return result;
      });

      boost::optional<std::function<void(model::ThermalZone*)>> resetDehumidifyingSchedule([](model::ThermalZone* z) {
        if (boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat()) {
          humidistat->resetDehumidifyingRelativeHumiditySetpointSchedule();
          if (!humidistat->humidifyingRelativeHumiditySetpointSchedule()) {
            z->resetZoneControlHumidistat();
          }
        }
      });

      boost::optional<std::function<bool(model::ThermalZone*)>> isDefaulted;

      boost::optional<std::function<std::vector<model::ModelObject>(const model::ThermalZone*)>> otherObjects([](const model::ThermalZone* z) {
        std::vector<model::ModelObject> result;
        boost::optional<model::ZoneControlHumidistat> humidistat = z->zoneControlHumidistat();
        if (humidistat) {
          result.push_back(*humidistat);
        }
        return result;
      });

      addDropZoneColumn(Heading(QString(DEHUMIDIFYINGSETPOINTSCHEDULE)), dehumidifyingSchedule, setDehumidifyingSchedule, resetDehumidifyingSchedule,
                        isDefaulted, otherObjects);

    } else if (field == ZONEEQUIPMENT) {
      std::function<boost::optional<model::ModelObject>(model::ThermalZone*)> getter;
      std::function<bool(model::ThermalZone*, const model::ModelObject&)> setter([](model::ThermalZone* t_z, const model::ModelObject& t_mo) {
        try {
          if (t_mo.cast<model::ZoneHVACComponent>().thermalZone()) {
            boost::optional<model::ModelObject> clone_mo = t_mo.cast<model::ZoneHVACComponent>().clone(t_mo.model());
            OS_ASSERT(clone_mo);
            auto success = clone_mo->cast<model::ZoneHVACComponent>().addToThermalZone(*t_z);
            OS_ASSERT(success);
            OS_ASSERT(t_mo.cast<model::ZoneHVACComponent>().thermalZone());
            OS_ASSERT(clone_mo->cast<model::ZoneHVACComponent>().thermalZone());
            return true;
          }
          return t_mo.cast<model::ZoneHVACComponent>().addToThermalZone(*t_z);
        } catch (const std::exception&) {
          return false;
        }
      });

      std::function<void(model::ThermalZone*)> reset;
      std::function<std::vector<model::ModelObject>(const model::ThermalZone&)> equipment([](const model::ThermalZone& t) {
        // we need to pass in a const &, but the function expects non-const, so let's copy the wrapper
        // object in the param list
        return t.equipmentInHeatingOrder();
      });

      addNameLineEditColumn(Heading(QString(ZONEEQUIPMENT)), true, false, CastNullAdapter<model::ModelObject>(&model::ModelObject::name),
                            CastNullAdapter<model::ModelObject>(&model::ModelObject::setName),
                            boost::optional<std::function<void(model::ModelObject*)>>(
                              std::function<void(model::ModelObject*)>([](model::ModelObject* t_mo) { t_mo->remove(); })),
                            boost::optional<std::function<bool(model::ModelObject*)>>(),
                            DataSource(equipment, false,
                                       QSharedPointer<DropZoneConcept>(new DropZoneConceptImpl<model::ModelObject, model::ThermalZone>(
                                         Heading(ZONEEQUIPMENT), getter, setter, reset, boost::none, boost::none))));

    } else if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::ThermalZone>(&model::ThermalZone::name),
                                  CastNullAdapter<model::ThermalZone>(&model::ThermalZone::setName),
                                  boost::optional<std::function<void(model::ThermalZone*)>>());
    } else if (field == DISPLAYNAME) {
      addNameLineEditColumn(Heading(QString(DISPLAYNAME), false, false),                                 // heading
                            false,                                                                       // isInspectable
                            false,                                                                       // isLocked
                            DisplayNameAdapter<model::ThermalZone>(&model::ThermalZone::displayName),    // getter
                            DisplayNameAdapter<model::ThermalZone>(&model::ThermalZone::setDisplayName)  // setter
      );
    } else if (field == CADOBJECTID) {
      addNameLineEditColumn(Heading(QString(CADOBJECTID), false, false),                                 // heading
                            false,                                                                       // isInspectable
                            false,                                                                       // isLocked
                            DisplayNameAdapter<model::ThermalZone>(&model::ThermalZone::cadObjectId),    // getter
                            DisplayNameAdapter<model::ThermalZone>(&model::ThermalZone::setCADObjectId)  // setter
      );
    } else if (field == AIRLOOPNAME) {
      std::function<std::vector<model::ModelObject>(const model::ThermalZone&)> airloops([](const model::ThermalZone& t) {
        // we need to pass in a const &, but the function expects non-const, so let's copy the wrapper
        // object in the param list
        return subsetCastVector<model::ModelObject>(t.airLoopHVACs());
      });

      // Notes: this only requires a static_cast because `name` comes from IdfObject
      // we are passing in an empty std::function for the separate parameter because there's no way to set it
      addNameLineEditColumn(Heading(QString(AIRLOOPNAME), true, false), false, false, CastNullAdapter<model::ModelObject>(&model::ModelObject::name),
                            std::function<boost::optional<std::string>(model::ModelObject*, const std::string&)>(),
                            boost::optional<std::function<void(model::ModelObject*)>>(), boost::optional<std::function<bool(model::ModelObject*)>>(),
                            // insert DataSourceAdapter
                            DataSource(airloops, true));

    } else if (field == MULTIPLIER) {
      addValueEditColumn(Heading(QString(MULTIPLIER)), NullAdapter(&model::ThermalZone::multiplier), NullAdapter(&model::ThermalZone::setMultiplier));

    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString ThermalZonesGridController::getColor(const model::ModelObject& /*modelObject*/) {
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  // TODO: The code below is currently commented out because a refresh crash bug is precluding rack color
  // updates due to rack assignments to cases and walk-ins.  No colors are better than wrong colors.

  //std::vector<model::RefrigerationSystem> refrigerationSystems = m_model.getConcreteModelObjects<model::RefrigerationSystem>();

  //boost::optional<model::ThermalZone> refrigerationCase = modelObject.optionalCast<model::ThermalZone>();
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

void ThermalZonesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void ThermalZonesGridController::onItemDropped(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::ThermalZone>()) {
      modelObject->clone(model());
    }
  }
}

void ThermalZonesGridController::refreshModelObjects() {
  std::vector<model::ThermalZone> thermalZones = model().getConcreteModelObjects<model::ThermalZone>();
  std::sort(thermalZones.begin(), thermalZones.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(thermalZones));
}

void ThermalZonesGridController::onComboBoxIndexChanged(int index) {}

}  // namespace openstudio
