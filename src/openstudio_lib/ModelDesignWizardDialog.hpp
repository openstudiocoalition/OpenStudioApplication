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

#ifndef OPENSTUDIO_MODELDESIGNWIZARDDIALOG_HPP
#define OPENSTUDIO_MODELDESIGNWIZARDDIALOG_HPP

#include "../shared_gui_components/DataPointJob.hpp"
#include "../shared_gui_components/HeaderViews.hpp"
#include "../shared_gui_components/OSDialog.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

#include <QJsonObject>

class QCloseEvent;
class QComboBox;
class QDoubleValidator;
class QProcess;
class QPushButton;
class QResizeEvent;
class QStackedWidget;
class QTextEdit;
class QTimer;
class QWidget;

namespace openstudio {

class TextEditDialog;
class WorkflowJSON;

namespace measuretab {
class MeasureStepItem;
}

class ModelDesignWizardDialog : public OSDialog
{
  Q_OBJECT

 public:
  explicit ModelDesignWizardDialog(QWidget* parent = nullptr);

  virtual ~ModelDesignWizardDialog();

  QSize sizeHint() const override;

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

  void onStandardTypeChanged(const QString& text);
  void populateStandardTypes();

  void onTargetStandardChanged(const QString& text);
  void populateTargetStandards();

  void onPrimaryBuildingTypeChanged(const QString& text);
  void populatePrimaryBuildingTypes();

  void populateSpaceTypeRatiosPage();

 signals:

  void reloadFile(const QString& fileToLoad, bool modified, bool saveCurrentTabs);

  void toolsUpdated();

 private:
  REGISTER_LOGGER("openstudio.ModelDesignWizardDialog");

  void createWidgets();
  QWidget* createTemplateSelectionPage();
  QWidget* createSpaceTypeRatiosPage();
  QWidget* createRunningPage();
  QWidget* createOutputPage();

  void runMeasure();

  void searchForExistingResults(const openstudio::path& t_runDir);

  void removeWorkingDir();

  void createWorkingDir();

  void populateBuildingTypeComboBox(QComboBox* comboBox);
  void populateSpaceTypeComboBox(QComboBox* comboBox, const QString& buildingType);

  boost::optional<BCLMeasure> m_bclMeasure;

  QSharedPointer<measuretab::MeasureStepItem> m_currentMeasureStepItem;

  boost::optional<model::Model> m_model;

  boost::optional<openstudio::path> m_reloadPath;

  QStackedWidget* m_mainPaneStackedWidget;

  QStackedWidget* m_rightPaneStackedWidget;

  QWidget* m_spaceTypeRatiosPageWidget;

  QTextEdit* m_argumentsFailedTextEdit;

  DataPointJobItemView* m_jobItemView;

  QTimer* m_timer;

  int m_templateSelectionPageIdx;

  int m_spaceTypeRatiosPageIdx;

  int m_runningPageIdx;

  int m_outputPageIdx;

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

  QJsonObject m_supportJsonObject;

  QComboBox* m_standardTypeComboBox;
  QComboBox* m_targetStandardComboBox;
  QComboBox* m_primaryBuildingTypeComboBox;

  QDoubleValidator* m_ratioValidator;
  QDoubleValidator* m_positiveDoubleValidator;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELDESIGNWIZARDDIALOG_HPP
