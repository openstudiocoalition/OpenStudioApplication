/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ElectricLoadCenterDistributionTabController.hpp"
#include "ElectricLoadCenterDistributionGraphicsItems.hpp"
#include "ElectricLoadCenterDistributionScene.hpp"
#include "ElectricLoadCenterDistributionTabView.hpp"
#include "MainRightColumnController.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"
#include "OSItem.hpp"

#include "../shared_gui_components/GraphicsItems.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ElectricLoadCenterDistribution.hpp>
#include <openstudio/model/ElectricLoadCenterDistribution_Impl.hpp>
#include <openstudio/model/ElectricLoadCenterTransformer.hpp>
#include <openstudio/model/ElectricLoadCenterTransformer_Impl.hpp>
#include <openstudio/model/Generator.hpp>
#include <openstudio/model/Generator_Impl.hpp>
#include <openstudio/model/Inverter.hpp>
#include <openstudio/model/Inverter_Impl.hpp>
#include <openstudio/model/ElectricalStorage_Impl.hpp>
#include <openstudio/model/ElectricalStorage.hpp>
#include <openstudio/model/ElectricLoadCenterStorageConverter.hpp>
#include <openstudio/model/ElectricLoadCenterStorageConverter_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/core/StringStreamLogSink.hpp>

#include <algorithm>

#include <QComboBox>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QPixmap>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTimer>

