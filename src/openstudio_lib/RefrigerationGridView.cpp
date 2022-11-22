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

#include "RefrigerationGridView.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include "ModelObjectItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/RefrigerationCase.hpp>
#include <openstudio/model/RefrigerationCase_Impl.hpp>
#include <openstudio/model/RefrigerationSystem.hpp>
#include <openstudio/model/RefrigerationSystem_Impl.hpp>
#include <openstudio/model/RefrigerationWalkIn.hpp>
#include <openstudio/model/RefrigerationWalkIn_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/ScheduleTypeRegistry.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/Refrigeration_Case_FieldEnums.hxx>
#include <openstudio/utilities/idd/Refrigeration_WalkIn_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QTimer>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define SELECTED "All"

// Display Case Fields
#define ANTISWEATHEATERCONTROLTYPE "Anti Sweat Heater Control Type"
#define AVAILABILITYSCHEDULE "Availability Schedule"                              // TODO this has not yet been implemented
#define AVERAGEREFRIGERANTCHARGEINVENTORY "Average Refrigerant Charge Inventory"  // TODO this has not yet been implemented
#define CASEANTISWEATHEATERPOWERPERUNITLENGTH "Case Anti Sweat Heater Power\nper Unit Length"
#define CASECREDITFRACTIONSCHEDULE "Case Credit Fraction Schedule"
#define CASEDEFROSTDRIPDOWNSCHEDULE "Case Defrost Drip Down Schedule"  // TODO this has not yet been implemented
#define CASEDEFROSTPOWERPERUNITLENGTH "Case Defrost Power\nper Unit Length"
#define CASEDEFROSTSCHEDULE "Case Defrost Schedule"  // TODO this has not yet been implemented
#define CASEDEFROSTTYPE "Case Defrost Type"          // TODO this has not yet been implemented
#define CASEHEIGHT "Case Height"                     // TODO this has not yet been implemented
#define CASELENGTH "Case Length"
#define CASELIGHTINGSCHEDULE "Case Lighting Schedule"
#define CASEOPERATINGTEMPERATURE "Case Operating Temperature"
#define DEFROSTENERGYCORRECTIONCURVE "Defrost Energy Correction Curve"  // TODO this has not yet been implemented
#define DEFROSTENERGYCORRECTIONCURVETYPE \
  "Defrost Energy Correction Curve Type"  // TODO ***************************************** AVAILABLE, but this has not yet been called
#define DESIGNEVAPORATORTEMPERATUREORBRINEINLETTEMPERATURE "Design Evaporator Temperature\nor Brine Inlet Temperature"
#define FRACTIONOFANTISWEATHEATERENERGYTOCASE "Fraction of Anti Sweat\nHeater Energy to Case"
#define FRACTIONOFLIGHTINGENERGYTOCASE "Fraction of Lighting Energy to Case"
#define HUMIDITYATZEROANTISWEATHEATERENERGY "Humidity at Zero Anti Sweat\nHeater Energy"
#define INSTALLEDCASELIGHTINGPOWERPERUNITLENGTH "Installed Case Lighting Power\nper Unit Length"
#define LATENTCASECREDITCURVETYPE "Latent Case Credit Curve Type"  // TODO this has not yet been implemented
#define MINIMUMANTISWEATHEATERPOWERPERUNITLENGTH "Minimum Anti Sweat Heater Power\nper Unit Length"
#define NAME "Name"
#define OPERATINGCASEFANPOWERPERUNITLENGTH "Operating Case Fan Power\nper Unit Length"
#define RATEDAMBIENTRELATIVEHUMIDITY "Rated Ambient Relative Humidity"  // TODO this has not yet been implemented
#define RATEDAMBIENTTEMPERATURE \
  "Rated Ambient Temperature"  // ************************************************************* TODO AVAILABLE, but this has not yet been called
#define RATEDLATENTHEATRATIO "Rated Latent Heat Ratio"
#define RATEDRUNTIMEFRACTION "Rated Runtime Fraction"
#define RATEDTOTALCOOLINGCAPACITYPERUNITLENGTH "Rated Total Cooling Capacity\nper Unit Length"
#define REFRIGERATEDCASERESTOCKINGSCHEDULE "Refrigerated Case Restocking Schedule"
#define STANDARDCASEFANPOWERPERUNITLENGTH "Standard Case Fan Power\nper Unit Length"
#define STANDARDCASELIGHTINGPOWERPERUNITLENGTH "Standard Case Lighting Power\nper Unit Length"
#define THERMALZONE "Thermal Zone"
#define RACK "Rack"
#define UNDERCASEHVACRETURNAIRFRACTION "Under Case HVAC Return Air Fraction"

