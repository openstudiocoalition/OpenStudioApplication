/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "MainWindow.hpp"

#include "HorizontalTabWidget.hpp"
#include "LibraryTabWidget.hpp"
#include "LoopLibraryDialog.hpp"
#include "MainMenu.hpp"
#include "MainTabView.hpp"
#include "OSDocument.hpp"
#include "VerticalTabWidget.hpp"

#include "../shared_gui_components/NetworkProxyDialog.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QColumnView>
#include <QComboBox>
#include <QFileDialog>
#include <QFrame>
#include <QGraphicsView>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QScrollArea>
#include <QSettings>
#include <QSizePolicy>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

namespace openstudio {

MainWindow::MainWindow(bool isPlugin, QWidget* parent)
  : QMainWindow(parent),
    m_isPlugin(isPlugin),
    m_mainRightColumnContainer(new QStackedWidget()),
    m_verticalTabWidget(new VerticalTabWidget()),
    m_mainSplitter(new QSplitter(Qt::Horizontal)),
    m_displayIP(true),
    m_currLang("en") {
  setMinimumSize(900, 658);
  setAcceptDrops(true);

  readSettings();

  this->statusBar()->show();

#ifdef Q_OS_LINUX
  QIcon icon = QIcon(":/images/os_16.png");
  icon.addPixmap(QPixmap(":/images/os_32.png"));
  icon.addPixmap(QPixmap(":/images/os_48.png"));
  icon.addPixmap(QPixmap(":/images/os_64.png"));
  icon.addPixmap(QPixmap(":/images/os_128.png"));
  icon.addPixmap(QPixmap(":/images/os_256.png"));
  setWindowIcon(icon);
#endif

  setObjectName("MainWindow");
  setStyleSheet("QWidget#MainWindow { background-color: #2C3233; }");

  connect(m_verticalTabWidget, &VerticalTabWidget::tabSelected, this, &MainWindow::verticalTabSelected);
  connect(m_verticalTabWidget, &VerticalTabWidget::tabSelected, this, &MainWindow::onVerticalTabSelected);
  m_mainSplitter->addWidget(m_verticalTabWidget);

  m_mainRightColumnContainer->setMinimumWidth(235);
  m_mainSplitter->addWidget(m_mainRightColumnContainer);

  m_mainSplitter->setStretchFactor(0, 10000);

  setCentralWidget(m_mainSplitter);

  m_analyticsHelper = new AnalyticsHelper(this);
  connect(this, &MainWindow::sendAnalytics, m_analyticsHelper, &AnalyticsHelper::sendAnalytics);

  auto* mainMenu = new MainMenu(m_displayIP, m_isPlugin, m_currLang, allowAnalytics());
  connect(mainMenu, &MainMenu::toggleUnitsClicked, this, &MainWindow::toggleUnits);
  connect(mainMenu, &MainMenu::changeLanguageClicked, this, &MainWindow::changeLanguage);
  connect(mainMenu, &MainMenu::downloadComponentsClicked, this, &MainWindow::downloadComponentsClicked);
  connect(mainMenu, &MainMenu::openLibDlgClicked, this, &MainWindow::openLibDlgClicked);

  this->setMenuBar(mainMenu);

  connect(mainMenu, &MainMenu::exportClicked, this, &MainWindow::exportClicked);
  connect(mainMenu, &MainMenu::exportgbXMLClicked, this, &MainWindow::exportgbXMLClicked);
  connect(mainMenu, &MainMenu::exportSDDClicked, this, &MainWindow::exportSDDClicked);
  connect(mainMenu, &MainMenu::importClicked, this, &MainWindow::importClicked);
  connect(mainMenu, &MainMenu::importgbXMLClicked, this, &MainWindow::importgbXMLClicked);
  connect(mainMenu, &MainMenu::importSDDClicked, this, &MainWindow::importSDDClicked);
  connect(mainMenu, &MainMenu::importIFCClicked, this, &MainWindow::importIFCClicked);
  connect(mainMenu, &MainMenu::loadFileClicked, this, &MainWindow::loadFileClicked);
  connect(mainMenu, &MainMenu::changeDefaultLibrariesClicked, this, &MainWindow::changeDefaultLibrariesClicked);
  connect(mainMenu, &MainMenu::configureExternalToolsClicked, this, &MainWindow::configureExternalToolsClicked);
  connect(mainMenu, &MainMenu::changeLanguageClicked, this, &MainWindow::changeLanguageClicked);
  connect(mainMenu, &MainMenu::loadLibraryClicked, this, &MainWindow::loadLibraryClicked);
  connect(mainMenu, &MainMenu::loadExampleModelClicked, this, &MainWindow::loadExampleModelClicked);
  connect(mainMenu, &MainMenu::modelDesignWizardClicked, this, &MainWindow::modelDesignWizardClicked);
  connect(mainMenu, &MainMenu::saveAsFileClicked, this, &MainWindow::saveAsFileClicked);
  connect(mainMenu, &MainMenu::saveFileClicked, this, &MainWindow::saveFileClicked);
  connect(mainMenu, &MainMenu::revertFileClicked, this, &MainWindow::revertFileClicked);
  connect(mainMenu, &MainMenu::newClicked, this, &MainWindow::newClicked);
  connect(mainMenu, &MainMenu::exitClicked, this, &MainWindow::exitClicked);
  connect(mainMenu, &MainMenu::helpClicked, this, &MainWindow::helpClicked);
  connect(mainMenu, &MainMenu::checkForUpdateClicked, this, &MainWindow::checkForUpdateClicked);
  connect(mainMenu, &MainMenu::aboutClicked, this, &MainWindow::aboutClicked);
  connect(mainMenu, &MainMenu::allowAnalyticsClicked, this, &MainWindow::toggleAnalytics);
  connect(mainMenu, &MainMenu::scanForToolsClicked, this, &MainWindow::scanForToolsClicked);
  connect(mainMenu, &MainMenu::showRunManagerPreferencesClicked, this, &MainWindow::showRunManagerPreferencesClicked);
  connect(mainMenu, &MainMenu::showRubyConsoleClicked, this, &MainWindow::showRubyConsoleClicked);
  connect(mainMenu, &MainMenu::toggleUnitsClicked, this, &MainWindow::toggleUnitsClicked);
  connect(mainMenu, &MainMenu::changeMyMeasuresDir, this, &MainWindow::changeMyMeasuresDir);
  connect(mainMenu, &MainMenu::applyMeasureClicked, this, &MainWindow::applyMeasureClicked);
  connect(mainMenu, &MainMenu::downloadMeasuresClicked, this, &MainWindow::downloadMeasuresClicked);
  connect(mainMenu, &MainMenu::changeBclLogin, this, &MainWindow::changeBclLogin);
  connect(mainMenu, &MainMenu::configureProxyClicked, this, &MainWindow::configureProxyClicked);
  connect(this, &MainWindow::enableRevertToSaved, mainMenu, &MainMenu::enableRevertToSavedAction);
  connect(this, &MainWindow::enableFileImports, mainMenu, &MainMenu::enableFileImportActions);
  connect(this, &MainWindow::enablePreferences, mainMenu, &MainMenu::enablePreferencesActions);
  connect(this, &MainWindow::enableComponentsMeasures, mainMenu, &MainMenu::enableComponentsMeasuresActions);
  connect(this, &MainWindow::enableAnalytics, mainMenu, &MainMenu::enableAnalytics);
}

MainWindow::~MainWindow() = default;

QSize MainWindow::sizeHint() const {
  return {1024, 700};
}

void MainWindow::setMainRightColumnView(QWidget* widget) {
  if (QWidget* oldwidget = m_mainRightColumnContainer->currentWidget()) {
    m_mainRightColumnContainer->removeWidget(oldwidget);
  }

  m_mainRightColumnContainer->addWidget(widget);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  event->ignore();

  if (isEnabled()) {
    writeSettings();
    emit closeClicked();
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasUrls()) {
    event->ignore();
    /*QUrl url = event->mimeData()->urls().first();
    if (url.scheme().compare("file", Qt::CaseInsensitive) == 0 && url.toString().toLower().endsWith(".osm"))
    {
      //event->accept();
      event->ignore();
    }
    else
    {
      event->ignore();
    }*/
  }
}

void MainWindow::dropEvent(QDropEvent* event) {
  if (event->mimeData()->hasUrls()) {
    emit osmDropped(event->mimeData()->urls().first().toLocalFile());
  }
}

void MainWindow::addVerticalTabButton(int id, const QString& toolTip, const QString& selectedImagePath, const QString& unSelectedImagePath,
                                      const QString& disabledImagePath) {
  m_verticalTabWidget->addTabButton(id, toolTip, selectedImagePath, unSelectedImagePath, disabledImagePath);
}

void MainWindow::setView(MainTabView* view, int id) {
  m_verticalTabWidget->setView(view, id);
}

MainTabView* MainWindow::view() const {
  return m_verticalTabWidget->view();
}

void MainWindow::selectVerticalTab(int id) {
  // TODO new call
  //m_verticalTabWidget->setCurrentId(id);
}

void MainWindow::selectVerticalTabByIndex(int index) {
  m_verticalTabWidget->setCurrentIndex(index);
}

int MainWindow::verticalTabIndex() {
  return m_verticalTabWidget->verticalTabIndex();
}

void MainWindow::closeSidebar() {
  QList<int> sizeList;

  sizeList.push_back(1);

  sizeList.push_back(0);

  m_mainSplitter->setSizes(sizeList);
}

void MainWindow::openSidebar() {
  QList<int> sizeList;

  sizeList.push_back(2);

  sizeList.push_back(1);

  m_mainSplitter->setSizes(sizeList);
}

bool MainWindow::displayIP() const {
  return m_displayIP;
}

void MainWindow::enableRevertToSavedAction(bool enable) {
  emit enableRevertToSaved(enable);
}

void MainWindow::enableFileImportActions(bool enable) {
  emit enableFileImports(enable);
}

void MainWindow::enablePreferencesActions(bool enable) {
  emit enablePreferences(enable);
}

void MainWindow::enableComponentsMeasuresActions(bool enable) {
  emit enableComponentsMeasures(enable);
}

void MainWindow::readSettings() {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(600, 400)).toSize();
  m_lastPath = settings.value("lastPath", QDir::homePath()).toString();
  resize(size);
  move(pos);
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("state").toByteArray());
  m_displayIP = settings.value("displayIP").toBool();
  m_verboseOutput = settings.value("verboseOutput").toBool();
  m_geometryDiagnostics = settings.value("geometryDiagnostics").toBool();
  m_currLang = settings.value("language", "en").toString();
  LOG_FREE(Debug, "MainWindow", "\n\n\nm_currLang=[" << m_currLang.toStdString() << "]\n\n\n");
  if (m_currLang.isEmpty()) {
    m_currLang = "en";
  }
  m_analyticsId = settings.value("analyticsId", "").toString();
}

