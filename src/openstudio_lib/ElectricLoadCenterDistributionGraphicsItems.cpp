/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ElectricLoadCenterDistributionGraphicsItems.hpp"
#include "../shared_gui_components/Buttons.hpp"

#include <QPainter>
#include <QApplication>
#include <QComboBox>
#include <QPixmap>
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

ELCDView::ELCDView()
  : header(new QWidget()),
    graphicsView(new QGraphicsView()),
    oneLevelUpButton(new OneLevelUpButton()),
    nameLabel(new QLabel()),
    bussTypeCombo(new QComboBox()),
    genOpSchemeCombo(new QComboBox()),
    validateButton(new QPushButton("✓ Validate")),
    validityLabel(new QLabel("●")) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setSpacing(0);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setAlignment(Qt::AlignTop);
  setLayout(mainVLayout);

  header->setObjectName("ELCDHeader");
  header->setStyleSheet("QWidget#ELCDHeader { background: #C3C3C3; }");
  mainVLayout->addWidget(header);

  auto* headerVLayout = new QVBoxLayout();
  headerVLayout->setContentsMargins(5, 4, 5, 4);
  headerVLayout->setSpacing(4);
  header->setLayout(headerVLayout);

  // ── Row 1: name label + back button ────────────────────────────────────────
  auto* row1 = new QHBoxLayout();
  row1->setSpacing(4);
  row1->setContentsMargins(0, 0, 0, 0);
  headerVLayout->addLayout(row1);

  row1->addWidget(nameLabel);
  row1->addStretch();

  auto* backLabel = new QLabel("Back to overview");
  row1->addWidget(backLabel);
  row1->addWidget(oneLevelUpButton);
  oneLevelUpButton->setFixedSize(20, 20);
  oneLevelUpButton->setToolTip("Return to the Electric Load Center overview");

  // ── Row 2: combos + validate ────────────────────────────────────────────────
  auto* row2 = new QHBoxLayout();
  row2->setSpacing(4);
  row2->setContentsMargins(0, 0, 0, 0);
  headerVLayout->addLayout(row2);

  row2->addWidget(new QLabel("Buss Type:"));
  row2->addWidget(bussTypeCombo);

  // Buss type items
  for (const char* item : {"AlternatingCurrent", "AlternatingCurrentWithStorage", "DirectCurrentWithInverter", "DirectCurrentWithInverterDCStorage",
                           "DirectCurrentWithInverterACStorage"}) {
    bussTypeCombo->addItem(item);
  }

  // 10 px spacer
  row2->addSpacing(10);

  row2->addWidget(new QLabel("Gen Op Scheme:"));
  row2->addWidget(genOpSchemeCombo);

  // Gen op scheme items
  for (const char* item :
       {"Baseload", "DemandLimit", "TrackElectrical", "TrackSchedule", "TrackMeter", "FollowThermal", "FollowThermalLimitElectrical"}) {
    genOpSchemeCombo->addItem(item);
  }

  row2->addStretch();
  row2->addWidget(validateButton);

  validityLabel->setStyleSheet("color: gray;");
  row2->addWidget(validityLabel);

  // ── Wire combo signals ──────────────────────────────────────────────────────
  connect(bussTypeCombo, &QComboBox::currentIndexChanged, this,
          [this](int /*index*/) { emit bussTypeChangeRequested(bussTypeCombo->currentText()); });
  connect(genOpSchemeCombo, &QComboBox::currentIndexChanged, this,
          [this](int /*index*/) { emit genOpSchemeChangeRequested(genOpSchemeCombo->currentText()); });
  connect(validateButton, &QPushButton::clicked, this, &ELCDView::validateRequested);

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

// ─── ELCDTransformerDropZoneView ─────────────────────────────────────────────

ELCDTransformerDropZoneView::ELCDTransformerDropZoneView(const QString& label) : m_placeholderText(label) {
  setText(label);
  setSize(165, 70);
  setAcceptDrops(true);

  removeButtonItem = new RemoveButtonItem();
  removeButtonItem->setParentItem(this);
  removeButtonItem->setVisible(false);
  connect(removeButtonItem, &RemoveButtonItem::mouseClicked, this, &ELCDTransformerDropZoneView::removeClicked);
}

