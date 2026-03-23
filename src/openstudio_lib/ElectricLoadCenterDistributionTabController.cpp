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

#include <QGraphicsScene>
#include <QGraphicsView>
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

  m_gridScene->addItem(m_elcdGridView);

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
  // Grid auto-refreshes via list controller signals; detail view refresh added in Phase 4.
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
