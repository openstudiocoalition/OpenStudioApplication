/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MAINMENU_HPP
#define OPENSTUDIO_MAINMENU_HPP

#include <QMenuBar>

class QMenu;

namespace openstudio {

class MainMenu : public QMenuBar
{
  Q_OBJECT

 public:
  MainMenu(bool isIP, bool isPlugin, const QString& currLang, bool allowAnalytics, bool useClassicCLI, bool displayAdditionalProps,
           QWidget* parent = nullptr);

  virtual ~MainMenu();

 signals:

  void exportClicked();

  void exportgbXMLClicked();

  void exportSDDClicked();

  void importClicked();

  void importgbXMLClicked();

  void importSDDClicked();

  void importIFCClicked();

  void loadFileClicked();

  void changeDefaultLibrariesClicked();

  void configureExternalToolsClicked();

  void loadLibraryClicked();

  void loadExampleModelClicked();

  void loadShoeboxModelClicked();

  void modelDesignWizardClicked();

  void revertFileClicked();

  void saveAsFileClicked();

  void saveFileClicked();

  void newClicked();

  void exitClicked();

  void helpClicked();

  void checkForUpdateClicked();

  void allowAnalyticsClicked(bool allowAnalytics);

  void useClassicCLIClicked(bool useClassicCLI);

  void debugWebglClicked();

  void aboutClicked();

  void showRubyConsoleClicked();

  void scanForToolsClicked();

  void showRunManagerPreferencesClicked();

  void toggleUnitsClicked(bool displayIP);

  void displayAdditionalPropsClicked(bool displayAdditionalProps);

  void changeLanguageClicked(const QString& rLanguage);

  void openLibDlgClicked();

  void configureProxyClicked();

  void applyMeasureClicked();

  void downloadMeasuresClicked();

  void downloadComponentsClicked();

  void changeMyMeasuresDir();

  void changeBclLogin();

 private:
  bool m_isPlugin;

  QMenu* m_fileMenu;

  QMenu* m_measureMenu;

  QMenu* m_helpMenu;

  QMenu* m_preferencesMenu;

  QAction* m_displayIPUnitsAction;

  QAction* m_displaySIUnitsAction;

  QAction* m_langEnglishAction;
  QAction* m_langFrenchAction;
  QAction* m_langSpanishAction;
  QAction* m_langFarsiAction;
  QAction* m_langItalianAction;
  QAction* m_langChineseAction;
  QAction* m_langGreekAction;
  QAction* m_langPolishAction;
  QAction* m_langCatalanAction;
  QAction* m_langHindiAction;
  QAction* m_langVietnameseAction;
  QAction* m_langJapaneseAction;
  QAction* m_langGermanAction;
  // QAction* m_langArabicAction;
  QAction* m_langHebrewAction;

  QAction* m_openLibDlgAction;

  QAction* m_configureProxy;

  QAction* m_revertToSavedAction;

  QAction* m_allowAnalyticsAction;

  QAction* m_useClassicCLIAction;

  QAction* m_displayAdditionalPropsAction;

  std::vector<QAction*> m_fileImportActions;

  std::vector<QAction*> m_preferencesActions;

  std::vector<QAction*> m_componentsMeasuresActions;

  bool m_isIP;

  QString m_currLang;

  bool m_allowAnalytics;

  bool m_useClassicCLI;

  bool m_displayAdditionalProps;

 public slots:

  void enableRevertToSavedAction(bool enable);

  void enableFileImportActions(bool enable);

  void enablePreferencesActions(bool enable);

  void enableComponentsMeasuresActions(bool enable);

  void enableAnalytics(bool enable);

  void toggleAnalytics();

  void enableUseClassicCLI(bool enable);

  void toggleUseClassicCLI();

  void enableDisplayAdditionalProps(bool displayAdditionalProps);

 private slots:

  void displaySIUnitsClicked();

  void displayIPUnitsClicked();

  void langEnglishClicked();
  void langFrenchClicked();
  void langSpanishClicked();
  void langFarsiClicked();
  void langItalianClicked();
  void langChineseClicked();
  void langGreekClicked();
  void langPolishClicked();
  void langCatalanClicked();
  void langHindiClicked();
  void langVietnameseClicked();
  void langJapaneseClicked();
  void langGermanClicked();
  // void langArabicClicked();
  void langHebrewClicked();
  void addingNewLanguageClicked();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINMENU_HPP
