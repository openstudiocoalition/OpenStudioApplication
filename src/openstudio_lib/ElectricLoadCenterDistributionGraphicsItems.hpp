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