// Walk In Fields
#define AVAILABILITYSCHEDULE "Availability Schedule"
#define AVERAGEREFRIGERANTCHARGEINVENTORY "Average Refrigerant Charge Inventory"  // TODO this has not yet been implemented
#define DEFROSTCONTROLTYPE "Defrost Control Type"
#define DEFROSTDRIPDOWNSCHEDULE "Defrost Drip Down Schedule"  // TODO this has not yet been implemented
#define DEFROSTPOWER "Defrost Power"
#define DEFROSTSCHEDULE "Defrost Schedule"  // TODO this has not yet been implemented
#define DEFROSTTYPE "Defrost Type"          // TODO this has not yet been implemented
#define HEATINGPOWERSCHEDULE "Heating Power Schedule"
#define INSULATEDFLOORSURFACEAREA "Insulated Floor Surface Area"  // TODO this has not yet been implemented
#define INSULATEDFLOORUVALUE "Insulated Floor U Value"
#define LIGHTINGSCHEDULE "Lighting Schedule"  // TODO this has not yet been implemented
#define NAME "Name"
#define OPERATINGTEMPERATURE "Operating Temperature"                      // TODO this has not yet been implemented
#define RATEDCIRCULATIONFANPOWER "Rated Circulation Fan Power"            // TODO this has not yet been implemented
#define RATEDCOILCOOLINGCAPACITY "Rated Coil Cooling Capacity"            // TODO this has not yet been implemented
#define RATEDCOOLINGCOILFANPOWER "Rated Cooling Coil Fan Power"           // TODO this has not yet been implemented
#define RATEDCOOLINGSOURCETEMPERATURE "Rated Cooling Source Temperature"  // TODO this has not yet been implemented
#define RATEDTOTALHEATINGPOWER "Rated Total Heating Power"                // TODO this has not yet been implemented
#define RATEDTOTALLIGHTINGPOWER "Rated Total Lighting Power"              // TODO this has not yet been implemented
#define RESTOCKINGSCHEDULE "Restocking Schedule"
#define TEMPERATURETERMINATIONDEFROSTFRACTIONTOICE "Temperature Termination\nDefrost Fraction to Ice"
#define ZONEBOUNDARIES "Zone Boundaries"  // TODO this has not yet been implemented
#define ZONEBOUNDARYTHERMALZONE "Thermal Zone"
#define ZONEBOUNDARYTOTALINSULATEDSURFACEAREAFACINGZONE "Total Insulated Surface\nArea Facing Zone"
#define ZONEBOUNDARYAREAOFGLASSREACHINDOORSFACINGZONE "Area of Glass Reach In\nDoors Facing Zone"
#define ZONEBOUNDARYHEIGHTOFGLASSREACHINDOORSFACINGZONE "Height of Glass Reach In\nDoors Facing Zone"
#define ZONEBOUNDARYAREAOFSTOCKINGDOORSFACINGZONE "Area of Stocking Doors Facing Zone"
#define ZONEBOUNDARYHEIGHTOFSTOCKINGDOORSFACINGZONE "Height of Stocking Doors Facing Zone"
#define ZONEBOUNDARYINSULATEDSURFACEUVALUEFACINGZONE "Insulated Surface U\nValue Facing Zone"
#define ZONEBOUNDARYGLASSREACHINDOORUVALUEFACINGZONE "Glass Reach In Door U\nValue Facing Zone"
#define ZONEBOUNDARYSTOCKINGDOORUVALUEFACINGZONE "Stocking Door U\nValue Facing Zone"
#define ZONEBOUNDARYSTOCKINGDOOROPENINGSCHEDULEFACINGZONE "Stocking Door Opening\nSchedule Facing Zone"