void ELCDTransformerDropZoneView::setFilled(bool filled, const QString& name) {
  m_filled = filled;
  setText(filled ? name : m_placeholderText);
  removeButtonItem->setPos(boundingRect().width() - removeButtonItem->boundingRect().width() - 4,
                           boundingRect().height() / 2.0 - removeButtonItem->boundingRect().height() / 2.0);
  removeButtonItem->setVisible(filled);
  update();
}

QRectF ELCDTransformerDropZoneView::boundingRect() const {
  return {0, 0, 165, 70};
}

void ELCDTransformerDropZoneView::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);

  if (m_filled) {
    painter->setBrush(QColor(220, 234, 250));
    painter->setPen(QPen(QColor(70, 130, 180), 1.5));
  } else {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor(109, 109, 109), 2, Qt::DashLine, Qt::RoundCap));
  }
  painter->drawRect(boundingRect());

  static const QPixmap kIcon = QPixmap(":/images/mini_icons/transformer.png");
  static constexpr int kIconSize = 24;
  static constexpr int kIconX = 5;

  if (!kIcon.isNull()) {
    const QPixmap scaled = kIcon.scaled(kIconSize, kIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const int iconY = static_cast<int>((boundingRect().height() - scaled.height()) / 2.0);
    painter->drawPixmap(kIconX, iconY, scaled);
  }

  QFont font = painter->font();
  font.setPixelSize(12);
  painter->setFont(font);
  painter->setPen(m_filled ? QColor(40, 60, 100) : QColor(80, 80, 80));

  const int textLeft = kIconX + kIconSize + 4;
  const qreal removeW = removeButtonItem->boundingRect().width();
  const qreal textRight = m_filled ? (boundingRect().width() - removeW - 8) : boundingRect().width() - 4;
  const QRectF textRect(textLeft, 0, textRight - textLeft, boundingRect().height());
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap, m_text);
}

// ─── ELCDUtilityGridPanel ────────────────────────────────────────────────────

ELCDUtilityGridPanel::ELCDUtilityGridPanel() {
  // ELCDTransformerDropZoneView is 165×70; position so its centre aligns with kPowerIn/OutCentreY
  static constexpr int kDropH = 70;
  static constexpr int kDropX = 12;

  powerInDropZone = new ELCDTransformerDropZoneView("PowerIn\nTransformer");
  powerInDropZone->setParentItem(this);
  powerInDropZone->setPos(kDropX, kPowerInCentreY - (kDropH / 2));

  powerOutDropZone = new ELCDTransformerDropZoneView("PowerOut\nTransformer");
  powerOutDropZone->setParentItem(this);
  powerOutDropZone->setPos(kDropX, kPowerOutCentreY - (kDropH / 2));
}

QRectF ELCDUtilityGridPanel::boundingRect() const {
  return {0, 0, kPanelWidth, kPanelHeight};
}

