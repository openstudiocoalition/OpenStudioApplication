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

#include "SimSettingsView.hpp"

#include "CollapsibleInspector.hpp"
#include "ModelObjectTypeListView.hpp"
#include "OSAppBase.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "OSDocument.hpp"
#include "../shared_gui_components/OSIntegerEdit.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/OSSwitch.hpp"
#include "SchedulesView.hpp"  // ScheduleCalendarWidget, MonthView

#include <algorithm>
#include <openstudio/model/ConvergenceLimits.hpp>
#include <openstudio/model/ConvergenceLimits_Impl.hpp>
#include <openstudio/model/HeatBalanceAlgorithm.hpp>
#include <openstudio/model/HeatBalanceAlgorithm_Impl.hpp>
#include <openstudio/model/InsideSurfaceConvectionAlgorithm.hpp>
#include <openstudio/model/InsideSurfaceConvectionAlgorithm_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/OutputControlReportingTolerances.hpp>
#include <openstudio/model/OutputControlReportingTolerances_Impl.hpp>
#include <openstudio/model/OutputJSON.hpp>
#include <openstudio/model/OutputJSON_Impl.hpp>
#include <openstudio/model/OutputTableSummaryReports.hpp>
#include <openstudio/model/OutputTableSummaryReports_Impl.hpp>
#include <openstudio/model/OutputDiagnostics.hpp>
#include <openstudio/model/OutputDiagnostics_Impl.hpp>
#include <openstudio/model/OutsideSurfaceConvectionAlgorithm.hpp>
#include <openstudio/model/OutsideSurfaceConvectionAlgorithm_Impl.hpp>
#include <openstudio/model/ProgramControl.hpp>
#include <openstudio/model/ProgramControl_Impl.hpp>
#include <openstudio/model/RadianceParameters.hpp>
#include <openstudio/model/RadianceParameters_Impl.hpp>
#include <openstudio/model/RunPeriod.hpp>
#include <openstudio/model/RunPeriod_Impl.hpp>
#include <openstudio/model/RunPeriodControlDaylightSavingTime.hpp>
#include <openstudio/model/RunPeriodControlDaylightSavingTime_Impl.hpp>
#include <openstudio/model/ShadowCalculation.hpp>
#include <openstudio/model/ShadowCalculation_Impl.hpp>
#include <openstudio/model/SimulationControl.hpp>
#include <openstudio/model/SimulationControl_Impl.hpp>
#include <openstudio/model/SizingParameters.hpp>
#include <openstudio/model/SizingParameters_Impl.hpp>
#include <openstudio/model/Timestep.hpp>
#include <openstudio/model/Timestep_Impl.hpp>
#include <openstudio/model/YearDescription.hpp>
#include <openstudio/model/YearDescription_Impl.hpp>
#include <openstudio/model/ZoneAirContaminantBalance.hpp>
#include <openstudio/model/ZoneAirContaminantBalance_Impl.hpp>
#include <openstudio/model/ZoneAirHeatBalanceAlgorithm.hpp>
#include <openstudio/model/ZoneAirHeatBalanceAlgorithm_Impl.hpp>
#include <openstudio/model/ZoneCapacitanceMultiplierResearchSpecial.hpp>
#include <openstudio/model/ZoneCapacitanceMultiplierResearchSpecial_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/ScheduleTypeLimits.hpp>
#include <openstudio/model/ScheduleTypeLimits_Impl.hpp>
#include <openstudio/model/ScheduleTypeRegistry.hpp>

#include <openstudio/utilities/time/Date.hpp>
#include <openstudio/utilities/time/Time.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QDate>
#include <QDateEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QTimer>

#define TEXT_FIELD_WIDTH 400
#define OSCOMBOBOX_FIELD_WIDTH 200
#define OSINTEGEREDIT_FIELD_WIDTH 90
#define SPACERITEM_HEIGHT 15
#define SPACERITEM_WIDTH 70
#define GRID_LAYOUT_SPACING 5

namespace openstudio {

SimSettingsView::SimSettingsView(bool isIP, const model::Model& model, QWidget* parent)
  : QWidget(parent),
    m_model(model),
    m_runSimWeatherFiles(nullptr),
    m_runSimDesignDays(nullptr),
    m_performZoneSizing(nullptr),
    m_performSystemSizing(nullptr),
    m_performPlantSizing(nullptr),
    m_runPeriodGroup(nullptr),
    m_radianceGroup(nullptr),
    m_runPeriodName(nullptr),
    m_isIP(isIP),
    // Run Period
    m_dateRangelabel(nullptr),
    m_startDateEdit(nullptr),
    m_endDateEdit(nullptr),
    m_useWeatherFileHolidaysandSpecialDays(nullptr),
    m_useWeatherFileDaylightSavingsPeriod(nullptr),
    m_applyWeekendHolidayRule(nullptr),
    m_useWeatherFileRainIndicators(nullptr),
    m_useWeatherFileSnowIndicators(nullptr),
    // SimulationControl
    m_doZoneSizingCalculation(nullptr),
    m_doSystemSizingCalculation(nullptr),
    m_doPlantSizingCalculation(nullptr),
    m_runSimulationforSizingPeriods(nullptr),
    m_runSimulationforWeatherFileRunPeriods(nullptr),
    m_maximumNumberofWarmupDays(nullptr),
    m_minimumNumberofWarmupDays(nullptr),
    m_loadsConvergenceToleranceValue(nullptr),
    m_temperatureConvergenceToleranceValue(nullptr),
    m_solarDistribution(nullptr),
    m_doHVACSizingSimulationforSizingPeriods(nullptr),
    m_maximumNumberofHVACSizingSimulationPasses(nullptr),

    // Radiance
    m_accumulatedRaysperRecord(nullptr),
    m_directThreshold(nullptr),
    m_directCertainty(nullptr),
    m_directJitter(nullptr),
    m_directPretest(nullptr),
    m_ambientBouncesVMX(nullptr),
    m_ambientBouncesDMX(nullptr),
    m_ambientDivisionsVMX(nullptr),
    m_ambientDivisionsDMX(nullptr),
    m_ambientSupersamples(nullptr),
    m_limitWeightVMX(nullptr),
    m_limitWeightDMX(nullptr),
    m_klemsSamplingDensity(nullptr),
    m_skyDiscretizationResolution(nullptr),
    m_accumulatedRaysperRecordLbl(nullptr),
    m_directThresholdLbl(nullptr),
    m_directCertaintyLbl(nullptr),
    m_directJitterLbl(nullptr),
    m_directPretestLbl(nullptr),
    m_ambientBouncesVMXLbl(nullptr),
    m_ambientBouncesDMXLbl(nullptr),
    m_ambientDivisionsVMXLbl(nullptr),
    m_ambientDivisionsDMXLbl(nullptr),
    m_ambientSupersamplesLbl(nullptr),
    m_limitWeightVMXLbl(nullptr),
    m_limitWeightDMXLbl(nullptr),
    m_klemsSamplingDensityLbl(nullptr),
    m_skyDiscretizationResolutionLbl(nullptr),
    // SizingParameters
    m_heatingSizingFactor(nullptr),
    m_coolingSizingFactor(nullptr),
    m_timestepsinAveragingWindow(nullptr),
    // ProgramControl
    m_numberOfThreadsAllowed(nullptr),
    // Timestep
    m_numberOfTimestepsPerHour(nullptr),
    // OutputControlReportingTolerances
    m_toleranceForTimeHeatingSetpointNotMet(nullptr),
    m_toleranceForTimeCoolingSetpointNotMet(nullptr),
    // ConvergenceLimits
    m_maximumHVACIterations(nullptr),
    m_minimumPlantIterations(nullptr),
    m_maximumPlantIterations(nullptr),
    m_minimumSystemTimestep(nullptr),
    // ShadowCalculation
    m_shadingCalculationUpdateFrequency(nullptr),
    m_maximumFiguresInShadowOverlapCalculations(nullptr),
    m_polygonClippingAlgorithm(nullptr),
    m_skyDiffuseModelingAlgorithm(nullptr),
    // SurfaceConvectionAlgorithmInside
    m_algorithmSurfaceConvectionInside(nullptr),
    // SurfaceConvectionAlgorithmOutside
    m_algorithmSurfaceConvectionOutside(nullptr),
    // HeatBalance
    m_algorithmHeatBalance(nullptr),
    m_surfaceTemperatureUpperLimit(nullptr),
    m_minimumSurfaceConvectionHeatTransferCoefficientValue(nullptr),
    m_maximumSurfaceConvectionHeatTransferCoefficientValue(nullptr),
    // ZoneAirHeatBalanceAlgorithm
    m_algorithmZoneAirHeatBalance(nullptr),
    // ZoneAirContaminantBalance
    m_carbonDioxideConcentration(nullptr),
    m_outdoorCarbonDioxideScheduleName(nullptr),
    // ZoneCapacitanceMultiplierResearchSpecial
    m_temperatureCapacityMultiplier(nullptr),
    m_humidityCapacityMultiplier(nullptr),
    m_carbonDioxideCapacityMultiplier(nullptr),
    // OutputJSON
    m_json_optionType(nullptr),
    m_json_outputJSON(nullptr),
    m_json_outputCBOR(nullptr),
    m_json_outputMessagePack(nullptr),

