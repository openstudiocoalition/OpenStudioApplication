/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSDropZone.hpp"

#include "IconLibrary.hpp"
#include "InspectorController.hpp"
#include "InspectorView.hpp"
#include "MainRightColumnController.hpp"
#include "ModelObjectItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItem.hpp"
#include "OSVectorController.hpp"

#include <openstudio/model/Component.hpp>
#include <openstudio/model/ComponentData.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QLayoutItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStyleOption>
#include <QStyle>
#include <QTimer>

#include <bitset>
#include <utility>

using namespace openstudio::model;

namespace openstudio {

OSDropZone::OSDropZone(OSVectorController* vectorController, const QString& text, const QSize& size, bool growsHorizontally, QWidget* parent)
  : QWidget(parent),
    m_vectorController(vectorController),
    m_minItems(0),
    m_itemsDraggable(false),
    m_itemsAcceptDrops(false),
    m_itemsRemoveable(true),
    m_allowAdd(false),
    m_scrollArea(new QScrollArea()),
    m_growsHorizontally(growsHorizontally),
    m_useLargeIcon(false),
    m_text(text),
    m_size(size) {
  auto* mainBox = new QWidget();
  mainBox->setObjectName("mainBox");
  mainBox->setStyleSheet("QWidget#mainBox { background: #CECECE; }");

  m_scrollArea->setFrameStyle(QFrame::NoFrame);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setWidget(mainBox);
  m_scrollArea->setMinimumWidth(OSItem::ITEM_WIDTH);

  QBoxLayout* mainLayout = nullptr;

  if (m_growsHorizontally) {
    m_scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout = new QHBoxLayout();
    m_mainBoxLayout = new QHBoxLayout();
  } else {
    m_scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mainLayout = new QVBoxLayout();
    m_mainBoxLayout = new QVBoxLayout();
  }

  mainLayout->setContentsMargins(10, 10, 10, 10);
  setLayout(mainLayout);

  mainLayout->addWidget(m_scrollArea);

  // for now we will allow this drop zone to manage memory of the vectorController
  OS_ASSERT(!m_vectorController->parent());
  m_vectorController->setParent(this);

  m_addButton = new QPushButton(this);
  m_addButton->setFlat(true);
  m_addButton->setObjectName("AddButton");
  m_addButton->setFixedSize(24, 24);
  mainLayout->addWidget(m_addButton, 0, Qt::AlignBottom);

  setObjectName("OSDropZone");
  QString mainBoxStyle;
  mainBoxStyle.append("QWidget#OSDropZone {");
  mainBoxStyle.append(" background: #CECECE;");
  if ((m_size.height() != 0) && (m_size.width() != 0)) {
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainBoxStyle.append(" border: none;");
  } else {
    mainBoxStyle.append(" border: 2px dashed #808080;");
    mainBoxStyle.append(" border-radius: 10px;");
  }
  mainBoxStyle.append("}");
  setStyleSheet(mainBoxStyle);

  m_mainBoxLayout->setContentsMargins(0, 0, 0, 0);
  m_mainBoxLayout->setSpacing(10);
  mainBox->setLayout(m_mainBoxLayout);

  connect(m_addButton, &QPushButton::clicked, this, &OSDropZone::addButtonClicked);

  connect(this, &OSDropZone::itemsRequested, vectorController, &OSVectorController::reportItems, Qt::QueuedConnection);

  connect(this, &OSDropZone::itemRemoveClicked, vectorController, &OSVectorController::removeItem);

  connect(this, &OSDropZone::itemReplacementDropped, vectorController, &OSVectorController::replaceItem);

  connect(this, &OSDropZone::itemDropped, vectorController, &OSVectorController::drop);

  connect(this, &OSDropZone::addButtonClicked, vectorController, &OSVectorController::makeNewItem);

  connect(vectorController, &OSVectorController::itemIds, this, &OSDropZone::setItemIds);

  emit itemsRequested();

  setMaxItems(std::numeric_limits<int>::max());

  hideAddButton();
}

void OSDropZone::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

int OSDropZone::maxItems() const {
  return m_maxItems;
}

bool OSDropZone::setMaxItems(int max) {
  if (max >= m_minItems && max >= 0) {
    m_maxItems = max;
    emit itemsRequested();
    if (max == 1) {
      if ((m_size.height() != 0) && (m_size.width() != 0)) {
        m_scrollArea->setFixedHeight(m_size.height());
        m_scrollArea->setMaximumWidth(m_size.width());
        setMaximumHeight(m_size.height());
        setMaximumWidth(m_size.width());
      } else if (m_growsHorizontally) {
        m_scrollArea->setFixedHeight(OSItem::ITEM_HEIGHT);
        m_scrollArea->setMaximumWidth(OSItem::ITEM_WIDTH);
        setMaximumWidth(OSItem::ITEM_WIDTH + 20);
      } else {
        m_scrollArea->setMaximumHeight(OSItem::ITEM_HEIGHT);
        m_scrollArea->setFixedWidth(OSItem::ITEM_WIDTH);
        setMaximumHeight(OSItem::ITEM_HEIGHT + 20);
      }
    } else {
      if ((m_size.height() != 0) && (m_size.width() != 0)) {
        m_scrollArea->setFixedHeight(m_size.height());
        m_scrollArea->setMaximumWidth(m_size.width());
        setMaximumHeight(m_size.height());
        setMaximumWidth(m_size.width());
      } else if (m_growsHorizontally) {
        m_scrollArea->setFixedHeight(OSItem::ITEM_SIDE);
        m_scrollArea->setMaximumWidth(QWIDGETSIZE_MAX);
        setMaximumWidth(QWIDGETSIZE_MAX);
      } else {
        m_scrollArea->setFixedWidth(OSItem::ITEM_WIDTH);
        m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
        setMaximumHeight(QWIDGETSIZE_MAX);
      }
    }
    return true;
  }
  return false;
}

int OSDropZone::minItems() const {
  return m_minItems;
}

bool OSDropZone::setMinItems(int min) {
  if (min <= m_maxItems && min >= 0) {
    m_minItems = min;
    emit itemsRequested();
    return true;
  }
  return false;
}

bool OSDropZone::itemsDraggable() const {
  return m_itemsDraggable;
}

void OSDropZone::setItemsDraggable(bool itemsDraggable) {
  m_itemsDraggable = itemsDraggable;
}

bool OSDropZone::itemsAcceptDrops() const {
  return m_itemsAcceptDrops;
}

void OSDropZone::setItemsAcceptDrops(bool itemsAcceptDrops) {
  m_itemsAcceptDrops = itemsAcceptDrops;
}

bool OSDropZone::itemsRemoveable() const {
  return m_itemsRemoveable;
}

void OSDropZone::setItemsRemoveable(bool itemsRemoveable) {
  m_itemsRemoveable = itemsRemoveable;
}

void OSDropZone::onDrop(const OSItemId& itemId) {
  emit itemDropped(itemId);
}

void OSDropZone::setItemIds(const std::vector<OSItemId>& itemIds) {
  QLayoutItem* child = nullptr;
  while ((child = m_mainBoxLayout->takeAt(0)) != nullptr) {
    QWidget* widget = child->widget();

    delete widget;

    delete child;
  }

  int numItems = 0;
  OSItemType type;
  if ((this->m_maxItems > 1) && (m_growsHorizontally)) {
    type = OSItemType::DropzoneSquare;
    // somewhere set show / hide scroll bar TODO
  } else {
    type = OSItemType::DropzoneRectangle;
  }

  for (const OSItemId& itemId : itemIds) {

    OSItem* item = OSItem::makeItem(itemId, type);
    if (!item) {
      continue;
    }

    // Set the position of the item in the dropzone
    item->setPosition(numItems);

    ++numItems;

    connect(item, &OSItem::itemRemoveClicked, this, &OSDropZone::itemRemoveClicked);

    connect(item, &OSItem::itemClicked, this, &OSDropZone::itemClicked);

    connect(item, &OSItem::itemReplacementDropped, this, &OSDropZone::itemReplacementDropped);

    item->setDraggable(m_itemsDraggable);

    item->setAcceptDrops(m_itemsAcceptDrops);

    item->setUseLargeIcon(m_useLargeIcon);

    if (item->removeable()) {
      if (numItems > m_minItems) {
        item->setRemoveable(true);
      } else {
        item->setRemoveable(false);
      }
    }

    if (!m_itemsRemoveable) {
      item->setRemoveable(false);
    }

    m_mainBoxLayout->addWidget(item, 0, Qt::AlignLeft);
  }

  if (numItems < m_maxItems) {
    auto* dropZone = new OSItemDropZone(this->m_growsHorizontally, m_text, m_size);
    m_mainBoxLayout->addWidget(dropZone, 0, Qt::AlignLeft);

    connect(dropZone, &OSItemDropZone::dropped, this, &OSDropZone::handleDrop);

    if (m_maxItems == 1) {
      dropZone->setExtensible(false);
    } else {
      dropZone->setExtensible(true);
    }

    if (m_allowAdd) {
      this->showAddButton();
    }

  } else {
    this->hideAddButton();
  }

  m_mainBoxLayout->addStretch();
}

void OSDropZone::handleDrop(QDropEvent* event) {
  OSItemId itemId(event->mimeData());
  this->onDrop(itemId);
}

void OSDropZone::showAddButton() {
  m_allowAdd = true;
  m_addButton->show();
}

void OSDropZone::hideAddButton() {
  m_addButton->hide();
}

bool OSDropZone::useLargeIcon() const {
  return m_useLargeIcon;
}

void OSDropZone::setUseLargeIcon(bool useLargeIcon) {
  m_useLargeIcon = useLargeIcon;
}

OSItemDropZone::OSItemDropZone(bool growsHorizontally, const QString& text, const QSize& size, QWidget* parent)
  : QWidget(parent), m_size(size), m_growsHorizontally(growsHorizontally) {
  setAcceptDrops(true);

  setObjectName("DropBox");

  QBoxLayout* mainLayout = nullptr;

  if (m_growsHorizontally) {
    mainLayout = new QHBoxLayout();
  } else {
    mainLayout = new QVBoxLayout();
  }
  mainLayout->setContentsMargins(10, 10, 10, 10);
  setLayout(mainLayout);

  auto* label = new QLabel();
  label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  label->setText(text);
  label->setWordWrap(true);

  label->setStyleSheet("QLabel { font: bold; color: #808080}");

  mainLayout->addWidget(label, 0, Qt::AlignCenter);

  setExtensible(false);
}

void OSItemDropZone::setExtensible(bool extensible) {
  QString style;

  if ((m_size.height() != 0) && (m_size.width() != 0)) {
    style.append("QWidget#DropBox {");
    style.append(" background: #CECECE;");
    style.append(" border: 2px dashed #808080;");
    style.append(" border-radius: 3px;");
    style.append("}");

    setFixedWidth(m_size.width());
    setFixedHeight(m_size.height());
  } else if (extensible) {
    style.append("QWidget#DropBox {");
    style.append(" background: #CECECE;");
    style.append(" border: 2px dashed #808080;");
    style.append(" border-radius: 10px;");
    style.append("}");

    if (m_growsHorizontally) {
      setFixedWidth(OSItem::ITEM_SIDE);
      setFixedHeight(OSItem::ITEM_SIDE);
    } else {
      setFixedWidth(OSItem::ITEM_WIDTH);
      setFixedHeight(OSItem::ITEM_HEIGHT);
    }
  } else {
    style.append("QWidget#DropBox {");
    style.append(" background: #CECECE;");
    style.append(" border: none;");
    style.append("}");

    setFixedWidth(OSItem::ITEM_WIDTH);
    setFixedHeight(OSItem::ITEM_HEIGHT);
  }

  setStyleSheet(style);
}

void OSItemDropZone::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OSItemDropZone::dragEnterEvent(QDragEnterEvent* event) {
  if (event->proposedAction() == Qt::CopyAction) {
    event->accept();
  }
}

void OSItemDropZone::dropEvent(QDropEvent* event) {
  event->accept();
  if (event->proposedAction() == Qt::CopyAction) {
    emit dropped(event);
  }
}

OSDropZoneItem::OSDropZoneItem() : m_mouseDown(false) {
  setAcceptHoverEvents(true);
  setAcceptDrops(true);

  setSize(100, 50);
}

void OSDropZoneItem::dropEvent(QGraphicsSceneDragDropEvent* event) {
  event->accept();

  if (event->proposedAction() == Qt::CopyAction) {
    OSItemId id = OSItemId(event->mimeData());

    emit componentDropped(id);
  }
}

QRectF OSDropZoneItem::boundingRect() const {
  return {0.0, 0.0, m_width, m_height};
}

void OSDropZoneItem::setSize(double width, double height) {
  prepareGeometryChange();
  m_width = width;
  m_height = height;
}

void OSDropZoneItem::setText(const QString& text) {
  m_text = text;
  update();
}

void OSDropZoneItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(QColor(109, 109, 109), 2, Qt::DashLine, Qt::RoundCap));

  painter->drawRect(boundingRect());

  QFont font = painter->font();
  font.setPixelSize(24);
  painter->setFont(font);
  painter->setPen(QPen(QColor(109, 109, 109), 2, Qt::DashLine, Qt::RoundCap));
  painter->drawText(boundingRect(), Qt::AlignCenter | Qt::TextWordWrap, m_text);
}

void OSDropZoneItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_mouseDown = true;

  this->update();

  event->accept();
}

void OSDropZoneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_mouseDown) {
    m_mouseDown = false;

    this->update();

    QApplication::processEvents();

    if (shape().contains(event->pos())) {
      event->accept();

      emit mouseClicked();
    }
  }
}

OSDropZone2::OSDropZone2() : m_label(new QLabel()) {
  setObjectName("OSDropZone");

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet(
    "QWidget#OSDropZone[style=\"000\"] { border: 2px dashed #808080; border-radius: 5px; background:#ffffff; } "  // Locked=0, Focused=0, Defaulted=0
    "QWidget#OSDropZone[style=\"001\"] { border: 2px dashed #808080; border-radius: 5px; background:#ffffff; } "  // Locked=0, Focused=0, Defaulted=1
    "QWidget#OSDropZone[style=\"010\"] { border: 2px dashed #808080; border-radius: 5px; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=0
    "QWidget#OSDropZone[style=\"011\"] { border: 2px dashed #808080; border-radius: 5px; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=1
    "QWidget#OSDropZone[style=\"100\"] { border: 2px dashed #808080; border-radius: 5px; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=0
    "QWidget#OSDropZone[style=\"101\"] { border: 2px dashed #808080; border-radius: 5px; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=1
    "QWidget#OSDropZone[style=\"110\"] { border: 2px dashed #808080; border-radius: 5px; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=0
    "QWidget#OSDropZone[style=\"111\"] { border: 2px dashed #808080; border-radius: 5px; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=1
  );

  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(5, 5, 5, 5);
  setLayout(layout);

  layout->addWidget(m_label);
  m_label->setStyleSheet("QLabel[style=\"000\"] { color:black; background:#ffffff; } "  // Locked=0, Focused=0, Defaulted=0
                         "QLabel[style=\"001\"] { color:green; background:#ffffff; } "  // Locked=0, Focused=0, Defaulted=1
                         "QLabel[style=\"010\"] { color:black; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=0
                         "QLabel[style=\"011\"] { color:green; background:#ffc627; } "  // Locked=0, Focused=1, Defaulted=1
                         "QLabel[style=\"100\"] { color:black; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=0
                         "QLabel[style=\"101\"] { color:green; background:#e6e6e6; } "  // Locked=1, Focused=0, Defaulted=1
                         "QLabel[style=\"110\"] { color:black; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=0
                         "QLabel[style=\"111\"] { color:green; background:#cc9a00; } "  // Locked=1, Focused=1, Defaulted=1
  );

  setFixedHeight(25);
  setMinimumWidth(75);
  setMaximumWidth(150);
}

