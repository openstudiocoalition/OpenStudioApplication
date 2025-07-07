/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SIMSETTINGSVIEW_HPP
#define OPENSTUDIO_SIMSETTINGSVIEW_HPP

#include <QWidget>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ShadowCalculation.hpp>
#include <openstudio/model/SimulationControl.hpp>
#include <openstudio/model/OutputTableSummaryReports.hpp>
#include <openstudio/model/OutputDiagnostics.hpp>
#include <openstudio/model/RunPeriod.hpp>

class QButtonGroup;
class QCheckBox;
class QDate;
class QDateEdit;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

namespace openstudio {

class OSComboBox2;
class OSIntegerEdit2;
class OSLineEdit2;
class OSQuantityEdit2;
class OSSwitch2;
class ScheduleCalendarWidget;

class SimSettingsView
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  SimSettingsView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SimSettingsView() {}

 private:
  void createWidgets();
  void setCoarseRadianceSettings();
  void setFineRadianceSettings();
  void enableRadianceParametersWidget(bool isEnabled);

  QWidget* createRunPeriodWidget();
  QWidget* createRunPeriodAdvancedWidget();
  QWidget* createRunControlWidget();
  QWidget* createSimulationControlWidget();
  QWidget* createSizingParametersWidget();
  QWidget* createProgramControlWidget();
  QWidget* createTimestepWidget();
  QWidget* createOutputControlReportingTolerancesWidget();
  QWidget* createConvergenceLimitsWidget();
  QWidget* createShadowCalculationWidget();
  QWidget* createSurfaceConvectionAlgorithmInsideWidget();
  QWidget* createSurfaceConvectionAlgorithmOutsideWidget();
  QWidget* createHeatBalanceAlgorithmWidget();
  QWidget* createZoneAirHeatBalanceAlgorithmWidget();
  QWidget* createZoneAirContaminantBalanceWidget();
  QWidget* createZoneCapacitanceMultipleResearchSpecialWidget();
  QWidget* createRadianceParametersWidget();
  QWidget* createOutputJSONWidget();
  QWidget* createOutputTableSummaryReportsWidget();
  QWidget* createOutputDiagnosticsWidget();
  QWidget* createOutputControlResilienceSummariesWidget();

  void addField(QGridLayout* gridLayout, int row, int column, QString text, OSComboBox2*& comboBox);