namespace openstudio {

RefrigerationGridView::RefrigerationGridView(bool isIP, const model::Model& model, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  QVBoxLayout* layout = nullptr;

  layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  auto* scrollLayout = new QVBoxLayout();
  scrollLayout->setSpacing(0);
  scrollLayout->setContentsMargins(0, 0, 0, 0);

  auto* scrollWidget = new QWidget();
  scrollWidget->setObjectName("ScrollWidget");
  scrollWidget->setStyleSheet("QWidget#ScrollWidget { background: transparent; }");
  scrollWidget->setLayout(scrollLayout);

  auto* scrollArea = new QScrollArea();
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->setFrameStyle(QFrame::NoFrame);
  scrollArea->setWidget(scrollWidget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setBackgroundRole(QPalette::NoRole);
  layout->addWidget(scrollArea);

  std::vector<model::RefrigerationCase> refrigerationCases = model.getConcreteModelObjects<model::RefrigerationCase>();
  std::vector<model::ModelObject> caseModelObjects = subsetCastVector<model::ModelObject>(refrigerationCases);

  auto* refrigerationCaseGridController =
    new RefrigerationCaseGridController(m_isIP, "Display Cases", IddObjectType::OS_Refrigeration_Case, model, caseModelObjects);
  auto* caseGridView = new OSGridView(refrigerationCaseGridController, "Display Cases", "Drop\nCase", true, parent);

  connect(caseGridView, &OSGridView::dropZoneItemClicked, this, &RefrigerationGridView::dropZoneItemClicked);

  scrollLayout->addWidget(caseGridView, 0, Qt::AlignTop);

  std::vector<model::RefrigerationWalkIn> refrigerationWalkIns = model.getConcreteModelObjects<model::RefrigerationWalkIn>();
  std::vector<model::ModelObject> walkInModelObjects = subsetCastVector<model::ModelObject>(refrigerationWalkIns);

  auto* refrigerationWalkInGridController =
    new RefrigerationWalkInGridController(m_isIP, "Walk Ins", IddObjectType::OS_Refrigeration_WalkIn, model, walkInModelObjects);
  auto* walkInView = new OSGridView(refrigerationWalkInGridController, "Walk Ins", "Drop\nWalk In", true, parent);

  connect(walkInView, &OSGridView::dropZoneItemClicked, this, &RefrigerationGridView::dropZoneItemClicked);

  scrollLayout->addWidget(walkInView, 0, Qt::AlignTop);

  scrollLayout->addStretch(1);

  connect(this, &RefrigerationGridView::toggleUnitsClicked, refrigerationCaseGridController, &RefrigerationCaseGridController::toggleUnitsClicked);

  connect(this, &RefrigerationGridView::toggleUnitsClicked, refrigerationWalkInGridController,
          &RefrigerationWalkInGridController::toggleUnitsClicked);

  connect(this, &RefrigerationGridView::toggleUnitsClicked, refrigerationCaseGridController, &RefrigerationCaseGridController::onToggleUnits);

  connect(this, &RefrigerationGridView::toggleUnitsClicked, refrigerationWalkInGridController, &RefrigerationWalkInGridController::onToggleUnits);

  // std::vector<model::RefrigerationSystem> refrigerationSystems =
  //   model.getConcreteModelObjects<model::RefrigerationSystem>();  // NOTE for horizontal system list
}

RefrigerationCaseGridController::RefrigerationCaseGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                                 const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void RefrigerationCaseGridController::setCategoriesAndFields() {

  {
    std::vector<QString> fields;
    fields.push_back(RACK);
    fields.push_back(THERMALZONE);
    fields.push_back(CASELENGTH);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(UNDERCASEHVACRETURNAIRFRACTION);
    fields.push_back(CASEOPERATINGTEMPERATURE);
    fields.push_back(DESIGNEVAPORATORTEMPERATUREORBRINEINLETTEMPERATURE);
    fields.push_back(RATEDRUNTIMEFRACTION);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Operation"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(RATEDTOTALCOOLINGCAPACITYPERUNITLENGTH);
    fields.push_back(CASECREDITFRACTIONSCHEDULE);
    fields.push_back(RATEDLATENTHEATRATIO);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Cooling\nCapacity"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(STANDARDCASEFANPOWERPERUNITLENGTH);
    fields.push_back(OPERATINGCASEFANPOWERPERUNITLENGTH);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Fan"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(STANDARDCASELIGHTINGPOWERPERUNITLENGTH);
    fields.push_back(INSTALLEDCASELIGHTINGPOWERPERUNITLENGTH);
    fields.push_back(FRACTIONOFLIGHTINGENERGYTOCASE);
    fields.push_back(CASELIGHTINGSCHEDULE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Lighting"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(CASEANTISWEATHEATERPOWERPERUNITLENGTH);
    fields.push_back(ANTISWEATHEATERCONTROLTYPE);
    fields.push_back(MINIMUMANTISWEATHEATERPOWERPERUNITLENGTH);
    fields.push_back(HUMIDITYATZEROANTISWEATHEATERENERGY);
    fields.push_back(FRACTIONOFANTISWEATHEATERENERGYTOCASE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Case\nAnti-Sweat\nHeaters"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(CASEDEFROSTPOWERPERUNITLENGTH);
    fields.push_back(REFRIGERATEDCASERESTOCKINGSCHEDULE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Defrost\nAnd\nRestocking"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void RefrigerationCaseGridController::addColumns(const QString& /*category*/, std::vector<QString>& fields) {
  // always show name column
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const QString& field : fields) {
    if (field == RATEDAMBIENTTEMPERATURE) {
      addQuantityEditColumn(Heading(QString(RATEDAMBIENTTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            NullAdapter(&model::RefrigerationCase::ratedAmbientTemperature),
                            NullAdapter(&model::RefrigerationCase::setRatedAmbientTemperature));
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &RefrigerationCaseGridController::onSelectAllStateChanged);
      connect(this, &RefrigerationCaseGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == RATEDAMBIENTRELATIVEHUMIDITY) {
      addValueEditColumn(Heading(QString(RATEDAMBIENTRELATIVEHUMIDITY)), NullAdapter(&model::RefrigerationCase::ratedAmbientRelativeHumidity),
                         NullAdapter(&model::RefrigerationCase::setRatedAmbientRelativeHumidity));
    } else if (field == RATEDTOTALCOOLINGCAPACITYPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(RATEDTOTALCOOLINGCAPACITYPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("Btu/hr*ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::ratedTotalCoolingCapacityperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setRatedTotalCoolingCapacityperUnitLength));
    } else if (field == RATEDLATENTHEATRATIO) {
      addValueEditColumn(Heading(QString(RATEDLATENTHEATRATIO)), NullAdapter(&model::RefrigerationCase::ratedLatentHeatRatio),
                         NullAdapter(&model::RefrigerationCase::setRatedLatentHeatRatio));
    } else if (field == RATEDRUNTIMEFRACTION) {
      addValueEditColumn(Heading(QString(RATEDRUNTIMEFRACTION)), NullAdapter(&model::RefrigerationCase::ratedRuntimeFraction),
                         NullAdapter(&model::RefrigerationCase::setRatedRuntimeFraction));
    } else if (field == CASELENGTH) {
      addQuantityEditColumn(Heading(QString(CASELENGTH)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::caseLength), NullAdapter(&model::RefrigerationCase::setCaseLength));
    } else if (field == CASEOPERATINGTEMPERATURE) {
      addQuantityEditColumn(Heading(QString(CASEOPERATINGTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            NullAdapter(&model::RefrigerationCase::caseOperatingTemperature),
                            NullAdapter(&model::RefrigerationCase::setCaseOperatingTemperature));
    } else if (field == STANDARDCASEFANPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(STANDARDCASEFANPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::standardCaseFanPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setStandardCaseFanPowerperUnitLength));
    } else if (field == OPERATINGCASEFANPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(OPERATINGCASEFANPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::operatingCaseFanPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setOperatingCaseFanPowerperUnitLength));
    } else if (field == FRACTIONOFLIGHTINGENERGYTOCASE) {
      addValueEditColumn(Heading(QString(FRACTIONOFLIGHTINGENERGYTOCASE)), NullAdapter(&model::RefrigerationCase::fractionofLightingEnergytoCase),
                         NullAdapter(&model::RefrigerationCase::setFractionofLightingEnergytoCase));
    } else if (field == CASEANTISWEATHEATERPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(CASEANTISWEATHEATERPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::caseAntiSweatHeaterPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setCaseAntiSweatHeaterPowerperUnitLength));
    } else if (field == MINIMUMANTISWEATHEATERPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(MINIMUMANTISWEATHEATERPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::minimumAntiSweatHeaterPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setMinimumAntiSweatHeaterPowerperUnitLength));
    } else if (field == CASEHEIGHT) {
      addQuantityEditColumn(Heading(QString(CASEHEIGHT)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::caseHeight), NullAdapter(&model::RefrigerationCase::setCaseHeight));
    } else if (field == FRACTIONOFANTISWEATHEATERENERGYTOCASE) {
      addValueEditColumn(Heading(QString(FRACTIONOFANTISWEATHEATERENERGYTOCASE)),
                         NullAdapter(&model::RefrigerationCase::fractionofAntiSweatHeaterEnergytoCase),
                         NullAdapter(&model::RefrigerationCase::setFractionofAntiSweatHeaterEnergytoCase));
    } else if (field == CASEDEFROSTPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(CASEDEFROSTPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::caseDefrostPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setCaseDefrostPowerperUnitLength));
    } else if (field == UNDERCASEHVACRETURNAIRFRACTION) {
      addValueEditColumn(Heading(QString(UNDERCASEHVACRETURNAIRFRACTION)), NullAdapter(&model::RefrigerationCase::underCaseHVACReturnAirFraction),
                         NullAdapter(&model::RefrigerationCase::setUnderCaseHVACReturnAirFraction));
    } else if (field == STANDARDCASELIGHTINGPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(STANDARDCASELIGHTINGPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::standardCaseLightingPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setStandardCaseLightingPowerperUnitLength));
    } else if (field == HUMIDITYATZEROANTISWEATHEATERENERGY) {
      addValueEditColumn(Heading(QString(HUMIDITYATZEROANTISWEATHEATERENERGY)),
                         NullAdapter(&model::RefrigerationCase::humidityatZeroAntiSweatHeaterEnergy),
                         NullAdapter(&model::RefrigerationCase::setHumidityatZeroAntiSweatHeaterEnergy));
    } else if (field == AVERAGEREFRIGERANTCHARGEINVENTORY) {
      addQuantityEditColumn(Heading(QString(AVERAGEREFRIGERANTCHARGEINVENTORY)), QString("kg/m"), QString("kg/m"), QString("lb/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::averageRefrigerantChargeInventory),
                            NullAdapter(&model::RefrigerationCase::setAverageRefrigerantChargeInventory));
    } else if (field == LATENTCASECREDITCURVETYPE) {
      addComboBoxColumn<std::string, model::RefrigerationCase>(
        Heading(QString(LATENTCASECREDITCURVETYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationCase::latentCaseCreditCurveTypeValues),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::latentCaseCreditCurveType),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setLatentCaseCreditCurveType),
        boost::optional<std::function<void(model::RefrigerationCase*)>>(), boost::optional<DataSource>());
    } else if (field == ANTISWEATHEATERCONTROLTYPE) {
      addComboBoxColumn<std::string, model::RefrigerationCase>(
        Heading(QString(ANTISWEATHEATERCONTROLTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationCase::antiSweatHeaterControlTypeValues),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::antiSweatHeaterControlType),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setAntiSweatHeaterControlType),
        boost::optional<std::function<void(model::RefrigerationCase*)>>(), boost::optional<DataSource>());
    } else if (field == CASEDEFROSTTYPE) {
      addComboBoxColumn<std::string, model::RefrigerationCase>(
        Heading(QString(CASEDEFROSTTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationCase::caseDefrostTypeValues),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::caseDefrostType),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setCaseDefrostType),
        boost::optional<std::function<void(model::RefrigerationCase*)>>(), boost::optional<DataSource>());
    } else if (field == DEFROSTENERGYCORRECTIONCURVETYPE) {
      addComboBoxColumn<std::string, model::RefrigerationCase>(
        Heading(QString(DEFROSTENERGYCORRECTIONCURVETYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationCase::defrostEnergyCorrectionCurveTypeValues),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::defrostEnergyCorrectionCurveType),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setDefrostEnergyCorrectionCurveType),
        boost::optional<std::function<void(model::RefrigerationCase*)>>(), boost::optional<DataSource>());
    } else if (field == INSTALLEDCASELIGHTINGPOWERPERUNITLENGTH) {
      addQuantityEditColumn(Heading(QString(INSTALLEDCASELIGHTINGPOWERPERUNITLENGTH)), QString("W/m"), QString("W/m"), QString("W/ft"), isIP(),
                            NullAdapter(&model::RefrigerationCase::installedCaseLightingPowerperUnitLength),
                            NullAdapter(&model::RefrigerationCase::setInstalledCaseLightingPowerperUnitLength));
    } else if (field == DESIGNEVAPORATORTEMPERATUREORBRINEINLETTEMPERATURE) {
      addQuantityEditColumn(Heading(QString(DESIGNEVAPORATORTEMPERATUREORBRINEINLETTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            NullAdapter(&model::RefrigerationCase::designEvaporatorTemperatureorBrineInletTemperature),
                            NullAdapter(&model::RefrigerationCase::setDesignEvaporatorTemperatureorBrineInletTemperature));
    } else if (field == CASELIGHTINGSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(CASELIGHTINGSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(std::bind(&openstudio::sortByObjectName<model::Schedule>,
                                                                std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase",
                                                                          "Case Lighting"))),  // NOTE these strings found in ScheduleTypeRegistry.cpp
        NullAdapter(&model::RefrigerationCase::caseLightingSchedule), NullAdapter(&model::RefrigerationCase::setCaseLightingSchedule),
        NullAdapter(&model::RefrigerationCase::resetCaseLightingSchedule));
    } else if (field == CASEDEFROSTSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(CASEDEFROSTSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase", "Case Defrost"))),
        NullAdapter(&model::RefrigerationCase::caseDefrostSchedule), NullAdapter(&model::RefrigerationCase::setCaseDefrostSchedule),
        NullAdapter(&model::RefrigerationCase::resetCaseDefrostSchedule));
    } else if (field == CASEDEFROSTDRIPDOWNSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(CASEDEFROSTDRIPDOWNSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase", "Case Defrost Drip-Down"))),
        NullAdapter(&model::RefrigerationCase::caseDefrostDripDownSchedule), NullAdapter(&model::RefrigerationCase::setCaseDefrostDripDownSchedule),
        NullAdapter(&model::RefrigerationCase::resetCaseDefrostDripDownSchedule));
    } else if (field == REFRIGERATEDCASERESTOCKINGSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(REFRIGERATEDCASERESTOCKINGSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase", "Refrigerated Case Restocking"))),
        NullAdapter(&model::RefrigerationCase::refrigeratedCaseRestockingSchedule),
        NullAdapter(&model::RefrigerationCase::setRefrigeratedCaseRestockingSchedule),
        NullAdapter(&model::RefrigerationCase::resetRefrigeratedCaseRestockingSchedule));
    } else if (field == CASECREDITFRACTIONSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(CASECREDITFRACTIONSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase", "Case Credit Fraction"))),
        NullAdapter(&model::RefrigerationCase::caseCreditFractionSchedule), NullAdapter(&model::RefrigerationCase::setCaseCreditFractionSchedule),
        NullAdapter(&model::RefrigerationCase::resetCaseCreditFractionSchedule));
    } else if (field == AVAILABILITYSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationCase>(
        Heading(QString(AVAILABILITYSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationCase", "Availability"))),
        NullAdapter(&model::RefrigerationCase::availabilitySchedule), NullAdapter(&model::RefrigerationCase::setAvailabilitySchedule),
        NullAdapter(&model::RefrigerationCase::resetAvailabilitySchedule));
    } else if (field == THERMALZONE) {
      addComboBoxColumn<model::ThermalZone, model::RefrigerationCase>(
        Heading(QString(THERMALZONE)), std::function<std::string(const openstudio::model::ThermalZone&)>(&openstudio::objectName),
        std::function<std::vector<model::ThermalZone>()>(std::bind(&openstudio::sortByObjectName<model::ThermalZone>,
                                                                   std::bind(&model::Model::getConcreteModelObjects<model::ThermalZone>, model()))),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::thermalZone),
        CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setThermalZone),
        boost::optional<std::function<void(openstudio::model::RefrigerationCase*)>>(NullAdapter(&model::RefrigerationCase::resetThermalZone)),
        boost::optional<openstudio::DataSource>());
    } else if (field == RACK) {
      addComboBoxColumn<model::RefrigerationSystem, model::RefrigerationCase>(
        Heading(QString(RACK)), &openstudio::objectName,
        std::function<std::vector<model::RefrigerationSystem>()>(
          std::bind(&openstudio::sortByObjectName<model::RefrigerationSystem>,
                    std::bind(&model::Model::getConcreteModelObjects<model::RefrigerationSystem>, model()))),
        NullAdapter(&model::RefrigerationCase::system), NullAdapter(&model::RefrigerationCase::addToSystem),
        NullAdapter(&model::RefrigerationCase::removeFromSystem));
    } else if (field == DEFROSTENERGYCORRECTIONCURVE) {
      //boost::optional<CurveCubic> defrostEnergyCorrectionCurve() const; TODO
    } else if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false,
                                  CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::name),
                                  CastNullAdapter<model::RefrigerationCase>(&model::RefrigerationCase::setName));
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString RefrigerationCaseGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  // TODO: The code below is currently commented out because a refresh crash bug is precluding rack color
  // updates due to rack assignments to cases and walk-ins.  No colors are better than wrong colors.

  //std::vector<model::RefrigerationSystem> refrigerationSystems = m_model.getConcreteModelObjects<model::RefrigerationSystem>();

  //boost::optional<model::RefrigerationCase> refrigerationCase = modelObject.optionalCast<model::RefrigerationCase>();
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

void RefrigerationCaseGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void RefrigerationCaseGridController::onItemDropped(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::RefrigerationCase>()) {
      modelObject->clone(model());
    }
  }
}

void RefrigerationCaseGridController::refreshModelObjects() {
  auto refrigerationCases = model().getConcreteModelObjects<model::RefrigerationCase>();
  std::sort(refrigerationCases.begin(), refrigerationCases.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(refrigerationCases));
}

void RefrigerationCaseGridController::onComboBoxIndexChanged(int index) {
  // Note: find the correct system color on RACK change,
  // but currently unable to know which row changed.
  const auto hh = horizontalHeaders();
  for (unsigned i = 0; i < hh.size(); ++i) {
    auto* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(hh.at(i));
    if (horizontalHeaderWidget->m_label->text() == RACK) {
      // NOTE required due to a race condition
      // Code below commented out due to a very infrequent crash in the bowels of Qt appears to be exasperated by this refresh.
      // A new refresh scheme with finer granularity may eliminate the problem, and restore rack colors.
      //QTimer::singleShot(0, this, SLOT(reset())); TODO
      break;
    }
  }
}

RefrigerationWalkInGridController::RefrigerationWalkInGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                                     const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void RefrigerationWalkInGridController::setCategoriesAndFields() {

  {
    std::vector<QString> fields;
    fields.push_back(RACK);
    fields.push_back(ZONEBOUNDARYTHERMALZONE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(ZONEBOUNDARYTOTALINSULATEDSURFACEAREAFACINGZONE);
    fields.push_back(ZONEBOUNDARYAREAOFGLASSREACHINDOORSFACINGZONE);
    fields.push_back(ZONEBOUNDARYHEIGHTOFGLASSREACHINDOORSFACINGZONE);
    fields.push_back(ZONEBOUNDARYAREAOFSTOCKINGDOORSFACINGZONE);
    fields.push_back(ZONEBOUNDARYHEIGHTOFSTOCKINGDOORSFACINGZONE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Dimensions"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(INSULATEDFLOORUVALUE);
    fields.push_back(ZONEBOUNDARYINSULATEDSURFACEUVALUEFACINGZONE);
    fields.push_back(ZONEBOUNDARYGLASSREACHINDOORUVALUEFACINGZONE);
    fields.push_back(ZONEBOUNDARYSTOCKINGDOORUVALUEFACINGZONE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Construction"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(AVAILABILITYSCHEDULE);
    fields.push_back(OPERATINGTEMPERATURE);
    fields.push_back(RATEDCOOLINGSOURCETEMPERATURE);
    fields.push_back(RATEDCOILCOOLINGCAPACITY);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Operation"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(RATEDCOOLINGCOILFANPOWER);
    fields.push_back(RATEDCIRCULATIONFANPOWER);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Fans"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(RATEDTOTALLIGHTINGPOWER);
    fields.push_back(LIGHTINGSCHEDULE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Lighting"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(RATEDTOTALHEATINGPOWER);
    fields.push_back(HEATINGPOWERSCHEDULE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Heating"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(DEFROSTTYPE);
    fields.push_back(DEFROSTCONTROLTYPE);
    fields.push_back(DEFROSTPOWER);
    fields.push_back(TEMPERATURETERMINATIONDEFROSTFRACTIONTOICE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Defrost"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(RESTOCKINGSCHEDULE);
    fields.push_back(ZONEBOUNDARYSTOCKINGDOOROPENINGSCHEDULEFACINGZONE);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Restocking"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void RefrigerationWalkInGridController::addColumns(const QString& /*category*/, std::vector<QString>& fields) {
  // always show name column
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const QString& field : fields) {
    if (field == DEFROSTTYPE) {
      addComboBoxColumn<std::string, model::RefrigerationWalkIn>(
        Heading(QString(DEFROSTTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationWalkIn::defrostTypeValues),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::defrostType),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setDefrostType),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(), boost::optional<DataSource>());
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &RefrigerationWalkInGridController::onSelectAllStateChanged);
      connect(this, &RefrigerationWalkInGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == DEFROSTCONTROLTYPE) {
      addComboBoxColumn<std::string, model::RefrigerationWalkIn>(
        Heading(QString(DEFROSTCONTROLTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RefrigerationWalkIn::defrostControlTypeValues),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::defrostControlType),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setDefrostControlType),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(), boost::optional<DataSource>());
    } else if (field == OPERATINGTEMPERATURE) {
      addQuantityEditColumn(Heading(QString(OPERATINGTEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::operatingTemperature),
                            NullAdapter(&model::RefrigerationWalkIn::setOperatingTemperature));
    } else if (field == RATEDCOOLINGSOURCETEMPERATURE) {
      addQuantityEditColumn(Heading(QString(RATEDCOOLINGSOURCETEMPERATURE)), QString("C"), QString("C"), QString("F"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedCoolingSourceTemperature),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedCoolingSourceTemperature));
    } else if (field == RATEDCOOLINGCOILFANPOWER) {
      addQuantityEditColumn(Heading(QString(RATEDCOOLINGCOILFANPOWER)), QString("W"), QString("W"), QString("W"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedCoolingCoilFanPower),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedCoolingCoilFanPower));
    } else if (field == RATEDCIRCULATIONFANPOWER) {
      addQuantityEditColumn(Heading(QString(RATEDCIRCULATIONFANPOWER)), QString("W"), QString("W"), QString("W"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedCirculationFanPower),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedCirculationFanPower));
    } else if (field == INSULATEDFLOORSURFACEAREA) {
      addQuantityEditColumn(Heading(QString(INSULATEDFLOORSURFACEAREA)), QString("m2"), QString("m2"), QString("ft^2"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::insulatedFloorSurfaceArea),
                            NullAdapter(&model::RefrigerationWalkIn::setInsulatedFloorSurfaceArea));
    } else if (field == INSULATEDFLOORUVALUE) {
      addQuantityEditColumn(Heading(QString(INSULATEDFLOORUVALUE)), QString("W/m^2*K"), QString("W/m^2*K"), QString("Btu/ft^2*hr*R"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::insulatedFloorUValue),
                            NullAdapter(&model::RefrigerationWalkIn::setInsulatedFloorUValue));
    } else if (field == RATEDCOILCOOLINGCAPACITY) {
      addQuantityEditColumn(Heading(QString(RATEDCOILCOOLINGCAPACITY)), QString("W"), QString("W"), QString("Btu/hr"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedCoilCoolingCapacity),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedCoilCoolingCapacity));
    } else if (field == RATEDTOTALHEATINGPOWER) {
      addQuantityEditColumn(Heading(QString(RATEDTOTALHEATINGPOWER)), QString("W"), QString("W"), QString("W"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedTotalHeatingPower),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedTotalHeatingPower));
    } else if (field == RATEDTOTALLIGHTINGPOWER) {
      addQuantityEditColumn(Heading(QString(RATEDTOTALLIGHTINGPOWER)), QString("W"), QString("W"), QString("W"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::ratedTotalLightingPower),
                            NullAdapter(&model::RefrigerationWalkIn::setRatedTotalLightingPower));
    } else if (field == AVERAGEREFRIGERANTCHARGEINVENTORY) {
      addQuantityEditColumn(Heading(QString(AVERAGEREFRIGERANTCHARGEINVENTORY)), QString("kg"), QString("kg"), QString("lb"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::averageRefrigerantChargeInventory),
                            NullAdapter(&model::RefrigerationWalkIn::setAverageRefrigerantChargeInventory));
    } else if (field == DEFROSTSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(DEFROSTSCHEDULE)), std::function<std::string(const model::Schedule&)>(&openstudio::objectName),
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Defrost"))),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::defrostSchedule),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setDefrostSchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(), boost::optional<std::function<bool(model::RefrigerationWalkIn*)>>(),
        boost::optional<DataSource>()

      );
    } else if (field == DEFROSTPOWER) {
      addQuantityEditColumn(Heading(QString(DEFROSTPOWER)), QString("W"), QString("W"), QString("W"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::defrostPower), NullAdapter(&model::RefrigerationWalkIn::setDefrostPower));
    } else if (field == TEMPERATURETERMINATIONDEFROSTFRACTIONTOICE) {
      addValueEditColumn(Heading(QString(TEMPERATURETERMINATIONDEFROSTFRACTIONTOICE)),
                         NullAdapter(&model::RefrigerationWalkIn::temperatureTerminationDefrostFractiontoIce),
                         NullAdapter(&model::RefrigerationWalkIn::setTemperatureTerminationDefrostFractiontoIce));
    } else if (field == AVAILABILITYSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(AVAILABILITYSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Availability"))),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::availabilitySchedule),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setAvailabilitySchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::resetAvailabilitySchedule)));
      //boost::optional<std::function<void(model::RefrigerationWalkIn*)> >(static_cast<void (model::RefrigerationWalkIn::*)()>(&model::RefrigerationWalkIn::resetAvailabilitySchedule)));
    } else if (field == HEATINGPOWERSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(HEATINGPOWERSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Heating Power"))),
        NullAdapter(&model::RefrigerationWalkIn::heatingPowerSchedule), NullAdapter(&model::RefrigerationWalkIn::setHeatingPowerSchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::resetHeatingPowerSchedule)));
    } else if (field == LIGHTINGSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(LIGHTINGSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Lighting"))),
        NullAdapter(&model::RefrigerationWalkIn::lightingSchedule), NullAdapter(&model::RefrigerationWalkIn::setLightingSchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::resetLightingSchedule)));
    } else if (field == DEFROSTDRIPDOWNSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(DEFROSTDRIPDOWNSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Defrost Drip-Down"))),
        NullAdapter(&model::RefrigerationWalkIn::defrostDripDownSchedule), NullAdapter(&model::RefrigerationWalkIn::setDefrostDripDownSchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::resetDefrostDripDownSchedule)));
    } else if (field == RESTOCKINGSCHEDULE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(RESTOCKINGSCHEDULE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(
          std::bind(&openstudio::sortByObjectName<model::Schedule>,
                    std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkIn", "Restocking"))),
        NullAdapter(&model::RefrigerationWalkIn::restockingSchedule), NullAdapter(&model::RefrigerationWalkIn::setRestockingSchedule),
        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::resetRestockingSchedule)));
    } else if (field == ZONEBOUNDARIES) {
      //std::vector<RefrigerationWalkInZoneBoundary> zoneBoundaries() const; TODO
    } else if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false,
                                  CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::name),
                                  CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setName));
    } else if (field == RACK) {
      addComboBoxColumn<model::RefrigerationSystem, model::RefrigerationWalkIn>(
        Heading(QString(RACK)), &openstudio::objectName,

        std::function<std::vector<model::RefrigerationSystem>()>(
          std::bind(&openstudio::sortByObjectName<model::RefrigerationSystem>,
                    std::bind(&model::Model::getConcreteModelObjects<model::RefrigerationSystem>, model()))),
        NullAdapter(&model::RefrigerationWalkIn::system), NullAdapter(&model::RefrigerationWalkIn::addToSystem),

        boost::optional<std::function<void(model::RefrigerationWalkIn*)>>(NullAdapter(&model::RefrigerationWalkIn::removeFromSystem)));
    } else if (field == ZONEBOUNDARYTHERMALZONE) {
      addComboBoxColumn<model::ThermalZone, model::RefrigerationWalkIn>(
        Heading(QString(ZONEBOUNDARYTHERMALZONE)), &openstudio::objectName,
        std::function<std::vector<model::ThermalZone>()>(std::bind(&openstudio::sortByObjectName<model::ThermalZone>,
                                                                   std::bind(&model::Model::getConcreteModelObjects<model::ThermalZone>, model()))),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::zoneBoundaryThermalZone),
        CastNullAdapter<model::RefrigerationWalkIn>(&model::RefrigerationWalkIn::setZoneBoundaryThermalZone),
        NullAdapter(&model::RefrigerationWalkIn::resetZoneBoundaryThermalZone));
    } else if (field == ZONEBOUNDARYTOTALINSULATEDSURFACEAREAFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYTOTALINSULATEDSURFACEAREAFACINGZONE)), QString("m^2"), QString("m^2"), QString("ft^2"),
                            isIP(), NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryTotalInsulatedSurfaceAreaFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryTotalInsulatedSurfaceAreaFacingZone));
    } else if (field == ZONEBOUNDARYAREAOFGLASSREACHINDOORSFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYAREAOFGLASSREACHINDOORSFACINGZONE)), QString("m^2"), QString("m^2"), QString("ft^2"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryAreaofGlassReachInDoorsFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryAreaofGlassReachInDoorsFacingZone));
    } else if (field == ZONEBOUNDARYHEIGHTOFGLASSREACHINDOORSFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYHEIGHTOFGLASSREACHINDOORSFACINGZONE)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryHeightofGlassReachInDoorsFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryHeightofGlassReachInDoorsFacingZone));
    } else if (field == ZONEBOUNDARYAREAOFSTOCKINGDOORSFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYAREAOFSTOCKINGDOORSFACINGZONE)), QString("m^2"), QString("m^2"), QString("ft^2"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryAreaofStockingDoorsFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryAreaofStockingDoorsFacingZone));
    } else if (field == ZONEBOUNDARYHEIGHTOFSTOCKINGDOORSFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYHEIGHTOFSTOCKINGDOORSFACINGZONE)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryHeightofStockingDoorsFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryHeightofStockingDoorsFacingZone));
    } else if (field == ZONEBOUNDARYINSULATEDSURFACEUVALUEFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYINSULATEDSURFACEUVALUEFACINGZONE)), QString("W/m^2*K"), QString("W/m^2*K"),
                            QString("Btu/ft^2*hr*R"), isIP(), NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryInsulatedSurfaceUValueFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryInsulatedSurfaceUValueFacingZone));
    } else if (field == ZONEBOUNDARYGLASSREACHINDOORUVALUEFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYGLASSREACHINDOORUVALUEFACINGZONE)), QString("W/m^2*K"), QString("W/m^2*K"),
                            QString("Btu/ft^2*hr*R"), isIP(), NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryGlassReachInDoorUValueFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryGlassReachInDoorUValueFacingZone));
    } else if (field == ZONEBOUNDARYSTOCKINGDOORUVALUEFACINGZONE) {
      addQuantityEditColumn(Heading(QString(ZONEBOUNDARYSTOCKINGDOORUVALUEFACINGZONE)), QString("W/m^2*K"), QString("W/m^2*K"),
                            QString("Btu/ft^2*hr*R"), isIP(), NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryStockingDoorUValueFacingZone),
                            NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryStockingDoorUValueFacingZone));
    } else if (field == ZONEBOUNDARYSTOCKINGDOOROPENINGSCHEDULEFACINGZONE) {
      addComboBoxColumn<model::Schedule, model::RefrigerationWalkIn>(
        Heading(QString(ZONEBOUNDARYSTOCKINGDOOROPENINGSCHEDULEFACINGZONE)), &openstudio::objectName,
        std::function<std::vector<model::Schedule>()>(std::bind(
          &openstudio::sortByObjectName<model::Schedule>,
          std::bind(&openstudio::model::getCompatibleSchedules, model(), "RefrigerationWalkInZoneBoundary", "Stocking Door Opening  Facing Zone"))),
        NullAdapter(&model::RefrigerationWalkIn::zoneBoundaryStockingDoorOpeningScheduleFacingZone),
        NullAdapter(&model::RefrigerationWalkIn::setZoneBoundaryStockingDoorOpeningScheduleFacingZone),

        NullAdapter(&model::RefrigerationWalkIn::resetZoneBoundaryStockingDoorOpeningScheduleFacingZone));
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString RefrigerationWalkInGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  // TODO: The code below is currently commented out because a refresh crash bug is precluding rack color
  // updates due to rack assignments to cases and walk-ins.  No colors are better than wrong colors.

  //std::vector<model::RefrigerationSystem> refrigerationSystems = m_model.getConcreteModelObjects<model::RefrigerationSystem>();

  //boost::optional<model::RefrigerationWalkIn> refrigerationWalkIn = modelObject.optionalCast<model::RefrigerationWalkIn>();
  //OS_ASSERT(refrigerationWalkIn);

  //boost::optional<model::RefrigerationSystem> refrigerationSystem = refrigerationWalkIn->system();
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

void RefrigerationWalkInGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void RefrigerationWalkInGridController::onItemDropped(const OSItemId& itemId) {
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject) {
    if (modelObject->optionalCast<model::RefrigerationWalkIn>()) {
      modelObject->clone(model());
    }
  }
}

void RefrigerationWalkInGridController::refreshModelObjects() {
  auto refrigerationWalkIns = model().getConcreteModelObjects<model::RefrigerationWalkIn>();
  std::sort(refrigerationWalkIns.begin(), refrigerationWalkIns.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(refrigerationWalkIns));
}

void RefrigerationWalkInGridController::onComboBoxIndexChanged(int index) {
  // Note: find the correct system color on RACK change,
  // but currently unable to know which row changed.
  const auto hh = horizontalHeaders();
  for (unsigned i = 0; i < hh.size(); ++i) {
    auto* horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget*>(hh.at(i));
    if (horizontalHeaderWidget->m_label->text() == RACK) {
      // NOTE required due to a race condition
      // Code below commented out due to a very infrequent crash in the bowels of Qt appears to be exasperated by this refresh.
      // A new refresh scheme with finer granularity may eliminate the problem, and restore rack colors.
      //QTimer::singleShot(0, this, SLOT(reset())); TODO
      break;
    }
  }
}

}  // namespace openstudio
