/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSITEM_HPP
#define OPENSTUDIO_OSITEM_HPP

#include "../shared_gui_components/LocalLibrary.hpp"
#include "../shared_gui_components/OSItemId.hpp"

#include <QVariant>
#include <QWidget>
#include <vector>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

class QDragEnterEvent;
class QDropEvent;
class QLabel;
class QPushButton;

namespace openstudio {

class MeasureBadge;

/**
 * OSItem is the base class for all draggable/selectable items displayed in list views and drop
 * zones throughout the application. Each OSItem wraps an OSItemId (module + handle pair) and
 * provides consistent drag-and-drop support, selection highlighting, and remove-button behaviour.
 */
class OSItem
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  // creates a new OSItem if possible
  static OSItem* makeItem(const OSItemId& itemId, OSItemType osItemType = OSItemType::ListItem);

  explicit OSItem(const OSItemId& itemId, OSItemType osItemType = OSItemType::ListItem, QWidget* parent = nullptr);

  virtual ~OSItem() {}

  OSItemId itemId() const;

  bool isDefaulted() const;
  void setIsDefaulted(bool isDefaulted);

  bool selected() const;
  void setSelected(bool selected);

  ///! is the remove button visible
  bool removeable() const;
  ///! show / hide the remove button
  void setRemoveable(bool removeable);

  bool draggable() const;
  void setDraggable(bool draggable);

  bool inspectable() const;
  void setInspectable(bool inspectable);

  QString text() const;
  void setText(const QString& text);

  QPixmap leftPixmap() const;
  void setLeftPixmap(const QPixmap& pixmap);

  QPixmap rightPixmap() const;
  void setRightPixmap(const QPixmap& pixmap);

  void setAspectRatio(AspectRatio aspectRatio);

  OSItemType osItemType() const;
  void setOSItemType(OSItemType osItemType);

  bool useLargeIcon() const;
  void setUseLargeIcon(bool useLargeIcon);

  virtual bool equal(const OSItem* other) const = 0;

  ///! The side length, if a square
  static const int ITEM_SIDE = 80;
  ///! The height, if a rectangle
  static const int ITEM_HEIGHT = 60;
  ///! The width, if a rectangle
  static const int ITEM_WIDTH = 150;
  ///! The height, if a header
  static const int HEADER_HEIGHT = 40;
  ///! The width, if a header
  static const int HEADER_WIDTH = ITEM_WIDTH;

  static const int IMAGE_HEIGHT = 30;
  static const int IMAGE_WIDTH = 30;
  static const int BTN_HEIGHT = 20;
  static const int BTN_WIDTH = 20;

  boost::optional<int> position() const;
  void setPosition(int position);

 signals:
  void itemClicked(OSItem*);
  void itemRemoveClicked(OSItem*);
  void itemReplacementDropped(OSItem* currentItem, const OSItemId& replacementItemId);

 public slots:
  void onRemoveClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;
  //void resizeEvent(QResizeEvent* event);
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

  bool isBold();
  void setBold(bool isBold);

  QColor textColor();
  void setTextColor(const QColor& color = Qt::black);
  MeasureBadge* m_measureBadge;

 private:
  void setFixedSize(const QSize& size);
  void setFixedWidth(int width);
  void setFixedHeight(int height);
  void setAttributes(OSItemType osItemType);
  void createLayout();
  //void setItemSize();
  static void setLabelPixmap(QLabel* label, const QPixmap& pixmap);

  OSItemId m_itemId;
  QWidget* m_selectionWidget;
  QWidget* m_borderWidget;
  QPushButton* m_removeButton;
  QLabel* m_textLbl;
  QLabel* m_imageLeftLbl;
  QLabel* m_imageRightLbl;
  QPoint m_dragStartPosition;
  bool m_mouseDown;
  bool m_selected;
  bool m_draggable;
  bool m_inspectable;
  bool m_acceptsDrops;
  QSize m_size;
  OSItemType m_osItemType;
  QColor m_borderColor;
  bool m_useLargeIcon;
  // Large icon used behind everything else
  // For items that map to model objects, this will be set to the icon as opposed to the mini icon mapped to the type.
  QPixmap m_largePixmap;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSITEM_HPP
