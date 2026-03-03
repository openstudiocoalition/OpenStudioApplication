/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GroundTemperatureWidget.hpp"

#include "YearSettingsWidget.hpp"

#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

#define TEMP_EDIT_WIDTH 90

namespace openstudio {

// ─────────────────────────────────────────────────────────
// SiteGroundTemperatureEntry
// ─────────────────────────────────────────────────────────

SiteGroundTemperatureEntry::SiteGroundTemperatureEntry(const QString& label, QWidget* parent) : QWidget(parent) {
  setFixedHeight(25);
  setMouseTracking(true);

  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(10, 0, 0, 0);
  setLayout(layout);

  m_label = new QLabel(label);
  m_label->setMouseTracking(true);
  layout->addWidget(m_label);
}

void SiteGroundTemperatureEntry::setSelected(bool selected) {
  m_selected = selected;
  update();
}

void SiteGroundTemperatureEntry::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

  if (m_hovering || m_selected) {
    p.setBrush(QBrush(QColor(207, 207, 207)));
    p.setPen(Qt::NoPen);
    p.drawRect(0, 0, size().width() - 1, size().height() - 1);
  }

  p.setPen(Qt::SolidLine);
  p.setBrush(QBrush(QColor(Qt::black)));
  p.drawLine(0, 0, size().width(), 0);
  p.drawLine(0, size().height() - 1, size().width(), size().height() - 1);
}

void SiteGroundTemperatureEntry::mousePressEvent(QMouseEvent* event) {
  m_mouseDown = true;
  event->accept();
}

void SiteGroundTemperatureEntry::mouseReleaseEvent(QMouseEvent* event) {
  if (m_mouseDown) {
    emit clicked();
  }
  m_mouseDown = false;
  event->accept();
}

void SiteGroundTemperatureEntry::mouseMoveEvent(QMouseEvent* event) {
  m_hovering = true;
  update();
  event->accept();
}

void SiteGroundTemperatureEntry::leaveEvent(QEvent* event) {
  m_mouseDown = false;
  m_hovering = false;
  update();
  event->accept();
}

// ─────────────────────────────────────────────────────────
// GroundTemperatureListView
// ─────────────────────────────────────────────────────────

GroundTemperatureListView::GroundTemperatureListView(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  m_bsEntry = new SiteGroundTemperatureEntry(tr("Building Surface Ground Temperatures"), this);
  m_shEntry = new SiteGroundTemperatureEntry(tr("Shallow Ground Temperatures"), this);

  connect(m_bsEntry, &SiteGroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onBuildingSurfaceClicked);
  connect(m_shEntry, &SiteGroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onShallowClicked);

  layout->addWidget(m_bsEntry);
  layout->addWidget(m_shEntry);
  layout->addStretch();
}

void GroundTemperatureListView::selectFirst() {
  onBuildingSurfaceClicked();
}

void GroundTemperatureListView::onBuildingSurfaceClicked() {
  m_bsEntry->setSelected(true);
  m_shEntry->setSelected(false);
  emit typeSelected(GroundTempType::BuildingSurface);
}

void GroundTemperatureListView::onShallowClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(true);
  emit typeSelected(GroundTempType::Shallow);
}

// ─────────────────────────────────────────────────────────
// GroundTemperatureNotPresentView
// ─────────────────────────────────────────────────────────

GroundTemperatureNotPresentView::GroundTemperatureNotPresentView(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(10);
  setLayout(layout);

  m_label = new QLabel();
  m_label->setWordWrap(true);
  layout->addWidget(m_label);

  m_addButton = new QPushButton(tr("Add"));
  m_addButton->setObjectName("StandardBlueButton");
  layout->addWidget(m_addButton, 0, Qt::AlignLeft);

  layout->addStretch();

  connect(m_addButton, &QPushButton::clicked, this, [this]() { emit addClicked(m_type); });
}

