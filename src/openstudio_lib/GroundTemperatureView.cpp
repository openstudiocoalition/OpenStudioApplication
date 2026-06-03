/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GroundTemperatureView.hpp"
#include "../model_editor/IddObjectDocUrl.hpp"

#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItemSelectorButtons.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureDeep_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureFCfactorMethod_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>
#include <openstudio/model/SiteWaterMainsTemperature.hpp>
#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/utilities/filetypes/EpwFile.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringLiteral>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

namespace openstudio {

// ─────────────────────────────────────────────────────────
// GroundTemperatureEntry
// ─────────────────────────────────────────────────────────

GroundTemperatureEntry::GroundTemperatureEntry(const QString& label, const QString& url, QWidget* parent) : QWidget(parent) {
  setFixedHeight(50);
  setObjectName("GroundTemperatureEntry");
  setProperty("style", "0");
  setStyleSheet("QWidget#GroundTemperatureEntry[style=\"0\"]       { background: #CECECE; border-bottom: 1px solid black; }"
                "QWidget#GroundTemperatureEntry[style=\"0\"]:hover { background: #BEBEBE; border-bottom: 1px solid black; }"
                "QWidget#GroundTemperatureEntry[style=\"1\"]       { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
                "  stop: 0.0 #636161, stop: 0.10 #636161, stop: 0.15 #A3A3A3, stop: 1.0 #A3A3A3); border-bottom: 1px solid black; }");

  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(9, 0, 9, 0);
  setLayout(layout);

  m_label = new QLabel();
  m_label->setObjectName("H2");
  m_label->setWordWrap(true);
  if (!url.isEmpty()) {
    m_label->setTextFormat(Qt::RichText);
    m_label->setOpenExternalLinks(true);
    m_label->setToolTip(url);
    m_label->setText(QStringLiteral(R"(<a href="%1" style="color: #0055cc; font-weight: bold;">%2</a>)").arg(url, label.toHtmlEscaped()));
  } else {
    m_label->setText(label);
  }
  layout->addWidget(m_label);
}

void GroundTemperatureEntry::setSelected(bool selected) {
  setProperty("style", selected ? "1" : "0");
  style()->unpolish(this);
  style()->polish(this);
}

void GroundTemperatureEntry::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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

// ─────────────────────────────────────────────────────────
// GroundTemperatureListView
// ─────────────────────────────────────────────────────────

GroundTemperatureListView::GroundTemperatureListView(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  m_bsEntry = new GroundTemperatureEntry(tr("Building Surface Ground Temperatures"), iddObjectDocUrl(QStringLiteral("OS:Site:GroundTemperature:BuildingSurface")), this);
  m_shEntry = new GroundTemperatureEntry(tr("Shallow Ground Temperatures"), iddObjectDocUrl(QStringLiteral("OS:Site:GroundTemperature:Shallow")), this);
  m_deepEntry = new GroundTemperatureEntry(tr("Deep Ground Temperatures"), iddObjectDocUrl(QStringLiteral("OS:Site:GroundTemperature:Deep")), this);
  m_fcEntry = new GroundTemperatureEntry(tr("FCfactorMethod Ground Temperatures"), iddObjectDocUrl(QStringLiteral("OS:Site:GroundTemperature:FCfactorMethod")), this);
  m_waterMainsEntry = new GroundTemperatureEntry(tr("Water Mains Temperature"), iddObjectDocUrl(QStringLiteral("OS:Site:WaterMainsTemperature")), this);

  connect(m_bsEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onBuildingSurfaceClicked);
  connect(m_shEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onShallowClicked);
  connect(m_deepEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onDeepClicked);
  connect(m_fcEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onFCfactorMethodClicked);
  connect(m_waterMainsEntry, &GroundTemperatureEntry::clicked, this, &GroundTemperatureListView::onWaterMainsClicked);

  layout->addWidget(m_bsEntry);
  layout->addWidget(m_shEntry);
  layout->addWidget(m_deepEntry);
  layout->addWidget(m_fcEntry);
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
  m_fcEntry->setSelected(false);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::BuildingSurface);
}

void GroundTemperatureListView::onShallowClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(true);
  m_deepEntry->setSelected(false);
  m_fcEntry->setSelected(false);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::Shallow);
}

void GroundTemperatureListView::onDeepClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(true);
  m_fcEntry->setSelected(false);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::Deep);
}

void GroundTemperatureListView::onFCfactorMethodClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(false);
  m_fcEntry->setSelected(true);
  m_waterMainsEntry->setSelected(false);
  emit typeSelected(GroundTempType::FCfactorMethod);
}

