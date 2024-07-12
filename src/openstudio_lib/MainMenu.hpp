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

#ifndef OPENSTUDIO_MAINMENU_HPP
#define OPENSTUDIO_MAINMENU_HPP

#include <QMenuBar>

class QMenu;

namespace openstudio {

class MainMenu : public QMenuBar
{
  Q_OBJECT

 public:
  MainMenu(bool isIP, bool isPlugin, const QString& currLang, bool allowAnalytics, bool useClassicCLI, QWidget* parent = nullptr);

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

  std::vector<QAction*> m_fileImportActions;

  std::vector<QAction*> m_preferencesActions;

  std::vector<QAction*> m_componentsMeasuresActions;

  bool m_isIP;

  QString m_currLang;

  bool m_allowAnalytics;

  bool m_useClassicCLI;

 public slots:

  void enableRevertToSavedAction(bool enable);

  void enableFileImportActions(bool enable);

  void enablePreferencesActions(bool enable);

  void enableComponentsMeasuresActions(bool enable);

  void enableAnalytics(bool enable);

  void toggleAnalytics();

  void enableUseClassicCLI(bool enable);

  void toggleUseClassicCLI();

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
