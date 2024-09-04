/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RESULTSTABVIEW_HPP
#define OPENSTUDIO_RESULTSTABVIEW_HPP

#include "MainTabView.hpp"
#include "OSWebEnginePage.hpp"

#include "../model_editor/QMetaTypes.hpp"

#include <openstudio/utilities/sql/SqlFile.hpp>
#include <openstudio/utilities/units/Unit.hpp>

#include "../shared_gui_components/ProgressBarWithError.hpp"

#include <QWidget>
#include <QWebEngineView>

class QComboBox;
class QPushButton;

namespace openstudio {

// main widget

class ResultsView : public QWidget
{
  Q_OBJECT;

 public:
  explicit ResultsView(QWidget* t_parent = nullptr);
  virtual ~ResultsView();
  void searchForExistingResults(const openstudio::path& t_runDir, const openstudio::path& t_reportsDir);

 public slots:
  void resultsGenerated(const openstudio::path& t_sqlFile, const openstudio::path& t_radianceResultsPath);
  void onUnitSystemChange(bool t_isIP);
  void treeChanged(const openstudio::UUID& t_uuid);

 private slots:
  void refreshClicked();
  void openDViewClicked();
  void comboBoxChanged(int index);

  // DLM: for debugging
  void onLoadFinished(bool ok);
  void onLoadProgress(int progress);
  void onLoadStarted();
  void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);

 private:
  REGISTER_LOGGER("openstudio::ResultsView");
  //openstudio::runmanager::RunManager runManager();
  void populateComboBox(const std::vector<openstudio::path>& reports);

  bool m_isIP;

  // utility bill results
  QLabel* m_reportLabel;

  ProgressBarWithError* m_progressBar;
  QPushButton* m_refreshBtn;
  QPushButton* m_openDViewBtn;

  openstudio::path m_dviewPath;
  openstudio::path m_sqlFilePath;
  openstudio::path m_radianceResultsPath;

  QWebEngineView* m_view;
  OSWebEnginePage* m_page;
  QComboBox* m_comboBox;
};

class ResultsTabView : public MainTabView
{
  Q_OBJECT;

 public:
  ResultsTabView(const QString& tabLabel, TabType tabType, QWidget* parent = nullptr);
  virtual ~ResultsTabView() {}

 public slots:

  void onUnitSystemChange(bool t_isIP);

 signals:

  void removeResultClicked();

  void importResultClicked();

  void treeChanged(const openstudio::UUID& t_uuid);

 private:
  ResultsView* m_resultsView;
  REGISTER_LOGGER("openstudio::ResultsTabView");
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RESULTSTABVIEW_HPP