void GroundTemperatureNotPresentView::setType(GroundTempType type, const QString& typeName, model::Model model) {
  m_type = type;
  m_model = model;
  m_label->setText(tr("The %1 object is not present in this model. Click Add to create it.").arg(typeName));
}

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
    double (BS::*getter)() const;
    bool (BS::*setter)(double);
    void (BS::*resetter)();
    bool (BS::*defaulted)() const;
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {&BS::januaryGroundTemperature, &BS::setJanuaryGroundTemperature, &BS::resetJanuaryGroundTemperature, &BS::isJanuaryGroundTemperatureDefaulted},
    {&BS::februaryGroundTemperature, &BS::setFebruaryGroundTemperature, &BS::resetFebruaryGroundTemperature, &BS::isFebruaryGroundTemperatureDefaulted},
    {&BS::marchGroundTemperature, &BS::setMarchGroundTemperature, &BS::resetMarchGroundTemperature, &BS::isMarchGroundTemperatureDefaulted},
    {&BS::aprilGroundTemperature, &BS::setAprilGroundTemperature, &BS::resetAprilGroundTemperature, &BS::isAprilGroundTemperatureDefaulted},
    {&BS::mayGroundTemperature, &BS::setMayGroundTemperature, &BS::resetMayGroundTemperature, &BS::isMayGroundTemperatureDefaulted},
    {&BS::juneGroundTemperature, &BS::setJuneGroundTemperature, &BS::resetJuneGroundTemperature, &BS::isJuneGroundTemperatureDefaulted},
    {&BS::julyGroundTemperature, &BS::setJulyGroundTemperature, &BS::resetJulyGroundTemperature, &BS::isJulyGroundTemperatureDefaulted},
    {&BS::augustGroundTemperature, &BS::setAugustGroundTemperature, &BS::resetAugustGroundTemperature, &BS::isAugustGroundTemperatureDefaulted},
    {&BS::septemberGroundTemperature, &BS::setSeptemberGroundTemperature, &BS::resetSeptemberGroundTemperature, &BS::isSeptemberGroundTemperatureDefaulted},
    {&BS::octoberGroundTemperature, &BS::setOctoberGroundTemperature, &BS::resetOctoberGroundTemperature, &BS::isOctoberGroundTemperatureDefaulted},
    {&BS::novemberGroundTemperature, &BS::setNovemberGroundTemperature, &BS::resetNovemberGroundTemperature, &BS::isNovemberGroundTemperatureDefaulted},
    {&BS::decemberGroundTemperature, &BS::setDecemberGroundTemperature, &BS::resetDecemberGroundTemperature, &BS::isDecemberGroundTemperatureDefaulted},
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& mb = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }
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
    double (SH::*getter)() const;
    bool (SH::*setter)(double);
    void (SH::*resetter)();
    bool (SH::*defaulted)() const;
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {&SH::januarySurfaceGroundTemperature, &SH::setJanuarySurfaceGroundTemperature, &SH::resetJanuarySurfaceGroundTemperature,
     &SH::isJanuarySurfaceGroundTemperatureDefaulted},
    {&SH::februarySurfaceGroundTemperature, &SH::setFebruarySurfaceGroundTemperature, &SH::resetFebruarySurfaceGroundTemperature,
     &SH::isFebruarySurfaceGroundTemperatureDefaulted},
    {&SH::marchSurfaceGroundTemperature, &SH::setMarchSurfaceGroundTemperature, &SH::resetMarchSurfaceGroundTemperature,
     &SH::isMarchSurfaceGroundTemperatureDefaulted},
    {&SH::aprilSurfaceGroundTemperature, &SH::setAprilSurfaceGroundTemperature, &SH::resetAprilSurfaceGroundTemperature,
     &SH::isAprilSurfaceGroundTemperatureDefaulted},
    {&SH::maySurfaceGroundTemperature, &SH::setMaySurfaceGroundTemperature, &SH::resetMaySurfaceGroundTemperature,
     &SH::isMaySurfaceGroundTemperatureDefaulted},
    {&SH::juneSurfaceGroundTemperature, &SH::setJuneSurfaceGroundTemperature, &SH::resetJuneSurfaceGroundTemperature,
     &SH::isJuneSurfaceGroundTemperatureDefaulted},
    {&SH::julySurfaceGroundTemperature, &SH::setJulySurfaceGroundTemperature, &SH::resetJulySurfaceGroundTemperature,
     &SH::isJulySurfaceGroundTemperatureDefaulted},
    {&SH::augustSurfaceGroundTemperature, &SH::setAugustSurfaceGroundTemperature, &SH::resetAugustSurfaceGroundTemperature,
     &SH::isAugustSurfaceGroundTemperatureDefaulted},
    {&SH::septemberSurfaceGroundTemperature, &SH::setSeptemberSurfaceGroundTemperature, &SH::resetSeptemberSurfaceGroundTemperature,
     &SH::isSeptemberSurfaceGroundTemperatureDefaulted},
    {&SH::octoberSurfaceGroundTemperature, &SH::setOctoberSurfaceGroundTemperature, &SH::resetOctoberSurfaceGroundTemperature,
     &SH::isOctoberSurfaceGroundTemperatureDefaulted},
    {&SH::novemberSurfaceGroundTemperature, &SH::setNovemberSurfaceGroundTemperature, &SH::resetNovemberSurfaceGroundTemperature,
     &SH::isNovemberSurfaceGroundTemperatureDefaulted},
    {&SH::decemberSurfaceGroundTemperature, &SH::setDecemberSurfaceGroundTemperature, &SH::resetDecemberSurfaceGroundTemperature,
     &SH::isDecemberSurfaceGroundTemperatureDefaulted},
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& mb = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_obj, DoubleGetter([this, g = mb.getter]() { return (m_obj.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = mb.setter](double v) { return (m_obj.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = mb.resetter]() { (m_obj.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = mb.defaulted]() { return (m_obj.get_ptr()->*d)(); }));
  }
}

// ─────────────────────────────────────────────────────────
// GroundTemperatureView
// ─────────────────────────────────────────────────────────

GroundTemperatureView::GroundTemperatureView(bool isIP, const model::Model& model, QWidget* parent)
  : QWidget(parent), m_model(model), m_isIP(isIP) {
  auto* mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);

  // Left pane (fixed width)
  auto* leftPane = new QWidget();
  leftPane->setFixedWidth(200);
  auto* leftLayout = new QVBoxLayout();
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(0);
  leftPane->setLayout(leftLayout);

  m_listView = new GroundTemperatureListView(leftPane);
  leftLayout->addWidget(m_listView);
  leftLayout->addStretch();

  mainLayout->addWidget(leftPane);

  // Right pane: stacked widget
  m_rightStack = new QStackedWidget();
  mainLayout->addWidget(m_rightStack, 1);

  m_notPresentView = new GroundTemperatureNotPresentView();
  m_rightStack->addWidget(m_notPresentView);  // index 0

  m_bsView = new SiteGroundTemperatureBuildingSurfaceWidget(isIP);
  m_rightStack->addWidget(m_bsView);  // index 1

  m_shView = new SiteGroundTemperatureShallowWidget(isIP);
  m_rightStack->addWidget(m_shView);  // index 2

  connect(m_listView, &GroundTemperatureListView::typeSelected, this, &GroundTemperatureView::onTypeSelected);
  connect(m_notPresentView, &GroundTemperatureNotPresentView::addClicked, this, &GroundTemperatureView::onObjectCreated);

  // Forward unit toggle to sub-views (signal-to-signal)
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_bsView, &SiteGroundTemperatureBuildingSurfaceWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_shView, &SiteGroundTemperatureShallowWidget::toggleUnitsClicked);

  // Auto-select first entry after the event loop starts (triggers both visual selection and right-pane update)
  QTimer::singleShot(0, this, [this]() { m_listView->selectFirst(); });
}

void GroundTemperatureView::onTypeSelected(GroundTempType type) {
  if (type == GroundTempType::BuildingSurface) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
    if (opt) {
      m_bsView->attach(*opt);
      m_rightStack->setCurrentIndex(1);
    } else {
      m_notPresentView->setType(type, tr("Site:GroundTemperature:BuildingSurface"), m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureShallow>();
    if (opt) {
      m_shView->attach(*opt);
      m_rightStack->setCurrentIndex(2);
    } else {
      m_notPresentView->setType(type, tr("Site:GroundTemperature:Shallow"), m_model);
      m_rightStack->setCurrentIndex(0);
    }
  }
}

void GroundTemperatureView::onObjectCreated(GroundTempType type) {
  if (type == GroundTempType::BuildingSurface) {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
    m_bsView->attach(obj);
    m_rightStack->setCurrentIndex(1);
  } else {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureShallow>();
    m_shView->attach(obj);
    m_rightStack->setCurrentIndex(2);
  }
}

}  // namespace openstudio
