/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ElectricLoadCenterDistributionGraphicsItems.hpp"
#include "../shared_gui_components/Buttons.hpp"

#include <QPainter>
#include <QApplication>
#include <cmath>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QKeyEvent>

namespace openstudio {

// ─── ELCDView ────────────────────────────────────────────────────────────────

ELCDView::ELCDView() : header(new QWidget()), graphicsView(new QGraphicsView()), oneLevelUpButton(new OneLevelUpButton()), nameLabel(new QLabel()) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setSpacing(0);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setAlignment(Qt::AlignTop);
  setLayout(mainVLayout);

  header->setObjectName("ELCDHeader");
  header->setStyleSheet("QWidget#ELCDHeader { background: #C3C3C3; }");
  header->setFixedHeight(35);
  mainVLayout->addWidget(header);

  auto* headerLayout = new QHBoxLayout();
  headerLayout->setContentsMargins(5, 5, 5, 5);
  headerLayout->setSpacing(0);
  header->setLayout(headerLayout);

  headerLayout->addWidget(nameLabel);
  headerLayout->addStretch();

  auto* backLabel = new QLabel("Back to overview");
  headerLayout->addWidget(backLabel);
  headerLayout->addWidget(oneLevelUpButton);
  oneLevelUpButton->setFixedSize(20, 20);
  oneLevelUpButton->setToolTip("Return to the Electric Load Center overview");

  graphicsView->setObjectName("GrayWidget");
  mainVLayout->addWidget(graphicsView);

  resetZoom();
}

void ELCDView::wheelEvent(QWheelEvent* event) {
  if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
    const double verticalAngle = event->angleDelta().y();
    if (verticalAngle != 0) {
      constexpr double zoom_factor_base = 1.0015;
      const double numDegrees = verticalAngle / 8.0;
      const double factor = std::pow(zoom_factor_base, 3.0 * numDegrees);
      graphicsView->scale(factor, factor);
      return;
    }
  }
  event->ignore();
}

void ELCDView::keyReleaseEvent(QKeyEvent* event) {
  if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
    if (event->key() == Qt::Key_Plus) {
      zoomIn();
      return;
    }
    if (event->key() == Qt::Key_Minus) {
      zoomOut();
      return;
    }
  }
  event->ignore();
}

void ELCDView::zoomIn() {
  graphicsView->scale(1.25, 1.25);
}

void ELCDView::zoomOut() {
  graphicsView->scale(0.8, 0.8);
}

void ELCDView::resetZoom() {
  graphicsView->resetTransform();
}

// ─── ELCDSystemMiniView ──────────────────────────────────────────────────────

// static sizes
int ELCDSystemMiniView::cellWidth() {
  return 280;
}

int ELCDSystemMiniView::headerHeight() {
  return 30;
}

int ELCDSystemMiniView::cellHeight() {
  return 160;
}

QSize ELCDSystemMiniView::cellSize() {
  return {cellWidth() + 20, cellHeight() + 20};
}

ELCDSystemMiniView::ELCDSystemMiniView() {
  adjustLayout();

  removeButtonItem = new RemoveButtonItem();
  removeButtonItem->setParentItem(this);
  removeButtonItem->setPos(cellWidth() - removeButtonItem->boundingRect().width() - 6,
                           headerHeight() / 2.0 - removeButtonItem->boundingRect().height() / 2.0);
  connect(removeButtonItem, &RemoveButtonItem::mouseClicked, this, &ELCDSystemMiniView::onRemoveButtonClicked);

  oneLevelDownButtonItem = new OneLevelDownButtonItem();
  oneLevelDownButtonItem->setToolTip("Inspect this Electric Load Center in detail");
  oneLevelDownButtonItem->setParentItem(this);
  oneLevelDownButtonItem->setPos(removeButtonItem->pos().x() - 6 - oneLevelDownButtonItem->boundingRect().width(),
                                 headerHeight() / 2.0 - oneLevelDownButtonItem->boundingRect().height() / 2.0);
  connect(oneLevelDownButtonItem, &OneLevelDownButtonItem::mouseClicked, this, &ELCDSystemMiniView::onZoomInButtonClicked);
}

void ELCDSystemMiniView::adjustLayout() {
  setPos(0, 0);
}

QRectF ELCDSystemMiniView::headerRect() const {
  return {0, 0, static_cast<double>(cellWidth()), static_cast<double>(headerHeight())};
}

QRectF ELCDSystemMiniView::contentRect() const {
  return {0, static_cast<double>(headerHeight()), static_cast<double>(cellWidth()), static_cast<double>(cellHeight() - headerHeight())};
}

QRectF ELCDSystemMiniView::boundingRect() const {
  return {0, 0, static_cast<double>(cellWidth()), static_cast<double>(cellHeight())};
}

void ELCDSystemMiniView::setName(const QString& name) {
  m_name = name;
  update();
}

void ELCDSystemMiniView::setBussType(const QString& bussType) {
  m_bussType = bussType;
  update();
}

void ELCDSystemMiniView::setHandle(const Handle& handle) {
  m_handle = handle;
}