OSDropZone2::~OSDropZone2() = default;

void OSDropZone2::enableClickFocus() {
  m_hasClickFocus = true;
  this->setFocusPolicy(Qt::ClickFocus);
}

void OSDropZone2::disableClickFocus() {
  m_hasClickFocus = false;
  this->setFocusPolicy(Qt::NoFocus);
  clearFocus();
}

bool OSDropZone2::hasData() const {
  return !this->m_label->text().isEmpty();
}

bool OSDropZone2::locked() const {
  return m_locked;
}

void OSDropZone2::setLocked(bool locked) {
  m_locked = locked;
  if (locked) {
    setAcceptDrops(false);
  }
  updateStyle();
}

void OSDropZone2::setDeleteObject(bool deleteObject) {
  m_deleteObject = deleteObject;
}

bool OSDropZone2::deleteObject() const {
  return m_deleteObject;
}

void OSDropZone2::refresh() {

  boost::optional<model::ModelObject> getterResult = updateGetterResult();

  updateOtherModelObjects();

  if (getterResult) {

    QString temp = QString::fromStdString(getterResult->name().get());
    if (m_label->text() != temp) {
      m_label->setText(temp);
      m_label->setToolTip(temp);
    }

    //// Adjust the width to accommodate the text
    //QFont myFont;
    //QFontMetrics fm(myFont);
    //auto width = fm.width(m_text);
    //setFixedWidth(width + 10);
  } else {

    if (!m_label->text().isEmpty()) {
      m_label->setText("");
      m_label->setToolTip("");
    }
  }

  updateStyle();
  update();
}