void MainWindow::writeSettings() {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState());
  settings.setValue("displayIP", m_displayIP);
  settings.setValue("verboseOutput", m_verboseOutput);
  settings.setValue("geometryDiagnostics", m_geometryDiagnostics);
  settings.setValue("language", m_currLang);
  settings.setValue("analyticsId", m_analyticsId);
}

QString MainWindow::lastPath() const {
  return QDir().exists(m_lastPath) ? m_lastPath : QDir::homePath();
}

bool MainWindow::allowAnalytics() const {
  return !m_analyticsId.isEmpty() && m_analyticsId != "DISABLED";
}

//QString MainWindow::currentLanguage() const {
//return m_currLang.isEmpty() ? "en" : m_currLang;
//}

void MainWindow::toggleUnits(bool displayIP) {
  m_displayIP = displayIP;
}

bool MainWindow::verboseOutput() const {
  return m_verboseOutput;
}

bool MainWindow::geometryDiagnostics() const {
  return m_geometryDiagnostics;
}

void MainWindow::onVerticalTabSelected(int verticalTabId) {
  if (allowAnalytics()) {
    static const std::string contentType("OSAppTab");
    std::string contentId = std::to_string(verticalTabId);
    switch (verticalTabId) {
      case OSDocument::SITE:
        contentId = "Location";
        break;
      case OSDocument::SCHEDULES:
        contentId = "Schedules";
        break;
      case OSDocument::CONSTRUCTIONS:
        contentId = "Constructions";
        break;
      case OSDocument::GEOMETRY:
        contentId = "Geometry";
        break;
      case OSDocument::LOADS:
        contentId = "Loads";
        break;
      case OSDocument::SPACE_TYPES:
        contentId = "SpaceTypes";
        break;
      case OSDocument::FACILITY:
        contentId = "Facility";
        break;
      case OSDocument::SPACES:
        contentId = "Spaces";
        break;
      case OSDocument::THERMAL_ZONES:
        contentId = "Thermal Zones";
        break;
      case OSDocument::HVAC_SYSTEMS:
        contentId = "HVAC Systems";
        break;
      case OSDocument::OUTPUT_VARIABLES:
        contentId = "Variables";
        break;
      case OSDocument::SIMULATION_SETTINGS:
        contentId = "Sim Settings";
        break;
      case OSDocument::RUBY_SCRIPTS:
        contentId = "Scripts";
        break;
      case OSDocument::RUN_SIMULATION:
        contentId = "Run";
        break;
      case OSDocument::RESULTS_SUMMARY:
        contentId = "Results";
        break;
      default:
        break;
    }

    emit sendAnalytics(m_analyticsId, contentType, contentId);
  }
}

