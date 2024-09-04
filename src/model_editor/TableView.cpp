/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <QKeyEvent>
#include <QTableView>

#include "TableView.hpp"

namespace modeleditor {

TableView::TableView(QWidget* parent) : QTableView(parent) {}

TableView::~TableView() = default;

void TableView::enterEvent(QEnterEvent* event) {
  emit eventEnter();
}

void TableView::leaveEvent(QEvent* event) {
  emit eventLeave();
}

void TableView::keyReleaseEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
    emit eventUpDnKeyRelease();
  }
}

bool TableView::getSelectedRows(QModelIndexList& rowList) {
  // bool success = false;
  QItemSelectionModel* selectionMod = nullptr;
  selectionMod = selectionModel();
  if (selectionMod) {
    //success = true;
    rowList = selectionMod->selectedRows();
  }
  return !rowList.empty();
}

bool TableView::hasSelectedRows() {
  QModelIndexList rowList;
  return getSelectedRows(rowList);
}

}  // namespace modeleditor
