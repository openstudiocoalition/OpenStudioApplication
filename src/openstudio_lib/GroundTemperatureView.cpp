/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GroundTemperatureView.hpp"

#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureDeep_Impl.hpp>
#include <openstudio/model/SiteWaterMainsTemperature.hpp>
#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/utilities/filetypes/EpwFile.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QLinearGradient>
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
  setFixedHeight(50);
  setMouseTracking(true);

  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(9, 0, 9, 0);
  setLayout(layout);

  m_label = new QLabel(label);
  m_label->setObjectName("H2");
  m_label->setWordWrap(true);
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

  const int w = size().width();
  const int h = size().height();

  if (m_selected) {
    // Gradient matching OSCollapsibleItemHeader selected state
    QLinearGradient gradient(0, 0, 0, h);
    gradient.setColorAt(0.00, QColor(0x63, 0x61, 0x61));
    gradient.setColorAt(0.10, QColor(0x63, 0x61, 0x61));
    gradient.setColorAt(0.15, QColor(0xA3, 0xA3, 0xA3));
    gradient.setColorAt(1.00, QColor(0xA3, 0xA3, 0xA3));
    p.fillRect(0, 0, w, h, gradient);
  } else if (m_hovering) {
    p.fillRect(0, 0, w, h, QColor(0xCE, 0xCE, 0xCE));
  }

  // Bottom border (matches OSCollapsibleItemHeader border-bottom: 1px solid black)
  p.setPen(QPen(Qt::black, 1));
  p.drawLine(0, h - 1, w, h - 1);
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
  m_deepEntry = new GroundTemperatureEntry(tr("Deep Ground Temperatures"), this);
  m_waterMainsEntry = new GroundTemperatureEntry(tr("Water Mains Temperature"), this);

  connect(m_bsEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onBuildingSurfaceClicked);
  connect(m_shEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onShallowClicked);
  connect(m_deepEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onDeepClicked);
  connect(m_waterMainsEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onWaterMainsClicked);

  layout->addWidget(m_bsEntry);
  layout->addWidget(m_shEntry);
  layout->addWidget(m_deepEntry);
  layout->addWidget(m_waterMainsEntry);
  layout->addStretch();
}

void GroundTemperatureListView::selectFirst() {
  onBuildingSurfaceClicked();
}

void GroundTemperatureListView::onBuildingSurfaceClicked() {
  m_bsEntry->setSelected(true);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(false);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::BuildingSurface);
}

void GroundTemperatureListView::onShallowClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(true);
  m_deepEntry->setSelected(false);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::Shallow);
}

void GroundTemperatureListView::onDeepClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(true);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::Deep);
}

void GroundTemperatureListView::onWaterMainsClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(false);
  m_waterMainsEntry->setSelected(true);
  emit typeSelected(GroundTempType::WaterMains);
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

  m_importFromEPWButton = new QPushButton(tr("Import from EPW"));
  m_importFromEPWButton->setObjectName("StandardGrayButton");
  m_importFromEPWButton->setEnabled(false);
  layout->addWidget(m_importFromEPWButton, 0, Qt::AlignLeft);

  layout->addStretch();

  connect(m_addButton, &QPushButton::clicked, this, [this]() { emit addClicked(m_type); });
}

