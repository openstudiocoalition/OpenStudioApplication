/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSCOLLAPSIBLEITEM_HPP
#define OPENSTUDIO_OSCOLLAPSIBLEITEM_HPP

#include <QWidget>
#include "OSItem.hpp"

class QButtonGroup;
class QComboBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QVBoxLayout;

namespace openstudio {

class OSCollapsibleItemHeader;
class OSItemId;
class OSItemList;

class OSCollapsibleItem : public QWidget
{
  Q_OBJECT

 public:
  OSCollapsibleItem(OSCollapsibleItemHeader* collapsibleItemHeader, OSItemList* itemList, QWidget* parent = nullptr);

  virtual ~OSCollapsibleItem() {}

  bool isSelected() const;
  void setSelected(bool selected);

  bool expanded() const;
  void setExpanded(bool expanded);

  OSCollapsibleItemHeader* collapsibleItemHeader() const;

  OSItemList* itemList() const;

  bool itemsDraggable() const;
  void setItemsDraggable(bool itemsDraggable);

  bool itemsRemoveable() const;
  void setItemsRemoveable(bool itemsRemoveable);

  OSItemType itemsType() const;
  void setItemsType(OSItemType type);

 signals:

  void collapsableItemSelected(OSCollapsibleItem*);

  void itemSelected(OSItem* item);

  void itemRemoveClicked(OSItem* item);

  void itemReplacementDropped(OSItem* currentItem, const OSItemId& replacementItemId);

  void selectionCleared();

  void openLibDlgClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private slots:

  void onHeaderClicked(OSCollapsibleItemHeader* header);

 private:
  void createLayout();

  OSCollapsibleItemHeader* m_collapsibleItemHeader;
  OSItemList* m_itemList;
  QVBoxLayout* m_mainLayout;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSCOLLAPSIBLEITEM_HPP
