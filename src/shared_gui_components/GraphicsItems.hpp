/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_GRAPHICSITEMS_HPP
#define SHAREDGUICOMPONENTS_GRAPHICSITEMS_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <QGraphicsObject>
#include <QSizeF>

namespace openstudio {

class OSListItem;
class OSListController;
class OSGraphicsItemDelegate;

// Button functionality without any visual elements
// ButtonItem already existed when AbstractButtonItem was factored out.
// Ideally ButtonItem derives from AbstractButtonItem.
class AbstractButtonItem : public QGraphicsObject
{
  Q_OBJECT;

 public:
  explicit AbstractButtonItem(QGraphicsItem* parent = nullptr);

  virtual ~AbstractButtonItem() {}

  void setChecked(bool checked);

 signals:

  void mouseClicked(bool checked = false);

  void toggled(bool checked);

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  bool m_checked;

 private:
  bool m_mouseDown;
};

class ButtonItem
  : public QGraphicsObject
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  ButtonItem(const QPixmap& image, const QPixmap& downImage, const QPixmap& hoverImage, QGraphicsItem* parent = nullptr);

  virtual ~ButtonItem() {}

  QRectF boundingRect() const override;

 signals:

  void mouseClicked();

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  bool m_mouseDown;

  bool m_isHovering;

  QPixmap m_image;

  QPixmap m_downImage;

  QPixmap m_hoverImage;
};

class RemoveButtonItem : public ButtonItem
{
 public:
  explicit RemoveButtonItem(QGraphicsItem* parent = nullptr);
};

class ZoomInButtonItem : public ButtonItem
{
 public:
  explicit ZoomInButtonItem(QGraphicsItem* parent = nullptr);
};

class ZoomOutButtonItem : public ButtonItem
{
 public:
  explicit ZoomOutButtonItem(QGraphicsItem* parent = nullptr);
};

class OneLevelDownButtonItem : public ButtonItem
{
 public:
  explicit OneLevelDownButtonItem(QGraphicsItem* parent = nullptr);
};

class OneLevelUpButtonItem : public ButtonItem
{
 public:
  explicit OneLevelUpButtonItem(QGraphicsItem* parent = nullptr);
};

class GridLayoutItem : public QGraphicsObject
{
  Q_OBJECT;

 public:
  GridLayoutItem();

  virtual ~GridLayoutItem();

  void setDelegate(QSharedPointer<OSGraphicsItemDelegate> delegate);

  void setListController(QSharedPointer<OSListController> listController);

  QSharedPointer<OSListController> listController() const;

  QRectF boundingRect() const override;

  QSizeF cellSize() const;

  void setCellSize(const QSizeF& size);

  int margin() const;

  void setMargin(int margin);

 public slots:

  void refreshAllItemViews();

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override {}

 private slots:

  void insertItemView(int index);

  void removeItemView(int index);

  void removePair(QObject* object);

  void refreshItemView(int i);

 private:
  int spacing() const;

  int rows() const;

  int columns() const;

  std::pair<int, int> gridPos(int index);

  QGraphicsObject* createNewItemView(int i);

  void setItemViewGridPos(QGraphicsObject* item, std::pair<int, int> gridPos);

  QGraphicsObject* viewFromGridPos(std::pair<int, int> location);

  QSharedPointer<OSGraphicsItemDelegate> m_delegate;

  QSharedPointer<OSListController> m_listController;

  // Use this to keep the OSListItem classes around for the life of the widget
  std::map<QObject*, QSharedPointer<OSListItem>> m_widgetItemPairs;

  std::map<std::pair<int, int>, QObject*> m_gridPosItemViewPairs;

  std::map<QObject*, std::pair<int, int>> m_itemViewGridPosPairs;

  QSizeF m_cellSize;

  int m_margin;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_GRAPHICSITEMS_HPP