void OSDropZone2::onModelObjectRemove(const Handle& /*handle*/) {
  refresh();
}

void OSDropZone2::onOtherModelObjectRemove(const Handle& /*handle*/) {
  refresh();
}

void OSDropZone2::bind(const model::ModelObject& modelObject, OptionalModelObjectGetter get, ModelObjectSetter set,
                       boost::optional<NoFailAction> reset, boost::optional<ModelObjectIsDefaulted> isDefaulted,
                       boost::optional<OtherModelObjects> otherObjects) {
  m_get = get;
  m_set = set;
  m_reset = std::move(reset);
  m_isDefaulted = std::move(isDefaulted);
  m_otherObjects = std::move(otherObjects);

  m_modelObject = modelObject;

  setAcceptDrops(true);

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    ->openstudio::model::detail::ModelObject_Impl::onChange.connect<OSDropZone2, &OSDropZone2::refresh>(this);
  refresh();
}

void OSDropZone2::unbind() {

  m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
    ->openstudio::model::detail::ModelObject_Impl::onChange.disconnect<OSDropZone2, &OSDropZone2::refresh>(this);

  m_modelObject.reset();
  m_get.reset();
  updateGetterResult();
  m_set.reset();
  m_reset.reset();
  m_isDefaulted.reset();
  m_otherObjects.reset();
  updateOtherModelObjects();

  setAcceptDrops(false);

  refresh();
}