    // Advanced Output
    m_table_allSummary(nullptr),
    m_diagnostics_displayExtraWarnings(nullptr) {

  connect(this, &SimSettingsView::toggleUnitsClicked, this, &SimSettingsView::toggleUnits);

  // when the year settings object changes need to update the year in all child widgets
  model::YearDescription yearDescription = m_model.getUniqueModelObject<model::YearDescription>();
  yearDescription.getImpl<model::detail::YearDescription_Impl>().get()->onChange.connect<SimSettingsView, &SimSettingsView::updateYearDescription>(
    this);

  createWidgets();
  attachAll();
  updateYearDescription();
}

void SimSettingsView::createWidgets() {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  setLayout(mainLayout);
  setContentsMargins(0, 0, 0, 0);

  CollapsibleInspector* collapsibleInspector = nullptr;

  //******************* OS:RunPeriod *******************
  mainLayout->addWidget(createRunPeriodWidget());

  //******************* OS:RunControl *******************
  mainLayout->addWidget(createRunControlWidget());

  //******************* OS:Sizing:Parameters *******************
  mainLayout->addWidget(createSizingParametersWidget());

  //******************* OS:Timestep *******************
  mainLayout->addWidget(createTimestepWidget());

  collapsibleInspector = new CollapsibleInspector("Advanced RunPeriod Parameters", createRunPeriodAdvancedWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:RadianceParameters *******************
  collapsibleInspector = new CollapsibleInspector("Radiance Parameters", createRadianceParametersWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:SimulationControl *******************
  collapsibleInspector = new CollapsibleInspector("Simulation Control", createSimulationControlWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ProgramControl *******************
  collapsibleInspector = new CollapsibleInspector("Program Control", createProgramControlWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:OutputControl:ReportingTolerances *******************
  collapsibleInspector = new CollapsibleInspector("Output Control Reporting Tolerances", createOutputControlReportingTolerancesWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ConvergenceLimits *******************
  collapsibleInspector = new CollapsibleInspector("Convergence Limits", createConvergenceLimitsWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ShadowCalculation *******************
  collapsibleInspector = new CollapsibleInspector("Shadow Calculation", createShadowCalculationWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:SurfaceConvectionAlgorithm:Inside *******************
  collapsibleInspector = new CollapsibleInspector("Inside Surface Convection Algorithm", createSurfaceConvectionAlgorithmInsideWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:SurfaceConvectionAlgorithm:Outside *******************
  collapsibleInspector = new CollapsibleInspector("Outside Surface Convection Algorithm", createSurfaceConvectionAlgorithmOutsideWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:HeatBalanceAlgorithm *******************
  collapsibleInspector = new CollapsibleInspector("Heat Balance Algorithm", createHeatBalanceAlgorithmWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ZoneAirHeatBalanceAlgorithm *******************
  collapsibleInspector = new CollapsibleInspector("Zone Air Heat Balance Algorithm", createZoneAirHeatBalanceAlgorithmWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ZoneAirContaminantBalance *******************
  collapsibleInspector = new CollapsibleInspector("Zone Air Contaminant Balance", createZoneAirContaminantBalanceWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:ZoneCapacitanceMultiplier:ResearchSpecial *******************
  collapsibleInspector = new CollapsibleInspector("Zone Capacitance Multiple Research Special", createZoneCapacitanceMultipleResearchSpecialWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:Output:JSON *******************
  collapsibleInspector = new CollapsibleInspector("Output JSON", createOutputJSONWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:Output:Table:SummaryReports *******************
  collapsibleInspector = new CollapsibleInspector("Output Table Summary Reports", createOutputTableSummaryReportsWidget());
  mainLayout->addWidget(collapsibleInspector);

  //******************* OS:Output:Diagnostics *******************
  collapsibleInspector = new CollapsibleInspector("Output Diagnostics", createOutputDiagnosticsWidget());
  mainLayout->addWidget(collapsibleInspector);

  mainLayout->addStretch();
}

QWidget* SimSettingsView::createRunPeriodWidget() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(7, 0, 0, 0);
  mainVLayout->setSpacing(0);

  auto* mainHLayout = new QHBoxLayout();
  mainHLayout->setContentsMargins(7, 7, 7, 7);
  mainHLayout->setSpacing(0);

  QLabel* label = nullptr;

  QVBoxLayout* vLayout = nullptr;

  QHBoxLayout* hLayout = nullptr;

  label = new QLabel("Run Period");
  label->setObjectName("H1");
  mainVLayout->addWidget(label);

  mainVLayout->addLayout(mainHLayout);

  // LEFT
  // DLM: removing name from the Run Period inspector because this is a unique object
  //vLayout = new QVBoxLayout() ;
  //vLayout->setContentsMargins(0,0,0,0);
  //vLayout->setSpacing(10);
  //mainHLayout->addLayout(vLayout);
  //
  //label = new QLabel("Name");
  //label->setObjectName("H2");
  //vLayout->addWidget(label);
  //
  //hLayout = new QHBoxLayout() ;
  //hLayout->setContentsMargins(10,0,0,0);
  //hLayout->setSpacing(10);
  //vLayout->addLayout(hLayout);
  //
  //m_runPeriodName = new QLineEdit();
  //m_runPeriodName->setReadOnly(true);
  //m_runPeriodName->setFixedWidth(TEXT_FIELD_WIDTH);
  //hLayout->addWidget(m_runPeriodName);
  //
  //mainHLayout->addStretch();

  // CENTER
  // DLM: removing this button group because this is not an exclusive choice
  //vLayout = new QVBoxLayout() ;
  //vLayout->setContentsMargins(0,0,0,0);
  //vLayout->setSpacing(10);
  //mainHLayout->addLayout(vLayout);
  //
  //bool isConnected = false;
  //
  //m_runPeriodGroup = new QButtonGroup(this);
  //
  //isConnected = connect(m_runPeriodGroup, SIGNAL(buttonClicked(int)),
  //  this, SLOT(on_runPeriodGroupClicked(int)));
  //OS_ASSERT(isConnected);
  //
  //QRadioButton * radioButton = nullptr;
  //
  //int buttonCount = 0;
  //
  //radioButton = new QRadioButton("Annual Simulation");
  //m_runPeriodGroup->addButton(radioButton,buttonCount++);
  //vLayout->addWidget(radioButton);
  //
  //radioButton = new QRadioButton("Custom Period");
  //m_runPeriodGroup->addButton(radioButton,buttonCount);
  //vLayout->addWidget(radioButton);
  //
  //m_runPeriodGroup->button(0)->setChecked(true);
  //
  //mainHLayout->addStretch();

  // RIGHT
  vLayout = new QVBoxLayout();
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(10);
  mainHLayout->addLayout(vLayout);

  mainHLayout->addStretch();

  m_dateRangelabel = new QLabel("Date Range");
  m_dateRangelabel->setObjectName("H2");
  m_dateRangelabel->setEnabled(true);
  vLayout->addWidget(m_dateRangelabel);

  hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(10);
  vLayout->addLayout(hLayout);

  m_startDateEdit = new QDateEdit();
  m_startDateEdit->setDisplayFormat("MMMM d");
  m_startDateEdit->setCalendarPopup(true);
  //m_startDateEdit->setEnabled(false);
  hLayout->addWidget(m_startDateEdit);

  connect(m_startDateEdit, &QDateEdit::dateChanged, this, &SimSettingsView::on_startDateChanged);

  m_endDateEdit = new QDateEdit();
  m_endDateEdit->setDisplayFormat("MMMM d");
  m_endDateEdit->setCalendarPopup(true);
  //m_endDateEdit->setEnabled(false);
  hLayout->addWidget(m_endDateEdit);

  connect(m_endDateEdit, &QDateEdit::dateChanged, this, &SimSettingsView::on_endDateChanged);

  auto* widget = new QWidget();
  widget->setLayout(mainVLayout);

  return widget;
}

QWidget* SimSettingsView::createRunPeriodAdvancedWidget() {
  // These are the "advanced" fields from the RunPeriod object
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  // Number of times repeated isn't translated

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Use Weather File Holidays and Special Days", m_useWeatherFileHolidaysandSpecialDays);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Use Weather File Daylight Savings Period", m_useWeatherFileDaylightSavingsPeriod);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Use Weather File Rain Indicators", m_useWeatherFileRainIndicators);
  col = col + 2;
  addField(gridLayout, row, col, "Use Weather File Snow Indicators", m_useWeatherFileSnowIndicators);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Apply Weekend Holiday Rule", m_applyWeekendHolidayRule);
  col = col + 2;
  // Number of times repeated isn't translated
  // addField(gridLayout,row,col,"Number of Times Runperiod to be Repeate", m_numberTimeRepeat);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createRunControlWidget() {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  QLabel* label = nullptr;

  label = new QLabel("Run Control");
  label->setObjectName("H1");
  mainLayout->addWidget(label);

  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(10, 0, 0, 0);
  layout->setSpacing(0);
  mainLayout->addLayout(layout);

  m_runSimWeatherFiles = new QCheckBox("Run Simulation for Weather File");
  layout->addWidget(m_runSimWeatherFiles);

  connect(m_runSimWeatherFiles, &QCheckBox::stateChanged, this, &SimSettingsView::on_runSimWeatherFiles);

  m_runSimDesignDays = new QCheckBox("Run Simulation for Design Days");
  layout->addWidget(m_runSimDesignDays);

  connect(m_runSimDesignDays, &QCheckBox::stateChanged, this, &SimSettingsView::on_runSimDesignDays);

  m_performZoneSizing = new QCheckBox("Perform Zone Sizing");
  layout->addWidget(m_performZoneSizing);

  connect(m_performZoneSizing, &QCheckBox::stateChanged, this, &SimSettingsView::on_performZoneSizing);

  m_performSystemSizing = new QCheckBox("Perform System Sizing");
  layout->addWidget(m_performSystemSizing);

  connect(m_performSystemSizing, &QCheckBox::stateChanged, this, &SimSettingsView::on_performSystemSizing);

  m_performPlantSizing = new QCheckBox("Perform Plant Sizing");
  layout->addWidget(m_performPlantSizing);

  connect(m_performPlantSizing, &QCheckBox::stateChanged, this, &SimSettingsView::on_performPlantSizing);

  auto* widget = new QWidget();
  widget->setLayout(mainLayout);
  widget->hide();  // TODO delete when ready to use

  return widget;
}

QWidget* SimSettingsView::createSimulationControlWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Do Zone Sizing Calculation", m_doZoneSizingCalculation);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Do System Sizing Calculation", m_doSystemSizingCalculation);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Do Plant Sizing Calculation", m_doPlantSizingCalculation);
  col = col + 2;
  addField(gridLayout, row, col, "Run Simulation For Sizing Periods", m_runSimulationforSizingPeriods);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Run Simulation For Weather File Run Periods", m_runSimulationforWeatherFileRunPeriods);
  col = col + 2;
  addField(gridLayout, row, col, "Maximum Number Of Warmup Days", m_maximumNumberofWarmupDays);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Minimum Number Of Warmup Days", m_minimumNumberofWarmupDays);
  col = col + 2;
  addField(gridLayout, row, col, "Loads Convergence Tolerance Value", "", "", "", m_loadsConvergenceToleranceValue);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Temperature Convergence Tolerance Value", "K", "K", "R", m_temperatureConvergenceToleranceValue);
  col = col + 2;
  addField(gridLayout, row, col, "Solar Distribution", m_solarDistribution);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Do HVAC Sizing Simulation for Sizing Periods", m_doHVACSizingSimulationforSizingPeriods);
  col = col + 2;
  addField(gridLayout, row, col, "Maximum Number of HVAC Sizing Simulation Passes", m_maximumNumberofHVACSizingSimulationPasses);

  std::vector<std::string> validSolarDistributionValues = model::SimulationControl::validSolarDistributionValues();
  for (const std::string& validSolarDistributionValue : validSolarDistributionValues) {
    m_solarDistribution->addItem(validSolarDistributionValue.c_str());
  }

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createSizingParametersWidget() {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(7, 7, 7, 7);
  mainLayout->setSpacing(0);

  QLabel* label = nullptr;

  label = new QLabel("Sizing Parameters");
  label->setObjectName("H1");
  mainLayout->addWidget(label);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);
  mainLayout->addLayout(gridLayout);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Heating Sizing Factor", "", "", "", m_heatingSizingFactor);
  row = row + 2;
  addField(gridLayout, row, col, "Cooling Sizing Factor", "", "", "", m_coolingSizingFactor);
  row = row + 2;
  addField(gridLayout, row, col, "Timesteps In Averaging Window", m_timestepsinAveragingWindow);

  auto* widget = new QWidget();
  widget->setLayout(mainLayout);

  return widget;
}

QWidget* SimSettingsView::createProgramControlWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Number Of Threads Allowed", m_numberOfThreadsAllowed);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createTimestepWidget() {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(7, 7, 7, 7);
  mainLayout->setSpacing(0);

  QLabel* label = nullptr;

  label = new QLabel("Timestep");
  label->setObjectName("H1");
  mainLayout->addWidget(label);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);
  mainLayout->addLayout(gridLayout);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Number Of Timesteps Per Hour", m_numberOfTimestepsPerHour);

  auto* widget = new QWidget();
  widget->setLayout(mainLayout);

  return widget;
}

QWidget* SimSettingsView::createOutputControlReportingTolerancesWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Tolerance For Time Heating Setpoint Not Met", "K", "K", "R", m_toleranceForTimeHeatingSetpointNotMet);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Tolerance For Time Cooling Setpoint Not Met", "K", "K", "R", m_toleranceForTimeCoolingSetpointNotMet);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createConvergenceLimitsWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Maximum HVAC Iterations", m_maximumHVACIterations);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Minimum Plant Iterations", m_minimumPlantIterations);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Maximum Plant Iterations", m_maximumPlantIterations);
  col = col + 2;
  addField(gridLayout, row, col, "Minimum System Timestep", m_minimumSystemTimestep);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createShadowCalculationWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Shading Calculation Update Frequency", m_shadingCalculationUpdateFrequency);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Maximum Figures In Shadow Overlap Calculations", m_maximumFiguresInShadowOverlapCalculations);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Polygon Clipping Algorithm", m_polygonClippingAlgorithm);
  col = col + 2;
  addField(gridLayout, row, col, "Sky Diffuse Modeling Algorithm", m_skyDiffuseModelingAlgorithm);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createSurfaceConvectionAlgorithmInsideWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Algorithm", m_algorithmSurfaceConvectionInside);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createSurfaceConvectionAlgorithmOutsideWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Algorithm", m_algorithmSurfaceConvectionOutside);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createHeatBalanceAlgorithmWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Surface Temperature Upper Limit", "C", "C", "F", m_surfaceTemperatureUpperLimit);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Minimum Surface Convection Heat Transfer Coefficient Value", "W/m^2*K", "W/m^2*K", "Btu/ft^2*hr*R",
           m_minimumSurfaceConvectionHeatTransferCoefficientValue);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Maximum Surface Convection Heat Transfer Coefficient Value", "W/m^2*K", "W/m^2*K", "Btu/ft^2*hr*R",
           m_maximumSurfaceConvectionHeatTransferCoefficientValue);
  col = col + 2;
  addField(gridLayout, row, col, "Algorithm", m_algorithmHeatBalance);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createZoneAirHeatBalanceAlgorithmWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Algorithm", m_algorithmZoneAirHeatBalance);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createZoneAirContaminantBalanceWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Carbon Dioxide Concentration", m_carbonDioxideConcentration);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Outdoor Carbon Dioxide Schedule Name", m_outdoorCarbonDioxideScheduleName);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createZoneCapacitanceMultipleResearchSpecialWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Temperature Capacity Multiplier", "", "", "", m_temperatureCapacityMultiplier);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Humidity Capacity Multiplier", "", "", "", m_humidityCapacityMultiplier);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Carbon Dioxide Capacity Multiplier", "", "", "", m_carbonDioxideCapacityMultiplier);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createRadianceParametersWidget() {
  auto* vLayout = new QVBoxLayout();

  m_radianceGroup = new QButtonGroup(this);

  connect(m_radianceGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &SimSettingsView::on_radianceGroupClicked);

  int buttonCount = 0;

  QRadioButton* radioButton = nullptr;

  radioButton = new QRadioButton("Coarse (Fast, less accurate)");
  m_radianceGroup->addButton(radioButton, buttonCount++);
  vLayout->addWidget(radioButton);

  radioButton = new QRadioButton("Fine (Slow, more accurate)");
  m_radianceGroup->addButton(radioButton, buttonCount++);
  vLayout->addWidget(radioButton);

  radioButton = new QRadioButton("Custom");
  m_radianceGroup->addButton(radioButton, buttonCount);
  vLayout->addWidget(radioButton);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, m_accumulatedRaysperRecordLbl, "Accumulated Rays per Record: ", m_accumulatedRaysperRecord);
  col++;
  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, m_directThresholdLbl, "Direct Threshold: ", "", "", "", m_directThreshold);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_directCertaintyLbl, "Direct Certainty: ", "", "", "", m_directCertainty);
  col = col + 2;
  addField(gridLayout, row, col, m_directJitterLbl, "Direct Jitter: ", "", "", "", m_directJitter);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_directPretestLbl, "Direct Pretest: ", "", "", "", m_directPretest);
  col = col + 2;
  addField(gridLayout, row, col, m_ambientBouncesVMXLbl, "Ambient Bounces VMX: ", m_ambientBouncesVMX);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_ambientBouncesDMXLbl, "Ambient Bounces DMX: ", m_ambientBouncesDMX);
  col = col + 2;
  addField(gridLayout, row, col, m_ambientDivisionsVMXLbl, "Ambient Divisions VMX: ", m_ambientDivisionsVMX);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_ambientDivisionsDMXLbl, "Ambient Divisions DMX: ", m_ambientDivisionsDMX);
  col = col + 2;
  addField(gridLayout, row, col, m_ambientSupersamplesLbl, "Ambient Supersamples: ", m_ambientSupersamples);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_limitWeightVMXLbl, "Limit Weight VMX: ", "", "", "", m_limitWeightVMX);
  col = col + 2;
  addField(gridLayout, row, col, m_limitWeightDMXLbl, "Limit Weight DMX: ", "", "", "", m_limitWeightDMX);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, m_klemsSamplingDensityLbl, "Klems Sampling Density: ", m_klemsSamplingDensity);
  col = col + 2;
  addField(gridLayout, row, col, m_skyDiscretizationResolutionLbl, "Sky Discretization Resolution: ", m_skyDiscretizationResolution);

  std::vector<std::string> skyDiscretizationResolutionValues = model::RadianceParameters::skyDiscretizationResolutionValues();
  for (const std::string& skyDiscretizationResolutionValue : skyDiscretizationResolutionValues) {
    m_skyDiscretizationResolution->addItem(skyDiscretizationResolutionValue.c_str());
  }

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  vLayout->addLayout(gridLayout);

  auto* widget = new QWidget();
  widget->setLayout(vLayout);
  widget->hide();

  attachRadianceParameters();

  auto radianceParameters = m_model.getUniqueModelObject<model::RadianceParameters>();
  if (radianceParameters.isCoarseSettings()) {
    m_radianceGroup->button(0)->setChecked(true);
    enableRadianceParametersWidget(false);

  } else if (radianceParameters.isFineSettings()) {
    m_radianceGroup->button(1)->setChecked(true);
    enableRadianceParametersWidget(false);
  } else {
    m_radianceGroup->button(2)->setChecked(true);
    enableRadianceParametersWidget(true);
  }

  return widget;
}

