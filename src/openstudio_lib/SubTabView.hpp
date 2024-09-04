/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SUBTABVIEW_HPP
#define OPENSTUDIO_SUBTABVIEW_HPP

#include "MainTabView.hpp"
#include "OSInspectorView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QSplitter>

class QPushButton;
class QScrollArea;

namespace openstudio {

class OSItemSelector;
class OSItemSelectorButtons;
class OSItem;
class OSItemId;

class SubTabView : public QSplitter
{
  Q_OBJECT

 public:
  SubTabView(OSItemSelector* itemSelector, OSInspectorView* inspectorView, bool showGridViewLayout = false, QWidget* parent = nullptr);

  virtual ~SubTabView() {}

  OSItemSelector* itemSelector();

  OSItemSelectorButtons* itemSelectorButtons();

  OSInspectorView* inspectorView();

  const OSItemSelector* itemSelector() const;

  const OSItemSelectorButtons* itemSelectorButtons() const;

  const OSInspectorView* inspectorView() const;

 signals:

  void itemSelected(OSItem* item);

  void itemRemoveClicked(OSItem* item);

  void itemReplacementDropped(OSItem* currentItem, const OSItemId& replacementItemId);

  void selectionCleared();

  void itemDropped(const OSItemId& itemId);

  void addClicked();

  void copyClicked();

  void removeClicked();

  void purgeClicked();

  void dropZoneItemClicked(OSItem* item);

  void downloadComponentsClicked();

  void openLibDlgClicked();

 protected slots:

  virtual void onDropZoneItemClicked(OSItem* item);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void connectItemSelector();

  void connectInspectorView();

  void connectItemSelectorButtons();

  void createLayout();

  void createGridViewLayout();

  QScrollArea* m_selectorScrollArea;

  OSItemSelector* m_itemSelector;

  OSItemSelectorButtons* m_itemSelectorButtons;

  OSInspectorView* m_inspectorView;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SUBTABVIEW_HPP
