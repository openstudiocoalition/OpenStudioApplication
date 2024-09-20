/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSCOLLAPSIBLEITEMLIST_HPP
#define OPENSTUDIO_OSCOLLAPSIBLEITEMLIST_HPP

#include "OSItemSelector.hpp"
#include "OSItem.hpp"
#include "OSCategoryPlaceholder.hpp"

class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;

namespace openstudio {

class OSItem;
class OSCollapsibleItem;
class OSCollapsibleItemHeader;
class OSVectorController;

class OSCollapsibleItemList : public OSItemSelector
{
  Q_OBJECT

 public:
  explicit OSCollapsibleItemList(bool addScrollArea, QWidget* parent = nullptr);

  virtual ~OSCollapsibleItemList() = default;

  void addCollapsibleItem(OSCollapsibleItem* collapsibleItem);
  void addCategoryPlaceholderItem(OSCategoryPlaceholder* categoryPlaceholderItem);

  OSCollapsibleItem* selectedCollapsibleItem() const;

  OSItem* selectedItem() const override;

  bool itemsDraggable() const;
  void setItemsDraggable(bool itemsDraggable);

  bool itemsRemoveable() const;
  void setItemsRemoveable(bool itemsRemoveable);

  OSItemType itemsType() const;
  void setItemsType(OSItemType type);

 signals:

  void openLibDlgClicked();

 private slots:

  void onCollapsableItemSelected(OSCollapsibleItem* selectedItem);

  void onItemSelected(OSItem* item);

  void onSearchTextEdited(const QString& text);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QVBoxLayout* m_vLayout;
  QLineEdit* m_searchBox;
  QHBoxLayout* m_contentLayout;
  OSCollapsibleItem* m_selectedCollapsibleItem;
  std::vector<OSCollapsibleItem*> m_collapsibleItems;
  std::vector<OSCategoryPlaceholder*> m_placeholderItems;
  bool m_searchActive;
  bool m_itemsDraggable;
  bool m_itemsRemoveable;
  OSItemType m_itemsType;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSCOLLAPSIBLEITEMLIST_HPP