void GroundTemperatureListView::onWaterMainsClicked() {
  m_bsEntry->setSelected(false);
  m_shEntry->setSelected(false);
  m_deepEntry->setSelected(false);
  m_fcEntry->setSelected(false);
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
  m_label->setTextFormat(Qt::RichText);
  layout->addWidget(m_label);

  m_addButton = new QPushButton(tr("Add"));
  m_addButton->setObjectName("StandardBlueButton");
  m_addButton->setMinimumWidth(100);
  layout->addWidget(m_addButton, 0, Qt::AlignLeft);

  m_epwInfoLabel = new QLabel();
  m_epwInfoLabel->setWordWrap(true);
  m_epwInfoLabel->setTextFormat(Qt::RichText);
  m_epwInfoLabel->hide();
  layout->addWidget(m_epwInfoLabel);

  m_importFromEPWButton = new QPushButton(tr("Import from EPW"));
  m_importFromEPWButton->setObjectName("StandardGrayButton");
  m_importFromEPWButton->setEnabled(false);
  layout->addWidget(m_importFromEPWButton, 0, Qt::AlignLeft);

  layout->addStretch();

  connect(m_addButton, &QPushButton::clicked, this, [this]() { emit addClicked(m_type); });
}

QString typeNameForGroundTempType(GroundTempType type) {
  switch (type) {
    case GroundTempType::BuildingSurface:
      return "Site:GroundTemperature:<span style=\"color: #1C7BBF;\">BuildingSurface</span>";
    case GroundTempType::Shallow:
      return "Site:GroundTemperature:<span style=\"color: #1C7BBF;\">Shallow</span>";
    case GroundTempType::Deep:
      return "Site:GroundTemperature:<span style=\"color: #1C7BBF;\">Deep</span>";
    case GroundTempType::FCfactorMethod:
      return "Site:GroundTemperature:<span style=\"color: #1C7BBF;\">FCfactorMethod</span>";
    case GroundTempType::WaterMains:
      return "Site:WaterMainsTemperature";
    default:
      // raise
      throw std::runtime_error("Invalid GroundTempType");
  }
}