void SimSettingsView::enableRadianceParametersWidget(bool isEnabled) {
  m_accumulatedRaysperRecord->setEnabled(isEnabled);
  m_directThreshold->setEnabled(isEnabled);
  m_directCertainty->setEnabled(isEnabled);
  m_directJitter->setEnabled(isEnabled);
  m_directPretest->setEnabled(isEnabled);
  m_ambientBouncesVMX->setEnabled(isEnabled);
  m_ambientBouncesDMX->setEnabled(isEnabled);
  m_ambientDivisionsVMX->setEnabled(isEnabled);
  m_ambientDivisionsDMX->setEnabled(isEnabled);
  m_ambientSupersamples->setEnabled(isEnabled);
  m_limitWeightVMX->setEnabled(isEnabled);
  m_limitWeightDMX->setEnabled(isEnabled);
  m_klemsSamplingDensity->setEnabled(isEnabled);
  m_skyDiscretizationResolution->setEnabled(isEnabled);
  m_accumulatedRaysperRecordLbl->setEnabled(isEnabled);
  m_directThresholdLbl->setEnabled(isEnabled);
  m_directCertaintyLbl->setEnabled(isEnabled);
  m_directJitterLbl->setEnabled(isEnabled);
  m_directPretestLbl->setEnabled(isEnabled);
  m_ambientBouncesVMXLbl->setEnabled(isEnabled);
  m_ambientBouncesDMXLbl->setEnabled(isEnabled);
  m_ambientDivisionsVMXLbl->setEnabled(isEnabled);
  m_ambientDivisionsDMXLbl->setEnabled(isEnabled);
  m_ambientSupersamplesLbl->setEnabled(isEnabled);
  m_limitWeightVMXLbl->setEnabled(isEnabled);
  m_limitWeightDMXLbl->setEnabled(isEnabled);
  m_klemsSamplingDensityLbl->setEnabled(isEnabled);
  m_skyDiscretizationResolutionLbl->setEnabled(isEnabled);
}

