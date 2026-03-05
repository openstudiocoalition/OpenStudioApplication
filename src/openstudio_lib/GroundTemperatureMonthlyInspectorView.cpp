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

#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
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
  });

  mainLayout->addStretch();
}

void SiteGroundTemperatureMonthlyWidget::detach() {
  for (auto* edit : m_edits) {
    if (edit) {
      edit->unbind();
    }
  }
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

  // Default value is the January one
  const double januaryCelsius = (m_obj.get_ptr()->*s_monthBinders[0].getter)();
  m_constantValueEdit->setValue(m_isIP ? januaryCelsius * 9.0 / 5.0 + 32.0 : januaryCelsius);
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

  const double januaryCelsius = (m_obj.get_ptr()->*s_monthBinders[0].getter)();
  m_constantValueEdit->setValue(m_isIP ? januaryCelsius * 9.0 / 5.0 + 32.0 : januaryCelsius);
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

  const double januaryCelsius = (m_obj.get_ptr()->*s_monthBinders[0].getter)();
  m_constantValueEdit->setValue(m_isIP ? januaryCelsius * 9.0 / 5.0 + 32.0 : januaryCelsius);
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
