/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <QMimeData>

#include "ListWidget.hpp"

namespace modeleditor {
QString modeleditor::ListWidget::mMimeType = "ListWidget data";

ListWidget::ListWidget(QWidget* parent) : QListWidget(parent) {}

ListWidget::~ListWidget() = default;

QMimeData* ListWidget::mimeData(const QList<QListWidgetItem*>& items) const {
  QMimeData* mimeData = QListWidget::mimeData(items);
  mimeData->setData(getMimeType(), QByteArray());

  QString string;
  for (int i = 0; i < items.size(); i++) {
    QListWidgetItem* item = items.at(i);
    string += item->text();
    if (i < items.size() - 1) {
      string += ",";
    }
  }
  mimeData->setText(string);

  return mimeData;
}

///! Slots

}  // namespace modeleditor