QWidget* SimSettingsView::createOutputJSONWidget() {
  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;
  QSpacerItem* spacerItem = nullptr;

  addField(gridLayout, row, col, "Option Type", m_json_optionType);
  col++;
  std::vector<std::string> optionValues = model::OutputJSON::optionTypeValues();
  for (const auto& optionValue : optionValues) {
    m_json_optionType->addItem(optionValue.c_str());
  }

  spacerItem = new QSpacerItem(SPACERITEM_WIDTH, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row, col++);
  addField(gridLayout, row, col, "Output JSON", m_json_outputJSON);

  row = row + 2;
  spacerItem = new QSpacerItem(1, SPACERITEM_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
  gridLayout->addItem(spacerItem, row++, 0);
  col = 0;

  addField(gridLayout, row, col, "Output CBOR", m_json_outputCBOR);
  col = col + 2;
  addField(gridLayout, row, col, "Output MessagePack", m_json_outputMessagePack);

  gridLayout->setRowStretch(100, 100);
  gridLayout->setColumnStretch(100, 100);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createOutputTableSummaryReportsWidget() {

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Enable AllSummary Report", m_table_allSummary);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

QWidget* SimSettingsView::createOutputDiagnosticsWidget() {

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(7, 7, 7, 7);
  gridLayout->setSpacing(GRID_LAYOUT_SPACING);
  gridLayout->setAlignment(Qt::AlignLeft);

  int row = 0;
  int col = 0;

  addField(gridLayout, row, col, "Enable DisplayExtraWarnings", m_diagnostics_displayExtraWarnings);

  auto* widget = new QWidget();
  widget->setLayout(gridLayout);
  widget->hide();

  return widget;
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QString text, OSComboBox2*& comboBox) {
  auto* label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  comboBox = new OSComboBox2(this);
  comboBox->setFixedWidth(OSCOMBOBOX_FIELD_WIDTH);
  gridLayout->addWidget(comboBox, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, OSComboBox2*& comboBox) {
  label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  comboBox = new OSComboBox2(this);
  comboBox->setFixedWidth(OSCOMBOBOX_FIELD_WIDTH);
  gridLayout->addWidget(comboBox, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QString text, OSIntegerEdit2*& integerEdit) {
  auto* label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  integerEdit = new OSIntegerEdit2(this);
  integerEdit->setFixedWidth(OSINTEGEREDIT_FIELD_WIDTH);
  gridLayout->addWidget(integerEdit, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, OSIntegerEdit2*& integerEdit) {
  label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  integerEdit = new OSIntegerEdit2(this);
  integerEdit->setFixedWidth(OSINTEGEREDIT_FIELD_WIDTH);
  gridLayout->addWidget(integerEdit, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QString text, OSLineEdit2*& lineEdit) {
  auto* label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  lineEdit = new OSLineEdit2(this);
  lineEdit->setFixedWidth(TEXT_FIELD_WIDTH);
  gridLayout->addWidget(lineEdit, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QString text, const std::string& modelUnits, const std::string& siUnits,
                               const std::string& ipUnits, OSQuantityEdit2*& quantityEdit) {
  auto* label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  quantityEdit = new OSQuantityEdit2(modelUnits, siUnits, ipUnits, m_isIP, this);
  connect(this, &SimSettingsView::toggleUnitsClicked, quantityEdit, &OSQuantityEdit2::onUnitSystemChange);

  gridLayout->addWidget(quantityEdit, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, const std::string& modelUnits,
                               const std::string& siUnits, const std::string& ipUnits, OSQuantityEdit2*& quantityEdit) {
  label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  quantityEdit = new OSQuantityEdit2(modelUnits, siUnits, ipUnits, m_isIP, this);
  connect(this, &SimSettingsView::toggleUnitsClicked, quantityEdit, &OSQuantityEdit2::onUnitSystemChange);

  gridLayout->addWidget(quantityEdit, row, column);
}

void SimSettingsView::addField(QGridLayout* gridLayout, int row, int column, QString text, OSSwitch2*& osSwitch) {
  auto* label = new QLabel(text, this);
  label->setFixedWidth(TEXT_FIELD_WIDTH);
  label->setObjectName("H2");
  gridLayout->addWidget(label, row++, column);

  osSwitch = new OSSwitch2(this);
  gridLayout->addWidget(osSwitch, row, column);
}

void SimSettingsView::attachAll() {
  attachRunPeriod();
  attachRunPeriodControlDaylightSavingTime();
  attachSimulationControl();
  attachSizingParameters();
  attachProgramControl();
  attachTimestep();
  attachOutputControlReportingTolerances();
  attachConvergenceLimits();
  attachShadowCalculation();
  attachSurfaceConvectionAlgorithmInside();
  attachSurfaceConvectionAlgorithmOutside();
  attachHeatBalanceAlgorithm();
  attachZoneAirHeatBalanceAlgorithm();
  attachZoneAirContaminantBalance();
  attachZoneCapacitanceMultipleResearchSpecial();
  attachOutputJSON();
  attachOutputTableSummaryReports();
  attachOutputDiagnostics();
}

void SimSettingsView::detachAll() {
  detachRunPeriod();
  detachRunPeriodControlDaylightSavingTime();
  detachSimulationControl();
  detachSizingParameters();
  detachProgramControl();
  detachTimestep();
  detachOutputControlReportingTolerances();
  detachConvergenceLimits();
  detachShadowCalculation();
  detachSurfaceConvectionAlgorithmInside();
  detachSurfaceConvectionAlgorithmOutside();
  detachHeatBalanceAlgorithm();
  detachZoneAirHeatBalanceAlgorithm();
  detachZoneAirContaminantBalance();
  detachZoneCapacitanceMultipleResearchSpecial();
  detachRadianceParameters();
  detachOutputJSON();
  detachOutputTableSummaryReports();
  detachOutputDiagnostics();
}

void SimSettingsView::attachRunPeriod() {
  m_runPeriod = m_model.getUniqueModelObject<model::RunPeriod>();

  m_useWeatherFileHolidaysandSpecialDays->bind(
    *m_runPeriod, std::bind(&model::RunPeriod::getUseWeatherFileHolidays, m_runPeriod.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::RunPeriod::setUseWeatherFileHolidays, m_runPeriod.get_ptr(), std::placeholders::_1)),
    boost::none,  // reset
    boost::none   // isDefaulted
  );

  m_useWeatherFileDaylightSavingsPeriod->bind(
    *m_runPeriod, std::bind(&model::RunPeriod::getUseWeatherFileDaylightSavings, m_runPeriod.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::RunPeriod::setUseWeatherFileDaylightSavings, m_runPeriod.get_ptr(), std::placeholders::_1)),
    boost::none,  // reset
    boost::none   // isDefaulted
  );

  m_useWeatherFileRainIndicators->bind(
    *m_runPeriod, std::bind(&model::RunPeriod::getUseWeatherFileRainInd, m_runPeriod.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::RunPeriod::setUseWeatherFileRainInd, m_runPeriod.get_ptr(), std::placeholders::_1)),
    boost::none,  // reset
    boost::none   // isDefaulted
  );

  m_useWeatherFileSnowIndicators->bind(
    *m_runPeriod, std::bind(&model::RunPeriod::getUseWeatherFileSnowInd, m_runPeriod.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::RunPeriod::setUseWeatherFileSnowInd, m_runPeriod.get_ptr(), std::placeholders::_1)),
    boost::none,  // reset
    boost::none   // isDefaulted
  );

  m_applyWeekendHolidayRule->bind(
    *m_runPeriod, std::bind(&model::RunPeriod::getApplyWeekendHolidayRule, m_runPeriod.get_ptr()),
    boost::optional<BoolSetter>(std::bind(&model::RunPeriod::setApplyWeekendHolidayRule, m_runPeriod.get_ptr(), std::placeholders::_1)),
    boost::none,  // reset
    boost::none   // isDefaulted
  );
}

void SimSettingsView::attachRunPeriodControlDaylightSavingTime() {}

void SimSettingsView::attachSimulationControl() {
  m_simulationControl = m_model.getUniqueModelObject<model::SimulationControl>();

  m_doZoneSizingCalculation->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::doZoneSizingCalculation, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setDoZoneSizingCalculationNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetDoZoneSizingCalculation, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isDoZoneSizingCalculationDefaulted, m_simulationControl.get_ptr())));
  m_doSystemSizingCalculation->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::doSystemSizingCalculation, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setDoSystemSizingCalculationNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetDoSystemSizingCalculation, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isDoSystemSizingCalculationDefaulted, m_simulationControl.get_ptr())));
  m_doPlantSizingCalculation->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::doPlantSizingCalculation, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setDoPlantSizingCalculationNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetDoPlantSizingCalculation, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isDoPlantSizingCalculationDefaulted, m_simulationControl.get_ptr())));
  m_runSimulationforSizingPeriods->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::runSimulationforSizingPeriods, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setRunSimulationforSizingPeriodsNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetRunSimulationforSizingPeriods, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isRunSimulationforSizingPeriodsDefaulted, m_simulationControl.get_ptr())));
  m_runSimulationforWeatherFileRunPeriods->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::runSimulationforWeatherFileRunPeriods, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setRunSimulationforWeatherFileRunPeriodsNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetRunSimulationforWeatherFileRunPeriods, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(
      std::bind(&model::SimulationControl::isRunSimulationforWeatherFileRunPeriodsDefaulted, m_simulationControl.get_ptr())));
  m_maximumNumberofWarmupDays->bind(
    *m_simulationControl, IntGetter(std::bind(&model::SimulationControl::maximumNumberofWarmupDays, m_simulationControl.get_ptr())),
    boost::optional<IntSetter>(
      std::bind(&model::SimulationControl::setMaximumNumberofWarmupDays, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetMaximumNumberofWarmupDays, m_simulationControl.get_ptr())), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isMaximumNumberofWarmupDaysDefaulted, m_simulationControl.get_ptr())));
  m_minimumNumberofWarmupDays->bind(
    *m_simulationControl, IntGetter(std::bind(&model::SimulationControl::minimumNumberofWarmupDays, m_simulationControl.get_ptr())),
    boost::optional<IntSetter>(
      std::bind(&model::SimulationControl::setMinimumNumberofWarmupDays, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetMinimumNumberofWarmupDays, m_simulationControl.get_ptr())), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isMinimumNumberofWarmupDaysDefaulted, m_simulationControl.get_ptr())));

  // m_loadsConvergenceToleranceValue->bind(*m_simulationControl,"loadsConvergenceToleranceValue",m_isIP);
  m_loadsConvergenceToleranceValue->bind(
    m_isIP, *m_simulationControl, DoubleGetter(std::bind(&model::SimulationControl::loadsConvergenceToleranceValue, m_simulationControl.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::SimulationControl::*)(double)>(&model::SimulationControl::setLoadsConvergenceToleranceValue),
                m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetLoadsConvergenceToleranceValue, m_simulationControl.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isLoadsConvergenceToleranceValueDefaulted, m_simulationControl.get_ptr())));

  // m_temperatureConvergenceToleranceValue->bind(*m_simulationControl,"temperatureConvergenceToleranceValue",m_isIP);
  m_temperatureConvergenceToleranceValue->bind(
    m_isIP, *m_simulationControl,
    DoubleGetter(std::bind(&model::SimulationControl::temperatureConvergenceToleranceValue, m_simulationControl.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::SimulationControl::*)(double)>(&model::SimulationControl::setTemperatureConvergenceToleranceValue),
                m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetTemperatureConvergenceToleranceValue, m_simulationControl.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SimulationControl::isTemperatureConvergenceToleranceValueDefaulted, m_simulationControl.get_ptr())));

  // m_solarDistribution->bind(*m_simulationControl,"solarDistribution");
  m_solarDistribution->bind<std::string>(
    *m_simulationControl, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    &model::SimulationControl::validSolarDistributionValues,
    StringGetter(std::bind(&model::SimulationControl::solarDistribution, m_simulationControl.get_ptr())),
    std::bind(&model::SimulationControl::setSolarDistribution, m_simulationControl.get_ptr(), std::placeholders::_1),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetSolarDistribution, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(std::bind(&model::SimulationControl::isSolarDistributionDefaulted, m_simulationControl.get_ptr())));

  m_doHVACSizingSimulationforSizingPeriods->bind(
    *m_simulationControl, std::bind(&model::SimulationControl::doHVACSizingSimulationforSizingPeriods, m_simulationControl.get_ptr()),
    boost::optional<BoolSetter>(
      std::bind(&model::SimulationControl::setDoHVACSizingSimulationforSizingPeriodsNoFail, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SimulationControl::resetDoHVACSizingSimulationforSizingPeriods, m_simulationControl.get_ptr())),
    boost::optional<BasicQuery>(
      std::bind(&model::SimulationControl::isDoHVACSizingSimulationforSizingPeriodsDefaulted, m_simulationControl.get_ptr())));

  m_maximumNumberofHVACSizingSimulationPasses->bind(
    *m_simulationControl, IntGetter(std::bind(&model::SimulationControl::maximumNumberofHVACSizingSimulationPasses, m_simulationControl.get_ptr())),
    boost::optional<IntSetter>(
      std::bind(&model::SimulationControl::setMaximumNumberofHVACSizingSimulationPasses, m_simulationControl.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SimulationControl::resetMaximumNumberofHVACSizingSimulationPasses, m_simulationControl.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SimulationControl::isMaximumNumberofHVACSizingSimulationPassesDefaulted, m_simulationControl.get_ptr())));
}

