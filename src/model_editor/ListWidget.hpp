/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_LISTWIDGET_HPP
#define MODELEDITOR_LISTWIDGET_HPP

#include <QListWidget>

namespace modeleditor {

class ListWidget : public QListWidget
{
  Q_OBJECT

 public:
  explicit ListWidget(QWidget* parent = nullptr);
  virtual ~ListWidget();

  static const QString& getMimeType() {
    return mMimeType;
  }

 public slots:

 signals:

 protected:
  virtual QMimeData* mimeData(const QList<QListWidgetItem*>& items) const override;

 private:
  static QString mMimeType;
};

}  // namespace modeleditor

#endif  // MODELEDITOR_LISTWIDGET_HPP