void GroundTemperatureNotPresentView::setType(GroundTempType type, model::Model model) {
  m_type = type;
  m_model = std::move(model);

  QString typeName = typeNameForGroundTempType(type);

  disconnect(m_importFromEPWButton, &QPushButton::clicked, nullptr, nullptr);
  m_importFromEPWButton->setEnabled(false);

  m_label->setText(tr("<p>The <b>%1</b> Unique ModelObject is not present in this model.</p>"
                      "<p>Click Add to instantiate it.</p>")
                     .arg(typeName));

  if (type == GroundTempType::BuildingSurface || type == GroundTempType::WaterMains) {
    m_epwInfoLabel->hide();
    m_importFromEPWButton->setVisible(false);
    return;
  }

  m_importFromEPWButton->setVisible(true);

  boost::optional<model::WeatherFile> weatherFile_ = m_model.weatherFile();
  if (!weatherFile_) {
    m_epwInfoLabel->setText(tr("No weather file is associated with the model, so the object will be added with default values."));
    m_epwInfoLabel->show();
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
    m_epwInfoLabel->setText(tr("While a weather file is associated with the model, could not locate the underlying EpwFile, "
                               "so the object will be added with default values."));
    m_epwInfoLabel->show();
    return;
  }

  std::vector<EpwGroundTemperatureDepth> ground_temps = epwFile_->groundTemperatureDepths();
  if (ground_temps.empty()) {
    m_epwInfoLabel->setText(tr("The weather file does not contain any ground temperature data, so the object will be added with default values."));
    m_epwInfoLabel->show();
    return;
  }

  const double target_depth = (type == GroundTempType::Deep) ? 4.0 : 0.5;  // Shallow and FCfactorMethod both use 0.5m
  // Now try to find the target_depth in the epw data, allowing for some tolerance since the epw spec doesn't require exact depths
  const double tolerance = 0.1;
  auto it = std::find_if(ground_temps.begin(), ground_temps.end(), [target_depth, tolerance](const EpwGroundTemperatureDepth& gtd) {
    return std::abs(gtd.groundTemperatureDepth() - target_depth) < tolerance;
  });
  if (it == ground_temps.end()) {
    m_epwInfoLabel->setText(tr("The weather file does not contain ground temperature data at the expected depth of %1 m, "
                               "so the object will be added with default values.")
                              .arg(QString::number(target_depth, 'f', 1)));
    m_epwInfoLabel->show();
    return;
  }

  // Capture the EpwGroundTemperatureDepth by value — 'it' is an iterator into a local vector
  // that goes out of scope when setType() returns, so dereferencing it later is UB.
  EpwGroundTemperatureDepth gtd = *it;

  m_epwInfoLabel->setText(tr("The weather file contains ground temperature data at a depth of "
                             "<b><span style=\"color: #1C7BBF;\">%1 m</span></b>, "
                             "so you can choose to import those values or add the object with default values.")
                            .arg(QString::number(gtd.groundTemperatureDepth(), 'f', 1)));
  m_epwInfoLabel->show();

  m_importFromEPWButton->setEnabled(true);
  // Connect button to a lambda that creates the object directly and fills up the value
  connect(m_importFromEPWButton, &QPushButton::clicked, this, [this, type, gtd]() {
    if (type == GroundTempType::Shallow) {
      auto ts = m_model.getUniqueModelObject<model::SiteGroundTemperatureShallow>();

      ts.setJanuarySurfaceGroundTemperature(gtd.janGroundTemperature());
      ts.setFebruarySurfaceGroundTemperature(gtd.febGroundTemperature());
      ts.setMarchSurfaceGroundTemperature(gtd.marGroundTemperature());
      ts.setAprilSurfaceGroundTemperature(gtd.aprGroundTemperature());
      ts.setMaySurfaceGroundTemperature(gtd.mayGroundTemperature());
      ts.setJuneSurfaceGroundTemperature(gtd.junGroundTemperature());
      ts.setJulySurfaceGroundTemperature(gtd.julGroundTemperature());
      ts.setAugustSurfaceGroundTemperature(gtd.augGroundTemperature());
      ts.setSeptemberSurfaceGroundTemperature(gtd.sepGroundTemperature());
      ts.setOctoberSurfaceGroundTemperature(gtd.octGroundTemperature());
      ts.setNovemberSurfaceGroundTemperature(gtd.novGroundTemperature());
      ts.setDecemberSurfaceGroundTemperature(gtd.decGroundTemperature());
    } else if (type == GroundTempType::FCfactorMethod) {
      auto tf = m_model.getUniqueModelObject<model::SiteGroundTemperatureFCfactorMethod>();

      tf.setJanuaryGroundTemperature(gtd.janGroundTemperature());
      tf.setFebruaryGroundTemperature(gtd.febGroundTemperature());
      tf.setMarchGroundTemperature(gtd.marGroundTemperature());
      tf.setAprilGroundTemperature(gtd.aprGroundTemperature());
      tf.setMayGroundTemperature(gtd.mayGroundTemperature());
      tf.setJuneGroundTemperature(gtd.junGroundTemperature());
      tf.setJulyGroundTemperature(gtd.julGroundTemperature());
      tf.setAugustGroundTemperature(gtd.augGroundTemperature());
      tf.setSeptemberGroundTemperature(gtd.sepGroundTemperature());
      tf.setOctoberGroundTemperature(gtd.octGroundTemperature());
      tf.setNovemberGroundTemperature(gtd.novGroundTemperature());
      tf.setDecemberGroundTemperature(gtd.decGroundTemperature());
    } else {  // GroundTempType::Deep
      auto td = m_model.getUniqueModelObject<model::SiteGroundTemperatureDeep>();

      td.setJanuaryDeepGroundTemperature(gtd.janGroundTemperature());
      td.setFebruaryDeepGroundTemperature(gtd.febGroundTemperature());
      td.setMarchDeepGroundTemperature(gtd.marGroundTemperature());
      td.setAprilDeepGroundTemperature(gtd.aprGroundTemperature());
      td.setMayDeepGroundTemperature(gtd.mayGroundTemperature());
      td.setJuneDeepGroundTemperature(gtd.junGroundTemperature());
      td.setJulyDeepGroundTemperature(gtd.julGroundTemperature());
      td.setAugustDeepGroundTemperature(gtd.augGroundTemperature());
      td.setSeptemberDeepGroundTemperature(gtd.sepGroundTemperature());
      td.setOctoberDeepGroundTemperature(gtd.octGroundTemperature());
      td.setNovemberDeepGroundTemperature(gtd.novGroundTemperature());
      td.setDecemberDeepGroundTemperature(gtd.decGroundTemperature());
    }
    emit addClicked(type);
  });
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

  m_selectorButtons = new OSItemSelectorButtons();
  m_selectorButtons->hideDropZone();
  m_selectorButtons->showAddButton();
  m_selectorButtons->disableAddButton();
  m_selectorButtons->hideCopyButton();
  m_selectorButtons->hidePurgeButton();
  m_selectorButtons->showRemoveButton();
  m_selectorButtons->disableRemoveButton();
  leftLayout->addWidget(m_selectorButtons);

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

  m_fcView = new SiteGroundTemperatureFCfactorMethodWidget(isIP);
  m_rightStack->addWidget(m_fcView);  // index 5

  connect(m_listView, &GroundTemperatureListView::typeSelected, this, &GroundTemperatureView::onTypeSelected);
  connect(m_notPresentView, &GroundTemperatureNotPresentView::addClicked, this, &GroundTemperatureView::onObjectCreated);
  connect(m_selectorButtons, &OSItemSelectorButtons::addClicked, this, [this]() { onObjectCreated(m_currentType); });
  connect(m_selectorButtons, &OSItemSelectorButtons::removeClicked, this, &GroundTemperatureView::onRemoveClicked);

  // Forward unit toggle to sub-views (signal-to-signal)
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_bsView, &SiteGroundTemperatureBuildingSurfaceWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_shView, &SiteGroundTemperatureShallowWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_deepView, &SiteGroundTemperatureDeepWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_fcView, &SiteGroundTemperatureFCfactorMethodWidget::toggleUnitsClicked);
  connect(this, &GroundTemperatureView::toggleUnitsClicked, m_waterMainsView, &SiteWaterMainsTemperatureWidget::toggleUnitsClicked);

  // Auto-select first entry after the event loop starts
  QTimer::singleShot(0, this, [this]() { m_listView->selectFirst(); });
}

