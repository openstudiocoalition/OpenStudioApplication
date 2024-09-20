/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "TableWidget.hpp"

using namespace openstudio::model;
using namespace openstudio;

namespace modeleditor {

TableWidget::TableWidget(QWidget* parent) : QTableWidget(parent) {}

TableWidget::~TableWidget() = default;

void TableWidget::populateTable(const openstudio::WorkspaceObject& obj) {
  clear();
  setCurrentCell(0, 0);
  setRowCount(1);

  openstudio::IddObject iddObj = obj.iddObject();
  for (unsigned i = 0; i < obj.numFields(); ++i) {
    bool defaultText = false;
    OptionalString val = obj.getString(i);
    if (!val) {
      defaultText = true;
      val = obj.getString(i, true);
      // if still no value it is just blank
      if (!val) {
        val = "";
      }
    }
    // setItem causes QTableWIdget to take ownership of newItem
    auto* newItem = new QTableWidgetItem((*val).c_str());
    if (defaultText) {
      newItem->setForeground(Qt::gray);
    }
    setItem(rowCount() - 1, 1, newItem);

    openstudio::OptionalIddField iddField = iddObj.getField(i);
    if (!iddField) {
      // log error
    } else {
      *val = iddField->name();
    }
    // setItem causes QTableWIdget to take ownership of newItem
    newItem = new QTableWidgetItem((*val).c_str());
    setItem(rowCount() - 1, 0, newItem);

    setRowCount(rowCount() + 1);
  }

  resizeColumnsToContents();
  resizeRowsToContents();
}

}  // namespace modeleditor
