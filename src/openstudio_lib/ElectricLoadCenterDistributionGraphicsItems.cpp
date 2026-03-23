/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ElectricLoadCenterDistributionGraphicsItems.hpp"
#include "../shared_gui_components/Buttons.hpp"

#include <QPainter>
#include <QApplication>
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
