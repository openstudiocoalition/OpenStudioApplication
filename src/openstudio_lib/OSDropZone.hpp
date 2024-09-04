/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSDROPZONE_HPP
#define OPENSTUDIO_OSDROPZONE_HPP

#include "OSItem.hpp"
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include "../shared_gui_components/FieldMethodTypedefs.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>

class QBoxLayout;
class QDropEvent;
class QDragEnterEvent;
class QDragLeaveEvent;
class QFocusEvent;
class QPushButton;
class QScrollArea;
class QLabel;

class OpenStudioLibFixture;

namespace openstudio {

class OSItem;
class OSVectorController;

// This is a "bindable" version of drop zone
// that looks different because it is initially designed to go
// in a grid.  It also works differently because it binds to function pointers.
// There is no use of VectorController like in OSDropZone.
class OSDropZone2
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  OSDropZone2();

  ~OSDropZone2();

  void enableClickFocus();
  void disableClickFocus();
  bool hasData() const;
  bool locked() const;
  void setLocked(bool locked);
  void setDeleteObject(bool deleteObject);
  bool deleteObject() const;

  void bind(const model::ModelObject& modelObject, OptionalModelObjectGetter get, ModelObjectSetter set,
            boost::optional<NoFailAction> reset = boost::none, boost::optional<ModelObjectIsDefaulted> isDefaulted = boost::none,
            boost::optional<OtherModelObjects> otherObjects = boost::none);

  void unbind();

 signals:

  void itemClicked(OSItem* item);
  void objectRemoved(boost::optional<model::ParentObject> parent);
  void inFocus(bool inFocus, bool hasData);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  virtual void focusInEvent(QFocusEvent* e) override;
  virtual void focusOutEvent(QFocusEvent* e) override;

 public slots:

  void onItemRemoveClicked();

 private slots:

  void refresh();
  void onModelObjectRemove(const Handle& handle);
  void onOtherModelObjectRemove(const Handle& handle);
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  // For testing
  friend class ::OpenStudioLibFixture;

  void updateStyle();
  void makeItem();
  boost::optional<model::ModelObject> updateGetterResult();
  void updateOtherModelObjects();

  struct ModelObjectHandleLess
  {
    bool operator()(const model::ModelObject& lhs, const model::ModelObject& rhs) const {
      return lhs.handle() < rhs.handle();
    }
  };

  boost::optional<OptionalModelObjectGetter> m_get;
  boost::optional<ModelObjectSetter> m_set;
  boost::optional<NoFailAction> m_reset;
  boost::optional<ModelObjectIsDefaulted> m_isDefaulted;
  boost::optional<OtherModelObjects> m_otherObjects;
  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<model::ModelObject> m_getterResult;
  std::set<model::ModelObject, ModelObjectHandleLess> m_otherModelObjects;
  bool m_hasClickFocus = false;
  bool m_focused = false;
  bool m_locked = false;
  //QString m_text;
  OSItem* m_item = nullptr;
  bool m_deleteObject = false;
  QLabel* m_label;
};

class OSDropZone
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit OSDropZone(OSVectorController* vectorController, const QString& text = "Drag From Library", const QSize& size = QSize(0, 0),
                      bool growsHorizontally = true, QWidget* parent = nullptr);

  virtual ~OSDropZone() = default;

  int maxItems() const;
  bool setMaxItems(int max);

  int minItems() const;
  bool setMinItems(int min);

  bool itemsDraggable() const;
  void setItemsDraggable(bool itemsDraggable);

  bool itemsAcceptDrops() const;
  void setItemsAcceptDrops(bool itemsAcceptDrops);

  bool itemsRemoveable() const;
  void setItemsRemoveable(bool itemsRemoveable);

  void showAddButton();
  void hideAddButton();

  bool useLargeIcon() const;
  void setUseLargeIcon(bool useLargeIcon);

 signals:

  // request items from vector controller
  void itemsRequested();

  // emitted in onDrop if item is created
  void itemDropped(const OSItemId& itemId);

  // emitted if item is clicked
  void itemClicked(OSItem* item);

  // emitted if item remove is clicked
  void itemRemoveClicked(OSItem* item);

  // emitted if item is dropped on existing item
  void itemReplacementDropped(OSItem* currentItem, const OSItemId& replacementItemId);

  void addButtonClicked();

 protected:
  // called when drop occurs, emit onDrop here if needed
  virtual void onDrop(const OSItemId& itemId);

  void paintEvent(QPaintEvent* event) override;

  //void resizeEvent(QResizeEvent * event);

 private slots:

  // set this objects item ids
  void setItemIds(const std::vector<OSItemId>& itemIds);

  // called on drop
  void handleDrop(QDropEvent* event);

 private:
  OSVectorController* m_vectorController;
  int m_minItems;
  int m_maxItems;
  bool m_itemsDraggable;
  bool m_itemsAcceptDrops;
  bool m_itemsRemoveable;
  bool m_allowAdd;
  QBoxLayout* m_mainBoxLayout;
  QScrollArea* m_scrollArea;
  QPushButton* m_addButton;
  bool m_growsHorizontally;
  bool m_useLargeIcon;
  QString m_text;
  QSize m_size;
};

class OSItemDropZone : public QWidget
{
  Q_OBJECT

 public:
  OSItemDropZone(bool growsHorizontally, const QString& text, const QSize& size, QWidget* parent = nullptr);

  virtual ~OSItemDropZone() = default;

  void setExtensible(bool extensible);

 signals:
  void dropped(QDropEvent* event);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  QSize m_size;
  bool m_growsHorizontally;
};

// This is a version of OSDropZone that works with QGraphicsScene
// as opposed to QWidget.  Much of the custom drawing in the HVAC
// interface is done with QGraphicsScene.
class OSDropZoneItem
  : public QGraphicsObject
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  OSDropZoneItem();

  virtual ~OSDropZoneItem() = default;

  QRectF boundingRect() const override;

  void setSize(double width, double height);

  void setText(const QString& text);

 signals:

  void mouseClicked();

  void componentDropped(const OSItemId& itemid);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  QString m_text;

 private:
  bool m_mouseDown;

  double m_width;
  double m_height;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSDROPZONE_HPP
