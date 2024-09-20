/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_SYNCMEASURESDIALOG_HPP
#define SHAREDGUICOMPONENTS_SYNCMEASURESDIALOG_HPP

#include <QDialog>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>

#include <vector>

class QScrollArea;
class QWidget;

namespace openstudio {

class Component;
class MeasureManager;
class SyncMeasuresDialogCentralWidget;

class SyncMeasuresDialog : public QDialog
{
  Q_OBJECT

 public:
  SyncMeasuresDialog(const WorkflowJSON& workflow, MeasureManager* measureManager, QWidget* parent = nullptr);
  virtual ~SyncMeasuresDialog() {}

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void createLayout();
  void findUpdates();

  SyncMeasuresDialogCentralWidget* m_centralWidget;
  QScrollArea* m_rightScrollArea;
  Component* m_expandedComponent;
  std::vector<BCLMeasure> m_measuresNeedingUpdates;
  WorkflowJSON m_workflow;
  MeasureManager* m_measureManager;  // DLM: why is this a raw pointer?

 private slots:
  void on_componentClicked(bool checked);
  void on_noComponents();
  void closeDlg();
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_SYNCMEASURESDIALOG_HPP
