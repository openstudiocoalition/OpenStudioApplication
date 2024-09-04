/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RUNTABVIEW_HPP
#define OPENSTUDIO_RUNTABVIEW_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/utilities/idf/WorkspaceObject_Impl.hpp>
#include <boost/smart_ptr.hpp>
#include "MainTabView.hpp"
#include "../shared_gui_components/ProgressBarWithError.hpp"
#include <QComboBox>
#include <QWidget>
#include <QProcess>
//#include "../runmanager/lib/ConfigOptions.hpp"
//#include "../runmanager/lib/RunManager.hpp"
//#include "../runmanager/lib/Workflow.hpp"

class QButtonGroup;
class QPlainTextEdit;
class QPushButton;
class QRadioButton;
class QStackedWidget;
class QToolButton;
class QTextEdit;
class QFileSystemWatcher;
class QTcpServer;
class QTcpSocket;
class QCheckBox;

namespace openstudio {

class RunView;

class RunView : public QWidget
{
  Q_OBJECT;

 public:
  RunView();

 private:
  REGISTER_LOGGER("openstudio::RunView");

  void playButtonClicked(bool t_checked);

  void onRunProcessFinished(int exitCode, QProcess::ExitStatus status);

  void onRunProcessErrored(QProcess::ProcessError error);

  //void onSimDirChanged(const QString &path);

  //void onFileChanged(const QString &path);

  void onOpenSimDirClicked();

  void onNewConnection();

  void onRunDataReady();
  void readyReadStandardOutput();
  void processLine(const QString& line, bool fromSocket);
  void readyReadStandardError();

  void resetFont();

  QToolButton* m_playButton;
  ProgressBarWithError* m_progressBar;
  QTextEdit* m_textInfo;
  QProcess* m_runProcess;
  QPushButton* m_openSimDirButton;
  QTcpServer* m_runTcpServer;
  QTcpSocket* m_runSocket;

  QCheckBox* m_verboseOutputBox;
  QCheckBox* m_useClassicCLIBox;

  openstudio::path m_basePath;
  //QFileSystemWatcher * m_simDirWatcher;
  //QFileSystemWatcher * m_eperrWatcher;

  enum State
  {
    stopped = 0,
    initialization = 1,
    os_measures = 2,
    translator = 3,
    ep_measures = 4,
    preprocess = 5,
    simulation = 6,
    reporting_measures = 7,
    postprocess = 8,
    complete = 9
  };
  State m_state = State::stopped;
  bool m_hasSocketConnection = false;
};

class RunTabView : public MainTabView
{
  Q_OBJECT;

 public:
  explicit RunTabView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~RunTabView() {}

  //signals:
  //  void resultsGenerated(const openstudio::path &t_sqlFile);

 private:
  RunView* m_runView;
  //openstudio::runmanager::JobStatusWidget * m_status;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RUNTABVIEW_HPP
