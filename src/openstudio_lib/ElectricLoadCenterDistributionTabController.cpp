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
#include "OSItem.hpp"

#include "../shared_gui_components/GraphicsItems.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ElectricLoadCenterDistribution_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <algorithm>

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>

namespace openstudio {

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
  constexpr int kIconColWidth = 120;
  constexpr int kIconSize = 100;
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

  m_mainPanelItem = new ELCDMainPanelItem();
  m_gridScene->addItem(m_mainPanelItem);
  m_mainPanelItem->setPos(kMainPanelX, 0);

  m_elcdGridView->setPos(kElcdGridX, 0);
  m_gridScene->addItem(m_elcdGridView);

  // Static connecting lines: icon ↔ utility panel ↔ main panel
  const QPen connectorPen(QColor(70, 130, 180), 1.5);
  // Icon → PowerIn transformer
  m_gridScene->addLine(kIconColWidth - 20, ELCDUtilityGridPanel::kPowerInCentreY, kUtilityPanelX, ELCDUtilityGridPanel::kPowerInCentreY,
                       connectorPen);
  // PowerOut transformer → icon
  m_gridScene->addLine(kUtilityPanelX, ELCDUtilityGridPanel::kPowerOutCentreY, kIconColWidth - 20, ELCDUtilityGridPanel::kPowerOutCentreY,
                       connectorPen);
  // PowerIn transformer → Main Panel
  m_gridScene->addLine(kUtilityPanelX + ELCDUtilityGridPanel::kPanelWidth, ELCDUtilityGridPanel::kPowerInCentreY, kMainPanelX,
                       ELCDUtilityGridPanel::kPowerInCentreY, connectorPen);
  // Main Panel → PowerOut transformer
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

  // Clear old dynamic connector lines
  for (QGraphicsLineItem* line : m_elcdConnectorLines) {
    if (line) {
      m_gridScene->removeItem(line);
      delete line;
    }
  }
  m_elcdConnectorLines.clear();

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

  // Draw one horizontal subpanel connector per ELCD
  // Center-y of ELCD i in scene = kMargin + i*(kCellH+kSpacing) + kCellH/2
  //                             = 10 + i*190 + 90 = 100 + i*190
  const QPen connectorPen(QColor(70, 130, 180), 1.5);
  for (int i = 0; i < elcdCount; ++i) {
    const int connY = kMargin + i * (kCellH + kSpacing) + kCellH / 2;
    auto* line = new QGraphicsLineItem(m_kMainPanelX + ELCDMainPanelItem::kPanelWidth, connY, m_kElcdGridX, connY);
    line->setPen(connectorPen);
    m_gridScene->addItem(line);
    m_elcdConnectorLines.append(line);
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
