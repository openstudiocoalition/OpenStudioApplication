/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_VRFGRAPHICSITEMS_HPP
#define OPENSTUDIO_VRFGRAPHICSITEMS_HPP

#include <QGraphicsObject>
#include "OSItem.hpp"
#include "OSDropZone.hpp"
#include "../shared_gui_components/OSListController.hpp"
#include "../shared_gui_components/GraphicsItems.hpp"

class QGraphicsView;
class QPushButton;
class QLabel;

namespace openstudio {

class VRFTerminalView;
class RemoveButtonItem;
class VRFThermalZoneDropZoneView;

class VRFView : public QWidget
{
  Q_OBJECT;

 public:
  VRFView();

  virtual ~VRFView() = default;

  QWidget* header;

  QGraphicsView* graphicsView;

  QPushButton* oneLevelUpButton;

  QLabel* nameLabel;

 protected:
  // Override to allow zooming with CTRL + Wheel Up/Down
  virtual void wheelEvent(QWheelEvent* event) override;
  // Override to allow zooming with CTRL + '+' / '-'
  virtual void keyReleaseEvent(QKeyEvent* event) override;

 public slots:
  // Display adjustments
  void zoomIn();
  void zoomOut();
  void resetZoom();
};

class VRFSystemMiniView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  VRFSystemMiniView();

  virtual ~VRFSystemMiniView() = default;

  RemoveButtonItem* removeButtonItem;
  OneLevelDownButtonItem* oneLevelDownButtonItem;

  QRectF boundingRect() const override;
  static QSize cellSize();

  void setNumberOfZones(int zones);
  void setNumberOfTerminals(int terminals);

 public slots:

  void setName(const QString& name);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  QRectF contentRect() const;

  QRectF headerRect() const;

  static int cellWidth();

  static int cellHeight();

  static int headerHeight();

  QString m_name;

  QPixmap m_vrfOutdoorPix;
  QPixmap m_vrfTransferPix;
  QPixmap m_vrfTerminalPix;
  QPixmap m_vrfZonePix;

  double m_length;

  int m_zones;
  int m_terminals;
};

class VRFSystemView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  VRFSystemView();

  virtual ~VRFSystemView() = default;

  QRectF boundingRect() const override;

  void setId(const OSItemId& id);

  OSDropZoneItem* terminalDropZone;

  OSDropZoneItem* zoneDropZone;

  ButtonItem* vrfIconButton;

  void adjustLayout();

  static const int margin;
  static const int terminalDropZoneWidth;
  static const int zoneDropZoneWidth;
  static const int dropZoneHeight;
  static const int terminalViewHeight;

  void addVRFTerminalView(VRFTerminalView* view);
  void removeAllVRFTerminalViews();

 signals:

  void inspectClicked(const OSItemId& id);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  bool m_mouseDown;

 private slots:

  void onVRFIconClicked();

 private:
  double m_width;
  double m_height;

  std::vector<QGraphicsObject*> m_terminalViews;

  OSItemId m_id;
  QPixmap m_vrfPixmap;
};

class VRFTerminalView : public QGraphicsObject
{
  Q_OBJECT;

 public:
  VRFTerminalView();

  virtual ~VRFTerminalView() = default;

  QRectF boundingRect() const override;

  VRFThermalZoneDropZoneView* zoneDropZone;

  ButtonItem* terminalIconButton;

  RemoveButtonItem* removeButtonItem;

  RemoveButtonItem* removeZoneButtonItem;

  VRFThermalZoneDropZoneView* vrfThermalZoneDropZoneView;

  void setId(const OSItemId& id);

 signals:

  void componentDroppedOnZone(const OSItemId& zoneHVACTerminalID, const OSItemId& dropComponentID);

  void removeZoneClicked(const OSItemId& zoneHVACTerminalID);

  void terminalIconClicked(const OSItemId& zoneHVACTerminalID);

  void removeTerminalClicked(const OSItemId& zoneHVACTerminalID);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private slots:

  void onComponenDroppedOnZone(const OSItemId& dropComponentID);

  void onRemoveZoneClicked();

  void onRemoveTerminalClicked();

  void onTerminalIconClicked();

 private:
  QRectF terminalPixmapRect() const;

  QPixmap m_terminalPixmap;

  OSItemId m_id;
};

class VRFThermalZoneDropZoneView : public OSDropZoneItem
{
  Q_OBJECT;

 public:
  VRFThermalZoneDropZoneView();

  // TODO: why isn't this one virtual ?
  ~VRFThermalZoneDropZoneView() = default;

  void setHasZone(bool hasZone);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
  bool m_hasZone;
};

class VRFSystemDropZoneView : public OSDropZoneItem
{
  Q_OBJECT;

 public:
  QRectF boundingRect() const override;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_VRFGRAPHICSITEMS_HPP