void GroundTemperatureNotPresentView::setType(GroundTempType type, const QString& typeName, model::Model model) {
  m_type = type;
  m_model = std::move(model);

  disconnect(m_importFromEPWButton, &QPushButton::clicked, nullptr, nullptr);

  QString label = tr("The %1 Unique ModelObject is not present in this model. Click Add to instantiate it.").arg(typeName);
  if (type == GroundTempType::BuildingSurface || type == GroundTempType::WaterMains) {
    m_label->setText(label);
    m_importFromEPWButton->setVisible(false);
    return;
  }

  m_importFromEPWButton->setVisible(true);

  boost::optional<model::WeatherFile> weatherFile_ = m_model.weatherFile();
  if (!weatherFile_) {
    label.append(tr(" No weather file is associated with the model, so the object will be added with default values."));
    m_label->setText(label);
    return;
  }

  openstudio::path filesDir;
  {
    auto companionFolder = toPath(OSAppBase::instance()->currentDocument()->modelTempDir());
    // auto savePath = OSAppBase::instance()->currentDocument()->savePath();
    // if (!savePath.isEmpty()) {
    //  openstudio::path companionFolder = getCompanionFolder(toPath(savePath));
    filesDir = companionFolder / toPath("resources/files/");
  }

  boost::optional<EpwFile> epwFile_ = weatherFile_->file(filesDir);
  if (!epwFile_) {

    label.append(tr(" While a weather file is associated with the model, could not locate the underlying EpwFile, so the object will be added with "
                    "default values."));
    m_label->setText(label);
    return;
  }

  std::vector<EpwGroundTemperatureDepth> ground_temps = epwFile_->groundTemperatureDepths();
  if (ground_temps.empty()) {
    label.append(tr(" The weather file does not contain any ground temperature data, so the object will be added with default values."));
    m_label->setText(label);
    return;
  }

  double target_depth = -999;
  if (type == GroundTempType::Shallow) {
    target_depth = 0.5;
  } else {  // GroundTempType::Deep
    target_depth = 4.0;
  }
  // Now try to find the target_depth in the epw data, allowing for some tolerance since the epw spec doesn't require exact depths
  const double tolerance = 0.1;
  auto it = std::find_if(ground_temps.begin(), ground_temps.end(), [target_depth, tolerance](const EpwGroundTemperatureDepth& gtd) {
    return std::abs(gtd.groundTemperatureDepth() - target_depth) < tolerance;
  });
  if (it == ground_temps.end()) {
    label.append(
      tr(" The weather file does not contain ground temperature data at the expected depth of %1 m, so the object will be added with default values.")
        .arg(target_depth));
    m_label->setText(label);
    return;
  }

  label.append(tr(" The weather file contains ground temperature data at a depth of %1 m, so you can choose to import those values or add the object "
                  "with default values.")
                 .arg(it->groundTemperatureDepth()));

  m_importFromEPWButton->setEnabled(true);
  // Connect button to a lambda that creates the object directly and fills up the value
  connect(m_importFromEPWButton, &QPushButton::clicked, this, [this, type, it]() {
    if (type == GroundTempType::Shallow) {
      auto ts = m_model.getUniqueModelObject<model::SiteGroundTemperatureShallow>();

      ts.setJanuarySurfaceGroundTemperature(it->janGroundTemperature());
      ts.setFebruarySurfaceGroundTemperature(it->febGroundTemperature());
      ts.setMarchSurfaceGroundTemperature(it->marGroundTemperature());
      ts.setAprilSurfaceGroundTemperature(it->aprGroundTemperature());
      ts.setMaySurfaceGroundTemperature(it->mayGroundTemperature());
      ts.setJuneSurfaceGroundTemperature(it->junGroundTemperature());
      ts.setJulySurfaceGroundTemperature(it->julGroundTemperature());
      ts.setAugustSurfaceGroundTemperature(it->augGroundTemperature());
      ts.setSeptemberSurfaceGroundTemperature(it->sepGroundTemperature());
      ts.setOctoberSurfaceGroundTemperature(it->octGroundTemperature());
      ts.setNovemberSurfaceGroundTemperature(it->novGroundTemperature());
      ts.setDecemberSurfaceGroundTemperature(it->decGroundTemperature());
    } else {  // GroundTempType::Deep
      auto td = m_model.getUniqueModelObject<model::SiteGroundTemperatureDeep>();

      td.setJanuaryDeepGroundTemperature(it->janGroundTemperature());
      td.setFebruaryDeepGroundTemperature(it->febGroundTemperature());
      td.setMarchDeepGroundTemperature(it->marGroundTemperature());
      td.setAprilDeepGroundTemperature(it->aprGroundTemperature());
      td.setMayDeepGroundTemperature(it->mayGroundTemperature());
      td.setJuneDeepGroundTemperature(it->junGroundTemperature());
      td.setJulyDeepGroundTemperature(it->julGroundTemperature());
      td.setAugustDeepGroundTemperature(it->augGroundTemperature());
      td.setSeptemberDeepGroundTemperature(it->sepGroundTemperature());
      td.setOctoberDeepGroundTemperature(it->octGroundTemperature());
      td.setNovemberDeepGroundTemperature(it->novGroundTemperature());
      td.setDecemberDeepGroundTemperature(it->decGroundTemperature());
    }
    emit addClicked(type);
  });

  m_label->setText(label);
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

  m_deepView = new SiteGroundTemperatureDeepWidget(isIP);
  m_rightStack->addWidget(m_deepView);  // index 3

  m_waterMainsView = new SiteWaterMainsTemperatureWidget(isIP);
  m_rightStack->addWidget(m_waterMainsView);  // index 4

  connect(m_listView, &GroundTemperatureListView::typeSelected, this, &GroundTemperatureView::onTypeSelected);
  connect(m_notPresentView, &GroundTemperatureNotPresentView::addClicked, this, &GroundTemperatureView::onObjectCreated);

  // Forward unit toggle to sub-views (signal-to-signal)
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_bsView, &SiteGroundTemperatureBuildingSurfaceWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_shView, &SiteGroundTemperatureShallowWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_deepView, &SiteGroundTemperatureDeepWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_waterMainsView, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked);

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
  } else if (type == GroundTempType::Shallow) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureShallow>();
    if (opt) {
      m_shView->attach(*opt);
      m_rightStack->setCurrentIndex(2);
    } else {
      m_notPresentView->setType(type, tr("Site:GroundTemperature:Shallow"), m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else if (type == GroundTempType::Deep) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureDeep>();
    if (opt) {
      m_deepView->attach(*opt);
      m_rightStack->setCurrentIndex(3);
    } else {
      m_notPresentView->setType(type, tr("Site:GroundTemperature:Deep"), m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else {
    auto opt = m_model.siteWaterMainsTemperature();
    if (opt) {
      m_waterMainsView->attach(*opt);
      m_rightStack->setCurrentIndex(4);
    } else {
      m_notPresentView->setType(type, tr("OS:Site:WaterMainsTemperature"), m_model);
      m_rightStack->setCurrentIndex(0);
    }
  }
}

void GroundTemperatureView::onObjectCreated(GroundTempType type) {
  if (type == GroundTempType::BuildingSurface) {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
    m_bsView->attach(obj);
    m_rightStack->setCurrentIndex(1);
  } else if (type == GroundTempType::Shallow) {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureShallow>();
    m_shView->attach(obj);
    m_rightStack->setCurrentIndex(2);
  } else if (type == GroundTempType::Deep) {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureDeep>();
    m_deepView->attach(obj);
    m_rightStack->setCurrentIndex(3);
  } else {
    auto obj = m_model.getUniqueModelObject<model::SiteWaterMainsTemperature>();
    m_waterMainsView->attach(obj);
    m_rightStack->setCurrentIndex(4);
  }
}

}  // namespace openstudio