namespace openstudio {

namespace {
// Add a line + open-V arrowhead at `to`, matching the drawArrow() style used in paint().
void addArrowLine(QGraphicsScene* scene, qreal x1, qreal y1, qreal x2, qreal y2, const QPen& pen) {
  scene->addLine(x1, y1, x2, y2, pen);
  QPointF dir(x2 - x1, y2 - y1);
  const double len = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
  if (len < 1.0) {
    return;
  }
  dir /= len;
  const QPointF perp(-dir.y(), dir.x());
  const QPointF tip(x2, y2);
  scene->addLine(tip.x(), tip.y(), tip.x() - dir.x() * 8.0 + perp.x() * 4.0, tip.y() - dir.y() * 8.0 + perp.y() * 4.0, pen);
  scene->addLine(tip.x(), tip.y(), tip.x() - dir.x() * 8.0 - perp.x() * 4.0, tip.y() - dir.y() * 8.0 - perp.y() * 4.0, pen);
}

// Returns true if the generator produces DC power (Photovoltaic / PVWatts).
bool generatorIsDC(const model::Generator& gen) {
  const std::string typeName = gen.iddObjectType().valueName();
  return typeName == "OS_Generator_Photovoltaic" || typeName == "OS_Generator_PVWatts";
}

// Returns true if the buss type requires DC generators.
bool bussTypeExpectsDC(const std::string& bussType) {
  return bussType == "DirectCurrentWithInverter" || bussType == "DirectCurrentWithInverterDCStorage"
         || bussType == "DirectCurrentWithInverterACStorage";
}
}  // namespace

// ─── ElectricLoadCenterDistributionTabController ──────────────────────────────

ElectricLoadCenterDistributionTabController::ElectricLoadCenterDistributionTabController(const model::Model& model)
  : MainTabController(new ELCDTabView()),
    m_model(model),
    m_elcdView(new ELCDView()),
    m_elcdGridView(new GridLayoutItem()),
    m_gridScene(new QGraphicsScene()) {

  connect(m_elcdView->oneLevelUpButton, &QPushButton::clicked, this, &ElectricLoadCenterDistributionTabController::zoomOutToGridView);

  // Connect detail view header signals
  connect(m_elcdView, &ELCDView::bussTypeChangeRequested, this, &ElectricLoadCenterDistributionTabController::onBussTypeChangeRequested);
  connect(m_elcdView, &ELCDView::genOpSchemeChangeRequested, this, &ElectricLoadCenterDistributionTabController::onGenOpSchemeChangeRequested);
  connect(m_elcdView, &ELCDView::validateRequested, this, &ElectricLoadCenterDistributionTabController::onValidateRequested);

  m_elcdGridView->setCellSize(ELCDSystemMiniView::cellSize());
  m_elcdGridView->setMargin(10);

  m_listController = QSharedPointer<ELCDListController>(new ELCDListController(this));
  m_elcdGridView->setListController(m_listController);
  m_elcdGridView->setDelegate(QSharedPointer<ELCDItemDelegate>(new ELCDItemDelegate()));
  m_elcdGridView->setColumns(1);

  // Refresh layout (panel height + connector lines) when ELCD list changes
  connect(m_listController.data(), &OSListController::itemInserted, this, &ElectricLoadCenterDistributionTabController::refresh);
  connect(m_listController.data(), &OSListController::itemRemoved, this, &ElectricLoadCenterDistributionTabController::refresh);

  // ── Overview scene layout ──────────────────────────────────────────────────
  // Column positions:
  //   x=0:   power_grid icon          (width=120, icon 100×100 centred)
  //   x=120: ELCDUtilityGridPanel     (width=190)
  //   x=350: ELCDMainPanelItem        (width=160, dynamic height)
  //   x=550: ELCD card grid (single column)
  constexpr int kIconColWidth = 220;
  constexpr int kIconSize = 200;
  constexpr int kUtilityPanelX = kIconColWidth;                                         // 120
  constexpr int kMainPanelX = kUtilityPanelX + ELCDUtilityGridPanel::kPanelWidth + 40;  // 350
  constexpr int kElcdGridX = kMainPanelX + ELCDMainPanelItem::kPanelWidth + 40;         // 550

  m_kMainPanelX = kMainPanelX;
  m_kElcdGridX = kElcdGridX;

  // Power grid icon (leftmost element)
  const QPixmap gridPixmap(":/images/power_grid.png");
  if (!gridPixmap.isNull()) {
    const QPixmap scaled = gridPixmap.scaled(kIconSize, kIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    auto* iconItem = m_gridScene->addPixmap(scaled);
    iconItem->setPos((kIconColWidth - scaled.width()) / 2, (ELCDUtilityGridPanel::kPanelHeight - scaled.height()) / 2);

    auto* label = m_gridScene->addSimpleText("Utility Grid");
    QFont labelFont;
    labelFont.setPointSize(8);
    label->setFont(labelFont);
    label->setBrush(QColor(60, 60, 80));
    const int labelX = (kIconColWidth - static_cast<int>(label->boundingRect().width())) / 2;
    label->setPos(labelX, (ELCDUtilityGridPanel::kPanelHeight - scaled.height()) / 2 + scaled.height() + 4);
  }

  m_utilityGridPanel = new ELCDUtilityGridPanel();
  m_gridScene->addItem(m_utilityGridPanel);
  m_utilityGridPanel->setPos(kUtilityPanelX, 0);
  connect(m_utilityGridPanel->powerInDropZone, &OSDropZoneItem::componentDropped, this,
          &ElectricLoadCenterDistributionTabController::onPowerInTransformerDrop);
  connect(m_utilityGridPanel->powerOutDropZone, &OSDropZoneItem::componentDropped, this,
          &ElectricLoadCenterDistributionTabController::onPowerOutTransformerDrop);
  connect(m_utilityGridPanel->powerInDropZone, &OSDropZoneItem::mouseClicked, this,
          &ElectricLoadCenterDistributionTabController::onPowerInTransformerClick);
  connect(m_utilityGridPanel->powerOutDropZone, &OSDropZoneItem::mouseClicked, this,
          &ElectricLoadCenterDistributionTabController::onPowerOutTransformerClick);
  connect(m_utilityGridPanel->powerInDropZone, &ELCDTransformerDropZoneView::removeClicked, this,
          &ElectricLoadCenterDistributionTabController::onPowerInTransformerRemove);
  connect(m_utilityGridPanel->powerOutDropZone, &ELCDTransformerDropZoneView::removeClicked, this,
          &ElectricLoadCenterDistributionTabController::onPowerOutTransformerRemove);

  m_mainPanelItem = new ELCDMainPanelItem();
  m_gridScene->addItem(m_mainPanelItem);
  m_mainPanelItem->setPos(kMainPanelX, 0);

  m_elcdGridView->setPos(kElcdGridX, 0);
  m_gridScene->addItem(m_elcdGridView);

  // Static connecting lines: icon ↔ utility panel ↔ main panel
  // Arrow tips are at the final destination; panel-internal stubs handle intermediate arrowheads.
  const QPen connectorPen(QColor(70, 130, 180), 1.5);
  // Icon → PowerIn transformer: plain line (arrowhead drawn inside panel at drop zone)
  m_gridScene->addLine(kIconColWidth - 20, ELCDUtilityGridPanel::kPowerInCentreY, kUtilityPanelX, ELCDUtilityGridPanel::kPowerInCentreY,
                       connectorPen);
  // PowerOut transformer → icon: arrowhead AT icon right edge
  addArrowLine(m_gridScene.data(), kUtilityPanelX, ELCDUtilityGridPanel::kPowerOutCentreY, kIconColWidth - 20, ELCDUtilityGridPanel::kPowerOutCentreY,
               connectorPen);
  // PowerIn transformer → Main Panel: arrowhead AT Main Panel left edge
  addArrowLine(m_gridScene.data(), kUtilityPanelX + ELCDUtilityGridPanel::kPanelWidth, ELCDUtilityGridPanel::kPowerInCentreY, kMainPanelX,
               ELCDUtilityGridPanel::kPowerInCentreY, connectorPen);
  // Main Panel → PowerOut transformer: plain line (arrowhead drawn inside panel at drop zone)
  m_gridScene->addLine(kMainPanelX, ELCDUtilityGridPanel::kPowerOutCentreY, kUtilityPanelX + ELCDUtilityGridPanel::kPanelWidth,
                       ELCDUtilityGridPanel::kPowerOutCentreY, connectorPen);
  // Dynamic ELCD subpanel connections are drawn in refreshNow()

  this->mainContentWidget()->addTabWidget(m_elcdView);

  zoomOutToGridView();
}

ElectricLoadCenterDistributionTabController::~ElectricLoadCenterDistributionTabController() {
  delete m_elcdView;
  delete m_detailScene;
}

void ElectricLoadCenterDistributionTabController::zoomInOnELCD(const Handle& handle) {
  auto elcd = m_model.getModelObject<model::ElectricLoadCenterDistribution>(handle);
  if (!elcd) {
    return;
  }

  m_currentELCD = elcd;

  m_elcdView->nameLabel->setText(QString::fromStdString(elcd->nameString()));
  m_elcdView->header->show();
  m_elcdView->graphicsView->setAlignment(Qt::AlignCenter);
  m_elcdView->resetZoom();

  // Update combos (block signals to avoid triggering model mutations)
  {
    QSignalBlocker blocker1(m_elcdView->bussTypeCombo);
    QSignalBlocker blocker2(m_elcdView->genOpSchemeCombo);
    const int btIdx = m_elcdView->bussTypeCombo->findText(QString::fromStdString(elcd->electricalBussType()));
    if (btIdx >= 0) {
      m_elcdView->bussTypeCombo->setCurrentIndex(btIdx);
    }
    const int gsIdx = m_elcdView->genOpSchemeCombo->findText(QString::fromStdString(elcd->generatorOperationSchemeType()));
    if (gsIdx >= 0) {
      m_elcdView->genOpSchemeCombo->setCurrentIndex(gsIdx);
    }
  }

  buildDetailScene(*elcd);
}

void ElectricLoadCenterDistributionTabController::zoomOutToGridView() {
  m_currentELCD = boost::none;

  model::OptionalModelObject mo;
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }

  m_listController->reset();

  m_elcdView->graphicsView->setScene(m_gridScene.data());
  m_elcdView->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_elcdView->resetZoom();
  m_elcdView->header->hide();

  refresh();
}

void ElectricLoadCenterDistributionTabController::refresh() {
  m_dirty = true;
  QTimer::singleShot(0, this, &ElectricLoadCenterDistributionTabController::refreshNow);
}

void ElectricLoadCenterDistributionTabController::refreshNow() {
  if (!m_dirty) {
    return;
  }
  m_dirty = false;

  // If in detail view, rebuild the detail scene
  if (m_currentELCD) {
    auto elcd_ = m_model.getModelObject<model::ElectricLoadCenterDistribution>(m_currentELCD->handle());
    if (elcd_) {
      m_currentELCD = elcd_;
      buildDetailScene(*elcd_);
      QSignalBlocker blocker1(m_elcdView->bussTypeCombo);
      QSignalBlocker blocker2(m_elcdView->genOpSchemeCombo);
      const int btIdx = m_elcdView->bussTypeCombo->findText(QString::fromStdString(elcd_->electricalBussType()));
      if (btIdx >= 0) {
        m_elcdView->bussTypeCombo->setCurrentIndex(btIdx);
      }
      const int gsIdx = m_elcdView->genOpSchemeCombo->findText(QString::fromStdString(elcd_->generatorOperationSchemeType()));
      if (gsIdx >= 0) {
        m_elcdView->genOpSchemeCombo->setCurrentIndex(gsIdx);
      }
    }
    return;  // don't update overview items when in detail view
  }

  // Clear old dynamic connector items (lines + arrowheads)
  for (QGraphicsItem* item : m_elcdConnectorItems) {
    if (item) {
      m_gridScene->removeItem(item);
      delete item;
    }
  }
  m_elcdConnectorItems.clear();

  // Recompute Main Panel height to match the ELCD grid
  // GridLayoutItem with 1 column: height = count * (cellH + spacing) - spacing + 2*margin
  // cellSize = {300, 180}, spacing = margin = 10
  // => height = count * 190 + 10
  const auto elcdList = m_model.getConcreteModelObjects<model::ElectricLoadCenterDistribution>();
  const int elcdCount = static_cast<int>(elcdList.size());
  const int itemCount = elcdCount + 1;  // +1 for drop zone
  constexpr int kCellH = 180;           // ELCDSystemMiniView::cellSize().height()
  constexpr int kSpacing = 10;
  constexpr int kMargin = 10;
  const int gridHeight = itemCount * (kCellH + kSpacing) - kSpacing + 2 * kMargin;
  const int panelHeight = std::max(gridHeight, ELCDUtilityGridPanel::kPanelHeight);
  if (m_mainPanelItem) {
    m_mainPanelItem->setHeight(panelHeight);
  }

  // Update transformer drop zone display
  if (m_utilityGridPanel) {
    boost::optional<model::ElectricLoadCenterTransformer> powerIn_;
    boost::optional<model::ElectricLoadCenterTransformer> powerOut_;
    int powerInCount = 0;
    int powerOutCount = 0;
    for (const auto& t : m_model.getConcreteModelObjects<model::ElectricLoadCenterTransformer>()) {
      if (openstudio::istringEqual(t.transformerUsage(), "PowerInFromGrid")) {
        ++powerInCount;
        if (!powerIn_) {
          powerIn_ = t;
        }
      } else if (openstudio::istringEqual(t.transformerUsage(), "PowerOutToGrid")) {
        ++powerOutCount;
        if (!powerOut_) {
          powerOut_ = t;
        }
      }
    }
    if (powerInCount > 1) {
      QMessageBox::warning(nullptr, "Multiple PowerInFromGrid Transformers",
                           QString("Found %1 transformers with usage 'PowerInFromGrid'. There should be at most one. "
                                   "Displaying the first one found.")
                             .arg(powerInCount));
    }
    if (powerOutCount > 1) {
      QMessageBox::warning(nullptr, "Multiple PowerOutToGrid Transformers",
                           QString("Found %1 transformers with usage 'PowerOutToGrid'. There should be at most one. "
                                   "Displaying the first one found.")
                             .arg(powerOutCount));
    }
    m_utilityGridPanel->powerInDropZone->setFilled(powerIn_.has_value(), powerIn_ ? QString::fromStdString(powerIn_->nameString()) : QString{});
    m_utilityGridPanel->powerOutDropZone->setFilled(powerOut_.has_value(), powerOut_ ? QString::fromStdString(powerOut_->nameString()) : QString{});
  }

  // Draw one horizontal subpanel connector per ELCD (arrowhead pointing left at Main Panel)
  // Center-y of ELCD i = kMargin + i*(kCellH+kSpacing) + kCellH/2 = 100 + i*190
  const QPen connectorPen(QColor(70, 130, 180), 1.5);
  for (int i = 0; i < elcdCount; ++i) {
    const int connY = kMargin + i * (kCellH + kSpacing) + (kCellH / 2);
    // Line from ELCD grid left edge → Main Panel right edge, tip at Main Panel
    // Line from ELCD grid left edge → Main Panel right edge, open-V tip at Main Panel
    const qreal tx = m_kMainPanelX + ELCDMainPanelItem::kPanelWidth;
    m_elcdConnectorItems.append(m_gridScene->addLine(m_kElcdGridX, connY, tx, connY, connectorPen));
    m_elcdConnectorItems.append(m_gridScene->addLine(tx, connY, tx + 8.0, connY - 4.0, connectorPen));
    m_elcdConnectorItems.append(m_gridScene->addLine(tx, connY, tx + 8.0, connY + 4.0, connectorPen));
  }
}

// ─── buildDetailScene ─────────────────────────────────────────────────────────

void ElectricLoadCenterDistributionTabController::buildDetailScene(const model::ElectricLoadCenterDistribution& elcd) {
  // ── Layout constants ───────────────────────────────────────────────────────
  // Energy flows right→left (Generators on right, LCPC/Main Panel on left).
  constexpr int kPad = 20;
  constexpr int kArrow = 30;
  constexpr int kSlotW = 140;
  constexpr int kSlotH = 70;
  constexpr int kSlotStep = kSlotW + kArrow;  // 170
  constexpr int kSONodeW = 90;                // Storage Operation visual node width
  constexpr int kBelowGap = 20;               // gap between main-line slot bottom and Storage slot top
  constexpr int kLaneOff = 50;                // vertical offset from centerY for two-lane (DCwithDCStorage)

  // ── 1. Clear & recreate scene ──────────────────────────────────────────────
  if (m_detailScene) {
    m_detailScene->clear();
    delete m_detailScene;
  }
  m_detailScene = new ELCDScene();
  m_elcdView->graphicsView->setScene(m_detailScene);

  const std::string bussType = elcd.electricalBussType();
  const bool isDC = bussTypeExpectsDC(bussType);
  const QPen arrowPen(QColor(70, 130, 180), 1.5);

  // ── 2. Slot pointers ───────────────────────────────────────────────────────
  ELCDComponentSlotView* lcpcSlot = nullptr;
  ELCDComponentSlotView* inverterSlot = nullptr;
  ELCDComponentSlotView* storageSlot = nullptr;
  ELCDComponentSlotView* converterSlot = nullptr;

  // ── 3. Generators panel (positioned per-case below) ────────────────────────
  auto* generatorsView = new ELCDGeneratorsView();
  m_detailScene->addItem(generatorsView);
  generatorsView->setGeneratorLabel(isDC ? "Generators (DC)" : "Generators (AC)");
  for (const auto& gen : elcd.generators()) {
    generatorsView->addGenerator(QString::fromStdString(gen.nameString()), gen.handle());
  }
  connect(generatorsView->dropZone, &OSDropZoneItem::componentDropped, this, &ElectricLoadCenterDistributionTabController::onDetailGeneratorDrop);
  connect(generatorsView, &ELCDGeneratorsView::generatorRemoveClicked, this, &ElectricLoadCenterDistributionTabController::onDetailGeneratorRemove);
  connect(generatorsView, &ELCDGeneratorsView::generatorInspectClicked, this, &ElectricLoadCenterDistributionTabController::onDetailGeneratorClick);

  // ── Local helpers ──────────────────────────────────────────────────────────

  // Create a slot view and add it to the scene
  auto makeSlot = [&](const QString& label, const QString& icon) -> ELCDComponentSlotView* {
    auto* slot = new ELCDComponentSlotView(label, icon);
    m_detailScene->addItem(slot);
    return slot;
  };

  // Add a "Storage Operation" dashed box centered at (cx, cy) with given height
  auto addStorageOpNode = [&](int cx, int cy, int nodeH) {
    QPen dp(QColor(100, 130, 180), 1.2, Qt::DashLine);
    m_detailScene->addRect(cx - kSONodeW / 2, cy - nodeH / 2, kSONodeW, nodeH, dp);
    auto* t = m_detailScene->addText("Storage\nOperation");
    QFont f;
    f.setPointSize(8);
    t->setFont(f);
    t->setDefaultTextColor(QColor(60, 80, 120));
    t->setPos(cx - t->boundingRect().width() / 2, cy - t->boundingRect().height() / 2);
  };

  // Bidir horizontal arrow pair (slightly offset in Y to show both directions)
  auto addBidirH = [&](int x1, int y, int x2) {
    addArrowLine(m_detailScene, x2, y - 4, x1, y - 4, arrowPen);  // pointing left
    addArrowLine(m_detailScene, x1, y + 4, x2, y + 4, arrowPen);  // pointing right
  };

  // Bidir vertical arrow pair (slightly offset in X)
  auto addBidirV = [&](int x, int y1, int y2) {
    addArrowLine(m_detailScene, x - 4, y2, x - 4, y1, arrowPen);  // pointing up
    addArrowLine(m_detailScene, x + 4, y1, x + 4, y2, arrowPen);  // pointing down
  };

  // "← Main Panel" label below the LCPC slot; bidir = show ←→
  auto addMainPanelLabel = [&](int slotX, int slotBottomY, bool bidir) {
    auto* lbl = m_detailScene->addSimpleText(bidir ? "\u2190\u2192 Main Panel" : "\u2190 Main Panel");
    QFont f;
    f.setPointSize(9);
    lbl->setFont(f);
    lbl->setBrush(QColor(60, 80, 120));
    lbl->setPos(slotX, slotBottomY + 4);
  };

  // ── 4. Per-buss-type layout ────────────────────────────────────────────────

  if (bussType == "AlternatingCurrent") {
    // [LCPC] ◄── [Gen]
    const int kGenX = kPad + kSlotStep;
    generatorsView->setPos(kGenX, kPad);
    const int centerY = kPad + generatorsView->totalHeight() / 2;

    lcpcSlot = makeSlot("LCPC Transformer", ":/images/mini_icons/transformer.png");
    lcpcSlot->setPos(kPad, centerY - kSlotH / 2);

    addArrowLine(m_detailScene, kGenX, centerY, kPad + kSlotW, centerY, arrowPen);
    addMainPanelLabel(kPad, centerY + kSlotH / 2, /*bidir=*/false);

  } else if (bussType == "AlternatingCurrentWithStorage") {
    // [LCPC] ◄► [SO] ◄── [Gen]
    //              ↕
    //          [Storage]
    constexpr int kSOH = 60;
    const int kSOCX = kPad + kSlotStep + kArrow + kSONodeW / 2;
    const int kGenX = kSOCX + kSONodeW / 2 + kArrow;
    generatorsView->setPos(kGenX, kPad);
    const int centerY = kPad + generatorsView->totalHeight() / 2;

    lcpcSlot = makeSlot("LCPC Transformer", ":/images/mini_icons/transformer.png");
    lcpcSlot->setPos(kPad, centerY - kSlotH / 2);

    addStorageOpNode(kSOCX, centerY, kSOH);

    const int storageTopY = centerY + kSOH / 2 + kBelowGap;
    storageSlot = makeSlot("Drop Storage", {});
    storageSlot->setPos(kSOCX - kSlotW / 2, storageTopY);

    addBidirH(kPad + kSlotW, centerY, kSOCX - kSONodeW / 2);
    addArrowLine(m_detailScene, kGenX, centerY, kSOCX + kSONodeW / 2, centerY, arrowPen);
    addBidirV(kSOCX, centerY + kSOH / 2, storageTopY);
    addMainPanelLabel(kPad, centerY + kSlotH / 2, /*bidir=*/true);

  } else if (bussType == "DirectCurrentWithInverter") {
    // [LCPC] ◄── [Inverter] ◄── [Gen]
    const int kInvX = kPad + kSlotStep;
    const int kGenX = kInvX + kSlotStep;
    generatorsView->setPos(kGenX, kPad);
    const int centerY = kPad + generatorsView->totalHeight() / 2;

    lcpcSlot = makeSlot("LCPC Transformer", ":/images/mini_icons/transformer.png");
    lcpcSlot->setPos(kPad, centerY - kSlotH / 2);

    inverterSlot = makeSlot("Drop Inverter", ":/images/mini_icons/ac_left_dc_right.png");
    inverterSlot->setPos(kInvX, centerY - kSlotH / 2);

    addArrowLine(m_detailScene, kInvX, centerY, kPad + kSlotW, centerY, arrowPen);
    addArrowLine(m_detailScene, kGenX, centerY, kInvX + kSlotW, centerY, arrowPen);
    addMainPanelLabel(kPad, centerY + kSlotH / 2, /*bidir=*/false);

  } else if (bussType == "DirectCurrentWithInverterDCStorage") {
    // Two-lane layout:
    //            <── [Inverter] <──┐
    // [LCPC] ────┤                [SO*] ◄── [Gen]
    //            └─> [Converter] >─┘
    //                              ↕
    //                          [Storage]
    const int kJuncLX = kPad + kSlotW;    // right edge of LCPC = left junction
    const int kLaneX = kJuncLX + kArrow;  // left edge of Inverter/Converter slots
    const int kJuncRX = kLaneX + kSlotW;  // right edge of Inverter/Converter = right junction
    const int kSOH = kLaneOff * 2 + 16;   // SO node height spans both lanes
    const int kSOCX = kJuncRX + kArrow + kSONodeW / 2;
    const int kGenX = kSOCX + kSONodeW / 2 + kArrow;

    generatorsView->setPos(kGenX, kPad);
    const int genH = generatorsView->totalHeight();
    const int centerY = kPad + std::max(genH / 2, kLaneOff + kSlotH / 2 + 10);
    // Re-center generators panel to align with centerY
    generatorsView->setPos(kGenX, centerY - genH / 2);

    const int invCY = centerY - kLaneOff;
    const int convCY = centerY + kLaneOff;
    const int storageTopY = std::max(convCY + kSlotH / 2, centerY + kSOH / 2) + kBelowGap;
    const int storageCX = kSOCX;

    lcpcSlot = makeSlot("LCPC Transformer", ":/images/mini_icons/transformer.png");
    lcpcSlot->setPos(kPad, centerY - kSlotH / 2);

    inverterSlot = makeSlot("Drop Inverter", ":/images/mini_icons/ac_left_dc_right.png");
    inverterSlot->setPos(kLaneX, invCY - kSlotH / 2);

    converterSlot = makeSlot("Drop Converter", ":/images/mini_icons/ac_left_dc_right.png");
    converterSlot->setPos(kLaneX, convCY - kSlotH / 2);

    storageSlot = makeSlot("Drop Storage", {});
    storageSlot->setPos(storageCX - kSlotW / 2, storageTopY);

    addStorageOpNode(kSOCX, centerY, kSOH);

    // Left vertical: LCPC right edge spans both lane center Y values
    m_detailScene->addLine(kJuncLX, invCY, kJuncLX, convCY, arrowPen);
    // Upper lane: left junction → Inverter (energy flows left, arrow tip at left junction)
    addArrowLine(m_detailScene, kLaneX, invCY, kJuncLX, invCY, arrowPen);

    // Lower lane: left junction LCPC -> Converter (unidirectional right, tip at Converter)
    addArrowLine(m_detailScene, kJuncLX, convCY, kLaneX, convCY, arrowPen);

    // Right vertical: Inverter/Converter right edges → SO node
    m_detailScene->addLine(kJuncRX, invCY, kJuncRX, convCY, arrowPen);
    addArrowLine(m_detailScene, kJuncRX, invCY, kSOCX - kSONodeW / 2, invCY, arrowPen);

    // NO, this is unidir converter -> SO addBidirH(kJuncRX, convCY, kSOCX - kSONodeW / 2);
    addArrowLine(m_detailScene, kJuncRX, convCY, kSOCX - kSONodeW / 2, convCY, arrowPen);

    // SO node → Generators (energy flows into SO from right)
    addArrowLine(m_detailScene, kGenX, centerY, kSOCX + kSONodeW / 2, centerY, arrowPen);

    // Storage below SO node (bidir vertical)
    addBidirV(storageCX, centerY + kSOH / 2, storageTopY);

    addMainPanelLabel(kPad, centerY + kSlotH / 2, /*bidir=*/true);

  } else {
    // DirectCurrentWithInverterACStorage (and fallback)
    // [LCPC] ◄► [SO] ◄── [Inverter] ◄── [Gen]
    //               ↕
    //           [Storage]
    constexpr int kSOH = 60;
    const int kSOCX = kPad + kSlotStep + kArrow + kSONodeW / 2;
    const int kInvX = kSOCX + kSONodeW / 2 + kArrow;
    const int kGenX = kInvX + kSlotStep;
    generatorsView->setPos(kGenX, kPad);
    const int centerY = kPad + generatorsView->totalHeight() / 2;

    lcpcSlot = makeSlot("LCPC Transformer", ":/images/mini_icons/transformer.png");
    lcpcSlot->setPos(kPad, centerY - kSlotH / 2);

    addStorageOpNode(kSOCX, centerY, kSOH);

    inverterSlot = makeSlot("Drop Inverter", ":/images/mini_icons/ac_left_dc_right.png");
    inverterSlot->setPos(kInvX, centerY - kSlotH / 2);

    const int storageTopY = centerY + kSOH / 2 + kBelowGap;
    storageSlot = makeSlot("Drop Storage", {});
    storageSlot->setPos(kSOCX - kSlotW / 2, storageTopY);

    addBidirH(kPad + kSlotW, centerY, kSOCX - kSONodeW / 2);
    addArrowLine(m_detailScene, kInvX, centerY, kSOCX + kSONodeW / 2, centerY, arrowPen);
    addArrowLine(m_detailScene, kGenX, centerY, kInvX + kSlotW, centerY, arrowPen);
    addBidirV(kSOCX, centerY + kSOH / 2, storageTopY);
    addMainPanelLabel(kPad, centerY + kSlotH / 2, /*bidir=*/true);
  }

  // ── 5. Populate slot states & wire signals ─────────────────────────────────
  if (lcpcSlot) {
    if (auto xfmr = elcd.transformer()) lcpcSlot->setFilled(true, QString::fromStdString(xfmr->nameString()));
    connect(lcpcSlot, &OSDropZoneItem::componentDropped, this, &ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerDrop);
    connect(lcpcSlot, &ELCDComponentSlotView::removeClicked, this, &ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerRemove);
    connect(lcpcSlot, &OSDropZoneItem::mouseClicked, this, &ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerClick);
  }
  if (inverterSlot) {
    if (auto inv = elcd.inverter()) inverterSlot->setFilled(true, QString::fromStdString(inv->nameString()));
    connect(inverterSlot, &OSDropZoneItem::componentDropped, this, &ElectricLoadCenterDistributionTabController::onDetailInverterDrop);
    connect(inverterSlot, &ELCDComponentSlotView::removeClicked, this, &ElectricLoadCenterDistributionTabController::onDetailInverterRemove);
    connect(inverterSlot, &OSDropZoneItem::mouseClicked, this, &ElectricLoadCenterDistributionTabController::onDetailInverterClick);
  }
  if (storageSlot) {
    if (auto sto = elcd.electricalStorage()) storageSlot->setFilled(true, QString::fromStdString(sto->nameString()));
    connect(storageSlot, &OSDropZoneItem::componentDropped, this, &ElectricLoadCenterDistributionTabController::onDetailStorageDrop);
    connect(storageSlot, &ELCDComponentSlotView::removeClicked, this, &ElectricLoadCenterDistributionTabController::onDetailStorageRemove);
    connect(storageSlot, &OSDropZoneItem::mouseClicked, this, &ElectricLoadCenterDistributionTabController::onDetailStorageClick);
  }
  if (converterSlot) {
    if (auto conv = elcd.storageConverter()) converterSlot->setFilled(true, QString::fromStdString(conv->nameString()));
    connect(converterSlot, &OSDropZoneItem::componentDropped, this, &ElectricLoadCenterDistributionTabController::onDetailConverterDrop);
    connect(converterSlot, &ELCDComponentSlotView::removeClicked, this, &ElectricLoadCenterDistributionTabController::onDetailConverterRemove);
    connect(converterSlot, &OSDropZoneItem::mouseClicked, this, &ElectricLoadCenterDistributionTabController::onDetailConverterClick);
  }
}

// ─── Transformer drop handlers ─────────────────────────────────────────────────

namespace {
// Drop a transformer into targetModel for the given slot usage.
// If from the component library, the object is cloned into targetModel first.
// If the transformer's usage differs from expectedUsage, the user is asked to confirm.
// On cancel (or failure), no model change is made.
void handleTransformerDrop(model::Model& targetModel, const OSItemId& itemId, const std::string& expectedUsage) {
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }

  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto srcTransformer_ = mo->optionalCast<model::ElectricLoadCenterTransformer>();
  if (!srcTransformer_) {
    return;
  }

  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::ElectricLoadCenterTransformer transformer =
    isFromLibrary ? srcTransformer_->clone(targetModel).cast<model::ElectricLoadCenterTransformer>() : *srcTransformer_;

