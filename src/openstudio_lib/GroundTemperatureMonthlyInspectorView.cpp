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
  m_obj = obj.cast<model::SiteGroundTemperatureBuildingSurface>();
  m_titleLabel->setText("Site:GroundTemperature:BuildingSurface");

  using BS = model::SiteGroundTemperatureBuildingSurface;

  struct MonthBinding
  {
    double (BS::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (BS::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (BS::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (BS::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {
      &BS::januaryGroundTemperature,
      &BS::setJanuaryGroundTemperature,
      &BS::resetJanuaryGroundTemperature,
      &BS::isJanuaryGroundTemperatureDefaulted,
    },
    {
      &BS::februaryGroundTemperature,
      &BS::setFebruaryGroundTemperature,
      &BS::resetFebruaryGroundTemperature,
      &BS::isFebruaryGroundTemperatureDefaulted,
    },
    {
      &BS::marchGroundTemperature,
      &BS::setMarchGroundTemperature,
      &BS::resetMarchGroundTemperature,
      &BS::isMarchGroundTemperatureDefaulted,
    },
    {
      &BS::aprilGroundTemperature,
      &BS::setAprilGroundTemperature,
      &BS::resetAprilGroundTemperature,
      &BS::isAprilGroundTemperatureDefaulted,
    },
    {
      &BS::mayGroundTemperature,
      &BS::setMayGroundTemperature,
      &BS::resetMayGroundTemperature,
      &BS::isMayGroundTemperatureDefaulted,
    },
    {
      &BS::juneGroundTemperature,
      &BS::setJuneGroundTemperature,
      &BS::resetJuneGroundTemperature,
      &BS::isJuneGroundTemperatureDefaulted,
    },
    {
      &BS::julyGroundTemperature,
      &BS::setJulyGroundTemperature,
      &BS::resetJulyGroundTemperature,
      &BS::isJulyGroundTemperatureDefaulted,
    },
    {
      &BS::augustGroundTemperature,
      &BS::setAugustGroundTemperature,
      &BS::resetAugustGroundTemperature,
      &BS::isAugustGroundTemperatureDefaulted,
    },
    {
      &BS::septemberGroundTemperature,
      &BS::setSeptemberGroundTemperature,
      &BS::resetSeptemberGroundTemperature,
      &BS::isSeptemberGroundTemperatureDefaulted,
    },
    {
      &BS::octoberGroundTemperature,
      &BS::setOctoberGroundTemperature,
      &BS::resetOctoberGroundTemperature,
      &BS::isOctoberGroundTemperatureDefaulted,
    },
    {
      &BS::novemberGroundTemperature,
      &BS::setNovemberGroundTemperature,
      &BS::resetNovemberGroundTemperature,
      &BS::isNovemberGroundTemperatureDefaulted,
    },
    {
      &BS::decemberGroundTemperature,
      &BS::setDecemberGroundTemperature,
      &BS::resetDecemberGroundTemperature,
      &BS::isDecemberGroundTemperatureDefaulted,
    },
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& mb = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }
}

void SiteGroundTemperatureBuildingSurfaceWidget::applyConstantValue(double celsius) {
  if (!m_obj) return;
  m_obj->setJanuaryGroundTemperature(celsius);
  m_obj->setFebruaryGroundTemperature(celsius);
  m_obj->setMarchGroundTemperature(celsius);
  m_obj->setAprilGroundTemperature(celsius);
  m_obj->setMayGroundTemperature(celsius);
  m_obj->setJuneGroundTemperature(celsius);
  m_obj->setJulyGroundTemperature(celsius);
  m_obj->setAugustGroundTemperature(celsius);
  m_obj->setSeptemberGroundTemperature(celsius);
  m_obj->setOctoberGroundTemperature(celsius);
  m_obj->setNovemberGroundTemperature(celsius);
  m_obj->setDecemberGroundTemperature(celsius);
}

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureShallowWidget
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureShallowWidget::SiteGroundTemperatureShallowWidget(bool isIP, QWidget* parent)
  : SiteGroundTemperatureMonthlyWidget(isIP, parent) {}

void SiteGroundTemperatureShallowWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<model::SiteGroundTemperatureShallow>();
  m_titleLabel->setText("Site:GroundTemperature:Shallow");

  using SH = model::SiteGroundTemperatureShallow;

  struct MonthBinding
  {
    double (SH::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (SH::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (SH::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (SH::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {
      &SH::januarySurfaceGroundTemperature,
      &SH::setJanuarySurfaceGroundTemperature,
      &SH::resetJanuarySurfaceGroundTemperature,
      &SH::isJanuarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::februarySurfaceGroundTemperature,
      &SH::setFebruarySurfaceGroundTemperature,
      &SH::resetFebruarySurfaceGroundTemperature,
      &SH::isFebruarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::marchSurfaceGroundTemperature,
      &SH::setMarchSurfaceGroundTemperature,
      &SH::resetMarchSurfaceGroundTemperature,
      &SH::isMarchSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::aprilSurfaceGroundTemperature,
      &SH::setAprilSurfaceGroundTemperature,
      &SH::resetAprilSurfaceGroundTemperature,
      &SH::isAprilSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::maySurfaceGroundTemperature,
      &SH::setMaySurfaceGroundTemperature,
      &SH::resetMaySurfaceGroundTemperature,
      &SH::isMaySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::juneSurfaceGroundTemperature,
      &SH::setJuneSurfaceGroundTemperature,
      &SH::resetJuneSurfaceGroundTemperature,
      &SH::isJuneSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::julySurfaceGroundTemperature,
      &SH::setJulySurfaceGroundTemperature,
      &SH::resetJulySurfaceGroundTemperature,
      &SH::isJulySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::augustSurfaceGroundTemperature,
      &SH::setAugustSurfaceGroundTemperature,
      &SH::resetAugustSurfaceGroundTemperature,
      &SH::isAugustSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::septemberSurfaceGroundTemperature,
      &SH::setSeptemberSurfaceGroundTemperature,
      &SH::resetSeptemberSurfaceGroundTemperature,
      &SH::isSeptemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::octoberSurfaceGroundTemperature,
      &SH::setOctoberSurfaceGroundTemperature,
      &SH::resetOctoberSurfaceGroundTemperature,
      &SH::isOctoberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::novemberSurfaceGroundTemperature,
      &SH::setNovemberSurfaceGroundTemperature,
      &SH::resetNovemberSurfaceGroundTemperature,
      &SH::isNovemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::decemberSurfaceGroundTemperature,
      &SH::setDecemberSurfaceGroundTemperature,
      &SH::resetDecemberSurfaceGroundTemperature,
      &SH::isDecemberSurfaceGroundTemperatureDefaulted,
    },
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& mb = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }
}

void SiteGroundTemperatureShallowWidget::applyConstantValue(double celsius) {
  if (!m_obj) return;
  m_obj->setJanuarySurfaceGroundTemperature(celsius);
  m_obj->setFebruarySurfaceGroundTemperature(celsius);
  m_obj->setMarchSurfaceGroundTemperature(celsius);
  m_obj->setAprilSurfaceGroundTemperature(celsius);
  m_obj->setMaySurfaceGroundTemperature(celsius);
  m_obj->setJuneSurfaceGroundTemperature(celsius);
  m_obj->setJulySurfaceGroundTemperature(celsius);
  m_obj->setAugustSurfaceGroundTemperature(celsius);
  m_obj->setSeptemberSurfaceGroundTemperature(celsius);
  m_obj->setOctoberSurfaceGroundTemperature(celsius);
  m_obj->setNovemberSurfaceGroundTemperature(celsius);
  m_obj->setDecemberSurfaceGroundTemperature(celsius);
}

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureDeepWidget
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureDeepWidget::SiteGroundTemperatureDeepWidget(bool isIP, QWidget* parent) : SiteGroundTemperatureMonthlyWidget(isIP, parent) {}

void SiteGroundTemperatureDeepWidget::attach(const model::ModelObject& obj) {
  detach();
  m_obj = obj.cast<model::SiteGroundTemperatureDeep>();
  m_titleLabel->setText("Site:GroundTemperature:Deep");

  using DP = model::SiteGroundTemperatureDeep;

  struct MonthBinding
  {
    double (DP::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (DP::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (DP::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (DP::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {
      &DP::januaryDeepGroundTemperature,
      &DP::setJanuaryDeepGroundTemperature,
      &DP::resetJanuaryDeepGroundTemperature,
      &DP::isJanuaryDeepGroundTemperatureDefaulted,
    },
    {
      &DP::februaryDeepGroundTemperature,
      &DP::setFebruaryDeepGroundTemperature,
      &DP::resetFebruaryDeepGroundTemperature,
      &DP::isFebruaryDeepGroundTemperatureDefaulted,
    },
    {
      &DP::marchDeepGroundTemperature,
      &DP::setMarchDeepGroundTemperature,
      &DP::resetMarchDeepGroundTemperature,
      &DP::isMarchDeepGroundTemperatureDefaulted,
    },
    {
      &DP::aprilDeepGroundTemperature,
      &DP::setAprilDeepGroundTemperature,
      &DP::resetAprilDeepGroundTemperature,
      &DP::isAprilDeepGroundTemperatureDefaulted,
    },
    {
      &DP::mayDeepGroundTemperature,
      &DP::setMayDeepGroundTemperature,
      &DP::resetMayDeepGroundTemperature,
      &DP::isMayDeepGroundTemperatureDefaulted,
    },
    {
      &DP::juneDeepGroundTemperature,
      &DP::setJuneDeepGroundTemperature,
      &DP::resetJuneDeepGroundTemperature,
      &DP::isJuneDeepGroundTemperatureDefaulted,
    },
    {
      &DP::julyDeepGroundTemperature,
      &DP::setJulyDeepGroundTemperature,
      &DP::resetJulyDeepGroundTemperature,
      &DP::isJulyDeepGroundTemperatureDefaulted,
    },
    {
      &DP::augustDeepGroundTemperature,
      &DP::setAugustDeepGroundTemperature,
      &DP::resetAugustDeepGroundTemperature,
      &DP::isAugustDeepGroundTemperatureDefaulted,
    },
    {
      &DP::septemberDeepGroundTemperature,
      &DP::setSeptemberDeepGroundTemperature,
      &DP::resetSeptemberDeepGroundTemperature,
      &DP::isSeptemberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::octoberDeepGroundTemperature,
      &DP::setOctoberDeepGroundTemperature,
      &DP::resetOctoberDeepGroundTemperature,
      &DP::isOctoberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::novemberDeepGroundTemperature,
      &DP::setNovemberDeepGroundTemperature,
      &DP::resetNovemberDeepGroundTemperature,
      &DP::isNovemberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::decemberDeepGroundTemperature,
      &DP::setDecemberDeepGroundTemperature,
      &DP::resetDecemberDeepGroundTemperature,
      &DP::isDecemberDeepGroundTemperatureDefaulted,
    },
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& mb = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }
}

void SiteGroundTemperatureDeepWidget::applyConstantValue(double celsius) {
  if (!m_obj) return;
  m_obj->setJanuaryDeepGroundTemperature(celsius);
  m_obj->setFebruaryDeepGroundTemperature(celsius);
  m_obj->setMarchDeepGroundTemperature(celsius);
  m_obj->setAprilDeepGroundTemperature(celsius);
  m_obj->setMayDeepGroundTemperature(celsius);
  m_obj->setJuneDeepGroundTemperature(celsius);
  m_obj->setJulyDeepGroundTemperature(celsius);
  m_obj->setAugustDeepGroundTemperature(celsius);
  m_obj->setSeptemberDeepGroundTemperature(celsius);
  m_obj->setOctoberDeepGroundTemperature(celsius);
  m_obj->setNovemberDeepGroundTemperature(celsius);
  m_obj->setDecemberDeepGroundTemperature(celsius);
}

}  // namespace openstudio
