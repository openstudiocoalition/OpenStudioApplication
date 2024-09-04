/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <QKeyEvent>
#include <QModelIndex>
#include <QTreeView>

#include "TreeView.hpp"

namespace modeleditor {

TreeView::TreeView(QWidget* parent) : QTreeView(parent) {}

TreeView::~TreeView() {}

void TreeView::enterEvent(QEvent* event) {
  emit eventEnter();
}

void TreeView::leaveEvent(QEvent* event) {
  emit eventLeave();
}

void TreeView::keyReleaseEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
    emit eventUpDnKeyRelease();
  }
}

bool TreeView::getSelectedRows(QModelIndexList& rowList) {
  //bool success = false;
  QItemSelectionModel* selectionMod = nullptr;
  selectionMod = selectionModel();
  if (selectionMod) {
    //success = true;
    rowList = selectionMod->selectedRows();
  }
  return !rowList.empty();
}

bool TreeView::hasSelectedRows() {
  QModelIndexList rowList;
  return getSelectedRows(rowList);
}

}  // namespace modeleditor
