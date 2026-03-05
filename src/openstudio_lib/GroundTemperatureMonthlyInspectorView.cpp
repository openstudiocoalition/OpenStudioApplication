/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GroundTemperatureMonthlyInspectorView.hpp"

#include "YearSettingsWidget.hpp"

#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureDeep_Impl.hpp>

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QStringList>
#include <QValueAxis>
#include <QVBoxLayout>

#define TEMP_EDIT_WIDTH 90

namespace openstudio {

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureMonthlyWidget (abstract base)
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureMonthlyWidget::SiteGroundTemperatureMonthlyWidget(bool isIP, QWidget* parent) : QWidget(parent), m_isIP(isIP) {
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  setLayout(mainLayout);

  m_titleLabel = new QLabel();
  m_titleLabel->setObjectName("H2");
  mainLayout->addWidget(m_titleLabel);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(0, 0, 0, 0);
  gridLayout->setSpacing(10);
  mainLayout->addLayout(gridLayout);

  auto* monthHeader = new QLabel(tr("Month"));
  monthHeader->setObjectName("H2");
  gridLayout->addWidget(monthHeader, 0, 0);

  auto* tempHeader = new QLabel(tr("Temperature"));
  tempHeader->setObjectName("H2");
  gridLayout->addWidget(tempHeader, 0, 1);

  const QStringList monthNames = YearSettingsWidget::months();
  for (int i = 0; i < 12; ++i) {
    gridLayout->addWidget(new QLabel(monthNames[i]), i + 1, 0);
    m_edits[i] = new OSQuantityEdit2("C", "C", "F", m_isIP);
    connect(this, &SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked, m_edits[i], &OSQuantityEdit2::onUnitSystemChange);
    if (auto* lineEdit = m_edits[i]->findChild<QLineEdit*>()) {
      connect(lineEdit, &QLineEdit::editingFinished, this, &SiteGroundTemperatureMonthlyWidget::refreshChartFromModel);
    }
    m_edits[i]->setFixedWidth(TEMP_EDIT_WIDTH);
    gridLayout->addWidget(m_edits[i], i + 1, 1, Qt::AlignLeft);
  }
  gridLayout->setColumnStretch(2, 1);

  auto* hRule = new QFrame();
  hRule->setFrameShape(QFrame::HLine);
  hRule->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(hRule);

  auto* setAllLayout = new QHBoxLayout();
  setAllLayout->setContentsMargins(0, 0, 0, 0);
  setAllLayout->setSpacing(6);

  auto* setAllLabel = new QLabel(tr("Set all months to:"));
  setAllLayout->addWidget(setAllLabel);

  m_constantValueEdit = new QDoubleSpinBox();
  m_constantValueEdit->setDecimals(2);
  if (isIP) {
    m_constantValueEdit->setRange(-148.0, 212.0);
    m_constantValueEdit->setSuffix(tr(" °F"));
  } else {
    m_constantValueEdit->setRange(-100.0, 100.0);
    m_constantValueEdit->setSuffix(tr(" °C"));
  }
  setAllLayout->addWidget(m_constantValueEdit);

  m_applyConstantButton = new QPushButton(tr("Apply"));
  setAllLayout->addWidget(m_applyConstantButton);
  setAllLayout->addStretch();

  mainLayout->addLayout(setAllLayout);

  connect(this, &SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked, this, [this](bool isIP) { m_isIP = isIP; });

  connect(this, &SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked, this, [this](bool isIP) {
    const double val = m_constantValueEdit->value();
    if (isIP) {
      m_constantValueEdit->setRange(-148.0, 212.0);
      m_constantValueEdit->setSuffix(tr(" °F"));
      m_constantValueEdit->setValue(val * 9.0 / 5.0 + 32.0);
    } else {
      m_constantValueEdit->setRange(-100.0, 100.0);
      m_constantValueEdit->setSuffix(tr(" °C"));
      m_constantValueEdit->setValue((val - 32.0) * 5.0 / 9.0);
    }
  });

  connect(m_applyConstantButton, &QPushButton::clicked, this, [this]() {
    const double val = m_constantValueEdit->value();
    const double celsius = m_isIP ? (val - 32.0) * 5.0 / 9.0 : val;
    applyConstantValue(celsius);
    m_cachedCelsius.fill(celsius);
    refreshChartDisplay();
  });

  // Chart
  m_chartBarSet = new QBarSet(QString());
  for (int i = 0; i < 12; ++i) {
    m_chartBarSet->append(0.0);
  }

  auto* barSeries = new QBarSeries;
  barSeries->append(m_chartBarSet);

  const QStringList monthAbbrevs = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  auto* axisX = new QBarCategoryAxis;
  axisX->append(monthAbbrevs);

  m_chartYAxis = new QValueAxis;
  m_chartYAxis->setTitleText(isIP ? tr("Temperature [°F]") : tr("Temperature [°C]"));

  auto* chart = new QChart;
  chart->legend()->hide();
  chart->addSeries(barSeries);
  chart->addAxis(axisX, Qt::AlignBottom);
  chart->addAxis(m_chartYAxis, Qt::AlignLeft);
  barSeries->attachAxis(axisX);
  barSeries->attachAxis(m_chartYAxis);
  chart->setAnimationOptions(QChart::SeriesAnimations);

  auto* chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumHeight(220);
  chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(chartView, 1);

  connect(this, &SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked, this, [this](bool) { refreshChartDisplay(); });
}

void SiteGroundTemperatureMonthlyWidget::detach() {
  for (auto* edit : m_edits) {
    if (edit) {
      edit->unbind();
    }
  }
  m_valuesGetter = nullptr;
}

void SiteGroundTemperatureMonthlyWidget::refreshChartFromModel() {
  if (m_valuesGetter) {
    setChartValues(m_valuesGetter());
  }
}

void SiteGroundTemperatureMonthlyWidget::setChartValues(const std::array<double, 12>& celsiusValues) {
  m_cachedCelsius = celsiusValues;
  refreshChartDisplay();
}

void SiteGroundTemperatureMonthlyWidget::refreshChartDisplay() {
  if (!m_chartBarSet || !m_chartYAxis) {
    return;
  }
  double minVal = m_isIP ? m_cachedCelsius[0] * 9.0 / 5.0 + 32.0 : m_cachedCelsius[0];
  double maxVal = minVal;
  for (int i = 0; i < 12; ++i) {
    const double val = m_isIP ? m_cachedCelsius[i] * 9.0 / 5.0 + 32.0 : m_cachedCelsius[i];
    m_chartBarSet->replace(i, val);
    if (val < minVal) {
      minVal = val;
    }
    if (val > maxVal) {
      maxVal = val;
    }
  }
  const double pad = m_isIP ? 3.6 : 2.0;  // ~2°C in °F
  m_chartYAxis->setRange(minVal - pad, maxVal + pad);
  m_chartYAxis->setTitleText(m_isIP ? tr("Temperature [°F]") : tr("Temperature [°C]"));
}

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureBuildingSurfaceWidget
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureBuildingSurfaceWidget::SiteGroundTemperatureBuildingSurfaceWidget(bool isIP, QWidget* parent)
  : SiteGroundTemperatureMonthlyWidget(isIP, parent) {}

void SiteGroundTemperatureBuildingSurfaceWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<MOType>();
  m_titleLabel->setText("Site:GroundTemperature:BuildingSurface");