void ELCDUtilityGridPanel::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  static constexpr int kDropH = 70;
  static constexpr int kDropW = 165;
  static constexpr int kDropX = 12;
  static constexpr int kPowerInDropY = kPowerInCentreY - (kDropH / 2);
  static constexpr int kPowerOutDropY = kPowerOutCentreY - (kDropH / 2);
  static constexpr int kMidX = kPanelWidth / 2;

  // Panel background + border
  painter->setBrush(QColor(245, 247, 250));
  painter->setPen(QPen(QColor(150, 160, 175), 1));
  painter->drawRect(0, 0, kPanelWidth, kPanelHeight);

  // Title
  QFont f = painter->font();
  f.setBold(true);
  f.setPointSize(14);
  painter->setFont(f);
  painter->setPen(QColor(60, 80, 120));
  painter->drawText(QRectF(0, 8, kPanelWidth, 20), Qt::AlignCenter, "GRID TRANSFORMERS");

  // Labels above drop zones
  QFont lf = painter->font();
  lf.setBold(false);
  lf.setPointSize(12);
  painter->setFont(lf);
  painter->setPen(QColor(60, 60, 80));
  painter->drawText(QRectF(kDropX, kPowerInDropY - 15, kDropW, 14), Qt::AlignVCenter | Qt::AlignLeft, "Power In From Grid:");
  painter->drawText(QRectF(kDropX, kPowerOutDropY - 15, kDropW, 14), Qt::AlignVCenter | Qt::AlignLeft, "Power Out To Grid:");

  QPen arrowPen(QColor(70, 130, 180), 1.5);
  painter->setPen(arrowPen);
  painter->setBrush(QColor(70, 130, 180));

  // Left-edge stubs: panel edge ↔ drop zone
  // PowerIn: tip AT drop zone (arrowhead is the final tip — power enters from left)
  drawArrow(painter, QPointF(0, kPowerInCentreY), QPointF(kDropX - 2, kPowerInCentreY));
  // PowerOut: plain line — the arrowhead tip lives on the scene-level line ending at the icon
  painter->drawLine(QPointF(kDropX - 2, kPowerOutCentreY), QPointF(0, kPowerOutCentreY));

  // Right-edge stubs: drop zone ↔ panel edge
  // PowerIn: plain line — the arrowhead tip lives on the scene-level line ending at Main Panel
  painter->drawLine(QPointF(kDropX + kDropW + 4, kPowerInCentreY), QPointF(kPanelWidth, kPowerInCentreY));
  // PowerOut: tip AT drop zone (arrowhead is the final tip — power enters from right)
  drawArrow(painter, QPointF(kPanelWidth - 1, kPowerOutCentreY), QPointF(kDropX + kDropW + 4, kPowerOutCentreY));

  (void)kMidX;
}

// ─── ELCDMainPanelItem ───────────────────────────────────────────────────────

ELCDMainPanelItem::ELCDMainPanelItem() = default;

void ELCDMainPanelItem::setHeight(int h) {
  prepareGeometryChange();
  m_height = (h > 60) ? h : 60;
  update();
}

QRectF ELCDMainPanelItem::boundingRect() const {
  return {0, 0, kPanelWidth, static_cast<double>(m_height)};
}

void ELCDMainPanelItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_mouseDown = true;
  event->accept();
}

void ELCDMainPanelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_mouseDown && boundingRect().contains(event->pos())) {
    emit clicked();
  }
  m_mouseDown = false;
  event->accept();
}

void ELCDMainPanelItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  // Dashed border box
  QPen dashBorder(QColor(100, 120, 160), 1.5, Qt::DashLine);
  painter->setBrush(QColor(240, 244, 250));
  painter->setPen(dashBorder);
  painter->drawRect(2, 2, kPanelWidth - 4, m_height - 4);

  // "MAIN PANEL" title
  QFont f = painter->font();
  f.setBold(true);
  f.setPointSize(14);
  painter->setFont(f);
  painter->setPen(QColor(60, 80, 120));
  painter->drawText(QRectF(0, 8, kPanelWidth, 20), Qt::AlignCenter, "MAIN PANEL");

  // "Building & HVAC Electric Meters" footer with down arrow
  QFont ff = painter->font();
  ff.setBold(false);
  ff.setPointSize(12);
  painter->setFont(ff);
  painter->setPen(QColor(60, 60, 80));
  QRectF footerRect(4, m_height - 60, kPanelWidth - 8, 40);
  painter->drawText(footerRect, Qt::AlignCenter | Qt::TextWordWrap, "Building & HVAC\nElectric Meters");

  QPen arrowPen2(QColor(100, 100, 120), 1.5);
  painter->setPen(arrowPen2);
  painter->setBrush(QColor(100, 100, 120));
  drawArrow(painter, QPointF(kPanelWidth / 2, m_height - 18), QPointF(kPanelWidth / 2, m_height - 4));
}

// ─── ELCDDetailContainerItem ─────────────────────────────────────────────────

ELCDDetailContainerItem::ELCDDetailContainerItem(const QRectF& rect, const Handle& elcdHandle) : m_rect(rect), m_handle(elcdHandle) {
  setZValue(-1);
}

QRectF ELCDDetailContainerItem::boundingRect() const {
  return m_rect;
}

void ELCDDetailContainerItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_mouseDown = true;
  event->accept();
}

void ELCDDetailContainerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_mouseDown && boundingRect().contains(event->pos())) {
    emit inspectClicked(m_handle);
  }
  m_mouseDown = false;
  event->accept();
}

void ELCDDetailContainerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(QColor(235, 242, 252));
  painter->setPen(QPen(QColor(70, 130, 180), 1.5));
  painter->drawRoundedRect(m_rect.adjusted(1, 1, -1, -1), 8, 8);
}

// ─── ELCDComponentSlotView ───────────────────────────────────────────────────

ELCDComponentSlotView::ELCDComponentSlotView(const QString& emptyLabel, const QString& iconPath)
  : m_placeholderText(emptyLabel), m_iconPath(iconPath) {
  setText(emptyLabel);
  setSize(200, 100);
  setAcceptDrops(true);

  removeButtonItem = new RemoveButtonItem();
  removeButtonItem->setParentItem(this);
  removeButtonItem->setVisible(false);
  connect(removeButtonItem, &RemoveButtonItem::mouseClicked, this, &ELCDComponentSlotView::removeClicked);
}

void ELCDComponentSlotView::setFilled(bool filled, const QString& name) {
  m_filled = filled;
  setText(filled ? name : m_placeholderText);
  removeButtonItem->setPos(boundingRect().width() - removeButtonItem->boundingRect().width() - 4,
                           boundingRect().height() / 2.0 - removeButtonItem->boundingRect().height() / 2.0);
  removeButtonItem->setVisible(filled);
  update();
}

QRectF ELCDComponentSlotView::boundingRect() const {
  return {0, 0, 200, 100};
}

void ELCDComponentSlotView::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);

  if (m_filled) {
    painter->setBrush(QColor(220, 234, 250));
    painter->setPen(QPen(QColor(70, 130, 180), 1.5));
  } else {
    painter->setBrush(QColor(230, 230, 230));
    painter->setPen(QPen(QColor(109, 109, 109), 2, Qt::DashLine, Qt::RoundCap));
  }
  painter->drawRect(boundingRect());

  static constexpr int kIconSize = 24;
  static constexpr int kIconX = 5;

  int textLeft = 4;
  if (!m_iconPath.isEmpty()) {
    const QPixmap icon(m_iconPath);
    if (!icon.isNull()) {
      const QPixmap scaled = icon.scaled(kIconSize, kIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      const int iconY = static_cast<int>((boundingRect().height() - scaled.height()) / 2.0);
      painter->drawPixmap(kIconX, iconY, scaled);
      textLeft = kIconX + kIconSize + 4;
    }
  }

  QFont font = painter->font();
  font.setPixelSize(12);
  painter->setFont(font);
  painter->setPen(m_filled ? QColor(40, 60, 100) : QColor(80, 80, 80));

  const qreal removeW = removeButtonItem->boundingRect().width();
  const qreal textRight = m_filled ? (boundingRect().width() - removeW - 8) : boundingRect().width() - 4;
  const QRectF textRect(textLeft, 0, textRight - textLeft, boundingRect().height());
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap, m_text);
}

// ─── ELCDGeneratorItemView ────────────────────────────────────────────────────

ELCDGeneratorItemView::ELCDGeneratorItemView(const QString& name, const Handle& handle) : m_name(name), m_handle(handle) {
  removeButtonItem = new RemoveButtonItem();
  removeButtonItem->setParentItem(this);
  const qreal bx = ELCDGeneratorsView::kWidth - ELCDGeneratorsView::kPad - removeButtonItem->boundingRect().width();
  const qreal by = (kHeight - removeButtonItem->boundingRect().height()) / 2.0;
  removeButtonItem->setPos(bx, by);
  connect(removeButtonItem, &RemoveButtonItem::mouseClicked, this, &ELCDGeneratorItemView::onRemoveButtonClicked);
}

QRectF ELCDGeneratorItemView::boundingRect() const {
  return {0, 0, static_cast<double>(ELCDGeneratorsView::kWidth), static_cast<double>(kHeight)};
}

void ELCDGeneratorItemView::onRemoveButtonClicked() {
  emit removeClicked(m_handle);
}

