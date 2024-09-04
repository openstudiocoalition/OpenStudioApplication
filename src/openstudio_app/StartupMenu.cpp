/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "StartupMenu.hpp"
#include "OpenStudioApp.hpp"
#include <QMenu>

namespace openstudio {

StartupMenu::StartupMenu(QWidget* parent) : QMenuBar(parent) {

  // File menu
  m_fileMenu = new QMenu(tr("&File"), this);
  auto* newAction = new QAction(tr("&New"), this);
  newAction->setShortcut(QKeySequence(QKeySequence::New));
  auto* loadFileAction = new QAction(tr("&Open"), this);
  loadFileAction->setShortcut(QKeySequence(QKeySequence::Open));
  auto* exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(QKeySequence(QKeySequence::Quit));

  auto* importMenu = new QMenu(tr("Import"), this);
  auto* action = new QAction(tr("IDF"), this);
  importMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::importClicked, Qt::QueuedConnection);
  action = new QAction(tr("gbXML"), this);
  importMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::importgbXMLClicked, Qt::QueuedConnection);
  action = new QAction(tr("SDD"), this);
  importMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::importSDDClicked, Qt::QueuedConnection);
  action = new QAction(tr("IFC"), this);
  importMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::importIFCClicked, Qt::QueuedConnection);

  connect(loadFileAction, &QAction::triggered, this, &StartupMenu::loadFileClicked, Qt::QueuedConnection);
  connect(newAction, &QAction::triggered, this, &StartupMenu::newClicked, Qt::QueuedConnection);
  connect(exitAction, &QAction::triggered, this, &StartupMenu::exitClicked, Qt::QueuedConnection);
  m_fileMenu->addAction(newAction);
  m_fileMenu->addAction(loadFileAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addMenu(importMenu);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(exitAction);

  addMenu(m_fileMenu);

  // Help menu
  m_helpMenu = new QMenu(tr("&Help"), this);

  addMenu(m_helpMenu);

  auto* helpAction = new QAction(tr("OpenStudio &Help"), this);

  connect(helpAction, &QAction::triggered, this, &StartupMenu::helpClicked, Qt::QueuedConnection);
  m_helpMenu->addAction(helpAction);

  action = new QAction(tr("Check For &Update"), this);
  m_helpMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::checkForUpdateClicked, Qt::QueuedConnection);

  action = new QAction(tr("Debug Webgl"), this);
  m_helpMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::debugWebglClicked, Qt::QueuedConnection);

  action = new QAction(tr("&About"), this);
  m_helpMenu->addAction(action);
  connect(action, &QAction::triggered, this, &StartupMenu::aboutClicked, Qt::QueuedConnection);
}

}  // namespace openstudio
