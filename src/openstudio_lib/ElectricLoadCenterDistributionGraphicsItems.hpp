/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONGRAPHICSITEMS_HPP
#define OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONGRAPHICSITEMS_HPP

#include "OSItem.hpp"
#include "OSDropZone.hpp"
#include "../shared_gui_components/OSListController.hpp"
#include "../shared_gui_components/GraphicsItems.hpp"
#include <openstudio/utilities/idf/Handle.hpp>

class QGraphicsView;
class QPushButton;
class QLabel;
class QComboBox;

namespace openstudio {

// ─── ELCDView ────────────────────────────────────────────────────────────────
// Top-level QWidget: optional detail header bar + QGraphicsView
class ELCDView : public QWidget
{
  Q_OBJECT;

 public:
  ELCDView();

  QWidget* header;  // detail-mode header bar (hidden in overview)
  QGraphicsView* graphicsView;

  QPushButton* oneLevelUpButton;
  QLabel* nameLabel;

  // Row 2 controls
  QComboBox* bussTypeCombo;
  QComboBox* genOpSchemeCombo;
  QPushButton* validateButton;
  QLabel* validityLabel;

 signals:
  void bussTypeChangeRequested(const QString& bussType);
  void genOpSchemeChangeRequested(const QString& scheme);
  void validateRequested();

 protected:
  void wheelEvent(QWheelEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

 public slots:
  void zoomIn();
  void zoomOut();
  void resetZoom();
};

// ─── ELCDSystemMiniView ──────────────────────────────────────────────────────
// One ELCD card shown in the overview grid.
class ELCDSystemMiniView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  explicit ELCDSystemMiniView();

  virtual ~ELCDSystemMiniView() = default;

  RemoveButtonItem* removeButtonItem;
  OneLevelDownButtonItem* oneLevelDownButtonItem;

  QRectF boundingRect() const override;
  void adjustLayout();

  static QSize cellSize();

 public slots:
  void setName(const QString& name);
  void setBussType(const QString& bussType);
  void setHandle(const Handle& handle);

 signals:
  void zoomInClicked(const Handle& handle);
  void removeClicked(const Handle& handle);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  QRectF headerRect() const;
  QRectF contentRect() const;

  static int cellWidth();
  static int headerHeight();
  static int cellHeight();

  QString m_name;
  QString m_bussType;
  Handle m_handle;

 private slots:
  void onRemoveButtonClicked();
  void onZoomInButtonClicked();
};

class ELCDTransformerDropZoneView;  // forward declaration — defined below

// ─── ELCDUtilityGridPanel ────────────────────────────────────────────────────
// Left column of the overview: Utility Grid label + two transformer drop zones.
class ELCDUtilityGridPanel : public QGraphicsObject
{
  Q_OBJECT;

 public:
  ELCDUtilityGridPanel();
  QRectF boundingRect() const override;

  ELCDTransformerDropZoneView* powerInDropZone{nullptr};   // PowerInFromGrid transformer
  ELCDTransformerDropZoneView* powerOutDropZone{nullptr};  // PowerOutToGrid transformer

  // Y-coordinates (panel-local) of each drop zone centre — used by the
  // controller to draw connecting arrows to the Main Panel item.
  static constexpr int kPowerInCentreY = 90;
  static constexpr int kPowerOutCentreY = 290;
  static constexpr int kPanelWidth = 190;
  static constexpr int kPanelHeight = 340;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

// ─── ELCDMainPanelItem ───────────────────────────────────────────────────────
// Centre column of the overview: Main Panel with subpanel connection stubs.
class ELCDMainPanelItem : public QGraphicsObject
{
  Q_OBJECT;

 public:
  ELCDMainPanelItem();
  QRectF boundingRect() const override;

  void setHeight(int h);

  static constexpr int kPanelWidth = 160;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  int m_height = 340;
};

// ─── ELCDTransformerDropZoneView ─────────────────────────────────────────────
// Small drop zone for transformer objects; overrides paint() with compact font.
class ELCDTransformerDropZoneView : public OSDropZoneItem
{
  Q_OBJECT;

 public:
  explicit ELCDTransformerDropZoneView(const QString& label);

  QRectF boundingRect() const override;

  // Pass filled=true + the transformer's name when one exists in the model;
  // filled=false to revert to the "drop here" placeholder.
  void setFilled(bool filled, const QString& name = {});

  RemoveButtonItem* removeButtonItem{nullptr};

 signals:
  void removeClicked();

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  QString m_placeholderText;
  bool m_filled{false};
};

// ─── ELCDComponentSlotView ───────────────────────────────────────────────────
// Generic filled/empty component drop zone for the detail view.
class ELCDComponentSlotView : public OSDropZoneItem
{
  Q_OBJECT;

 public:
  explicit ELCDComponentSlotView(const QString& emptyLabel, const QString& iconPath = {});

  QRectF boundingRect() const override;
  void setFilled(bool filled, const QString& name = {});

  RemoveButtonItem* removeButtonItem{nullptr};

 signals:
  void removeClicked();

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  QString m_placeholderText;
  QString m_iconPath;
  bool m_filled{false};
};

// ─── ELCDGeneratorItemView ────────────────────────────────────────────────────
// One row in the generator list.
class ELCDGeneratorsView;  // forward declaration

class ELCDGeneratorItemView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  explicit ELCDGeneratorItemView(const QString& name, const Handle& handle);

  QRectF boundingRect() const override;

  static constexpr int kHeight = 28;

  RemoveButtonItem* removeButtonItem{nullptr};

 signals:
  void removeClicked(const Handle& handle);

  void inspectClicked(const Handle& handle);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  bool m_mouseDown;

 private:
  QString m_name;
  Handle m_handle;

 private slots:
  void onRemoveButtonClicked();
};

// ─── ELCDGeneratorsView ──────────────────────────────────────────────────────
// Generator list panel with dynamic height.
class ELCDGeneratorsView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  explicit ELCDGeneratorsView();

  QRectF boundingRect() const override;

  void setGeneratorLabel(const QString& label);
  void clearGenerators();
  void addGenerator(const QString& name, const Handle& handle);
  int totalHeight() const;

  OSDropZoneItem* dropZone{nullptr};

  static constexpr int kWidth = 240;
  static constexpr int kHeaderH = 28;
  static constexpr int kItemH = ELCDGeneratorItemView::kHeight;
  static constexpr int kPad = 4;
  static constexpr int kDropZoneH = 44;

 signals:
  void generatorRemoveClicked(const Handle& handle);
  void generatorInspectClicked(const Handle& handle);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  QString m_generatorLabel{"Generators"};
  QVector<ELCDGeneratorItemView*> m_generatorItems;
  void repositionItems();
};

// ─── ELCDDropZoneView ────────────────────────────────────────────────────────
// "Add / Drop Electric Load Center" placeholder card in overview.
class ELCDDropZoneView : public OSDropZoneItem
{
  Q_OBJECT;

 public:
  ELCDDropZoneView();

  QRectF boundingRect() const override;

 public slots:
  void onCreateNewELCD();

 signals:
  void createNewELCDClicked();

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONGRAPHICSITEMS_HPP