void ELCDGeneratorItemView::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, false);

  painter->setBrush(QColor(235, 240, 248));
  painter->setPen(QPen(QColor(160, 175, 200), 1));
  painter->drawRect(boundingRect());

  QFont font = painter->font();
  font.setPixelSize(11);
  painter->setFont(font);
  painter->setPen(QColor(40, 60, 100));

  const qreal removeW = removeButtonItem->boundingRect().width();
  const QRectF textRect(ELCDGeneratorsView::kPad, 0, boundingRect().width() - removeW - ELCDGeneratorsView::kPad * 2 - 4, kHeight);
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_name);
}

void ELCDGeneratorItemView::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_mouseDown = true;

  update();

  event->accept();
}

void ELCDGeneratorItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_mouseDown) {
    m_mouseDown = false;

    this->update();

    QApplication::processEvents();

    if (shape().contains(event->pos())) {
      event->accept();

      update();

      emit inspectClicked(m_handle);
    }
  }
}

// ─── ELCDGeneratorsView ──────────────────────────────────────────────────────

ELCDGeneratorsView::ELCDGeneratorsView() {
  dropZone = new OSDropZoneItem();
  dropZone->setParentItem(this);
  dropZone->setSize(kWidth - 2 * kPad, kDropZoneH);
  dropZone->setText("Drop Generator here");
  dropZone->setAcceptDrops(true);
  dropZone->setPos(kPad, kHeaderH + kPad);
}

int ELCDGeneratorsView::totalHeight() const {
  return kHeaderH + kPad + static_cast<int>(m_generatorItems.size()) * kItemH + kPad + kDropZoneH + kPad;
}

QRectF ELCDGeneratorsView::boundingRect() const {
  return {0, 0, static_cast<double>(kWidth), static_cast<double>(totalHeight())};
}

void ELCDGeneratorsView::setGeneratorLabel(const QString& label) {
  m_generatorLabel = label;
  update();
}

void ELCDGeneratorsView::clearGenerators() {
  for (auto* item : m_generatorItems) {
    delete item;
  }
  m_generatorItems.clear();
  prepareGeometryChange();
  repositionItems();
  update();
}

void ELCDGeneratorsView::addGenerator(const QString& name, const Handle& handle) {
  auto* item = new ELCDGeneratorItemView(name, handle);
  item->setParentItem(this);
  connect(item, &ELCDGeneratorItemView::removeClicked, this, &ELCDGeneratorsView::generatorRemoveClicked);
  connect(item, &ELCDGeneratorItemView::inspectClicked, this, &ELCDGeneratorsView::generatorInspectClicked);
  m_generatorItems.append(item);
  prepareGeometryChange();
  repositionItems();
  update();
}

void ELCDGeneratorsView::repositionItems() {
  for (int i = 0; i < m_generatorItems.size(); ++i) {
    m_generatorItems[i]->setPos(kPad, kHeaderH + kPad + i * kItemH);
  }
  const int dropY = kHeaderH + kPad + static_cast<int>(m_generatorItems.size()) * kItemH + kPad;
  dropZone->setPos(kPad, dropY);
}

void ELCDGeneratorsView::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  const int h = totalHeight();

  // Panel background + border
  painter->setBrush(QColor(240, 245, 255));
  painter->setPen(QPen(QColor(100, 130, 180), 1));
  painter->drawRect(0, 0, kWidth, h);

  // Header background
  painter->setBrush(QColor(70, 130, 180));
  painter->setPen(Qt::NoPen);
  painter->drawRect(0, 0, kWidth, kHeaderH);

  // Header text
  painter->setPen(Qt::white);
  QFont boldFont = painter->font();
  boldFont.setBold(true);
  boldFont.setPixelSize(13);
  painter->setFont(boldFont);
  painter->drawText(QRectF(kPad, 0, kWidth - 2 * kPad, kHeaderH), Qt::AlignVCenter | Qt::AlignLeft, m_generatorLabel);

  // Separator line below header
  painter->setPen(QPen(QColor(100, 130, 180), 1));
  painter->drawLine(0, kHeaderH, kWidth, kHeaderH);
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