void SimSettingsView::attachSizingParameters() {
  model::SizingParameters mo = m_model.getUniqueModelObject<model::SizingParameters>();

  // m_heatingSizingFactor->bind(mo,"heatingSizingFactor",m_isIP);
  m_heatingSizingFactor->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::SizingParameters::heatingSizingFactor, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::SizingParameters::*)(double)>(&model::SizingParameters::setHeatingSizingFactor), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SizingParameters::resetHeatingSizingFactor, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::SizingParameters::isHeatingSizingFactorDefaulted, mo)));

  // m_coolingSizingFactor->bind(mo,"coolingSizingFactor",m_isIP);
  m_coolingSizingFactor->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::SizingParameters::coolingSizingFactor, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::SizingParameters::*)(double)>(&model::SizingParameters::setCoolingSizingFactor), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SizingParameters::resetCoolingSizingFactor, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::SizingParameters::isCoolingSizingFactorDefaulted, mo)));
  // m_timestepsinAveragingWindow->bind(mo,"timestepsinAveragingWindow");

  m_timestepsinAveragingWindow->bind(
    mo, OptionalIntGetter(std::bind(&model::SizingParameters::timestepsinAveragingWindow, mo)),
    boost::optional<IntSetter>(std::bind(&model::SizingParameters::setTimestepsinAveragingWindow, mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::SizingParameters::resetTimestepsinAveragingWindow, mo)), boost::optional<NoFailAction>(),
    boost::optional<NoFailAction>(), boost::optional<BasicQuery>(), boost::optional<BasicQuery>(), boost::optional<BasicQuery>());
}

void SimSettingsView::attachProgramControl() {
  auto mo = m_model.getUniqueModelObject<model::ProgramControl>();

  // m_numberOfThreadsAllowed->bind(mo,"numberofThreadsAllowed");
  m_numberOfThreadsAllowed->bind(mo, OptionalIntGetter(std::bind(&model::ProgramControl::numberofThreadsAllowed, mo)),
                                 boost::optional<IntSetter>(std::bind(&model::ProgramControl::setNumberofThreadsAllowed, mo, std::placeholders::_1)),
                                 boost::optional<NoFailAction>(std::bind(&model::ProgramControl::resetNumberofThreadsAllowed, mo)));
}

void SimSettingsView::attachTimestep() {
  model::Timestep mo = m_model.getUniqueModelObject<model::Timestep>();

  // m_numberOfTimestepsPerHour->bind(mo,"numberOfTimestepsPerHour");
  m_numberOfTimestepsPerHour->bind(mo,
                                   // FIXED_LAST
                                   IntGetter(std::bind(&model::Timestep::numberOfTimestepsPerHour, mo)),
                                   boost::optional<IntSetter>(std::bind(&model::Timestep::setNumberOfTimestepsPerHour, mo, std::placeholders::_1)),
                                   boost::optional<NoFailAction>(std::bind(&model::Timestep::resetNumberOfTimestepsPerHour, mo)), boost::none,
                                   boost::none, boost::optional<BasicQuery>(std::bind(&model::Timestep::isNumberOfTimestepsPerHourDefaulted, mo)));
}

void SimSettingsView::attachOutputControlReportingTolerances() {
  model::OutputControlReportingTolerances mo = m_model.getUniqueModelObject<model::OutputControlReportingTolerances>();

  // m_toleranceForTimeHeatingSetpointNotMet->bind(mo,"toleranceforTimeHeatingSetpointNotMet",m_isIP);
  m_toleranceForTimeHeatingSetpointNotMet->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::OutputControlReportingTolerances::toleranceforTimeHeatingSetpointNotMet, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::OutputControlReportingTolerances::*)(double)>(
                                              &model::OutputControlReportingTolerances::setToleranceforTimeHeatingSetpointNotMet),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::OutputControlReportingTolerances::resetToleranceforTimeHeatingSetpointNotMet, mo)), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::OutputControlReportingTolerances::isToleranceforTimeHeatingSetpointNotMetDefaulted, mo)));

  // m_toleranceForTimeCoolingSetpointNotMet->bind(mo,"toleranceforTimeCoolingSetpointNotMet",m_isIP);
  m_toleranceForTimeCoolingSetpointNotMet->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::OutputControlReportingTolerances::toleranceforTimeCoolingSetpointNotMet, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::OutputControlReportingTolerances::*)(double)>(
                                              &model::OutputControlReportingTolerances::setToleranceforTimeCoolingSetpointNotMet),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::OutputControlReportingTolerances::resetToleranceforTimeCoolingSetpointNotMet, mo)), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::OutputControlReportingTolerances::isToleranceforTimeCoolingSetpointNotMetDefaulted, mo)));
}

void SimSettingsView::attachConvergenceLimits() {
  model::ConvergenceLimits mo = m_model.getUniqueModelObject<model::ConvergenceLimits>();

  // m_maximumHVACIterations->bind(mo,"maximumHVACIterations");
  m_maximumHVACIterations->bind(mo, IntGetter(std::bind(&model::ConvergenceLimits::maximumHVACIterations, mo)),
                                boost::optional<IntSetter>(std::bind(&model::ConvergenceLimits::setMaximumHVACIterations, mo, std::placeholders::_1)),
                                boost::optional<NoFailAction>(std::bind(&model::ConvergenceLimits::resetMaximumHVACIterations, mo)), boost::none,
                                boost::none, boost::optional<BasicQuery>(std::bind(&model::ConvergenceLimits::isMaximumHVACIterationsDefaulted, mo)));

  // m_minimumPlantIterations->bind(mo,"minimumPlantIterations");
  m_minimumPlantIterations->bind(
    mo, IntGetter(std::bind(&model::ConvergenceLimits::minimumPlantIterations, mo)),
    boost::optional<IntSetter>(std::bind(&model::ConvergenceLimits::setMinimumPlantIterations, mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ConvergenceLimits::resetMinimumPlantIterations, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::ConvergenceLimits::isMinimumPlantIterationsDefaulted, mo)));

  // m_maximumPlantIterations->bind(mo,"maximumPlantIterations");
  m_maximumPlantIterations->bind(
    mo, IntGetter(std::bind(&model::ConvergenceLimits::maximumPlantIterations, mo)),
    boost::optional<IntSetter>(std::bind(&model::ConvergenceLimits::setMaximumPlantIterations, mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ConvergenceLimits::resetMaximumPlantIterations, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::ConvergenceLimits::isMaximumPlantIterationsDefaulted, mo)));

  // m_minimumSystemTimestep->bind(mo,"minimumSystemTimestep");
  m_minimumSystemTimestep->bind(mo, OptionalIntGetter(std::bind(&model::ConvergenceLimits::minimumSystemTimestep, mo)),
                                boost::optional<IntSetter>(std::bind(&model::ConvergenceLimits::setMinimumSystemTimestep, mo, std::placeholders::_1)),
                                boost::optional<NoFailAction>(std::bind(&model::ConvergenceLimits::resetMinimumSystemTimestep, mo)));
}

void SimSettingsView::attachShadowCalculation() {
  m_shadowCalculation = m_model.getUniqueModelObject<model::ShadowCalculation>();

  // m_shadingCalculationUpdateFrequency->bind(*m_shadowCalculation,"shadingCalculationUpdateFrequency");
  m_shadingCalculationUpdateFrequency->bind(
    *m_shadowCalculation, IntGetter(std::bind(&model::ShadowCalculation::shadingCalculationUpdateFrequency, m_shadowCalculation.get_ptr())),
    boost::optional<IntSetter>(
      std::bind(&model::ShadowCalculation::setShadingCalculationUpdateFrequency, m_shadowCalculation.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ShadowCalculation::resetShadingCalculationUpdateFrequency, m_shadowCalculation.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::ShadowCalculation::isShadingCalculationUpdateFrequencyDefaulted, m_shadowCalculation.get_ptr())));

  // m_maximumFiguresInShadowOverlapCalculations->bind(*m_shadowCalculation, "maximumFiguresInShadowOverlapCalculations");
  m_maximumFiguresInShadowOverlapCalculations->bind(
    *m_shadowCalculation, IntGetter(std::bind(&model::ShadowCalculation::maximumFiguresInShadowOverlapCalculations, m_shadowCalculation.get_ptr())),
    boost::optional<IntSetter>(
      std::bind(&model::ShadowCalculation::setMaximumFiguresInShadowOverlapCalculations, m_shadowCalculation.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::ShadowCalculation::resetMaximumFiguresInShadowOverlapCalculations, m_shadowCalculation.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::ShadowCalculation::isMaximumFiguresInShadowOverlapCalculationsDefaulted, m_shadowCalculation.get_ptr())));

  m_polygonClippingAlgorithm->bind<std::string>(
    *m_shadowCalculation, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    std::bind(model::ShadowCalculation::validPolygonClippingAlgorithmValues),
    std::function<boost::optional<std::string>()>(std::bind(&model::ShadowCalculation::polygonClippingAlgorithm, m_shadowCalculation.get_ptr())),
    std::bind(&model::ShadowCalculation::setPolygonClippingAlgorithm, m_shadowCalculation.get_ptr(), std::placeholders::_1),
    NoFailAction(std::bind(&model::ShadowCalculation::resetPolygonClippingAlgorithm, m_shadowCalculation.get_ptr())));

  m_skyDiffuseModelingAlgorithm->bind<std::string>(
    *m_shadowCalculation, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    std::bind(model::ShadowCalculation::validSkyDiffuseModelingAlgorithmValues),
    std::function<boost::optional<std::string>()>(std::bind(&model::ShadowCalculation::skyDiffuseModelingAlgorithm, m_shadowCalculation.get_ptr())),
    std::bind(&model::ShadowCalculation::setSkyDiffuseModelingAlgorithm, m_shadowCalculation.get_ptr(), std::placeholders::_1),
    NoFailAction(std::bind(&model::ShadowCalculation::resetSkyDiffuseModelingAlgorithm, m_shadowCalculation.get_ptr())));
}

