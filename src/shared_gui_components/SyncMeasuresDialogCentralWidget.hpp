/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_SYNCMEASURESDIALOGCENTRALWIDGET_HPP
#define SHAREDGUICOMPONENTS_SYNCMEASURESDIALOGCENTRALWIDGET_HPP

#include <QWidget>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>

#include "../shared_gui_components/ProgressBarWithError.hpp"

class QPushButton;

namespace openstudio {

class BCLComponent;
class BCLMeasure;
class Component;
class ComponentList;
class CollapsibleComponentList;
class MeasureManager;

class SyncMeasuresDialogCentralWidget : public QWidget
{
  Q_OBJECT

 public:
  SyncMeasuresDialogCentralWidget(const WorkflowJSON& workflow, MeasureManager* measureManager, QWidget* parent = nullptr);
  virtual ~SyncMeasuresDialogCentralWidget() {}

  Component* checkedComponent() const;
  void setMeasures(const std::vector<BCLMeasure>& measures);
  ProgressBarWithError* progressBar;
  QPushButton* lowerPushButton;

 protected:
 private:
  void createLayout();
  void init();
  void displayMeasures(int pageIdx);

  CollapsibleComponentList* m_collapsibleComponentList;
  ComponentList* m_componentList;
  int m_pageIdx;
  std::vector<BCLMeasure> m_measures;
  WorkflowJSON m_workflow;
  MeasureManager* m_measureManager;

 signals:
  void componentClicked(bool checked);
  void getComponentsByPage(int pageNum);
  void noComponents();
  void closeDlg();

 private slots:
  void upperPushButtonClicked();
  void lowerPushButtonClicked();
  void on_getComponentsByPage(int pageIdx);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_SYNCMEASURESDIALOGCENTRALWIDGET_HPP
