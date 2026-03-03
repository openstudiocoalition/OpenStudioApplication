/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GroundTemperatureView.hpp"

#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

namespace openstudio {

// ─────────────────────────────────────────────────────────
// GroundTemperatureEntry
// ─────────────────────────────────────────────────────────

GroundTemperatureEntry::GroundTemperatureEntry(const QString& label, QWidget* parent) : QWidget(parent) {
  setFixedHeight(25);
  setMouseTracking(true);

  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(10, 0, 0, 0);
  setLayout(layout);

  m_label = new QLabel(label);
  m_label->setMouseTracking(true);
  layout->addWidget(m_label);
}

void GroundTemperatureEntry::setSelected(bool selected) {
  m_selected = selected;
  update();
}

void GroundTemperatureEntry::paintEvent(QPaintEvent* /*event*/) {
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

void GroundTemperatureEntry::mousePressEvent(QMouseEvent* event) {
  m_mouseDown = true;
  event->accept();
}

void GroundTemperatureEntry::mouseReleaseEvent(QMouseEvent* event) {
  if (m_mouseDown) {
    emit clicked();
  }
  m_mouseDown = false;
  event->accept();
}

void GroundTemperatureEntry::mouseMoveEvent(QMouseEvent* event) {
  m_hovering = true;
  update();
  event->accept();
}

void GroundTemperatureEntry::leaveEvent(QEvent* event) {
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

  m_bsEntry = new GroundTemperatureEntry(tr("Building Surface Ground Temperatures"), this);
  m_shEntry = new GroundTemperatureEntry(tr("Shallow Ground Temperatures"), this);

  connect(m_bsEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onBuildingSurfaceClicked);
  connect(m_shEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onShallowClicked);

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
  m_addButton->setMinimumWidth(100);
  layout->addWidget(m_addButton, 0, Qt::AlignLeft);

  layout->addStretch();

  connect(m_addButton, &QPushButton::clicked, this, [this]() { emit addClicked(m_type); });
}

void GroundTemperatureNotPresentView::setType(GroundTempType type, const QString& typeName, model::Model model) {
  m_type = type;
  m_model = std::move(model);
  m_label->setText(tr("The %1 Unique ModelObject is not present in this model. Click Add to instantiate it.").arg(typeName));
}

// ─────────────────────────────────────────────────────────
// GroundTemperatureView
// ─────────────────────────────────────────────────────────

GroundTemperatureView::GroundTemperatureView(bool isIP, const model::Model& model, QWidget* parent) : QWidget(parent), m_model(model), m_isIP(isIP) {
  setObjectName("GrayWidgetWithLeftTopBorders");

  auto* mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(1, 1, 0, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);

  // Left pane (sizes to content; right stack takes remaining space via stretch=1)
  auto* leftPane = new QWidget();
  leftPane->setFixedWidth(250);
  auto* leftLayout = new QVBoxLayout();
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(0);
  leftPane->setLayout(leftLayout);

  m_listView = new GroundTemperatureListView(leftPane);
  leftLayout->addWidget(m_listView);
  leftLayout->addStretch();

  mainLayout->addWidget(leftPane);

  // Vertical separator (matches Schedules tab style)
  auto* vLine = new QWidget();
  vLine->setObjectName("VLine");
  vLine->setStyleSheet("QWidget#VLine { background: #445051;}");
  vLine->setFixedWidth(2);
  mainLayout->addWidget(vLine);

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

  // Auto-select first entry after the event loop starts
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