void SimSettingsView::attachSurfaceConvectionAlgorithmInside() {
  model::InsideSurfaceConvectionAlgorithm mo = m_model.getUniqueModelObject<model::InsideSurfaceConvectionAlgorithm>();

  m_algorithmSurfaceConvectionInside->bind<std::string>(
    mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    std::bind(model::InsideSurfaceConvectionAlgorithm::validAlgorithmValues),
    std::function<boost::optional<std::string>()>(std::bind(&model::InsideSurfaceConvectionAlgorithm::algorithm, mo)),
    std::bind(&model::InsideSurfaceConvectionAlgorithm::setAlgorithm, mo, std::placeholders::_1),
    NoFailAction(std::bind(&model::InsideSurfaceConvectionAlgorithm::resetAlgorithm, mo)));
}

void SimSettingsView::attachSurfaceConvectionAlgorithmOutside() {
  model::OutsideSurfaceConvectionAlgorithm mo = m_model.getUniqueModelObject<model::OutsideSurfaceConvectionAlgorithm>();

  m_algorithmSurfaceConvectionOutside->bind<std::string>(
    mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    std::bind(model::OutsideSurfaceConvectionAlgorithm::validAlgorithmValues),
    std::function<boost::optional<std::string>()>(std::bind(&model::OutsideSurfaceConvectionAlgorithm::algorithm, mo)),
    std::bind(&model::OutsideSurfaceConvectionAlgorithm::setAlgorithm, mo, std::placeholders::_1),
    NoFailAction(std::bind(&model::OutsideSurfaceConvectionAlgorithm::resetAlgorithm, mo)));
}

void SimSettingsView::attachHeatBalanceAlgorithm() {
  model::HeatBalanceAlgorithm mo = m_model.getUniqueModelObject<model::HeatBalanceAlgorithm>();

  m_algorithmHeatBalance->bind<std::string>(mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
                                            std::bind(model::HeatBalanceAlgorithm::algorithmValues),
                                            std::function<boost::optional<std::string>()>(std::bind(&model::HeatBalanceAlgorithm::algorithm, mo)),
                                            std::bind(&model::HeatBalanceAlgorithm::setAlgorithm, mo, std::placeholders::_1),
                                            NoFailAction(std::bind(&model::HeatBalanceAlgorithm::resetAlgorithm, mo)));

  // m_surfaceTemperatureUpperLimit->bind(mo, "surfaceTemperatureUpperLimit", m_isIP);
  m_surfaceTemperatureUpperLimit->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::HeatBalanceAlgorithm::surfaceTemperatureUpperLimit, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::HeatBalanceAlgorithm::*)(double)>(&model::HeatBalanceAlgorithm::setSurfaceTemperatureUpperLimit), mo,
                std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HeatBalanceAlgorithm::resetSurfaceTemperatureUpperLimit, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HeatBalanceAlgorithm::isSurfaceTemperatureUpperLimitDefaulted, mo)));

  // m_minimumSurfaceConvectionHeatTransferCoefficientValue->bind(mo,"minimumSurfaceConvectionHeatTransferCoefficientValue",m_isIP);
  m_minimumSurfaceConvectionHeatTransferCoefficientValue->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::HeatBalanceAlgorithm::minimumSurfaceConvectionHeatTransferCoefficientValue, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::HeatBalanceAlgorithm::*)(double)>(
                                              &model::HeatBalanceAlgorithm::setMinimumSurfaceConvectionHeatTransferCoefficientValue),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HeatBalanceAlgorithm::resetMinimumSurfaceConvectionHeatTransferCoefficientValue, mo)),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HeatBalanceAlgorithm::isMinimumSurfaceConvectionHeatTransferCoefficientValueDefaulted, mo)));

  // m_maximumSurfaceConvectionHeatTransferCoefficientValue->bind(mo,"maximumSurfaceConvectionHeatTransferCoefficientValue",m_isIP);
  m_maximumSurfaceConvectionHeatTransferCoefficientValue->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::HeatBalanceAlgorithm::maximumSurfaceConvectionHeatTransferCoefficientValue, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::HeatBalanceAlgorithm::*)(double)>(
                                              &model::HeatBalanceAlgorithm::setMaximumSurfaceConvectionHeatTransferCoefficientValue),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::HeatBalanceAlgorithm::resetMaximumSurfaceConvectionHeatTransferCoefficientValue, mo)),
    boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::HeatBalanceAlgorithm::isMaximumSurfaceConvectionHeatTransferCoefficientValueDefaulted, mo)));
}

void SimSettingsView::attachZoneAirHeatBalanceAlgorithm() {
  model::ZoneAirHeatBalanceAlgorithm mo = m_model.getUniqueModelObject<model::ZoneAirHeatBalanceAlgorithm>();

  m_algorithmZoneAirHeatBalance->bind<std::string>(
    mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString), std::bind(model::ZoneAirHeatBalanceAlgorithm::validAlgorithmValues),
    std::function<boost::optional<std::string>()>(std::bind(&model::ZoneAirHeatBalanceAlgorithm::algorithm, mo)),
    std::bind(&model::ZoneAirHeatBalanceAlgorithm::setAlgorithm, mo, std::placeholders::_1),
    NoFailAction(std::bind(&model::ZoneAirHeatBalanceAlgorithm::resetAlgorithm, mo)));
}

void SimSettingsView::attachZoneAirContaminantBalance() {
  model::ZoneAirContaminantBalance mo = m_model.getUniqueModelObject<model::ZoneAirContaminantBalance>();

  // m_carbonDioxideConcentration->bind(mo,"carbonDioxideConcentration");
  m_carbonDioxideConcentration->bind(
    mo, std::bind(&model::ZoneAirContaminantBalance::carbonDioxideConcentration, mo),
    boost::optional<BoolSetter>(std::bind(&model::ZoneAirContaminantBalance::setCarbonDioxideConcentrationNoFail, mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ZoneAirContaminantBalance::resetCarbonDioxideConcentration, mo)),
    boost::optional<BasicQuery>(std::bind(&model::ZoneAirContaminantBalance::isCarbonDioxideConcentrationDefaulted, mo)));

  /* // This would work fine too
   *std::function<std::string (const model::Schedule &)> toString =
   *  [](const model::Schedule& t_s) {
   *    return t_s.nameString();
   *  };
   */

  std::function<std::vector<model::Schedule>(model::ZoneAirContaminantBalance*)> choices_lambda = [](model::ZoneAirContaminantBalance* t_z) {
    // openstudio::model::getCompatibleSchedules uses stringent checking, allow non-stringent checking here
    //std::vector<model::Schedule> results =
    //  openstudio::model::getCompatibleSchedules(t_z->model(), "ZoneAirContaminantBalance", "Outdoor Carbon Dioxide");

    model::Model model = t_z->model();
    model::ScheduleType scheduleType = model::ScheduleTypeRegistry::instance().getScheduleType("ZoneAirContaminantBalance", "Outdoor Carbon Dioxide");

    bool isStringent = false;
    model::ScheduleTypeLimitsVector okTypes;
    for (const model::ScheduleTypeLimits& candidate : model.getConcreteModelObjects<model::ScheduleTypeLimits>()) {
      if (isCompatible(scheduleType, candidate, isStringent)) {
        okTypes.push_back(candidate);
      }
    }

    model::ScheduleVector results;
    for (const model::Schedule& candidate : model.getModelObjects<model::Schedule>()) {
      if (model::OptionalScheduleTypeLimits candidateType = candidate.scheduleTypeLimits()) {
        if (std::find(okTypes.begin(), okTypes.end(), *candidateType) != okTypes.end()) {
          results.push_back(candidate);
        }
      } else {
        // by default, keep all non-typed schedules
        results.push_back(candidate);
      }
    }

    return openstudio::sortByObjectName<model::Schedule>(results);
  };

  /*
   * // The following works fine too
   *std::function<std::vector<model::Schedule> (model::ZoneAirContaminantBalance *)> choices = std::bind(&openstudio::sortByObjectName<model::Schedule>,
   *                                                                   std::bind(&openstudio::model::getCompatibleSchedules,
   *                                                                             m_model,
   *                                                                             "ZoneAirContaminantBalance",
   *                                                                             "Outdoor Carbon Dioxide"));
   */

  std::function<boost::optional<model::Schedule>(model::ZoneAirContaminantBalance*)> getter = [](model::ZoneAirContaminantBalance* t_z) {
    return t_z->outdoorCarbonDioxideSchedule();
  };

  std::function<bool(model::ZoneAirContaminantBalance*, model::Schedule)> setter = [](model::ZoneAirContaminantBalance* t_z, model::Schedule sch) {
    return t_z->setOutdoorCarbonDioxideSchedule(sch);
  };

  std::function<void(model::ZoneAirContaminantBalance*)> reset = [](model::ZoneAirContaminantBalance* t_z) {
    t_z->resetOutdoorCarbonDioxideSchedule();
  };

  m_outdoorCarbonDioxideScheduleName->bind<model::Schedule, model::ZoneAirContaminantBalance>(mo,
                                                                                              &openstudio::objectName,  // toString
                                                                                              choices_lambda,           // choices
                                                                                              getter,                   // getter
                                                                                              setter,                   // setter
                                                                                              reset                     // reset
  );
}