void ELCDSystemMiniView::onRemoveButtonClicked() {
  emit removeClicked(m_handle);
}

void ELCDSystemMiniView::onZoomInButtonClicked() {
  emit zoomInClicked(m_handle);
}

void ELCDSystemMiniView::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  // Header background
  painter->setBrush(QColor(70, 130, 180));  // steel-blue header
  painter->setPen(Qt::NoPen);
  painter->drawRect(headerRect());

  // Header text (name)
  painter->setPen(Qt::white);
  QFont headerFont = painter->font();
  headerFont.setBold(true);
  painter->setFont(headerFont);
  QRectF nameRect(6, 0, oneLevelDownButtonItem->pos().x() - 10, headerHeight());
  painter->drawText(nameRect, Qt::AlignVCenter | Qt::AlignLeft, m_name);

  // Content background
  painter->setBrush(QColor(220, 230, 242));
  painter->setPen(QPen(QColor(70, 130, 180), 1));
  painter->drawRect(contentRect());

  // Buss type label
  painter->setPen(QColor(60, 60, 60));
  QFont contentFont = painter->font();
  contentFont.setBold(false);
  contentFont.setPointSize(9);
  painter->setFont(contentFont);
  QRectF bussRect(8, headerHeight() + 8, cellWidth() - 16, 20);
  painter->drawText(bussRect, Qt::AlignVCenter | Qt::AlignLeft, "Buss: " + m_bussType);
}

// ─── ELCDUtilityGridPanel ────────────────────────────────────────────────────

namespace {
// Draw a line with an open arrowhead (V-shape) at `to`.
void drawArrow(QPainter* painter, QPointF from, QPointF to) {
  painter->drawLine(from, to);
  QPointF dir = to - from;
  const double len = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
  if (len < 1.0) {
    return;
  }
  dir /= len;
  const QPointF perp(-dir.y(), dir.x());
  painter->drawLine(to, to - dir * 8.0 + perp * 4.0);
  painter->drawLine(to, to - dir * 8.0 - perp * 4.0);
}
}  // namespace

ELCDUtilityGridPanel::ELCDUtilityGridPanel() {
  static constexpr int kDropW = 160;
  static constexpr int kDropH = 65;
  static constexpr int kDropX = 15;
  static constexpr int kPowerInDropY = kPowerInCentreY - kDropH / 2;    // = 57
  static constexpr int kPowerOutDropY = kPowerOutCentreY - kDropH / 2;  // = 257

  powerInDropZone = new OSDropZoneItem();
  powerInDropZone->setSize(kDropW, kDropH);
  powerInDropZone->setText("Drop PowerInFromGrid\nTransformer");
  powerInDropZone->setParentItem(this);
  powerInDropZone->setPos(kDropX, kPowerInDropY);
  powerInDropZone->setAcceptDrops(true);

  powerOutDropZone = new OSDropZoneItem();
  powerOutDropZone->setSize(kDropW, kDropH);
  powerOutDropZone->setText("Drop PowerOutToGrid\nTransformer");
  powerOutDropZone->setParentItem(this);
  powerOutDropZone->setPos(kDropX, kPowerOutDropY);
  powerOutDropZone->setAcceptDrops(true);
}

QRectF ELCDUtilityGridPanel::boundingRect() const {
  return {0, 0, kPanelWidth, kPanelHeight};
}

void ELCDUtilityGridPanel::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  static constexpr int kDropH = 65;
  static constexpr int kDropX = 15;
  static constexpr int kDropW = 160;
  static constexpr int kPowerInDropY = kPowerInCentreY - kDropH / 2;
  static constexpr int kPowerOutDropY = kPowerOutCentreY - kDropH / 2;
  static constexpr int kMidX = kPanelWidth / 2;

  // Panel background + border (no title bar — the power grid icon to the left serves that role)
  painter->setBrush(QColor(245, 247, 250));
  painter->setPen(QPen(QColor(150, 160, 175), 1));
  painter->drawRect(0, 0, kPanelWidth, kPanelHeight);

  // Labels above drop zones
  QFont lf = painter->font();
  lf.setBold(false);
  lf.setPointSize(7);
  painter->setFont(lf);
  painter->setPen(QColor(60, 60, 80));
  painter->drawText(QRectF(kDropX, kPowerInDropY - 15, kDropW, 14), Qt::AlignVCenter | Qt::AlignLeft, "Power In From Grid:");
  painter->drawText(QRectF(kDropX, kPowerOutDropY - 15, kDropW, 14), Qt::AlignVCenter | Qt::AlignLeft, "Power Out To Grid:");

  // Vertical dashed line connecting the two drop zones
  QPen dashPen(QColor(130, 140, 160), 1, Qt::DashLine);
  painter->setPen(dashPen);
  painter->drawLine(kMidX, kPowerInDropY + kDropH + 4, kMidX, kPowerOutDropY - 4);

  QPen arrowPen(QColor(70, 130, 180), 1.5);
  painter->setPen(arrowPen);
  painter->setBrush(QColor(70, 130, 180));

  // Left-edge stubs: connections from the Utility Grid icon (left) to each drop zone
  // PowerIn: arrow pointing right (→ power flows from grid into transformer)
  drawArrow(painter, QPointF(0, kPowerInCentreY), QPointF(kDropX - 2, kPowerInCentreY));
  // PowerOut: arrow pointing left (← power flows from transformer back to grid)
  drawArrow(painter, QPointF(kDropX - 2, kPowerOutCentreY), QPointF(0, kPowerOutCentreY));

  // Right-edge stubs: connections from each drop zone to the Main Panel (right)
  // PowerIn: arrow pointing right (→)
  drawArrow(painter, QPointF(kDropX + kDropW + 4, kPowerInCentreY), QPointF(kPanelWidth, kPowerInCentreY));
  // PowerOut: arrow pointing left (←)
  drawArrow(painter, QPointF(kPanelWidth - 1, kPowerOutCentreY), QPointF(kDropX + kDropW + 4, kPowerOutCentreY));
}

