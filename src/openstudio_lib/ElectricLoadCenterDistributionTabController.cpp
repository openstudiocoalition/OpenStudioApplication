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
#include <openstudio/utilities/core/Assert.hpp>

#include <algorithm>
#include <openstudio/utilities/core/Compare.hpp>

#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QPixmap>
#include <QPushButton>
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
}  // namespace

// ─── ElectricLoadCenterDistributionTabController ──────────────────────────────

ElectricLoadCenterDistributionTabController::ElectricLoadCenterDistributionTabController(const model::Model& model)
  : MainTabController(new ELCDTabView()),
    m_model(model),
    m_elcdView(new ELCDView()),
    m_elcdGridView(new GridLayoutItem()),
    m_gridScene(new QGraphicsScene()) {

  connect(m_elcdView->oneLevelUpButton, &QPushButton::clicked, this, &ElectricLoadCenterDistributionTabController::zoomOutToGridView);

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

  if (m_detailScene) {
    m_detailScene->deleteLater();
  }
  m_detailScene = new ELCDScene();

  m_elcdView->nameLabel->setText(QString::fromStdString(elcd->nameString()));
  m_elcdView->header->show();
  m_elcdView->graphicsView->setScene(m_detailScene);
  m_elcdView->graphicsView->setAlignment(Qt::AlignCenter);
  m_elcdView->resetZoom();
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
    for (const auto& t : m_model.getConcreteModelObjects<model::ElectricLoadCenterTransformer>()) {
      if (!powerIn_ && openstudio::istringEqual(t.transformerUsage(), "PowerInFromGrid")) {
        powerIn_ = t;
      } else if (!powerOut_ && openstudio::istringEqual(t.transformerUsage(), "PowerOutToGrid")) {
        powerOut_ = t;
      }
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
