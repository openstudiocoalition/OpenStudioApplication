/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_APPLYMEASURENOWDIALOG_HPP
#define OPENSTUDIO_APPLYMEASURENOWDIALOG_HPP

#include "../shared_gui_components/HeaderViews.hpp"
#include "../shared_gui_components/OSDialog.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

#include <mutex>

class QPushButton;
class QStackedWidget;
class QTextEdit;
class QTimer;
class QProcess;

namespace openstudio {

class DateTime;
class EditController;
class LocalLibraryController;
class WorkflowStepResult;
class WorkflowJSON;
class TextEditDialog;

namespace measuretab {
class MeasureStepItem;
}

// Local Forward Decs
class DataPointJobItemView;

class ApplyMeasureNowDialog : public OSDialog
{
  Q_OBJECT

 public:
  explicit ApplyMeasureNowDialog(QWidget* parent = nullptr);

  virtual ~ApplyMeasureNowDialog();

  QSize sizeHint() const override;

  void displayMeasure();

  QSharedPointer<EditController> m_editController;

  QSharedPointer<LocalLibraryController> m_localLibraryController;

 protected slots:

  virtual void on_cancelButton(bool checked) override;

  virtual void on_backButton(bool checked) override;

  virtual void on_okButton(bool checked) override;

 protected:
  void closeEvent(QCloseEvent* event) override;

  void resizeEvent(QResizeEvent* event) override;  // Put back QDialog::resizeEvent so it can be resized

 private slots:

  void disableOkButton(bool disable);

  void requestReload();

  void showAdvancedOutput();

  void displayResults();

 signals:

  void reloadFile(const QString& fileToLoad, bool modified, bool saveCurrentTabs);

  void toolsUpdated();

 private:
  REGISTER_LOGGER("openstudio::ApplyMeasureNowDialog");

  void createWidgets();

  void runMeasure();

  void searchForExistingResults(const openstudio::path& t_runDir);

  void removeWorkingDir();

  void createWorkingDir();

  boost::optional<BCLMeasure> m_bclMeasure;

  QSharedPointer<measuretab::MeasureStepItem> m_currentMeasureStepItem;

  boost::optional<model::Model> m_model;

  boost::optional<openstudio::path> m_reloadPath;

  QStackedWidget* m_mainPaneStackedWidget;

  QStackedWidget* m_rightPaneStackedWidget;

  QTextEdit* m_argumentsLoadingTextEdit;

  QTextEdit* m_argumentsFailedTextEdit;

  DataPointJobItemView* m_jobItemView;

  QTimer* m_timer;

  int m_inputPageIdx;

  int m_runningPageIdx;

  int m_outputPageIdx;

  int m_argumentsLoadingPageIdx;

  int m_argumentsFailedPageIdx;

  int m_argumentsOkPageIdx;

  QProcess* m_runProcess;

  QLabel* m_jobPath;

  QPushButton* m_showAdvancedOutput;

  QString m_advancedOutput;

  openstudio::path m_workingDir;

  openstudio::path m_workingFilesDir;

  TextEditDialog* m_advancedOutputDialog;

  WorkflowJSON m_modelWorkflowJSON;

  WorkflowJSON m_tempWorkflowJSON;

  std::mutex m_displayMutex;
};

class DataPointJobHeaderView : public OSHeader
{
  Q_OBJECT

 public:
  DataPointJobHeaderView();

  virtual ~DataPointJobHeaderView() {}

  void setName(const std::string& name);

  void setLastRunTime(const boost::optional<openstudio::DateTime>& lastRunTime);

  void setStatus(const std::string& status, bool isCanceled);

  void setNA(bool na);

  void setNumWarnings(unsigned numWarnings);

  void setNumErrors(unsigned numErrors);

  QLabel* m_name;
  QLabel* m_lastRunTime;
  QLabel* m_status;
  QLabel* m_na;
  QLabel* m_warnings;
  QLabel* m_errors;
};

class DataPointJobContentView : public QWidget
{
  Q_OBJECT

 public:
  DataPointJobContentView();

  virtual ~DataPointJobContentView() {}

  void clear();

  void addInitialConditionMessage(const std::string& message);

  void addFinalConditionMessage(const std::string& message);

  void addInfoMessage(const std::string& message);

  void addWarningMessage(const std::string& message);

  void addErrorMessage(const std::string& message);

  void addStdErrorMessage(const std::string& message);

 private:
  static QString formatMessageForHTML(const std::string& t_message);

  QLabel* m_textEdit;
};

class DataPointJobItemView : public OSCollapsibleView
{
  Q_OBJECT

 public:
  DataPointJobItemView();

  virtual ~DataPointJobItemView() {}

  DataPointJobHeaderView* m_dataPointJobHeaderView;

  DataPointJobContentView* m_dataPointJobContentView;

  void update(const BCLMeasure& bclMeasure, const boost::optional<WorkflowJSON>& outWorkflowJSON, bool canceled);

 protected:
  void paintEvent(QPaintEvent* e) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_APPLYMEASURENOWDIALOG_HPP
