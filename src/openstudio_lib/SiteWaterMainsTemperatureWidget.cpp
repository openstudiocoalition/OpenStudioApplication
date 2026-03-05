/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SiteWaterMainsTemperatureWidget.hpp"

#include "OSDropZone.hpp"
#include "ModelObjectItem.hpp"

#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/FieldMethodTypedefs.hpp"

#include <openstudio/model/SiteWaterMainsTemperature_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>

#include <openstudio/utilities/idd/OS_Site_WaterMainsTemperature_FieldEnums.hxx>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace openstudio {

// TemperatureScheduleVC

void TemperatureScheduleVC::onChangeRelationship(const model::ModelObject& modelObject, int index, Handle /*newHandle*/, Handle /*oldHandle*/) {
  if (index == OS_Site_WaterMainsTemperatureFields::TemperatureScheduleName) {
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> TemperatureScheduleVC::makeVector() {
  std::vector<OSItemId> result;
  if (m_modelObject) {
    auto obj = m_modelObject->cast<model::SiteWaterMainsTemperature>();
    if (auto schedule = obj.temperatureSchedule()) {
      result.push_back(modelObjectToItemId(*schedule, false));
    }
  }
  return result;
}

void TemperatureScheduleVC::onRemoveItem(OSItem* /*item*/) {
  if (m_modelObject) {
    m_modelObject->cast<model::SiteWaterMainsTemperature>().resetTemperatureSchedule();
  }
}

void TemperatureScheduleVC::onReplaceItem(OSItem* /*currentItem*/, const OSItemId& replacementItemId) {
  onDrop(replacementItemId);
}

void TemperatureScheduleVC::onDrop(const OSItemId& itemId) {
  if (m_modelObject) {
    auto obj = m_modelObject->cast<model::SiteWaterMainsTemperature>();
    if (auto modelObject = this->getModelObject(itemId)) {
      if (auto schedule = modelObject->optionalCast<model::Schedule>()) {
        if (this->fromComponentLibrary(itemId)) {
          modelObject = modelObject->clone(m_modelObject->model());
          schedule = modelObject->cast<model::Schedule>();
        }
        obj.setTemperatureSchedule(*schedule);
      }
    }
  }
}

// SiteWaterMainsTemperatureWidget

SiteWaterMainsTemperatureWidget::SiteWaterMainsTemperatureWidget(bool isIP, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  setLayout(mainLayout);

  auto* titleLabel = new QLabel("Site:WaterMainsTemperature");
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

  m_scheduleVC = new TemperatureScheduleVC();
  m_scheduleDropZone = new OSDropZone(m_scheduleVC);
  m_scheduleDropZone->setMinItems(0);
  m_scheduleDropZone->setMaxItems(1);
  m_scheduleDropZone->setItemsAcceptDrops(true);
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

  m_scheduleVC->attach(*m_obj);
  m_scheduleVC->reportItems();

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
  m_scheduleVC->detach();
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