  if (!openstudio::istringEqual(transformer.transformerUsage(), expectedUsage)) {
    const auto reply = QMessageBox::question(nullptr, "Change Transformer Usage",
                                             QString("Set this transformer's usage to '%1'?").arg(QString::fromStdString(expectedUsage)),
                                             QMessageBox::Yes | QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) {
      if (isFromLibrary) {
        transformer.remove();
      }
      return;
    }
    transformer.setTransformerUsage(expectedUsage);
  }
}
}  // namespace

void ElectricLoadCenterDistributionTabController::onPowerInTransformerDrop(const OSItemId& itemId) {
  handleTransformerDrop(m_model, itemId, "PowerInFromGrid");
  refresh();
}

void ElectricLoadCenterDistributionTabController::onPowerOutTransformerDrop(const OSItemId& itemId) {
  handleTransformerDrop(m_model, itemId, "PowerOutToGrid");
  refresh();
}

namespace {
boost::optional<model::ElectricLoadCenterTransformer> findTransformerByUsage(const model::Model& model, const std::string& usage) {
  for (const auto& t : model.getConcreteModelObjects<model::ElectricLoadCenterTransformer>()) {
    if (openstudio::istringEqual(t.transformerUsage(), usage)) {
      return t;
    }
  }
  return boost::none;
}
}  // namespace

