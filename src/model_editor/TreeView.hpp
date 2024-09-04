/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_TREEVIEW_HPP
#define MODELEDITOR_TREEVIEW_HPP

#include <QTreeView>

class QModelIndex;

namespace modeleditor {

class TreeView : public QTreeView
{
  Q_OBJECT

 public:
  explicit TreeView(QWidget* parent = nullptr);
  virtual ~TreeView();
  bool getSelectedRows(QModelIndexList& rowList);
  bool hasSelectedRows();

 public slots:

 signals:
  void eventEnter();
  void eventLeave();
  void eventUpDnKeyRelease();

 protected:
  virtual void enterEvent(QEvent* event) override;
  virtual void leaveEvent(QEvent* event) override;
  virtual void keyReleaseEvent(QKeyEvent* event) override;

 private:
};

}  // namespace modeleditor

#endif  // MODELEDITOR_TREEVIEW_HPP
