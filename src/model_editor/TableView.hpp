/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_TABLEVIEW_HPP
#define MODELEDITOR_TABLEVIEW_HPP

#include <QTableView>

namespace modeleditor {

class TableView : public QTableView
{
  Q_OBJECT

 public:
  explicit TableView(QWidget* parent = nullptr);
  virtual ~TableView();
  bool getSelectedRows(QModelIndexList& rowList);
  bool hasSelectedRows();

 public slots:

 signals:
  void eventEnter();
  void eventLeave();
  void eventUpDnKeyRelease();

 protected:
  virtual void enterEvent(QEnterEvent* event) override;
  virtual void leaveEvent(QEvent* event) override;
  virtual void keyReleaseEvent(QKeyEvent* event) override;

 private:
};

}  // namespace modeleditor

#endif  // MODELEDITOR_TABLEVIEW_HPP