void GroundTemperatureView::onTypeSelected(GroundTempType type) {
  m_currentType = type;
  bool objectExists = false;

  if (type == GroundTempType::BuildingSurface) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
    if (opt) {
      objectExists = true;
      m_bsView->attach(*opt);
      m_rightStack->setCurrentIndex(1);
    } else {
      m_notPresentView->setType(type, m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else if (type == GroundTempType::Shallow) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureShallow>();
    if (opt) {
      objectExists = true;
      m_shView->attach(*opt);
      m_rightStack->setCurrentIndex(2);
    } else {
      m_notPresentView->setType(type, m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else if (type == GroundTempType::Deep) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureDeep>();
    if (opt) {
      objectExists = true;
      m_deepView->attach(*opt);
      m_rightStack->setCurrentIndex(3);
    } else {
      m_notPresentView->setType(type, m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else if (type == GroundTempType::FCfactorMethod) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureFCfactorMethod>();
    if (opt) {
      objectExists = true;
      m_fcView->attach(*opt);
      m_rightStack->setCurrentIndex(5);
    } else {
      m_notPresentView->setType(type, m_model);
      m_rightStack->setCurrentIndex(0);
    }
  } else {
    auto opt = m_model.siteWaterMainsTemperature();
    if (opt) {
      objectExists = true;
      m_waterMainsView->attach(*opt);
      m_rightStack->setCurrentIndex(4);
    } else {
      m_notPresentView->setType(type, m_model);
      m_rightStack->setCurrentIndex(0);
    }
  }

  if (objectExists) {
    m_selectorButtons->enableRemoveButton();
    m_selectorButtons->disableAddButton();
  } else {
    m_selectorButtons->disableRemoveButton();
    m_selectorButtons->enableAddButton();
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
  } else if (type == GroundTempType::FCfactorMethod) {
    auto obj = m_model.getUniqueModelObject<model::SiteGroundTemperatureFCfactorMethod>();
    m_fcView->attach(obj);
    m_rightStack->setCurrentIndex(5);
  } else {
    auto obj = m_model.getUniqueModelObject<model::SiteWaterMainsTemperature>();
    m_waterMainsView->attach(obj);
    m_rightStack->setCurrentIndex(4);
  }
  m_selectorButtons->enableRemoveButton();
  m_selectorButtons->disableAddButton();
}

void GroundTemperatureView::onRemoveClicked() {
  if (m_currentType == GroundTempType::BuildingSurface) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
    if (opt) {
      m_bsView->detach();
      opt->remove();
    }
    m_notPresentView->setType(m_currentType, m_model);
  } else if (m_currentType == GroundTempType::Shallow) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureShallow>();
    if (opt) {
      m_shView->detach();
      opt->remove();
    }
    m_notPresentView->setType(m_currentType, m_model);
  } else if (m_currentType == GroundTempType::Deep) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureDeep>();
    if (opt) {
      m_deepView->detach();
      opt->remove();
    }
    m_notPresentView->setType(m_currentType, m_model);
  } else if (m_currentType == GroundTempType::FCfactorMethod) {
    auto opt = m_model.getOptionalUniqueModelObject<model::SiteGroundTemperatureFCfactorMethod>();
    if (opt) {
      m_fcView->detach();
      opt->remove();
    }
    m_notPresentView->setType(m_currentType, m_model);
  } else {
    auto opt = m_model.siteWaterMainsTemperature();
    if (opt) {
      m_waterMainsView->detach();
      opt->remove();
    }
    m_notPresentView->setType(m_currentType, m_model);
  }
  m_rightStack->setCurrentIndex(0);
  m_selectorButtons->disableRemoveButton();
  m_selectorButtons->enableAddButton();
}

}  // namespace openstudio
