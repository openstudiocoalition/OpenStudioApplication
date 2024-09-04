/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOG_HPP
#define SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOG_HPP

#include <QDialog>
#include <vector>

class QButtonGroup;
class QGridLayout;
class QLineEdit;
class QModelIndex;
class QPushButton;
class QScrollArea;
class QStackedWidget;
class QTableWidget;
class QTimer;
class QTreeView;
class QWidget;

namespace openstudio {

class BuildingComponentDialogCentralWidget;
class Component;

class BuildingComponentDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit BuildingComponentDialog(std::string& filterType, bool isBclDlg = true, QWidget* parent = nullptr);
  virtual ~BuildingComponentDialog() {}

  int currentTIDSelection();
  int rootTID();
  void setRootTID(int tid);
  bool showNewComponents();
  void setShowNewComponents(bool showNewComponents);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void requestComponents(const std::string& filterType, int tids, int pageIdx, const QString& searchString);
  void createLayout(bool isBclDlg);

  std::string m_filterType;
  QString m_dlgTitle;
  QTreeView* m_tidTreeView;
  BuildingComponentDialogCentralWidget* m_centralWidget;
  QScrollArea* m_rightScrollArea;
  Component* m_expandedComponent;
  QLineEdit* m_lineEdit;
  QStackedWidget* m_stackedWidget;
  QTimer* m_timer;

 signals:
  void headerClicked(bool checked);
  void componentClicked(bool checked);
  void collapsibleComponentClicked(bool checked);
  void getComponentsByPage(int pageNum);

 private slots:
  void on_tidClicked(const QModelIndex& index);
  void on_headerClicked(bool checked);
  void on_componentClicked(bool checked);
  void on_collapsibleComponentClicked(bool checked);
  void on_getComponentsByPage(int pageIdx);
  void on_searchButton();
  void on_componentsReady();
  void on_noComponents();
  void on_requestComponents(const std::string& filterType, int tids, int pageIdx, const QString& searchString);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOG_HPP
