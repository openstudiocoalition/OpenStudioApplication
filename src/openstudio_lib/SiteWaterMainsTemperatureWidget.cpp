/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SiteWaterMainsTemperatureWidget.hpp"

#include "OSDropZone.hpp"

#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/FieldMethodTypedefs.hpp"

#include <openstudio/model/SiteWaterMainsTemperature_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace openstudio {

SiteWaterMainsTemperatureWidget::SiteWaterMainsTemperatureWidget(bool isIP, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  setLayout(mainLayout);

  auto* titleLabel = new QLabel("OS:Site:WaterMainsTemperature");
  titleLabel->setObjectName("H2");
  mainLayout->addWidget(titleLabel);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(0, 0, 0, 0);
  gridLayout->setSpacing(10);
  gridLayout->setColumnStretch(0, 1);
  gridLayout->setColumnStretch(1, 1);
  mainLayout->addLayout(gridLayout);

  int row = 0;

  // Calculation Method — label above, combobox left-aligned
  auto* methodLabel = new QLabel(tr("Calculation Method"));
  methodLabel->setObjectName("H2");
  gridLayout->addWidget(methodLabel, row++, 0, 1, 2);

  m_calculationMethod = new OSComboBox2();
  gridLayout->addWidget(m_calculationMethod, row++, 0, Qt::AlignLeft);

  // Temperature Schedule (visible only when method == "Schedule")
  m_scheduleLabel = new QLabel(tr("Temperature Schedule"));
  m_scheduleLabel->setObjectName("H2");
  gridLayout->addWidget(m_scheduleLabel, row++, 0, 1, 2);

  m_scheduleDropZone = new OSDropZone2();
  gridLayout->addWidget(m_scheduleDropZone, row++, 0, 1, 2);

  // Annual Average Outdoor Air Temperature and Max Diff (visible only when method == "Correlation")
  m_annualAvgLabel = new QLabel(tr("Annual Average Outdoor Air Temperature"));
  m_annualAvgLabel->setObjectName("H2");
  gridLayout->addWidget(m_annualAvgLabel, row, 0);

  m_maxDiffLabel = new QLabel(tr("Maximum Difference In Monthly Average\nOutdoor Air Temperatures"));
  m_maxDiffLabel->setObjectName("H2");
  gridLayout->addWidget(m_maxDiffLabel, row++, 1);

  m_annualAvgTemp = new OSQuantityEdit2("C", "C", "F", m_isIP);
  connect(this, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked, m_annualAvgTemp, &OSQuantityEdit2::onUnitSystemChange);
  gridLayout->addWidget(m_annualAvgTemp, row, 0);

  m_maxDiffTemp = new OSQuantityEdit2("K", "K", "R", m_isIP);
  connect(this, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked, m_maxDiffTemp, &OSQuantityEdit2::onUnitSystemChange);
  gridLayout->addWidget(m_maxDiffTemp, row++, 1);

  // Temperature Multiplier and Offset (hidden when method == "Schedule")
  m_multiplierLabel = new QLabel(tr("Temperature Multiplier"));
  m_multiplierLabel->setObjectName("H2");
  gridLayout->addWidget(m_multiplierLabel, row, 0);

  m_offsetLabel = new QLabel(tr("Temperature Offset"));
  m_offsetLabel->setObjectName("H2");
  gridLayout->addWidget(m_offsetLabel, row++, 1);

  m_multiplier = new OSQuantityEdit2("", "", "", m_isIP);
  gridLayout->addWidget(m_multiplier, row, 0);

  m_offset = new OSQuantityEdit2("K", "K", "R", m_isIP);
  connect(this, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked, m_offset, &OSQuantityEdit2::onUnitSystemChange);
  gridLayout->addWidget(m_offset, row++, 1);

  connect(this, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked, this, [this](bool isIP) { m_isIP = isIP; });

  // Refresh field visibility whenever the combo selection changes
  connect(m_calculationMethod, &QComboBox::currentIndexChanged, this, [this](int) { refreshVisibility(); });

  mainLayout->addStretch();
}

void SiteWaterMainsTemperatureWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<model::SiteWaterMainsTemperature>();

  m_calculationMethod->bind<std::string>(
    *m_obj,
    // toString
    [](const std::string& s) -> std::string { return s; },
    // choices
    []() -> std::vector<std::string> { return model::SiteWaterMainsTemperature::calculationMethodValues(); },
    // getter
    [this]() -> std::string { return m_obj->calculationMethod(); },
    // setter
    [this](const std::string& s) -> bool { return m_obj->setCalculationMethod(s); },
    // reset
    boost::none,
    // isDefaulted
    boost::none);

  m_scheduleDropZone->bind(
    // modelObject
    *m_obj,
    // get
    OptionalModelObjectGetter([this]() -> boost::optional<model::ModelObject> {
      auto opt = m_obj->temperatureSchedule();
      if (opt) {
        return boost::optional<model::ModelObject>(opt.get());
      }
      return boost::none;
    }),
    // set
    ModelObjectSetter([this](const model::ModelObject& mo) -> bool {
      auto sch = mo.optionalCast<model::Schedule>();
      if (!sch) {
        return false;
      }
      return m_obj->setTemperatureSchedule(sch.get());
    }),
    // reset
    boost::optional<NoFailAction>([this]() { m_obj->resetTemperatureSchedule(); }));

  m_annualAvgTemp->bind(m_isIP, *m_obj, OptionalDoubleGetter([this]() { return m_obj->annualAverageOutdoorAirTemperature(); }),
                        boost::optional<DoubleSetter>([this](double v) { return m_obj->setAnnualAverageOutdoorAirTemperature(v); }),
                        boost::optional<NoFailAction>([this]() { m_obj->resetAnnualAverageOutdoorAirTemperature(); }));

  m_maxDiffTemp->bind(
    m_isIP, *m_obj, OptionalDoubleGetter([this]() { return m_obj->maximumDifferenceInMonthlyAverageOutdoorAirTemperatures(); }),
    boost::optional<DoubleSetter>([this](double v) { return m_obj->setMaximumDifferenceInMonthlyAverageOutdoorAirTemperatures(v); }),
    boost::optional<NoFailAction>([this]() { m_obj->resetMaximumDifferenceInMonthlyAverageOutdoorAirTemperatures(); }));

  m_multiplier->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->temperatureMultiplier(); }),
                     boost::optional<DoubleSetter>([this](double v) { return m_obj->setTemperatureMultiplier(v); }));

  m_offset->bind(m_isIP, *m_obj, DoubleGetter([this]() { return m_obj->temperatureOffset(); }),
                 boost::optional<DoubleSetter>([this](double v) { return m_obj->setTemperatureOffset(v); }));

  refreshVisibility();
}

void SiteWaterMainsTemperatureWidget::detach() {
  m_calculationMethod->unbind();
  if (m_obj) {
    // OSDropZone2::unbind() crashes if it was never bound (no guard on m_modelObject)
    m_scheduleDropZone->unbind();
  }
  m_annualAvgTemp->unbind();
  m_maxDiffTemp->unbind();
  m_multiplier->unbind();
  m_offset->unbind();
  m_obj = boost::none;
}

void SiteWaterMainsTemperatureWidget::refreshVisibility() {
  const QString method = m_calculationMethod->currentText();
  const bool isSchedule = (method == "Schedule");
  const bool isCorrelation = (method == "Correlation");

  m_scheduleLabel->setVisible(isSchedule);
  m_scheduleDropZone->setVisible(isSchedule);

  m_annualAvgLabel->setVisible(isCorrelation);
  m_annualAvgTemp->setVisible(isCorrelation);
  m_maxDiffLabel->setVisible(isCorrelation);
  m_maxDiffTemp->setVisible(isCorrelation);

  m_multiplierLabel->setVisible(!isSchedule);
  m_multiplier->setVisible(!isSchedule);
  m_offsetLabel->setVisible(!isSchedule);
  m_offset->setVisible(!isSchedule);
}

}  // namespace openstudio
