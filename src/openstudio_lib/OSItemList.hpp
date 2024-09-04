/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSITEMLIST_HPP
#define OPENSTUDIO_OSITEMLIST_HPP

#include "OSItemSelector.hpp"
#include "OSItem.hpp"

class QVBoxLayout;

namespace openstudio {

class OSItem;
class OSVectorController;

class OSItemList : public OSItemSelector
{
  Q_OBJECT
 public:
  OSItemList(OSVectorController* vectorController, bool addScrollArea, QWidget* parent = nullptr);

  virtual ~OSItemList() = default;

  // return the selected item in the list, NULL if list is empty
  OSItem* selectedItem() const override;

  // return the first item in the list, NULL if list is empty
  OSItem* firstItem();

  // return the last item in the list, NULL if list is empty
  OSItem* lastItem();

  // return all items in the list
  std::vector<OSItem*> items();

  OSVectorController* vectorController() const;

  bool itemsDraggable() const;
  void setItemsDraggable(bool itemsDraggable);

  bool itemsRemoveable() const;
  void setItemsRemoveable(bool itemsRemoveable);

  OSItemType itemsType() const;
  void setItemsType(OSItemType type);

 public slots:

  // adds an item, optionally selects it
  void addItem(OSItem* item, bool selectItem = true);

  // selects an item, emits objectSelected if object is selected, emits selectionCleared if item is null or not found
  void selectItem(OSItem* selectItem);

  // selects an item by id
  void selectItemId(const OSItemId& itemId);

  // clears the selection, does not emit selectionCleared
  void clearSelection();

 private slots:

  void setItemIds(const std::vector<OSItemId>& itemIds);

 signals:

  void itemsRequested();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  OSVectorController* m_vectorController;
  QVBoxLayout* m_vLayout;
  OSItem* m_selectedItem;
  bool m_itemsDraggable;
  bool m_itemsRemoveable;
  OSItemType m_type;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSITEMLIST_HPP