void OSDropZone2::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

  //p.drawText(rect(),Qt::AlignCenter,m_text);
}

void OSDropZone2::dragEnterEvent(QDragEnterEvent* event) {
  if (event->proposedAction() == Qt::CopyAction) {
    event->accept();
  }
}

void OSDropZone2::dropEvent(QDropEvent* event) {
  event->accept();

  std::shared_ptr<OSDocument> doc = OSAppBase::instance()->currentDocument();

  if ((event->proposedAction() == Qt::CopyAction) && m_modelObject && m_set) {

    OSItemId itemId(event->mimeData());

    boost::optional<model::ModelObject> modelObject;

    // component data is only available from BCL components
    boost::optional<model::Component> component;
    boost::optional<model::ComponentData> componentData;

    // If what you dragged is from the BCL, then VT it and insert it in model
    // TODO: should we modify OSDocument::getModelObject instead?
    // DLM: initially thought so but we also want to keep track of the component data as well
    if (doc->fromBCL(itemId)) {
      component = doc->getComponent(itemId);
      if (component) {
        if (component->primaryObject().optionalCast<model::ModelObject>()) {
          componentData = doc->model().insertComponent(*component);
          if (componentData) {
            modelObject = componentData->primaryComponentObject();
          }
        }
      }
    } else {
      modelObject = doc->getModelObject(itemId);
    }

    OS_ASSERT(modelObject);

    bool success = false;
    if (doc->fromBCL(itemId)) {
      // model object already cloned above
      OS_ASSERT(componentData);
      success = (*m_set)(modelObject.get());
      if (!success) {
        std::vector<Handle> handlesToRemove;
        for (const auto& object : componentData->componentObjects()) {
          handlesToRemove.push_back(object.handle());
        }
        doc->model().removeObjects(handlesToRemove);
        // removing objects in component will remove component data object via component watcher
        //componentData->remove();
        OS_ASSERT(componentData->handle().isNull());
      }
    } else if (doc->fromComponentLibrary(itemId)) {
      modelObject = modelObject->clone(m_modelObject->model());
      success = (*m_set)(modelObject.get());
      if (!success) {
        modelObject->remove();
      }
    } else {
      if (m_set) {
        success = (*m_set)(modelObject.get());
      }
    }

    if (success) {
      delete m_item;
      m_item = OSItem::makeItem(itemId, OSItemType::ListItem);
      m_item->setParent(this);
      connect(m_item, &OSItem::itemRemoveClicked, this, &OSDropZone2::onItemRemoveClicked);
    }

    refresh();
  }
}

