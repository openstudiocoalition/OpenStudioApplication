/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScriptsTabView.hpp"

#include "MainWindow.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

#include "../shared_gui_components/Buttons.hpp"
#include "../shared_gui_components/MeasureManager.hpp"
#include "../shared_gui_components/OSListView.hpp"
#include "../shared_gui_components/SyncMeasuresDialog.hpp"
#include "../shared_gui_components/EditController.hpp"

#include <openstudio/energyplus/ForwardTranslator.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace openstudio {

ScriptsTabView::ScriptsTabView(QWidget* t_parent) : MainTabView("Measures", MainTabView::MAIN_TAB, t_parent) {
  //setTitle("Organize and Edit Measures for Project");

  // Main Content

  mainContent = new QWidget();

  auto* mainContentVLayout = new QVBoxLayout();
  mainContentVLayout->setContentsMargins(0, 0, 0, 0);
  mainContentVLayout->setSpacing(0);
  mainContentVLayout->setAlignment(Qt::AlignTop);
  mainContent->setLayout(mainContentVLayout);

  addTabWidget(mainContent);

  workflowView = new OSListView(true);
  workflowView->setContentsMargins(0, 0, 0, 0);
  workflowView->setSpacing(0);
  mainContentVLayout->addWidget(workflowView);

  QString style;
  style.append("QWidget#Footer {");
  style.append("border-top: 1px solid black; ");
  style.append("background-color: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop: 0 #B6B5B6, stop: 1 #737172); ");
  style.append("}");

  auto* footer = new QWidget();
  footer->setObjectName("Footer");
  footer->setStyleSheet(style);
  mainContentVLayout->addWidget(footer);

  auto* layout = new QHBoxLayout();
  layout->setSpacing(0);
  footer->setLayout(layout);

  m_updateMeasuresButton = new GrayButton();
  m_updateMeasuresButton->setText("Sync Project Measures with Library");
  m_updateMeasuresButton->setToolTip("Check the Library for Newer Versions of the Measures in Your Project and Provides Sync Option");
  layout->addStretch();
  layout->addWidget(m_updateMeasuresButton);

  connect(m_updateMeasuresButton, &QPushButton::clicked, this, &ScriptsTabView::openUpdateMeasuresDlg);
}

void ScriptsTabView::showEvent(QShowEvent* e) {
  MainTabView::showEvent(e);

  workflowView->refreshAllViews();
}

//*****SLOTS*****

void ScriptsTabView::openUpdateMeasuresDlg() {
  m_updateMeasuresButton->setEnabled(false);

  openstudio::OSAppBase* app = OSAppBase::instance();

  // Disable all Vertical Tab Buttons (so you can't click on another tab while it's doing its thing)
  app->currentDocument()->disable();

  WorkflowJSON workflow = app->currentDocument()->model().workflowJSON();

  m_syncMeasuresDialog = boost::shared_ptr<SyncMeasuresDialog>(new SyncMeasuresDialog(workflow, &(app->measureManager())));
  m_syncMeasuresDialog->setGeometry(app->currentDocument()->mainWindow()->geometry());
  m_syncMeasuresDialog->exec();

  app->editController()->reset();
  workflowView->refreshAllViews();

  m_updateMeasuresButton->setEnabled(true);

  // Re-enable the ability to switch tabs
  app->currentDocument()->enable();
}

}  // namespace openstudio
