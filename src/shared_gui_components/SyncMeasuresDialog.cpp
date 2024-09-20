/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "../shared_gui_components/SyncMeasuresDialog.hpp"

#include "../shared_gui_components/Component.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/SyncMeasuresDialogCentralWidget.hpp"

#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <vector>

#include <QBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QSplitter>
#include <QStyleOption>

namespace openstudio {

SyncMeasuresDialog::SyncMeasuresDialog(const WorkflowJSON& workflow, MeasureManager* measureManager, QWidget* parent)
  : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
    m_centralWidget(nullptr),
    m_rightScrollArea(nullptr),
    m_expandedComponent(nullptr),
    m_workflow(workflow),
    m_measureManager(measureManager) {
  createLayout();
  findUpdates();
}

void SyncMeasuresDialog::createLayout() {
  setWindowTitle("Updates Available in Library");

  setModal(true);

  setSizeGripEnabled(true);

  setObjectName("BlueGradientWidget");

  // The central pane
  m_centralWidget = new SyncMeasuresDialogCentralWidget(m_workflow, m_measureManager);

  connect(m_centralWidget, &SyncMeasuresDialogCentralWidget::componentClicked, this, &SyncMeasuresDialog::on_componentClicked);

  connect(m_centralWidget, &SyncMeasuresDialogCentralWidget::noComponents, this, &SyncMeasuresDialog::on_noComponents);

  connect(m_centralWidget, &SyncMeasuresDialogCentralWidget::closeDlg, this, &SyncMeasuresDialog::closeDlg);

  auto* centralScrollArea = new QScrollArea(this);
  centralScrollArea->setFrameStyle(QFrame::NoFrame);
  centralScrollArea->setObjectName("GrayWidget");
  centralScrollArea->setWidgetResizable(true);
  centralScrollArea->setWidget(m_centralWidget);

  // The right pane

  m_rightScrollArea = new QScrollArea(this);
  m_rightScrollArea->setFrameStyle(QFrame::NoFrame);
  m_rightScrollArea->setObjectName("GrayWidget");
  m_rightScrollArea->setWidgetResizable(true);

  auto* splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(centralScrollArea);
  splitter->addWidget(m_rightScrollArea);

  auto* mainLayout = new QHBoxLayout();
  mainLayout->addWidget(splitter);

  setLayout(mainLayout);

  m_centralWidget->lowerPushButton->setFocus();
}

void SyncMeasuresDialog::findUpdates() {
  // this will update the xmls
  m_measureManager->updateMeasuresLists();

  // DLM: measure manager will filter out duplicate measures for us
  std::vector<BCLMeasure> measures = m_measureManager->combinedMeasures();

  // DLM: should we sort these in any way?

  m_measuresNeedingUpdates.clear();

  m_centralWidget->progressBar->setVisible(true);
  m_centralWidget->progressBar->setStatusTip("Checking for updates");
  m_centralWidget->progressBar->setMinimum(0);
  m_centralWidget->progressBar->setMaximum(measures.size());

  int progressValue = 0;
  for (auto& measure : measures) {
    m_centralWidget->progressBar->setValue(progressValue);

    boost::optional<BCLMeasure> workflowMeasure = m_workflow.getBCLMeasureByUUID(measure.uuid());
    if (workflowMeasure) {
      if (workflowMeasure->versionUUID() != measure.versionUUID()) {
        if (workflowMeasure->versionModified() && measure.versionModified()) {
          if (workflowMeasure->versionModified().get() < measure.versionModified().get()) {
            m_measuresNeedingUpdates.push_back(measure);
          }
        } else {
          m_measuresNeedingUpdates.push_back(measure);
        }
      }
    }

    ++progressValue;
  }

  m_centralWidget->progressBar->setVisible(false);
  m_centralWidget->progressBar->reset();
  m_centralWidget->progressBar->setStatusTip("");

  // DLM: if m_measuresNeedingUpdates is empty should we do something else?
  // just say "No updates available" and quit?

  m_centralWidget->setMeasures(m_measuresNeedingUpdates);
}

void SyncMeasuresDialog::paintEvent(QPaintEvent* /*event*/) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

///! Slots

void SyncMeasuresDialog::on_componentClicked(bool /*checked*/) {
  if (m_expandedComponent) {
    delete m_expandedComponent;
    m_expandedComponent = nullptr;
  }
  m_expandedComponent = new Component(*m_centralWidget->checkedComponent());
  m_expandedComponent->setCheckable(false);
  m_rightScrollArea->setWidget(m_expandedComponent);
}

void SyncMeasuresDialog::on_noComponents() {
  m_expandedComponent = new Component();
  m_expandedComponent->setCheckable(false);
  m_rightScrollArea->setWidget(m_expandedComponent);
}

void SyncMeasuresDialog::closeDlg() {
  QDialog::close();
}

}  // namespace openstudio