  for (int i = 0; i < 12; ++i) {
    const auto& mb = s_monthBinders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }

  m_valuesGetter = [this]() {
    std::array<double, 12> vals{};
    for (int i = 0; i < 12; ++i) {
      vals[i] = (m_obj.get_ptr()->*s_monthBinders[i].getter)();
    }
    return vals;
  };
  const auto vals = m_valuesGetter();
  setChartValues(vals);
  m_constantValueEdit->setValue(m_isIP ? vals[0] * 9.0 / 5.0 + 32.0 : vals[0]);
}

void SiteGroundTemperatureBuildingSurfaceWidget::applyConstantValue(double celsius) {
  if (!m_obj) {
    return;
  }
  for (const auto& mb : s_monthBinders) {
    (m_obj.get_ptr()->*mb.setter)(celsius);
  }
}

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureShallowWidget
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureShallowWidget::SiteGroundTemperatureShallowWidget(bool isIP, QWidget* parent)
  : SiteGroundTemperatureMonthlyWidget(isIP, parent) {}

void SiteGroundTemperatureShallowWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<MOType>();
  m_titleLabel->setText("Site:GroundTemperature:Shallow");

  for (int i = 0; i < 12; ++i) {
    const auto& mb = s_monthBinders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }

  m_valuesGetter = [this]() {
    std::array<double, 12> vals{};
    for (int i = 0; i < 12; ++i) {
      vals[i] = (m_obj.get_ptr()->*s_monthBinders[i].getter)();
    }
    return vals;
  };
  const auto vals = m_valuesGetter();
  setChartValues(vals);
  m_constantValueEdit->setValue(m_isIP ? vals[0] * 9.0 / 5.0 + 32.0 : vals[0]);
}

void SiteGroundTemperatureShallowWidget::applyConstantValue(double celsius) {
  if (!m_obj) {
    return;
  }
  for (const auto& mb : s_monthBinders) {
    (m_obj.get_ptr()->*mb.setter)(celsius);
  }
}

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureDeepWidget
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureDeepWidget::SiteGroundTemperatureDeepWidget(bool isIP, QWidget* parent) : SiteGroundTemperatureMonthlyWidget(isIP, parent) {}

void SiteGroundTemperatureDeepWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<MOType>();
  m_titleLabel->setText("Site:GroundTemperature:Deep");

  for (int i = 0; i < 12; ++i) {
    const auto& mb = s_monthBinders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }

  m_valuesGetter = [this]() {
    std::array<double, 12> vals{};
    for (int i = 0; i < 12; ++i) {
      vals[i] = (m_obj.get_ptr()->*s_monthBinders[i].getter)();
    }
    return vals;
  };
  const auto vals = m_valuesGetter();
  setChartValues(vals);
  m_constantValueEdit->setValue(m_isIP ? vals[0] * 9.0 / 5.0 + 32.0 : vals[0]);
}

void SiteGroundTemperatureDeepWidget::applyConstantValue(double celsius) {
  if (!m_obj) {
    return;
  }
  for (const auto& mb : s_monthBinders) {
    (m_obj.get_ptr()->*mb.setter)(celsius);
  }
}

}  // namespace openstudio