void SimSettingsView::attachZoneCapacitanceMultipleResearchSpecial() {
  model::ZoneCapacitanceMultiplierResearchSpecial mo = m_model.getUniqueModelObject<model::ZoneCapacitanceMultiplierResearchSpecial>();

  // m_temperatureCapacityMultiplier->bind(mo,"temperatureCapacityMultiplier",m_isIP);]
  m_temperatureCapacityMultiplier->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::temperatureCapacityMultiplier, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::ZoneCapacitanceMultiplierResearchSpecial::*)(double)>(
                                              &model::ZoneCapacitanceMultiplierResearchSpecial::setTemperatureCapacityMultiplier),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::resetTemperatureCapacityMultiplier, mo)), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::isTemperatureCapacityMultiplierDefaulted, mo)));

  // m_humidityCapacityMultiplier->bind(mo,"humidityCapacityMultiplier",m_isIP);
  m_humidityCapacityMultiplier->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::humidityCapacityMultiplier, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::ZoneCapacitanceMultiplierResearchSpecial::*)(double)>(
                                              &model::ZoneCapacitanceMultiplierResearchSpecial::setHumidityCapacityMultiplier),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::resetHumidityCapacityMultiplier, mo)), boost::none,
    boost::none, boost::optional<BasicQuery>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::isHumidityCapacityMultiplierDefaulted, mo)));

  // m_carbonDioxideCapacityMultiplier->bind(mo,"carbonDioxideCapacityMultiplier",m_isIP);
  m_carbonDioxideCapacityMultiplier->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::carbonDioxideCapacityMultiplier, mo)),
    boost::optional<DoubleSetter>(std::bind(static_cast<bool (model::ZoneCapacitanceMultiplierResearchSpecial::*)(double)>(
                                              &model::ZoneCapacitanceMultiplierResearchSpecial::setCarbonDioxideCapacityMultiplier),
                                            mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::resetCarbonDioxideCapacityMultiplier, mo)), boost::none,
    boost::none,
    boost::optional<BasicQuery>(std::bind(&model::ZoneCapacitanceMultiplierResearchSpecial::isCarbonDioxideCapacityMultiplierDefaulted, mo)));
}

void SimSettingsView::attachRadianceParameters() {
  model::RadianceParameters mo = m_model.getUniqueModelObject<model::RadianceParameters>();

  // m_accumulatedRaysperRecord->bind(mo,"accumulatedRaysperRecord");
  m_accumulatedRaysperRecord->bind(
    mo, IntGetter(std::bind(&model::RadianceParameters::accumulatedRaysperRecord, mo)),
    boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAccumulatedRaysperRecord, mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAccumulatedRaysperRecord, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAccumulatedRaysperRecordDefaulted, mo)));

  // m_directThreshold->bind(mo,"directThreshold",m_isIP);
  m_directThreshold->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::directThreshold, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setDirectThreshold), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetDirectThreshold, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isDirectThresholdDefaulted, mo)));

  // m_directCertainty->bind(mo,"directCertainty",m_isIP);
  m_directCertainty->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::directCertainty, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setDirectCertainty), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetDirectCertainty, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isDirectCertaintyDefaulted, mo)));

  // m_directJitter->bind(mo,"directJitter",m_isIP);
  m_directJitter->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::directJitter, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setDirectJitter), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetDirectJitter, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isDirectJitterDefaulted, mo)));

  // m_directPretest->bind(mo,"directPretest",m_isIP);
  m_directPretest->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::directPretest, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setDirectPretest), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetDirectPretest, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isDirectPretestDefaulted, mo)));

  // m_ambientBouncesVMX->bind(mo,"ambientBouncesVMX");
  m_ambientBouncesVMX->bind(mo, IntGetter(std::bind(&model::RadianceParameters::ambientBouncesVMX, mo)),
                            boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAmbientBouncesVMX, mo, std::placeholders::_1)),
                            boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAmbientBouncesVMX, mo)), boost::none,
                            boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAmbientBouncesVMXDefaulted, mo)));

  // m_ambientBouncesDMX->bind(mo,"ambientBouncesDMX");
  m_ambientBouncesDMX->bind(mo, IntGetter(std::bind(&model::RadianceParameters::ambientBouncesDMX, mo)),
                            boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAmbientBouncesDMX, mo, std::placeholders::_1)),
                            boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAmbientBouncesDMX, mo)), boost::none,
                            boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAmbientBouncesDMXDefaulted, mo)));

  // m_ambientDivisionsVMX->bind(mo,"ambientDivisionsVMX");
  m_ambientDivisionsVMX->bind(mo, IntGetter(std::bind(&model::RadianceParameters::ambientDivisionsVMX, mo)),
                              boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAmbientDivisionsVMX, mo, std::placeholders::_1)),
                              boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAmbientDivisionsVMX, mo)), boost::none,
                              boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAmbientDivisionsVMXDefaulted, mo)));

  // m_ambientDivisionsDMX->bind(mo,"ambientDivisionsDMX");
  m_ambientDivisionsDMX->bind(mo, IntGetter(std::bind(&model::RadianceParameters::ambientDivisionsDMX, mo)),
                              boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAmbientDivisionsDMX, mo, std::placeholders::_1)),
                              boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAmbientDivisionsDMX, mo)), boost::none,
                              boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAmbientDivisionsDMXDefaulted, mo)));

  // m_ambientSupersamples->bind(mo,"ambientSupersamples");
  m_ambientSupersamples->bind(mo, IntGetter(std::bind(&model::RadianceParameters::ambientSupersamples, mo)),
                              boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setAmbientSupersamples, mo, std::placeholders::_1)),
                              boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetAmbientSupersamples, mo)), boost::none,
                              boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isAmbientSupersamplesDefaulted, mo)));

  // m_limitWeightVMX->bind(mo,"limitWeightVMX",m_isIP);
  m_limitWeightVMX->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::limitWeightVMX, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setLimitWeightVMX), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetLimitWeightVMX, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isLimitWeightVMXDefaulted, mo)));

  // m_limitWeightDMX->bind(mo,"limitWeightDMX",m_isIP);
  m_limitWeightDMX->bind(
    m_isIP, mo, DoubleGetter(std::bind(&model::RadianceParameters::limitWeightDMX, mo)),
    boost::optional<DoubleSetter>(
      std::bind(static_cast<bool (model::RadianceParameters::*)(double)>(&model::RadianceParameters::setLimitWeightDMX), mo, std::placeholders::_1)),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetLimitWeightDMX, mo)), boost::none, boost::none,
    boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isLimitWeightDMXDefaulted, mo)));

  // m_klemsSamplingDensity->bind(mo,"klemsSamplingDensity");
  m_klemsSamplingDensity->bind(mo, IntGetter(std::bind(&model::RadianceParameters::klemsSamplingDensity, mo)),
                               boost::optional<IntSetter>(std::bind(&model::RadianceParameters::setKlemsSamplingDensity, mo, std::placeholders::_1)),
                               boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetKlemsSamplingDensity, mo)), boost::none,
                               boost::none, boost::optional<BasicQuery>(std::bind(&model::RadianceParameters::isKlemsSamplingDensityDefaulted, mo)));
  // m_skyDiscretizationResolution->bind(mo,"skyDiscretizationResolution");
  m_skyDiscretizationResolution->bind<std::string>(
    mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString), &model::RadianceParameters::skyDiscretizationResolutionValues,
    StringGetter(std::bind(&model::RadianceParameters::skyDiscretizationResolution, mo)),
    std::bind(&model::RadianceParameters::setSkyDiscretizationResolution, mo, std::placeholders::_1),
    boost::optional<NoFailAction>(std::bind(&model::RadianceParameters::resetSkyDiscretizationResolution, mo)));
}

void SimSettingsView::attachOutputJSON() {
  bool forceOutputJSON = false;
  // If it wasn't already in the model, it'll be initialized, and the Ctor defaults Output JSON field to "Yes"
  // We do NOT want to enable Output:JSON for all models now, so force it to False
  if (!m_model.getOptionalUniqueModelObject<model::OutputJSON>()) {
    forceOutputJSON = true;
  }

  model::OutputJSON mo = m_model.getUniqueModelObject<model::OutputJSON>();
  if (forceOutputJSON) {
    mo.setOutputJSON(false);
  }

  m_json_optionType->bind<std::string>(mo, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
                                       &model::OutputJSON::optionTypeValues, StringGetter(std::bind(&model::OutputJSON::optionType, mo)),
                                       std::bind(&model::OutputJSON::setOptionType, mo, std::placeholders::_1),
                                       boost::none,  // No reset
                                       boost::none   // No isDefaulted
  );

  m_json_outputJSON->bind(mo, std::bind(&model::OutputJSON::outputJSON, mo),
                          boost::optional<BoolSetter>(std::bind(&model::OutputJSON::setOutputJSON, mo, std::placeholders::_1)),
                          boost::none,  // reset
                          boost::none   // isDefaulted;
  );

  m_json_outputCBOR->bind(mo, std::bind(&model::OutputJSON::outputCBOR, mo),
                          boost::optional<BoolSetter>(std::bind(&model::OutputJSON::setOutputCBOR, mo, std::placeholders::_1)),
                          boost::none,  // reset
                          boost::none   // isDefaulted;
  );

  m_json_outputMessagePack->bind(mo, std::bind(&model::OutputJSON::outputMessagePack, mo),
                                 boost::optional<BoolSetter>(std::bind(&model::OutputJSON::setOutputMessagePack, mo, std::placeholders::_1)),
                                 boost::none,  // reset
                                 boost::none   // isDefaulted;
  );
}

void SimSettingsView::attachOutputTableSummaryReports() {
  bool forceAllSummary = false;
  // If it wasn't already in the model, it'll be initialized, and the Ctor defaults it to no fields, while the FT will add AllSummary if not present
  // in the model, so to maintain behavior, we add the AllSummary report if needed
  if (!m_model.outputTableSummaryReports()) {
    forceAllSummary = true;
  }

  m_outputTableSummaryReports = m_model.getUniqueModelObject<model::OutputTableSummaryReports>();
  if (forceAllSummary) {
    m_outputTableSummaryReports->enableAllSummaryReport();
  }

  // typedef std::function<bool()> BoolGetter
  std::function<bool()> getter = [this]() { return m_outputTableSummaryReports->summaryReportIndex("AllSummary").has_value(); };

  // typedef std::function<void(bool)> BoolSetter;
  std::function<void(bool)> setter = [this](bool value) {
    if (value) {
      m_outputTableSummaryReports->enableAllSummaryReport();
    } else if (boost::optional<unsigned> index = m_outputTableSummaryReports->summaryReportIndex("AllSummary")) {
      m_outputTableSummaryReports->removeSummaryReport(index.get());
    }
  };

  // void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none,
  //           boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);
  m_table_allSummary->bind(*m_outputTableSummaryReports, getter, boost::optional<BoolSetter>(setter),
                           boost::none,  // reset
                           boost::none   // isDefaulted;
  );
}