void MainWindow::toggleVerboseOutput(bool verboseOutput) {
  m_verboseOutput = verboseOutput;
}

void MainWindow::toggleGeometryDiagnostics(bool geometryDiagnostics) {
  m_geometryDiagnostics = geometryDiagnostics;
}

void MainWindow::promptAnalytics() {
  if (m_analyticsId.isEmpty()) {
    QMessageBox::StandardButton reply;
    reply =
      QMessageBox::question(this, tr("Allow Analytics"),
                            tr("Allow OpenStudio Coalition to collect anonymous usage statistics to help improve the OpenStudio Application? See the "
                               "<a href=\"https://openstudiocoalition.org/about/privacy_policy/\">privacy policy</a> for more information."),
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
      emit enableAnalytics(true);
    } else {
      emit enableAnalytics(false);
    }
  }
}

void MainWindow::toggleAnalytics(bool allowAnalytics) {
  if (allowAnalytics) {
    m_analyticsId = openstudio::toQString(openstudio::createUUID());
    writeSettings();
  } else {
    m_analyticsId = "DISABLED";
    writeSettings();
  }
}

void MainWindow::changeLanguage(const QString& rLanguage) {
  m_currLang = rLanguage;
  writeSettings();
}

void MainWindow::configureProxyClicked() {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  NetworkProxyDialog dialog(settings, this);
  auto result = static_cast<QDialog::DialogCode>(dialog.exec());

  if (result == QDialog::Accepted) {
    QNetworkProxy proxy = dialog.createProxy();
    if (proxy.type() != QNetworkProxy::NoProxy) {
      if (dialog.testProxy(proxy)) {
        QNetworkProxy::setApplicationProxy(proxy);
        //setRubyProxyEnvironment(proxy);
      } else {
        QMessageBox::critical(this, "Proxy Configuration Error", "There is an error in your proxy configuration.");
      }
    }
  }
}

void MainWindow::loadProxySettings() {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  QNetworkProxy proxy = NetworkProxyDialog::createProxy(settings);
  if (proxy.type() != QNetworkProxy::NoProxy) {
    if (NetworkProxyDialog::testProxy(proxy, this)) {
      QNetworkProxy::setApplicationProxy(proxy);
      //setRubyProxyEnvironment(proxy);
    } else {
      QMessageBox::critical(this, "Proxy Configuration Error", "There is an error in your proxy configuration.");
      configureProxyClicked();
    }
  }
}

}  // namespace openstudio