void ElectricLoadCenterDistributionTabController::onPowerInTransformerClick() {
  auto transformer_ = findTransformerByUsage(m_model, "PowerInFromGrid");
  if (!transformer_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *transformer_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onPowerOutTransformerClick() {
  auto transformer_ = findTransformerByUsage(m_model, "PowerOutToGrid");
  if (!transformer_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *transformer_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onPowerInTransformerRemove() {
  auto transformer_ = findTransformerByUsage(m_model, "PowerInFromGrid");
  if (transformer_) {
    transformer_->remove();
  }
  refresh();
}

void ElectricLoadCenterDistributionTabController::onPowerOutTransformerRemove() {
  auto transformer_ = findTransformerByUsage(m_model, "PowerOutToGrid");
  if (transformer_) {
    transformer_->remove();
  }
  refresh();
}

// ─── Detail view slot implementations ─────────────────────────────────────────

void ElectricLoadCenterDistributionTabController::onBussTypeChangeRequested(const QString& bussType) {
  if (!m_currentELCD) {
    return;
  }

  // Determine incompatible components for the requested buss type
  const bool hasInverter = m_currentELCD->inverter().has_value();
  const bool hasStorage = m_currentELCD->electricalStorage().has_value();
  const bool hasConverter = m_currentELCD->storageConverter().has_value();

  bool inverterIncompat = false;
  bool storageIncompat = false;
  bool converterIncompat = false;

  if (bussType == "AlternatingCurrent") {
    inverterIncompat = hasInverter;
    storageIncompat = hasStorage;
    converterIncompat = hasConverter;
  } else if (bussType == "AlternatingCurrentWithStorage") {
    inverterIncompat = hasInverter;
  } else if (bussType == "DirectCurrentWithInverter") {
    storageIncompat = hasStorage;
    converterIncompat = hasConverter;
  } else if (bussType == "DirectCurrentWithInverterDCStorage") {
    // nothing incompatible
  } else if (bussType == "DirectCurrentWithInverterACStorage") {
    converterIncompat = hasConverter;
  }

  const bool anyIncompat = inverterIncompat || storageIncompat || converterIncompat;

  if (anyIncompat) {
    // Build incompatible component list string
    QString incompatList;
    if (inverterIncompat && m_currentELCD->inverter()) {
      incompatList += QString("  \u2022 Inverter: '%1'\n").arg(QString::fromStdString(m_currentELCD->inverter()->nameString()));
    }
    if (storageIncompat && m_currentELCD->electricalStorage()) {
      incompatList += QString("  \u2022 Storage: '%1'\n").arg(QString::fromStdString(m_currentELCD->electricalStorage()->nameString()));
    }
    if (converterIncompat && m_currentELCD->storageConverter()) {
      incompatList += QString("  \u2022 Converter: '%1'\n").arg(QString::fromStdString(m_currentELCD->storageConverter()->nameString()));
    }

    const QString msg = QString("Changing the Electrical Buss Type to '%1' will make the following objects "
                                "incompatible (they will be ignored during simulation):\n\n%2\n"
                                "Would you like to remove them from the model now for clarity?")
                          .arg(bussType)
                          .arg(incompatList);

    QMessageBox msgBox(nullptr);
    msgBox.setWindowTitle("Incompatible Components");
    msgBox.setText(msg);
    auto* removeBtn = msgBox.addButton("Remove && Switch", QMessageBox::AcceptRole);
    auto* keepBtn = msgBox.addButton("Keep && Switch", QMessageBox::AcceptRole);
    auto* cancelBtn = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelBtn);
    msgBox.exec();

    if (msgBox.clickedButton() == cancelBtn) {
      // Revert combo
      QSignalBlocker blocker(m_elcdView->bussTypeCombo);
      const int idx = m_elcdView->bussTypeCombo->findText(QString::fromStdString(m_currentELCD->electricalBussType()));
      if (idx >= 0) {
        m_elcdView->bussTypeCombo->setCurrentIndex(idx);
      }
      return;
    }

    if (msgBox.clickedButton() == removeBtn) {
      if (inverterIncompat) {
        m_currentELCD->resetInverter();
      }
      if (storageIncompat) {
        m_currentELCD->resetElectricalStorage();
      }
      if (converterIncompat) {
        m_currentELCD->resetStorageConverter();
      }
    }

    (void)keepBtn;  // Keep & Switch falls through to setElectricalBussType below
  }

  m_currentELCD->setElectricalBussType(bussType.toStdString());
  refresh();
}

void ElectricLoadCenterDistributionTabController::onGenOpSchemeChangeRequested(const QString& scheme) {
  if (!m_currentELCD) {
    return;
  }
  m_currentELCD->setGeneratorOperationSchemeType(scheme.toStdString());
  refresh();
}

void ElectricLoadCenterDistributionTabController::onValidateRequested() {
  if (!m_currentELCD) {
    return;
  }

  openstudio::StringStreamLogSink sink;
  sink.setLogLevel(Info);
  const bool sdkValid = m_currentELCD->validityCheck();
  const auto msgs = sink.logMessages();

  // Custom check: generator AC/DC compatibility (SDK validityCheck does not catch this)
  const bool expectsDC = bussTypeExpectsDC(m_currentELCD->electricalBussType());
  QStringList customErrors;
  for (const auto& gen : m_currentELCD->generators()) {
    if (expectsDC != generatorIsDC(gen)) {
      const QString expected = expectsDC ? "DC" : "AC";
      const QString actual = generatorIsDC(gen) ? "DC" : "AC";
      customErrors << QString("[Error] Generator '%1' produces %2 power but this ELCD uses a %3 buss type.")
                        .arg(QString::fromStdString(gen.nameString()), actual, expected);
    }
  }

  const bool valid = sdkValid && customErrors.isEmpty();
  QString text = valid ? "\u2713 Configuration is valid." : "\u2717 Configuration is invalid.";

  if (!msgs.empty()) {
    text += "\n\n";
    for (const auto& msg : msgs) {
      const QString level = (msg.logLevel() == Error) ? "[Error]" : (msg.logLevel() == Warn) ? "[Warning]" : "[Info]";
      text += level + " " + QString::fromStdString(msg.logMessage()) + "\n";
    }
  }
  if (!customErrors.isEmpty()) {
    if (msgs.empty()) {
      text += "\n\n";
    }
    for (const auto& err : customErrors) {
      text += err + "\n";
    }
  }

  QMessageBox::information(nullptr, "Validation Result", text);

  if (valid) {
    m_elcdView->validityLabel->setText("\u25CF");
    m_elcdView->validityLabel->setStyleSheet("color: green;");
  } else {
    m_elcdView->validityLabel->setText("\u26A0");
    m_elcdView->validityLabel->setStyleSheet("color: red;");
  }
}

void ElectricLoadCenterDistributionTabController::onDetailGeneratorDrop(const OSItemId& itemId) {
  if (!m_currentELCD) {
    return;
  }
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }
  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto gen_ = mo->optionalCast<model::Generator>();
  if (!gen_) {
    return;
  }
  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::Generator generator = isFromLibrary ? gen_->clone(m_model).cast<model::Generator>() : *gen_;

  // Reject if generator AC/DC type does not match the buss type
  const bool expectsDC = bussTypeExpectsDC(m_currentELCD->electricalBussType());
  const bool genIsDC = generatorIsDC(generator);
  if (expectsDC != genIsDC) {
    const QString expected = expectsDC ? "DC" : "AC";
    const QString actual = genIsDC ? "DC (Photovoltaic / PVWatts)" : "AC (FuelCell / MicroTurbine / WindTurbine)";
    QMessageBox::warning(nullptr, "Incompatible Generator Type",
                         QString("This Electric Load Center uses a %1 buss type, but '%2' is a %3 generator.\n\n"
                                 "Only %1 generators can be added to this configuration.")
                           .arg(expected, QString::fromStdString(generator.nameString()), actual));
    if (isFromLibrary) {
      generator.remove();
    }
    return;
  }

  m_currentELCD->addGenerator(generator);
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailGeneratorRemove(const Handle& handle) {
  if (!m_currentELCD) {
    return;
  }
  for (auto gen : m_currentELCD->generators()) {
    if (gen.handle() == handle) {
      m_currentELCD->removeGenerator(gen);
      break;
    }
  }
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailInverterDrop(const OSItemId& itemId) {
  if (!m_currentELCD) {
    return;
  }
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }
  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto inv_ = mo->optionalCast<model::Inverter>();
  if (!inv_) {
    return;
  }
  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::Inverter inverter = isFromLibrary ? inv_->clone(m_model).cast<model::Inverter>() : *inv_;
  m_currentELCD->setInverter(inverter);
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailInverterRemove() {
  if (!m_currentELCD) {
    return;
  }
  m_currentELCD->resetInverter();
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailStorageDrop(const OSItemId& itemId) {
  if (!m_currentELCD) {
    return;
  }
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }
  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto sto_ = mo->optionalCast<model::ElectricalStorage>();
  if (!sto_) {
    return;
  }
  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::ElectricalStorage storage = isFromLibrary ? sto_->clone(m_model).cast<model::ElectricalStorage>() : *sto_;
  m_currentELCD->setElectricalStorage(storage);
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailStorageRemove() {
  if (!m_currentELCD) {
    return;
  }
  m_currentELCD->resetElectricalStorage();
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailConverterDrop(const OSItemId& itemId) {
  if (!m_currentELCD) {
    return;
  }
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }
  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto conv_ = mo->optionalCast<model::ElectricLoadCenterStorageConverter>();
  if (!conv_) {
    return;
  }
  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::ElectricLoadCenterStorageConverter converter =
    isFromLibrary ? conv_->clone(m_model).cast<model::ElectricLoadCenterStorageConverter>() : *conv_;
  m_currentELCD->setStorageConverter(converter);
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailConverterRemove() {
  if (!m_currentELCD) {
    return;
  }
  m_currentELCD->resetStorageConverter();
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerDrop(const OSItemId& itemId) {
  if (!m_currentELCD) {
    return;
  }
  auto doc = OSAppBase::instance()->currentDocument();
  if (!doc) {
    return;
  }
  auto mo = doc->getModelObject(itemId);
  if (!mo) {
    return;
  }
  auto xfmr_ = mo->optionalCast<model::ElectricLoadCenterTransformer>();
  if (!xfmr_) {
    return;
  }
  const bool isFromLibrary = doc->fromComponentLibrary(itemId);
  model::ElectricLoadCenterTransformer transformer = isFromLibrary ? xfmr_->clone(m_model).cast<model::ElectricLoadCenterTransformer>() : *xfmr_;

  const std::string expectedUsage = "LoadCenterPowerConditioning";
  if (!openstudio::istringEqual(transformer.transformerUsage(), expectedUsage)) {
    const auto reply = QMessageBox::question(nullptr, "Change Transformer Usage",
                                             QString("Set this transformer's usage to '%1'?").arg(QString::fromStdString(expectedUsage)),
                                             QMessageBox::Yes | QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) {
      if (isFromLibrary) {
        transformer.remove();
      }
      return;
    }
    transformer.setTransformerUsage(expectedUsage);
  }

  m_currentELCD->setTransformer(transformer);
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerRemove() {
  if (!m_currentELCD) {
    return;
  }
  m_currentELCD->resetTransformer();
  refresh();
}

void ElectricLoadCenterDistributionTabController::onDetailInverterClick() {
  if (!m_currentELCD) {
    return;
  }
  auto inv_ = m_currentELCD->inverter();
  if (!inv_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *inv_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onDetailStorageClick() {
  if (!m_currentELCD) {
    return;
  }
  auto sto_ = m_currentELCD->electricalStorage();
  if (!sto_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *sto_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onDetailConverterClick() {
  if (!m_currentELCD) {
    return;
  }
  auto conv_ = m_currentELCD->storageConverter();
  if (!conv_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *conv_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onDetailLCPCTransformerClick() {
  if (!m_currentELCD) {
    return;
  }
  auto xfmr_ = m_currentELCD->transformer();
  if (!xfmr_) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = *xfmr_;
    doc->mainRightColumnController()->inspectModelObject(mo, false);
  }
}

void ElectricLoadCenterDistributionTabController::onDetailGeneratorClick(const Handle& handle) {
  if (!m_currentELCD) {
    return;
  }
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::OptionalModelObject mo = m_currentELCD->model().getModelObject<model::ModelObject>(handle);
    doc->mainRightColumnController()->inspectModelObject(mo, false);
    // Don't think I need to protect here
    // for (auto gen : m_currentELCD->generators()) {
    //   if (gen.handle() == handle) {
    //     if (auto doc = OSAppBase::instance()->currentDocument()) {
    //       model::OptionalModelObject mo = gen;
    //       doc->mainRightColumnController()->inspectModelObject(mo, false);
    //     }
    //     break;
    //   }
    // }
  }
}

// ─── ELCDListItem ─────────────────────────────────────────────────────────────

ELCDListItem::ELCDListItem(const model::ElectricLoadCenterDistribution& elcd, OSListController* listController)
  : OSListItem(listController), m_elcd(elcd) {}

QString ELCDListItem::name() const {
  return QString::fromStdString(m_elcd.nameString());
}

QString ELCDListItem::bussType() const {
  return QString::fromStdString(m_elcd.electricalBussType());
}

model::ElectricLoadCenterDistribution ELCDListItem::elcd() const {
  return m_elcd;
}

void ELCDListItem::remove() {
  if (auto* lc = qobject_cast<ELCDListController*>(controller())) {
    lc->removeELCD(m_elcd);
  }
}

void ELCDListItem::zoomIn() {
  if (auto* lc = qobject_cast<ELCDListController*>(controller())) {
    lc->tabController()->zoomInOnELCD(m_elcd.handle());
  }
}

// ─── ELCDListDropZoneItem ─────────────────────────────────────────────────────

ELCDListDropZoneItem::ELCDListDropZoneItem(OSListController* listController) : OSListItem(listController) {}

// ─── ELCDListController ───────────────────────────────────────────────────────

ELCDListController::ELCDListController(ElectricLoadCenterDistributionTabController* tabController) : m_tabController(tabController) {
  auto doc = OSAppBase::instance()->currentDocument();
  OS_ASSERT(doc);

  doc->model().getImpl<model::detail::Model_Impl>()->addWorkspaceObject.connect<ELCDListController, &ELCDListController::onModelObjectAdd>(this);

  connect(this, &ELCDListController::itemInsertedPrivate, this, &ELCDListController::itemInserted, Qt::QueuedConnection);
}

ElectricLoadCenterDistributionTabController* ELCDListController::tabController() const {
  return m_tabController;
}

std::vector<model::ElectricLoadCenterDistribution> ELCDListController::elcds() const {
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    return doc->model().getConcreteModelObjects<model::ElectricLoadCenterDistribution>();
  }
  return {};
}

int ELCDListController::elcdIndex(const model::ElectricLoadCenterDistribution& elcd) const {
  auto systems = elcds();
  for (int i = 0; i < static_cast<int>(systems.size()); ++i) {
    if (systems[i].handle() == elcd.handle()) {
      return i;
    }
  }
  return -1;
}

QSharedPointer<OSListItem> ELCDListController::itemAt(int i) {
  auto systems = elcds();
  if (i < static_cast<int>(systems.size())) {
    return QSharedPointer<ELCDListItem>(new ELCDListItem(systems[i], this));
  }
  return QSharedPointer<ELCDListDropZoneItem>(new ELCDListDropZoneItem(this));
}

int ELCDListController::count() {
  return static_cast<int>(elcds().size()) + 1;  // +1 for the drop zone
}

void ELCDListController::reset() {
  emit modelReset();
}

void ELCDListController::createNewELCD() {
  if (auto doc = OSAppBase::instance()->currentDocument()) {
    model::ElectricLoadCenterDistribution(doc->model());
    // onModelObjectAdd will fire and emit itemInsertedPrivate
  }
}

void ELCDListController::addELCDFromDrop(const OSItemId& /*itemId*/) {
  // Phase 9: hook up library drops. For now, create new.
  createNewELCD();
}

void ELCDListController::removeELCD(model::ElectricLoadCenterDistribution& elcd) {
  int index = elcdIndex(elcd);
  elcd.remove();
  if (index >= 0) {
    emit itemRemoved(index);
  }
}

void ELCDListController::onModelObjectAdd(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType,
                                          const openstudio::UUID& /*handle*/) {
  if (iddObjectType == model::ElectricLoadCenterDistribution::iddObjectType()) {
    emit itemInsertedPrivate(elcdIndex(object.cast<model::ElectricLoadCenterDistribution>()));
  }
}

// ─── ELCDItemDelegate ─────────────────────────────────────────────────────────

QGraphicsObject* ELCDItemDelegate::view(QSharedPointer<OSListItem> dataSource) {
  if (auto* item = qobject_cast<ELCDListItem*>(dataSource.data())) {
    auto* miniView = new ELCDSystemMiniView();
    miniView->setName(item->name());
    miniView->setBussType(item->bussType());
    miniView->setHandle(item->elcd().handle());

    connect(miniView, &ELCDSystemMiniView::removeClicked, item, &ELCDListItem::remove);
    connect(miniView, &ELCDSystemMiniView::zoomInClicked, qobject_cast<ELCDListController*>(item->controller())->tabController(),
            &ElectricLoadCenterDistributionTabController::zoomInOnELCD);

    return miniView;
  }

  if (auto* dropItem = qobject_cast<ELCDListDropZoneItem*>(dataSource.data())) {
    auto* dropZone = new ELCDDropZoneView();
    auto* lc = qobject_cast<ELCDListController*>(dropItem->controller());

    connect(dropZone, &ELCDDropZoneView::mouseClicked, lc, &ELCDListController::createNewELCD);
    connect(dropZone, &ELCDDropZoneView::componentDropped, lc, &ELCDListController::addELCDFromDrop);

    return dropZone;
  }

  return nullptr;
}

}  // namespace openstudio