void OSDropZone2::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    event->accept();

    if (!m_item) {
      makeItem();
    }

    if (m_item) {
      emit itemClicked(m_item);
    }
  }
}

void OSDropZone2::focusInEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason) {
    if (hasData()) {
      m_focused = true;
      updateStyle();
      emit inFocus(m_focused, true);
    }
  }

  QWidget::focusInEvent(e);
}

void OSDropZone2::focusOutEvent(QFocusEvent* e) {
  if (e->reason() == Qt::MouseFocusReason) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);

    auto mouseOverInspectorView =
      OSAppBase::instance()->currentDocument()->mainRightColumnController()->inspectorController()->inspectorView()->mouseOverInspectorView();
    if (!mouseOverInspectorView) {
      emit itemClicked(nullptr);
    }
  }

  QWidget::focusOutEvent(e);
}

void OSDropZone2::onItemRemoveClicked() {
  if (m_reset && !m_locked) {
    boost::optional<model::ModelObject> modelObject = updateGetterResult();
    boost::optional<model::ParentObject> parent = boost::none;
    if (modelObject) {
      parent = modelObject->parent();
    }
    (*m_reset)();
    if (modelObject && m_deleteObject) {
      modelObject->remove();
    }
    if (m_item) {
      delete m_item;
      m_item = nullptr;
    }
    refresh();
    emit objectRemoved(parent);
  }
}