// ─── ELCDMainPanelItem ───────────────────────────────────────────────────────

ELCDMainPanelItem::ELCDMainPanelItem() = default;

QRectF ELCDMainPanelItem::boundingRect() const {
  return {0, 0, kPanelWidth, kPanelHeight};
}

void ELCDMainPanelItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  // Dashed border box
  QPen dashBorder(QColor(100, 120, 160), 1.5, Qt::DashLine);
  painter->setBrush(QColor(240, 244, 250));
  painter->setPen(dashBorder);
  painter->drawRect(2, 2, kPanelWidth - 4, kPanelHeight - 4);

  // "MAIN PANEL" title
  QFont f = painter->font();
  f.setBold(true);
  f.setPointSize(8);
  painter->setFont(f);
  painter->setPen(QColor(60, 80, 120));
  painter->drawText(QRectF(0, 8, kPanelWidth, 20), Qt::AlignCenter, "MAIN PANEL");

  // Subpanel arrow stubs on the right edge — indicate ELCD subpanel connections
  // Two representative rows (y=90 and y=280, matching ELCD card row centres)
  static constexpr int kSubpanelY0 = ELCDUtilityGridPanel::kPowerInCentreY;  // y=90
  static constexpr int kSubpanelY1 = 280;
  QPen arrowPen(QColor(70, 130, 180), 1.5);
  painter->setPen(arrowPen);
  painter->setBrush(QColor(70, 130, 180));
  // Arrows pointing left (◀) — ELCD subpanels feed power INTO the Main Panel
  drawArrow(painter, QPointF(kPanelWidth - 1, kSubpanelY0), QPointF(8, kSubpanelY0));
  drawArrow(painter, QPointF(kPanelWidth - 1, kSubpanelY1), QPointF(8, kSubpanelY1));

  // "subpanel" labels next to each arrow
  QFont sf = painter->font();
  sf.setBold(false);
  sf.setPointSize(7);
  painter->setFont(sf);
  painter->setPen(QColor(80, 100, 140));
  painter->drawText(QRectF(10, kSubpanelY0 - 14, kPanelWidth - 14, 12), Qt::AlignLeft | Qt::AlignVCenter, "subpanel");
  painter->drawText(QRectF(10, kSubpanelY1 - 14, kPanelWidth - 14, 12), Qt::AlignLeft | Qt::AlignVCenter, "subpanel");

  // "Building & HVAC Electric Meters" footer with down arrow
  QFont ff = painter->font();
  ff.setPointSize(7);
  painter->setFont(ff);
  painter->setPen(QColor(60, 60, 80));
  QRectF footerRect(4, kPanelHeight - 60, kPanelWidth - 8, 40);
  painter->drawText(footerRect, Qt::AlignCenter | Qt::TextWordWrap, "Building & HVAC\nElectric Meters");

  QPen arrowPen2(QColor(100, 100, 120), 1.5);
  painter->setPen(arrowPen2);
  painter->setBrush(QColor(100, 100, 120));
  drawArrow(painter, QPointF(kPanelWidth / 2, kPanelHeight - 18), QPointF(kPanelWidth / 2, kPanelHeight - 4));
}

// ─── ELCDDropZoneView ────────────────────────────────────────────────────────

ELCDDropZoneView::ELCDDropZoneView() {
  setSize(ELCDSystemMiniView::cellSize().width() - 20, ELCDSystemMiniView::cellSize().height() - 20);
  setText("Add Electric Load Center");
  setAcceptDrops(true);
}

QRectF ELCDDropZoneView::boundingRect() const {
  return {0, 0, static_cast<double>(ELCDSystemMiniView::cellSize().width() - 20), static_cast<double>(ELCDSystemMiniView::cellSize().height() - 20)};
}

void ELCDDropZoneView::onCreateNewELCD() {
  emit createNewELCDClicked();
}

void ELCDDropZoneView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  // Delegate to base class for the dashed-border drop zone look
  OSDropZoneItem::paint(painter, option, widget);
}

void ELCDDropZoneView::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  OSDropZoneItem::mousePressEvent(event);
}

void ELCDDropZoneView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  OSDropZoneItem::mouseReleaseEvent(event);
}

}  // namespace openstudio
