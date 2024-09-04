/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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

  bool displayAdditionalProps() const;

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

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

 public slots:

  void onVerticalTabSelected(int verticalTabId);

  void toggleVerboseOutput(bool verboseOutput);

  void toggleGeometryDiagnostics(bool geometryDiagnostics);

  void promptAnalytics();

  void toggleAnalytics(bool allowAnalytics);

  void toggleUseClassicCLI(bool useClassicCLI);

  void toggleDisplayAdditionalProps(bool displayAdditionalProps);

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

  bool m_useClassicCLI = false;

  bool m_displayAdditionalProps = false;

  QString m_currLang;

  QString m_lastPath;

  QString m_analyticsId;

 private slots:

  void toggleUnits(bool displayIP);

  void changeLanguage(const QString& rLanguage);

  void configureProxyClicked();

  void loadProxySettings();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINWINDOW_HPP
