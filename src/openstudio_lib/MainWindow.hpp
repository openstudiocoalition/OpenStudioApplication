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

#ifndef OPENSTUDIO_MAINWINDOW_HPP
#define OPENSTUDIO_MAINWINDOW_HPP

#include <QMainWindow>

#include "AnalyticsHelper.hpp"

class QStackedWidget;

class QSplitter;

namespace openstudio {

class HorizontalTabWidget;

class MainTabView;

class VerticalTabWidget;

class MainMenu;

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow(bool isPlugin, QWidget* parent = nullptr);
  virtual ~MainWindow();

  void addVerticalTabButton(int id, const QString& toolTip, const QString& selectedImagePath, const QString& unSelectedImagePath,
                            const QString& disabledImagePath);

  void setView(MainTabView* view, int id);

  MainTabView* view() const;

  void setMainRightColumnView(QWidget* widget);

  void selectVerticalTab(int id);

  void selectVerticalTabByIndex(int index);

  int verticalTabIndex();

  void closeSidebar();

  void openSidebar();

  bool displayIP() const;

  bool verboseOutput() const;

  bool useClassicCLI() const;

  void enableRevertToSavedAction(bool enable);

  void enableFileImportActions(bool enable);

  void enablePreferencesActions(bool enable);

  void enableComponentsMeasuresActions(bool enable);

  bool geometryDiagnostics() const;

  QString lastPath() const;

  VerticalTabWidget* verticalTabWidget() {
    return m_verticalTabWidget;
  }

 signals:

  void closeClicked();

  void verticalTabSelected(int id);

  void exportClicked();

  void exportgbXMLClicked();

  void exportSDDClicked();

  void importClicked();

  void importgbXMLClicked();

  void importSDDClicked();

  void importIFCClicked();

  void loadFileClicked();

  void osmDropped(QString path);

  void changeDefaultLibrariesClicked();

  void configureExternalToolsClicked();

  void changeLanguageClicked(const QString& rLanguage);

  void loadLibraryClicked();

  void loadExampleModelClicked();

  void loadShoeboxModelClicked();

  void saveAsFileClicked();

  void saveFileClicked();

  void revertFileClicked();

  void scanForToolsClicked();

  void showRunManagerPreferencesClicked();

  void newClicked();

  void exitClicked();

  void helpClicked();

  void checkForUpdateClicked();

  void debugWebglClicked();

  void aboutClicked();

  void showRubyConsoleClicked();

  void toggleUnitsClicked(bool displayIP);

  void downloadComponentsClicked();

  void openLibDlgClicked();

  void changeMyMeasuresDir();

  void changeBclLogin();

  void applyMeasureClicked();

  void downloadMeasuresClicked();

  void enableRevertToSaved(bool enable);

  void enableFileImports(bool enable);

  void enablePreferences(bool enable);

  void enableComponentsMeasures(bool enable);

  void enableAnalytics(bool enable);

  void sendAnalytics(const QString& analyticsId, const std::string& contentType, const std::string& contentId);

  void enableUseClassicCLI(bool enable);

 public slots:

  void onVerticalTabSelected(int verticalTabId);

  void toggleVerboseOutput(bool verboseOutput);

  void toggleGeometryDiagnostics(bool geometryDiagnostics);

  void promptAnalytics();

  void toggleAnalytics(bool allowAnalytics);

  void toggleUseClassicCLI(bool useClassicCLI);

 protected:
  void closeEvent(QCloseEvent* event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;

  void dropEvent(QDropEvent* event) override;

  QSize sizeHint() const override;

 private:
  void readSettings();

  void writeSettings();

  bool allowAnalytics() const;

  bool m_isPlugin;

  QStackedWidget* m_mainRightColumnContainer;

  VerticalTabWidget* m_verticalTabWidget;

  QSplitter* m_mainSplitter;

  AnalyticsHelper* m_analyticsHelper;

  bool m_displayIP;

  bool m_verboseOutput = false;

  bool m_geometryDiagnostics = false;

  QString m_currLang;

  QString m_lastPath;

  QString m_analyticsId;

  bool m_useClassicCLI = false;

 private slots:

  void toggleUnits(bool displayIP);

  void changeLanguage(const QString& rLanguage);

  void configureProxyClicked();

  void loadProxySettings();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINWINDOW_HPP