  void addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, OSComboBox2*& comboBox);

  void addField(QGridLayout* gridLayout, int row, int column, QString text, OSIntegerEdit2*& integerEdit);

  void addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, OSIntegerEdit2*& integerEdit);

  void addField(QGridLayout* gridLayout, int row, int column, QString text, OSLineEdit2*& lineEdit);

  void addField(QGridLayout* gridLayout, int row, int column, QString text, const std::string& modelUnits, const std::string& siUnits,
                const std::string& ipUnits, OSQuantityEdit2*& quantityEdit);

  void addField(QGridLayout* gridLayout, int row, int column, QLabel*& label, QString text, const std::string& modelUnits, const std::string& siUnits,
                const std::string& ipUnits, OSQuantityEdit2*& quantityEdit);

  void addField(QGridLayout* gridLayout, int row, int column, QString text, OSSwitch2*& osSwitch);

  void attachAll();
  void attachRunPeriod();
  void attachRunPeriodControlDaylightSavingTime();
  void attachSimulationControl();
  void attachSizingParameters();
  void attachProgramControl();
  void attachTimestep();
  void attachOutputControlReportingTolerances();
  void attachConvergenceLimits();
  void attachShadowCalculation();
  void attachSurfaceConvectionAlgorithmInside();
  void attachSurfaceConvectionAlgorithmOutside();
  void attachHeatBalanceAlgorithm();
  void attachZoneAirHeatBalanceAlgorithm();
  void attachZoneAirContaminantBalance();
  void attachZoneCapacitanceMultipleResearchSpecial();
  void attachRadianceParameters();
  void attachOutputJSON();
  void attachOutputTableSummaryReports();
  void attachOutputDiagnostics();
  void attachOutputControlResilienceSummaries();

  void detachAll();
  void detachRunPeriod();
  void detachRunPeriodControlDaylightSavingTime();
  void detachSimulationControl();
  void detachSizingParameters();
  void detachProgramControl();
  void detachTimestep();
  void detachOutputControlReportingTolerances();
  void detachConvergenceLimits();
  void detachShadowCalculation();
  void detachSurfaceConvectionAlgorithmInside();
  void detachSurfaceConvectionAlgorithmOutside();
  void detachHeatBalanceAlgorithm();
  void detachZoneAirHeatBalanceAlgorithm();
  void detachZoneAirContaminantBalance();
  void detachZoneCapacitanceMultipleResearchSpecial();
  void detachRadianceParameters();
  void detachOutputJSON();
  void detachOutputTableSummaryReports();
  void detachOutputDiagnostics();
  void detachOutputControlResilienceSummaries();

  model::Model m_model;
  boost::optional<model::ShadowCalculation> m_shadowCalculation;
  boost::optional<model::SimulationControl> m_simulationControl;
  boost::optional<model::OutputTableSummaryReports> m_outputTableSummaryReports;
  boost::optional<model::OutputDiagnostics> m_outputDiagnostics;

  QCheckBox* m_runSimWeatherFiles;
  QCheckBox* m_runSimDesignDays;
  QCheckBox* m_performZoneSizing;
  QCheckBox* m_performSystemSizing;
  QCheckBox* m_performPlantSizing;

  QButtonGroup* m_runPeriodGroup;
  QButtonGroup* m_radianceGroup;

  QLineEdit* m_runPeriodName;

  bool m_isIP;

  // Run Period
  boost::optional<model::RunPeriod> m_runPeriod;
  QLabel* m_dateRangelabel;
  QDateEdit* m_startDateEdit;
  QDateEdit* m_endDateEdit;
  OSSwitch2* m_useWeatherFileHolidaysandSpecialDays;
  OSSwitch2* m_useWeatherFileDaylightSavingsPeriod;
  OSSwitch2* m_applyWeekendHolidayRule;
  OSSwitch2* m_useWeatherFileRainIndicators;
  OSSwitch2* m_useWeatherFileSnowIndicators;

  // SimulationControl
  OSSwitch2* m_doZoneSizingCalculation;
  OSSwitch2* m_doSystemSizingCalculation;
  OSSwitch2* m_doPlantSizingCalculation;
  OSSwitch2* m_runSimulationforSizingPeriods;
  OSSwitch2* m_runSimulationforWeatherFileRunPeriods;
  OSIntegerEdit2* m_maximumNumberofWarmupDays;
  OSIntegerEdit2* m_minimumNumberofWarmupDays;
  OSQuantityEdit2* m_loadsConvergenceToleranceValue;
  OSQuantityEdit2* m_temperatureConvergenceToleranceValue;
  OSComboBox2* m_solarDistribution;
  OSSwitch2* m_doHVACSizingSimulationforSizingPeriods;
  OSIntegerEdit2* m_maximumNumberofHVACSizingSimulationPasses;

  // Radiance
  OSIntegerEdit2* m_accumulatedRaysperRecord;
  OSQuantityEdit2* m_directThreshold;
  OSQuantityEdit2* m_directCertainty;
  OSQuantityEdit2* m_directJitter;
  OSQuantityEdit2* m_directPretest;
  OSIntegerEdit2* m_ambientBouncesVMX;
  OSIntegerEdit2* m_ambientBouncesDMX;
  OSIntegerEdit2* m_ambientDivisionsVMX;
  OSIntegerEdit2* m_ambientDivisionsDMX;
  OSIntegerEdit2* m_ambientSupersamples;
  OSQuantityEdit2* m_limitWeightVMX;
  OSQuantityEdit2* m_limitWeightDMX;
  OSIntegerEdit2* m_klemsSamplingDensity;
  OSComboBox2* m_skyDiscretizationResolution;
  QLabel* m_accumulatedRaysperRecordLbl;
  QLabel* m_directThresholdLbl;
  QLabel* m_directCertaintyLbl;
  QLabel* m_directJitterLbl;
  QLabel* m_directPretestLbl;
  QLabel* m_ambientBouncesVMXLbl;
  QLabel* m_ambientBouncesDMXLbl;
  QLabel* m_ambientDivisionsVMXLbl;
  QLabel* m_ambientDivisionsDMXLbl;
  QLabel* m_ambientSupersamplesLbl;
  QLabel* m_limitWeightVMXLbl;
  QLabel* m_limitWeightDMXLbl;
  QLabel* m_klemsSamplingDensityLbl;
  QLabel* m_skyDiscretizationResolutionLbl;

  // SizingParameters
  OSQuantityEdit2* m_heatingSizingFactor;
  OSQuantityEdit2* m_coolingSizingFactor;
  OSIntegerEdit2* m_timestepsinAveragingWindow;

  // ProgramControl
  OSIntegerEdit2* m_numberOfThreadsAllowed;

  // Timestep
  OSIntegerEdit2* m_numberOfTimestepsPerHour;

  // OutputControlReportingTolerances
  OSQuantityEdit2* m_toleranceForTimeHeatingSetpointNotMet;
  OSQuantityEdit2* m_toleranceForTimeCoolingSetpointNotMet;

  // ConvergenceLimits
  OSIntegerEdit2* m_maximumHVACIterations;
  OSIntegerEdit2* m_minimumPlantIterations;
  OSIntegerEdit2* m_maximumPlantIterations;
  OSIntegerEdit2* m_minimumSystemTimestep;

  // ShadowCalculation
  OSIntegerEdit2* m_shadingCalculationUpdateFrequency;
  OSIntegerEdit2* m_maximumFiguresInShadowOverlapCalculations;
  OSComboBox2* m_polygonClippingAlgorithm;
  OSComboBox2* m_skyDiffuseModelingAlgorithm;

  // SurfaceConvectionAlgorithmInside
  //OSLineEdit2 * m_algorithmSurfaceConvectionInside;
  OSComboBox2* m_algorithmSurfaceConvectionInside;

  // SurfaceConvectionAlgorithmOutside
  //OSLineEdit2 * m_algorithmSurfaceConvectionOutside;
  OSComboBox2* m_algorithmSurfaceConvectionOutside;

  // HeatBalance
  //OSLineEdit2 * m_algorithmHeatBalance;
  OSComboBox2* m_algorithmHeatBalance;
  OSQuantityEdit2* m_surfaceTemperatureUpperLimit;
  OSQuantityEdit2* m_minimumSurfaceConvectionHeatTransferCoefficientValue;
  OSQuantityEdit2* m_maximumSurfaceConvectionHeatTransferCoefficientValue;

  // ZoneAirHeatBalanceAlgorithm
  //OSLineEdit2 * m_algorithmZoneAirHeatBalance;
  OSComboBox2* m_algorithmZoneAirHeatBalance;

  // ZoneAirContaminantBalance
  OSSwitch2* m_carbonDioxideConcentration;
  OSComboBox2* m_outdoorCarbonDioxideScheduleName;

  // ZoneCapacitanceMultiplierResearchSpecial
  OSQuantityEdit2* m_temperatureCapacityMultiplier;
  OSQuantityEdit2* m_humidityCapacityMultiplier;
  OSQuantityEdit2* m_carbonDioxideCapacityMultiplier;

  // OutputJSON
  OSComboBox2* m_json_optionType;
  OSSwitch2* m_json_outputJSON;
  OSSwitch2* m_json_outputCBOR;
  OSSwitch2* m_json_outputMessagePack;

  // Advanced Output
  // These are extensible groups in the SDK, but we care only about common settings
  OSSwitch2* m_table_allSummary;
  OSSwitch2* m_diagnostics_displayExtraWarnings;

  OSComboBox2* m_outputControlResilienceSummaries_heatIndexAlgorithm;

 signals:

  void toggleUnitsClicked(bool displayIP);

 private slots:

  void on_startDateChanged(const QDate& date);

  void on_endDateChanged(const QDate& date);

  void on_runPeriodGroupClicked(int idx);

  void on_radianceGroupClicked(int idx);

  void on_runSimWeatherFiles(int state);

  void on_runSimDesignDays(int state);

  void on_performZoneSizing(int state);

  void on_performSystemSizing(int state);

  void on_performPlantSizing(int state);

  void toggleUnits(bool displayIP);

  void updateYearDescription();

  void initStartDateEdit();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SIMSETTINGSVIEW_HPP