void OSDropZone2::updateStyle() {

  bool thisDefaulted = false;
  if (m_isDefaulted) {
    thisDefaulted = (*m_isDefaulted)(*m_modelObject);
  }

  // Locked, Focused, Defaulted
  std::bitset<3> style;
  style[0] = thisDefaulted;
  style[1] = m_focused;
  style[2] = m_locked;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);

    m_label->setProperty("style", thisStyle);
    m_label->style()->unpolish(m_label);
    m_label->style()->polish(m_label);
  }
}

void OSDropZone2::makeItem() {
  if (!m_item) {
    boost::optional<model::ModelObject> modelObject = updateGetterResult();
    if (modelObject) {

      bool isDefaulted = false;
      if (m_isDefaulted && m_getterResult) {
        isDefaulted = (*m_isDefaulted)(*m_getterResult);
      }

      // override isDefaulted to prevent deleting in the inspector
      if (m_locked) {
        isDefaulted = true;
      }

      m_item = OSItem::makeItem(modelObjectToItemId(*modelObject, isDefaulted));
      m_item->setParent(this);

      if (!isDefaulted && m_reset) {
        m_item->setRemoveable(true);
      } else {
        m_item->setRemoveable(false);
      }
      connect(m_item, &OSItem::itemRemoveClicked, this, &OSDropZone2::onItemRemoveClicked);
    }
  }
}

boost::optional<model::ModelObject> OSDropZone2::updateGetterResult() {
  if (!m_get) {
    if (m_getterResult) {
      // we don't care about this object anymore
      m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSDropZone2, &OSDropZone2::refresh>(this);
      m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()
        ->onRemoveFromWorkspace.disconnect<OSDropZone2, &OSDropZone2::onModelObjectRemove>(this);
      m_getterResult.reset();
    }
    return boost::none;
  }

  boost::optional<model::ModelObject> newGetterResult = (*m_get)();

  if (m_getterResult == newGetterResult) {
    // no change
    return m_getterResult;
  }

  if (m_getterResult) {
    // we don't care about this object anymore
    m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSDropZone2, &OSDropZone2::refresh>(this);
    m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSDropZone2, &OSDropZone2::onModelObjectRemove>(this);
  }

  m_getterResult = newGetterResult;

  if (m_getterResult) {
    m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSDropZone2, &OSDropZone2::refresh>(this);
    m_getterResult->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.connect<OSDropZone2, &OSDropZone2::onModelObjectRemove>(this);
  }

  return m_getterResult;
}

void OSDropZone2::updateOtherModelObjects() {

  if (!m_modelObject || !m_otherObjects) {
    // we don't care about these other objects anymore
    for (const auto& otherModelObject : m_otherModelObjects) {
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSDropZone2, &OSDropZone2::refresh>(this);
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()
        ->onRemoveFromWorkspace.disconnect<OSDropZone2, &OSDropZone2::onOtherModelObjectRemove>(this);
    }
    m_otherModelObjects.clear();
    return;
  }

  std::set<model::ModelObject, ModelObjectHandleLess> newOtherModelObjects;
  for (const auto& otherModelObject : (*m_otherObjects)(*m_modelObject)) {
    if (m_otherModelObjects.find(otherModelObject) == m_otherModelObjects.end()) {
      // new other object, connect to it
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSDropZone2, &OSDropZone2::refresh>(this);
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()
        ->onRemoveFromWorkspace.connect<OSDropZone2, &OSDropZone2::onOtherModelObjectRemove>(this);
    }
    newOtherModelObjects.insert(otherModelObject);
  }

  for (const auto& otherModelObject : m_otherModelObjects) {
    if (newOtherModelObjects.find(otherModelObject) == newOtherModelObjects.end()) {
      // removed other object, disconnect from it
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSDropZone2, &OSDropZone2::refresh>(this);
      otherModelObject.getImpl<openstudio::model::detail::ModelObject_Impl>()
        ->onRemoveFromWorkspace.disconnect<OSDropZone2, &OSDropZone2::onOtherModelObjectRemove>(this);
    }
  }

  m_otherModelObjects = newOtherModelObjects;
}

}  // namespace openstudio