void SimSettingsView::attachOutputDiagnostics() {

  m_outputDiagnostics = m_model.getUniqueModelObject<model::OutputDiagnostics>();

  // typedef std::function<bool()> BoolGetter
  std::function<bool()> getter = [this]() {
    auto ks = m_outputDiagnostics->keys();
    return std::find_if(ks.begin(), ks.end(), [](const std::string& k) { return openstudio::istringEqual(k, "DisplayExtraWarnings"); }) != ks.end();
  };

  // void bind(const model::ModelObject& modelObject, BoolGetter get, boost::optional<BoolSetter> set = boost::none,
  //           boost::optional<NoFailAction> reset = boost::none, boost::optional<BasicQuery> isDefaulted = boost::none);

  // typedef std::function<void(bool)> BoolSetter;
  std::function<void(bool)> setter = [this](bool value) {
    if (value) {
      m_outputDiagnostics->enableDisplayExtraWarnings();
    } else {
      auto ks = m_outputDiagnostics->keys();
      ks.erase(std::remove_if(ks.begin(), ks.end(), [](const std::string& k) { return openstudio::istringEqual(k, "DisplayExtraWarnings"); }),
               ks.end());
      m_outputDiagnostics->setKeys(ks);
    }
  };

  m_diagnostics_displayExtraWarnings->bind(*m_outputDiagnostics, getter, setter,
                                           boost::none,  // reset
                                           boost::none   // isDefaulted;
  );
}

void SimSettingsView::detachRunPeriod() {
  m_useWeatherFileHolidaysandSpecialDays->unbind();
  m_useWeatherFileDaylightSavingsPeriod->unbind();
  m_applyWeekendHolidayRule->unbind();
  m_useWeatherFileRainIndicators->unbind();
  m_useWeatherFileSnowIndicators->unbind();
}

void SimSettingsView::detachRunPeriodControlDaylightSavingTime() {}

void SimSettingsView::detachSimulationControl() {
  m_doZoneSizingCalculation->unbind();
  m_doSystemSizingCalculation->unbind();
  m_doPlantSizingCalculation->unbind();
  m_runSimulationforSizingPeriods->unbind();
  m_runSimulationforWeatherFileRunPeriods->unbind();
  m_maximumNumberofWarmupDays->unbind();
  m_minimumNumberofWarmupDays->unbind();
  m_loadsConvergenceToleranceValue->unbind();
  m_temperatureConvergenceToleranceValue->unbind();
  m_solarDistribution->unbind();
  m_doHVACSizingSimulationforSizingPeriods->unbind();
  m_maximumNumberofHVACSizingSimulationPasses->unbind();
}

void SimSettingsView::detachSizingParameters() {
  m_heatingSizingFactor->unbind();
  m_coolingSizingFactor->unbind();
  m_timestepsinAveragingWindow->unbind();
}

void SimSettingsView::detachProgramControl() {
  m_numberOfThreadsAllowed->unbind();
}

void SimSettingsView::detachTimestep() {
  m_numberOfTimestepsPerHour->unbind();
}
void SimSettingsView::detachOutputControlReportingTolerances() {
  m_toleranceForTimeHeatingSetpointNotMet->unbind();
  m_toleranceForTimeCoolingSetpointNotMet->unbind();
}

void SimSettingsView::detachConvergenceLimits() {
  m_maximumHVACIterations->unbind();
  m_minimumPlantIterations->unbind();
  m_maximumPlantIterations->unbind();
  m_minimumSystemTimestep->unbind();
}

void SimSettingsView::detachShadowCalculation() {
  m_shadingCalculationUpdateFrequency->unbind();
  m_maximumFiguresInShadowOverlapCalculations->unbind();
  m_polygonClippingAlgorithm->unbind();
  m_skyDiffuseModelingAlgorithm->unbind();
}

void SimSettingsView::detachSurfaceConvectionAlgorithmInside() {
  m_algorithmSurfaceConvectionInside->unbind();
}

void SimSettingsView::detachSurfaceConvectionAlgorithmOutside() {
  m_algorithmSurfaceConvectionOutside->unbind();
}

void SimSettingsView::detachHeatBalanceAlgorithm() {
  m_algorithmHeatBalance->unbind();
  m_surfaceTemperatureUpperLimit->unbind();
  m_minimumSurfaceConvectionHeatTransferCoefficientValue->unbind();
  m_maximumSurfaceConvectionHeatTransferCoefficientValue->unbind();
}

void SimSettingsView::detachZoneAirHeatBalanceAlgorithm() {
  m_algorithmZoneAirHeatBalance->unbind();
}

void SimSettingsView::detachZoneAirContaminantBalance() {
  m_carbonDioxideConcentration->unbind();
  m_outdoorCarbonDioxideScheduleName->unbind();
}

void SimSettingsView::detachZoneCapacitanceMultipleResearchSpecial() {
  m_temperatureCapacityMultiplier->unbind();
  m_humidityCapacityMultiplier->unbind();
  m_carbonDioxideCapacityMultiplier->unbind();
}

void SimSettingsView::detachRadianceParameters() {
  m_accumulatedRaysperRecord->unbind();
  m_directThreshold->unbind();
  m_directCertainty->unbind();
  m_directJitter->unbind();
  m_directPretest->unbind();
  m_ambientBouncesVMX->unbind();
  m_ambientBouncesDMX->unbind();
  m_ambientDivisionsVMX->unbind();
  m_ambientDivisionsDMX->unbind();
  m_ambientSupersamples->unbind();
  m_limitWeightVMX->unbind();
  m_limitWeightDMX->unbind();
  m_klemsSamplingDensity->unbind();
  m_skyDiscretizationResolution->unbind();
}

void SimSettingsView::setCoarseRadianceSettings() {
  auto radianceParameters = m_model.getUniqueModelObject<model::RadianceParameters>();

  radianceParameters.applyCoarseSettings();
}

void SimSettingsView::setFineRadianceSettings() {
  auto radianceParameters = m_model.getUniqueModelObject<model::RadianceParameters>();

  radianceParameters.applyFineSettings();
}

void SimSettingsView::detachOutputJSON() {
  m_json_optionType->unbind();
  m_json_outputJSON->unbind();
  m_json_outputCBOR->unbind();
  m_json_outputMessagePack->unbind();
}

void SimSettingsView::detachOutputTableSummaryReports() {
  m_table_allSummary->unbind();
}

void SimSettingsView::detachOutputDiagnostics() {
  m_diagnostics_displayExtraWarnings->unbind();
}

//***** SLOTS *****

void SimSettingsView::on_runPeriodGroupClicked(int idx) {
  if (idx == 0) {
    m_dateRangelabel->setEnabled(false);
    m_startDateEdit->setEnabled(false);
    m_endDateEdit->setEnabled(false);
  } else if (idx == 1) {
    m_dateRangelabel->setEnabled(true);
    m_startDateEdit->setEnabled(true);
    m_endDateEdit->setEnabled(true);
  } else {
    // Should never get here
    OS_ASSERT(false);
  }
}

void SimSettingsView::on_radianceGroupClicked(int idx) {
  if (idx == 0) {
    enableRadianceParametersWidget(false);
    setCoarseRadianceSettings();
  } else if (idx == 1) {
    enableRadianceParametersWidget(false);
    setFineRadianceSettings();
  } else if (idx == 2) {
    enableRadianceParametersWidget(true);
  } else {
    // Should never get here
    OS_ASSERT(false);
  }
}

void SimSettingsView::on_startDateChanged(const QDate& date) {
  static bool firstTimeInFunction = true;
  if (firstTimeInFunction && m_startDateEdit->date().month() == 12 && m_startDateEdit->date().day() == 31) {
    firstTimeInFunction = false;
    QTimer::singleShot(0, this, &SimSettingsView::initStartDateEdit);
    return;
  }

  if (date > m_endDateEdit->date()) {
    m_endDateEdit->blockSignals(true);
    m_endDateEdit->setDate(date);
    m_endDateEdit->blockSignals(false);
  }

  m_runPeriod = m_model.getUniqueModelObject<model::RunPeriod>();

  m_runPeriod->setBeginMonth(m_startDateEdit->date().month());
  m_runPeriod->setBeginDayOfMonth(m_startDateEdit->date().day());

  m_runPeriod->setEndMonth(m_endDateEdit->date().month());
  m_runPeriod->setEndDayOfMonth(m_endDateEdit->date().day());
}

void SimSettingsView::initStartDateEdit() {
  // Note: QDateEdit bug workaround
  int year = m_startDateEdit->date().year();
  m_runPeriod = m_model.getUniqueModelObject<model::RunPeriod>();
  m_startDateEdit->blockSignals(true);
  m_startDateEdit->setDate(QDate(year, m_runPeriod->getBeginMonth(), m_runPeriod->getBeginDayOfMonth()));
  m_startDateEdit->blockSignals(false);
}

void SimSettingsView::on_endDateChanged(const QDate& date) {
  if (m_startDateEdit->date() > date) {
    m_startDateEdit->blockSignals(true);
    m_startDateEdit->setDate(date);
    m_startDateEdit->blockSignals(false);
  }

  m_runPeriod = m_model.getUniqueModelObject<model::RunPeriod>();

  m_runPeriod->setBeginMonth(m_startDateEdit->date().month());
  m_runPeriod->setBeginDayOfMonth(m_startDateEdit->date().day());

  m_runPeriod->setEndMonth(m_endDateEdit->date().month());
  m_runPeriod->setEndDayOfMonth(m_endDateEdit->date().day());
}

void SimSettingsView::on_runSimWeatherFiles(int state) {
  // TODO
}
void SimSettingsView::on_runSimDesignDays(int state) {
  // TODO
}

void SimSettingsView::on_performZoneSizing(int state) {
  // TODO
}

void SimSettingsView::on_performSystemSizing(int state) {
  // TODO
}

void SimSettingsView::on_performPlantSizing(int state) {
  // TODO
}

void SimSettingsView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

void SimSettingsView::updateYearDescription() {
  model::YearDescription yearDescription = m_model.getUniqueModelObject<model::YearDescription>();
  model::RunPeriod runPeriod = m_model.getUniqueModelObject<model::RunPeriod>();

  m_startDateEdit->blockSignals(true);
  m_startDateEdit->setMinimumDate(QDate(yearDescription.assumedYear(), 1, 1));
  m_startDateEdit->setMaximumDate(QDate(yearDescription.assumedYear(), 12, 31));
  m_startDateEdit->setDate(QDate(yearDescription.assumedYear(), runPeriod.getBeginMonth(), runPeriod.getBeginDayOfMonth()));
  m_startDateEdit->blockSignals(false);

  m_endDateEdit->blockSignals(true);
  m_endDateEdit->setMinimumDate(QDate(yearDescription.assumedYear(), 1, 1));
  m_endDateEdit->setMaximumDate(QDate(yearDescription.assumedYear(), 12, 31));
  m_endDateEdit->setDate(QDate(yearDescription.assumedYear(), runPeriod.getEndMonth(), runPeriod.getEndDayOfMonth()));
  m_endDateEdit->blockSignals(false);
}

}  // namespace openstudio
