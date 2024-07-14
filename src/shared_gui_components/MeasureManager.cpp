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

#include "MeasureManager.hpp"

#include "BaseApp.hpp"
#include "BCLMeasureDialog.hpp"
#include "LocalLibraryController.hpp"
#include "ProcessEventsProgressBar.hpp"

#include "BuildingComponentDialog.hpp"
#include "OSDialog.hpp"

#include "../model_editor/Application.hpp"
#include "../model_editor/UserSettings.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/measure/OSArgument.hpp>

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/core/System.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/bcl/RemoteBCL.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>
#include <openstudio/utilities/filetypes/WorkflowJSON.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep_Impl.hpp>

#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"

#include <json/json.h>

#include <QAbstractButton>
#include <QBoxLayout>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QRadioButton>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
// Debug only
//#include <QSslError>
//#include <QDateTime>

namespace openstudio {

MeasureManager::MeasureManager(BaseApp* t_app) : m_app(t_app), m_started(false) {}

QUrl MeasureManager::url() const {
  return m_url;
}

void MeasureManager::setUrl(const QUrl& url) {
  m_url = url;
}

void MeasureManager::setResourcesPath(const openstudio::path& resourcesPath) {
  m_resourcesPath = resourcesPath;
}

bool MeasureManager::waitForStarted(int msec) {
  if (m_started) {
    return true;
  }

  // ping server until get a started response
  bool success = false;

  QUrl thisUrl;
  QNetworkAccessManager manager;

  const int msecPerLoop = 20;
  const int numTries = msec / msecPerLoop;
  int current = 0;
  while (!success && current < numTries) {

    // m_url may change if measure manager is restarted
    thisUrl = m_url;
    thisUrl.setPath("/");
    QNetworkRequest request(thisUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = manager.get(request);

    // If trying to debug a potential SSL error
    // connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    // connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply, SLOT(ignoreSslErrors()));

    while (reply->isRunning()) {
      Application::instance().processEvents();
    }

    auto error = reply->error();
    delete reply;

    if (error == QNetworkReply::NoError) {
      success = true;
    } else {
      // We actually want to pause for msecPerLoop, processEvents will only put an upper limit the event processing that needs doing
      // > processEvents: Processes pending events for the calling thread for maxTime_ms milliseconds or
      // > ** until there are no more events to process,whichever is shorter**.
      // Application::instance().processEvents(msecPerLoop);

      // So we use QThread::msleep instead, which is not 100% accurate but we do not care. I am getting about 20ms delay in between tries
      // Debug
      // LOG(Debug, "[" << current << ", " << QDateTime::currentDateTime().toMSecsSinceEpoch() << " ms]: QNetworkReply is " << error);

      QThread::msleep(msecPerLoop);
    }

    ++current;
  }

  if (success) {
    m_started = true;
  } else {
    LOG(Error, "Measure manager server failed to start. Was looking at URL=" << toString(thisUrl.toString()));
  }

  return m_started;
}

bool MeasureManager::isStarted() const {
  return m_started;
}

/* If trying to debug a potential SSL error
 *void MeasureManager::sslErrors(const QList<QSslError>& errors)
 *{
 *  // Prints each SSL eorr
 *  foreach(QSslError error, errors)
 *  {
 *    LOG(Debug, "sslError" << toString(error.errorString()));
 *  }
 *}
 */

openstudio::path MeasureManager::tempModelPath() const {
  return m_tempModelPath;
}

void MeasureManager::saveTempModel(const path& tempDir) {
  waitForStarted();

  boost::optional<model::Model> model = m_app->currentModel();

  // DLM: don't get tempDir from app because this requires OSDocument and we may call saveTempModel from OSDocument ctor
  //boost::optional<path> tempDir2 = m_app->tempDir();

  if (!model) {
    return;
  }
  if (!exists(tempDir)) {
    return;
  }

  m_tempModelPath = tempDir / toPath("temp_measure_manager.osm");

  model->save(m_tempModelPath, true);

  m_measureArguments.clear();
}

boost::optional<BCLMeasure> MeasureManager::standardReportMeasure() const {
  // DLM: Breaking changes in openstudio_results measures prevent us from being able to ensure
  // that measure in users local BCL or remote BCL will work, just use measure in installer
  return BCLMeasure::load(m_resourcesPath / toPath("openstudio_results"));
}

std::vector<BCLMeasure> MeasureManager::bclMeasures() const {
  std::vector<BCLMeasure> result;
  result.reserve(m_bclMeasures.size());
  for (const auto& bclMeasure : m_bclMeasures) {
    result.push_back(bclMeasure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::myMeasures() const {
  std::vector<BCLMeasure> result;
  result.reserve(m_myMeasures.size());
  for (const auto& measure : m_myMeasures) {
    result.push_back(measure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::combinedMeasures() const {
  std::vector<BCLMeasure> result;
  result.reserve(m_myMeasures.size() + m_bclMeasures.size());

  std::set<UUID> resultUUIDs;

  // insert my measures
  for (auto it = m_myMeasures.begin(), itend = m_myMeasures.end(); it != itend; ++it) {
    if (resultUUIDs.find(it->first) == resultUUIDs.end()) {
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    } else {
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

  // insert bcl measures
  for (auto it = m_bclMeasures.begin(), itend = m_bclMeasures.end(); it != itend; ++it) {
    if (resultUUIDs.find(it->first) == resultUUIDs.end()) {
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    } else {
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

  return result;
}

boost::optional<BCLMeasure> MeasureManager::getMeasure(const UUID& id) {
  boost::optional<BCLMeasure> result;

  std::map<UUID, BCLMeasure>::iterator it;

  // search my measures
  it = m_myMeasures.find(id);
  if (it != m_myMeasures.end()) {
    if (result) {
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory()
                                             << "' will be used instead");
    } else {
      result = it->second;
    }
  }

  // search bcl measures
  it = m_bclMeasures.find(id);
  if (it != m_bclMeasures.end()) {
    if (result) {
      LOG(Error, "UUID of bcl measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory()
                                            << "' will be used instead");
    } else {
      result = it->second;
    }
  }

  return result;
}

BCLMeasure MeasureManager::insertReplaceMeasure(const UUID& t_id) {
  // DLM: TODO, emit signal to rebuild WorkflowController

  boost::optional<BCLMeasure> measure = getMeasure(t_id);
  if (!measure) {
    LOG_AND_THROW("Cannot find measure '" << toString(t_id) << "'");
  }

  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
  boost::optional<BCLMeasure> existingMeasure = workflowJSON.getBCLMeasureByUUID(t_id);

  if (existingMeasure && (existingMeasure->versionUUID() != measure->versionUUID())) {
    QDialog dialog(m_app->mainWidget(), Qt::WindowFlags(Qt::Dialog | Qt::WindowTitleHint));
    auto* mainContentVLayout = new QVBoxLayout();
    dialog.setWindowTitle(QCoreApplication::applicationName());

    dialog.setLayout(mainContentVLayout);
    auto* label = new QLabel("A modified copy of this measure is already being used in this project.");
    auto* replace = new QRadioButton("Replace all instances of this measure in this project with this one.");
    replace->setChecked(true);
    auto* copy = new QRadioButton("Create a new instance using this project's copy of this measure.");
    mainContentVLayout->addWidget(label);
    mainContentVLayout->addWidget(replace);
    mainContentVLayout->addWidget(copy);

    auto* buttons = new QHBoxLayout();

    auto* cancel = new QPushButton("Cancel");
    auto* apply = new QPushButton("Apply");

    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(apply);

    connect(cancel, &QPushButton::pressed, &dialog, &QDialog::reject);

    connect(apply, &QPushButton::pressed, &dialog, &QDialog::accept);

    mainContentVLayout->addLayout(buttons);

    if (dialog.exec() == QDialog::Accepted) {
      if (replace->isChecked()) {
        LOG(Info, "User chose to replace existing instances with new version of measure");
        std::pair<bool, std::string> updateResult = updateMeasure(*measure);
        if (updateResult.first) {
          // reloading measure from project
          boost::optional<BCLMeasure> updatedMeasure = workflowJSON.getBCLMeasureByUUID(t_id);
          OS_ASSERT(updatedMeasure);
          return *updatedMeasure;
        } else {
          QMessageBox::critical(m_app->mainWidget(), QString("Error Updating Measure"), QString::fromStdString(updateResult.second));
          throw std::runtime_error("Unknown error occurred in updateMeasure");
        }
      } else {
        LOG(Info, "User chose to use existing copy of measure for new instance");
        return *existingMeasure;
      }
    } else {
      LOG(Info, "User canceled drop event for mismatched measure version");
      throw std::runtime_error("User canceled update / insert of measure");
    }
  } else if (existingMeasure) {
    return *existingMeasure;
  }

  boost::optional<BCLMeasure> projectmeasure = workflowJSON.addMeasure(*measure);
  if (!projectmeasure) {
    LOG(Info, "WorkflowJSON::addMeasure failed");
    throw std::runtime_error("Failed to add measure to the project");
  }
  return *projectmeasure;
}

std::pair<bool, std::string> MeasureManager::updateMeasure(const BCLMeasure& t_measure) {
  // check that measure is from outside of project?

  std::pair<bool, std::string> result(true, "");
  try {
    // this check is just to make sure the script does not have errors
    measure::OSArgumentVector args = getArguments(t_measure);

    WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
    boost::optional<BCLMeasure> measure = workflowJSON.addMeasure(t_measure);

    if (measure) {
      // Since we set the measure_paths, we only neeed to reference the name of the directory (=last level directory name)
      // eg: /path/to/measure_folder => measure_folder
      result = std::pair<bool, std::string>(true, toString(getLastLevelDirectoryName(measure->directory())));
    } else {
      std::stringstream ss;
      ss << "An error occurred while adding measure '" << t_measure.displayName() << "' to the project.";
      LOG(Error, ss.str());
      result = std::pair<bool, std::string>(false, ss.str());
    }

  } catch (const std::exception& e) {
    std::stringstream ss;
    ss << "An error occurred while updating measure '" << t_measure.displayName() << "':" << std::endl;
    ss << "  " << e.what();
    LOG(Error, ss.str());
    result = std::pair<bool, std::string>(false, ss.str());
  }

  return result;
}

void MeasureManager::updateMeasures(const std::vector<BCLMeasure>& newMeasures, bool t_showMessage) {
  std::vector<BCLMeasure> measures;

  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
  for (const auto& measure : newMeasures) {
    if (workflowJSON.getBCLMeasureByUUID(measure.uuid())) {
      measures.push_back(measure);
    }
  }

  auto* progress = new ProcessEventsProgressBar();
  progress->setMaximum(std::numeric_limits<int>::max());

  size_t loc = 0;
  std::vector<std::string> failMessages;
  for (const auto& measure : measures) {
    progress->setValue(loc);
    std::pair<bool, std::string> updateResult = updateMeasure(measure);
    if (!updateResult.first) {
      failMessages.push_back(updateResult.second);
    }
    ++loc;
  }
  progress->setValue(measures.size());

  delete progress;

  if (t_showMessage) {
    size_t numUpdated = loc - failMessages.size();
    size_t numFailed = failMessages.size();

    std::stringstream ss;

    ss << numUpdated << " measure";
    if (numUpdated == 0 || numUpdated > 1) {
      ss << "s";
    }
    ss << " updated";

    if (numFailed > 0) {
      ss << std::endl << numFailed << " measure update";
      if (numFailed > 1) {
        ss << "s";
      }
      ss << " failed";
      QString errors;
      for (const std::string& failMessage : failMessages) {
        errors.append(QString::fromStdString(failMessage));
        errors.append("\n\n");
      }

      auto* messageBox = new QMessageBox(m_app->mainWidget());
      messageBox->setWindowTitle(QString("Measures Updated"));
      messageBox->setText(toQString(ss.str()));
      messageBox->setDetailedText(errors);
      //messageBox->setMinimumWidth(330);
      //messageBox->setSizeGripEnabled(true);

      // DLM: there is a bug in QMessageBox where setMinimumWidth is not used
      // http://www.qtcentre.org/threads/22298-QMessageBox-Controlling-the-width?p=113348#post113348
      auto* horizontalSpacer = new QSpacerItem(330, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
      auto* layout = static_cast<QGridLayout*>(messageBox->layout());
      layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

      messageBox->exec();
      delete messageBox;
    } else {
      QMessageBox::information(m_app->mainWidget(), QString("Measures Updated"), toQString(ss.str()));
    }
  }
}

std::vector<measure::OSArgument> MeasureManager::getArguments(const BCLMeasure& t_measure) {

  auto it = m_measureArguments.find(t_measure.directory());
  if (it != m_measureArguments.end()) {
    return it->second;
  }

  QUrl thisUrl(m_url);
  thisUrl.setPath("/compute_arguments");

  // std::string url_s = m_url.toString().toStdString();

  QNetworkRequest request(thisUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject obj;
  obj["measure_dir"] = toQString(t_measure.directory());
  obj["osm_path"] = toQString(m_tempModelPath);
  const QJsonDocument doc(obj);
  const QByteArray data = doc.toJson(QJsonDocument::Compact);

  QNetworkAccessManager manager;
  QNetworkReply* reply = manager.post(request, data);

  while (reply->isRunning()) {
    Application::instance().processEvents();
  }

  const QString replyString = reply->readAll();
  const std::string s = replyString.toStdString();
  auto error = reply->error();
  delete reply;

  if (error != QNetworkReply::NoError) {
    LOG_AND_THROW("Error computing arguments: " << s)
  }

  std::vector<measure::OSArgument> result;

  Json::CharReaderBuilder rbuilder;
  std::istringstream ss(s);
  std::string errorString;
  Json::Value json;
  bool parsingSuccessful = Json::parseFromStream(rbuilder, ss, &json, &errorString);

  if (parsingSuccessful && json.type() == Json::objectValue) {

    Json::Value arguments = json.get("arguments", Json::Value(Json::arrayValue));

    Json::ArrayIndex n = arguments.size();
    for (Json::ArrayIndex i = 0; i < n; ++i) {

      Json::Value argument = arguments[i];

      try {
        std::string typeString = argument.get("type", Json::Value("")).asString();
        measure::OSArgumentType type(typeString);
        boost::optional<measure::OSArgument> osArgument = getArgument(type, argument);
        if (osArgument) {
          result.push_back(*osArgument);
        } else {
          errorString += "Could not convert argument.";
        }
      } catch (const std::exception& e) {
        errorString += std::string("Error occurred: ") + e.what();
        continue;
      }
    }

  } else {
    LOG_AND_THROW("Error computing arguments: " << s)
  }

  if (!errorString.empty()) {
    LOG_AND_THROW(errorString);
  }

  m_measureArguments.insert(std::make_pair(t_measure.directory(), result));

  return result;
}

boost::optional<measure::OSArgument> MeasureManager::getArgument(const measure::OSArgumentType& type, const Json::Value& argument) {
  std::string name = argument.get("name", Json::Value("")).asString();
  bool required = argument.get("required", Json::Value(false)).asBool();
  bool modelDependent = argument.get("model_dependent", Json::Value(false)).asBool();

  boost::optional<measure::OSArgument> result;
  if (type.value() == measure::OSArgumentType::Boolean) {
    result = measure::OSArgument::makeBoolArgument(name, required, modelDependent);

    if (argument.isMember("default_value")) {
      bool defaultValue = argument.get("default_value", Json::Value(false)).asBool();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Double) {
    result = measure::OSArgument::makeDoubleArgument(name, required, modelDependent);

    if (argument.isMember("default_value")) {
      double defaultValue = argument.get("default_value", Json::Value(0.0)).asDouble();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Quantity) {
    LOG(Warn, "Mapping deprecated OSArgumentType::Quantity to Double");
    result = measure::OSArgument::makeDoubleArgument(name, required, modelDependent);

    if (argument.isMember("default_value")) {
      double defaultValue = argument.get("default_value", Json::Value(0.0)).asDouble();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Integer) {
    result = measure::OSArgument::makeIntegerArgument(name, required, modelDependent);

    if (argument.isMember("default_value")) {
      int defaultValue = argument.get("default_value", Json::Value(0.0)).asInt();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::String) {
    result = measure::OSArgument::makeStringArgument(name, required, modelDependent);

    if (argument.isMember("default_value")) {
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Choice) {

    Json::Value choiceValues = argument.get("choice_values", Json::Value(Json::arrayValue));
    Json::Value choiceDisplayNames = argument.get("choice_display_names", Json::Value(Json::arrayValue));

    if (choiceValues.empty()) {
      choiceValues = argument.get("choices_values", Json::Value(Json::arrayValue));
    }

    Json::ArrayIndex n = choiceValues.size();
    if (n != choiceDisplayNames.size()) {
      choiceDisplayNames = choiceValues;
    }

    std::vector<std::string> choices;
    std::vector<std::string> displayNames;
    for (Json::ArrayIndex i = 0; i < n; ++i) {
      choices.push_back(choiceValues[i].asString());
      displayNames.push_back(choiceDisplayNames[i].asString());
    }

    result = measure::OSArgument::makeChoiceArgument(name, choices, displayNames, required, modelDependent);

    if (argument.isMember("default_value")) {
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Path) {

    bool isRead = argument.get("is_read", Json::Value(false)).asBool();
    std::string extension = argument.get("extension", Json::Value("*")).asString();

    result = measure::OSArgument::makePathArgument(name, isRead, extension, required, modelDependent);

    if (argument.isMember("default_value")) {
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Separator) {

    result = measure::OSArgument::makeSeparatorArgument(name, modelDependent);
  }
  OS_ASSERT(result);

  if (argument.isMember("display_name")) {
    std::string displayName = argument.get("display_name", Json::Value("")).asString();
    result->setDisplayName(displayName);
  }

  if (argument.isMember("description")) {
    std::string description = argument.get("description", Json::Value("")).asString();
    result->setDescription(description);
  }

  if (argument.isMember("units")) {
    std::string units = argument.get("units", Json::Value("")).asString();
    result->setUnits(units);
  }

  return result;
}

//void MeasureManager::updateOpenStudioMeasures(analysisdriver::SimpleProject &t_project)
//{
//  updateMeasuresLists();
//
//  std::vector<BCLMeasure> toUpdate;
//
//  std::vector<BCLMeasure> measures = openstudioMeasures();
//
//  for (auto itr = measures.begin();
//      itr != measures.end();
//      ++itr)
//  {
//    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(itr->uuid());
//    if (projectmeasure)
//    {
//      if (projectmeasure->versionUUID() != itr->versionUUID())
//      {
//        toUpdate.push_back(*itr);
//      }
//    }
//  }
//
//  updateMeasures(t_project, toUpdate, false);
//}
//
//void MeasureManager::updateBCLMeasures(analysisdriver::SimpleProject &t_project)
//{
//  updateMeasuresLists();
//
//  std::vector<BCLMeasure> toUpdate;
//
//  std::vector<BCLMeasure> measures = bclMeasures();
//
//  for (auto & measure : measures)
//  {
//    if (m_openstudioMeasures.find(measure.uuid()) != m_openstudioMeasures.end()){
//      // do not attempt to update built in measures with different version in bcl
//      LOG(Warn, "Skipping update of built in measure");
//      continue;
//    }
//
//    if (m_myMeasures.find(measure.uuid()) != m_myMeasures.end()){
//      // do not attempt to update user measures with different version in bcl
//      LOG(Warn, "Skipping update of user measure");
//      continue;
//    }
//
//    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(measure.uuid());
//    if (projectmeasure)
//    {
//      if (projectmeasure->versionUUID() != measure.versionUUID())
//      {
//        toUpdate.push_back(measure);
//      }
//    }
//  }
//
//  updateMeasures(t_project, toUpdate);
//}

std::string MeasureManager::suggestMeasureName(const BCLMeasure& t_measure) {
  std::string baseName = t_measure.displayName();

  std::set<std::string> allNames;
  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
  for (const auto& step : workflowJSON.workflowSteps()) {
    if (step.optionalCast<MeasureStep>()) {
      boost::optional<std::string> name = step.cast<MeasureStep>().name();
      if (name) {
        allNames.insert(*name);
      }
    }
  }

  std::string result = baseName;
  int i = 1;
  while (allNames.find(result) != allNames.end()) {
    result = baseName + " " + QString::number(i).toStdString();
    i++;
  }

  return result;
}

bool MeasureManager::isMeasureSelected() {
  if (m_libraryController) {
    QPointer<LibraryItem> item = m_libraryController->selectedItem();
    return !item.isNull();
  }
  return false;
}

void MeasureManager::updateMeasuresLists() {
  openstudio::path umd = userMeasuresDir();

  auto updateUserMeasures = true;
  if (isNetworkPath(umd) && !isNetworkPathAvailable(umd)) {
    updateUserMeasures = false;
  }

  updateMeasuresLists(updateUserMeasures);
}

void MeasureManager::updateMeasuresLists(bool updateUserMeasures) {
  checkForLocalBCLUpdates();

  if (updateUserMeasures) {
    checkForUpdates(userMeasuresDir(), false);
  }

  if (!m_mutex.tryLock()) {
    return;
  }

  m_myMeasures.clear();
  m_bclMeasures.clear();
  m_measureArguments.clear();

  if (updateUserMeasures) {
    std::vector<BCLMeasure> uMeasures = userMeasures();
    for (auto& measure : uMeasures) {
      bool updateUUID = false;
      if (m_myMeasures.find(measure.uuid()) != m_myMeasures.end()) {
        updateUUID = true;
      }

      if (updateUUID) {
        // duplicate measure detected, manual copy and paste likely cause
        // assign measure a new UUID here and save
        measure.changeUID();
        measure.incrementVersionId();
        measure.save();
      }

      m_myMeasures.insert(std::pair<UUID, BCLMeasure>(measure.uuid(), measure));
    }
  }

  std::vector<BCLMeasure> lbm = localBCLMeasures();
  for (auto& measure : lbm) {
    auto it = m_bclMeasures.find(measure.uuid());
    if (it != m_bclMeasures.end()) {
      // duplicate measure detected
      LOG(Error, "UUID of bcl measure at '" << measure.directory() << "' conflicts with other bcl measure, measure at '" << it->second.directory()
                                            << "' will be used instead");
    } else {
      m_bclMeasures.insert(std::pair<UUID, BCLMeasure>(measure.uuid(), measure));
    }
  }

  m_mutex.unlock();

  if (m_libraryController) {
    m_libraryController->reset();
  }
}

//void MeasureManager::updateMyMeasures(analysisdriver::SimpleProject &t_project)
//{
//  updateMeasuresLists();
//
//  std::vector<BCLMeasure> toUpdate;
//
//  std::vector<BCLMeasure> measures = myMeasures();
//
//  for (auto & measure : measures)
//  {
//
//    if (m_openstudioMeasures.find(measure.uuid()) != m_openstudioMeasures.end()){
//      // do not attempt to update built in measures
//      LOG(Warn, "Skipping update of built in measure");
//      continue;
//    }
//
//    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(measure.uuid());
//    if (projectmeasure)
//    {
//      if (projectmeasure->versionUUID() != measure.versionUUID())
//      {
//        toUpdate.push_back(measure);
//      }
//    }
//  }
//
//  updateMeasures(t_project, toUpdate);
//}

bool MeasureManager::reset() {
  waitForStarted();

  if (!m_mutex.tryLock()) {
    return false;
  }

  QUrl thisUrl(m_url);
  thisUrl.setPath("/reset");

  // std::string url_s = m_url.toString().toStdString();

  QNetworkRequest request(thisUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  const QByteArray data = QString("{}").toUtf8();

  QNetworkAccessManager manager;
  QNetworkReply* reply = manager.post(request, data);

  while (reply->isRunning()) {
    Application::instance().processEvents();
  }

  // QString replyString = reply->readAll();
  // std::string s = replyString.toStdString();

  bool result = true;
  if (reply->error() != QNetworkReply::NoError) {
    result = false;
  }

  delete reply;

  m_mutex.unlock();

  return result;
}

bool MeasureManager::checkForLocalBCLUpdates() {
  waitForStarted();

  if (!m_mutex.tryLock()) {
    return false;
  }

  QUrl thisUrl(m_url);
  thisUrl.setPath("/bcl_measures");

  // std::string url_s = m_url.toString().toStdString();

  QNetworkRequest request(thisUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  const QByteArray data = QString("{}").toUtf8();

  QNetworkAccessManager manager;
  QNetworkReply* reply = manager.post(request, data);

  while (reply->isRunning()) {
    Application::instance().processEvents();
  }

  // QString replyString = reply->readAll();
  // std::string s = replyString.toStdString();

  bool result = true;
  if (reply->error() != QNetworkReply::NoError) {
    result = false;
  }

  delete reply;

  m_mutex.unlock();

  return result;
}

bool MeasureManager::checkForUpdates(const openstudio::path& measureDir, bool force) {
  waitForStarted();

  if (!m_mutex.tryLock()) {
    return false;
  }

  QUrl thisUrl(m_url);
  thisUrl.setPath("/update_measures");

  // std::string url_s = m_url.toString().toStdString();

  QJsonObject obj;
  obj["measure_dir"] = toQString(measureDir);
  obj["force_reload"] = force;
  const QJsonDocument doc(obj);
  const QByteArray data = doc.toJson();

  QNetworkRequest request(thisUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QNetworkAccessManager manager;
  QNetworkReply* reply = manager.post(request, data);

  while (reply->isRunning()) {
    Application::instance().processEvents();
  }

  // QString replyString = reply->readAll();
  // std::string s = replyString.toStdString();

  bool result = true;
  if (reply->error() != QNetworkReply::NoError) {
    result = false;
  }

  delete reply;

  m_mutex.unlock();

  return result;
}

void MeasureManager::checkForRemoteBCLUpdates() {
  RemoteBCL remoteBCL;
  remoteBCL.checkForMeasureUpdates();
  std::vector<BCLSearchResult> updates = remoteBCL.measuresWithUpdates();

  // remove false updates (e.g. measure was updated after downloading from bcl due to incorrect sha)
  updates.erase(std::remove_if(updates.begin(), updates.end(),
                               [this](const BCLSearchResult& update) {
                                 auto current = m_bclMeasures.find(toUUID(update.uid()));
                                 if (current != m_bclMeasures.end()) {
                                   if (update.versionModified() && current->second.versionModified()) {
                                     return update.versionModified().get() < current->second.versionModified().get();
                                   }
                                 }
                                 return false;
                               }),
                updates.end());

  int numUpdates = updates.size();

  if (numUpdates == 0) {
    QMessageBox::information(m_app->mainWidget(), tr("Measures Updated"), tr("All measures are up-to-date."));
  } else {

    QString text(QString::number(numUpdates) + tr(" measures have been updated on BCL compared to your local BCL directory.\n")
                 + tr("Would you like update them?"));

    QString detailedText;
    for (const BCLSearchResult& update : updates) {
      detailedText += toQString("* name: " + update.name() + "\n");
      detailedText += toQString(" - uid: " + update.uid() + "\n");
      auto current = m_bclMeasures.find(toUUID(update.uid()));
      if (current != m_bclMeasures.end()) {
        detailedText += toQString(" - old versionId: " + current->second.versionId() + "\n");
      }
      detailedText += toQString(" - new versionId: " + update.versionId() + "\n\n");
    }
    QMessageBox msg(QMessageBox::Question, tr("Measures Updated"), text, QMessageBox::Yes | QMessageBox::No, m_app->mainWidget());
    msg.setDetailedText(detailedText);

    int result = msg.exec();
    if (result == QMessageBox::Yes) {
      remoteBCL.updateMeasures();

      // remoteBCL.updateMeasures should remove outdated measures, but won't work correctly until https://github.com/NREL/OpenStudio/pull/5129
      // if we have the new measure, delete outdated ones
      for (const BCLSearchResult& update : updates) {
        if (OSAppBase::instance()->currentDocument()->getLocalMeasure(update.uid(), update.versionId())) {
          OSAppBase::instance()->currentDocument()->removeOutdatedLocalMeasures(update.uid(), update.versionId());
        }
      }

      updateMeasuresLists(false);
    }
  }
}

void MeasureManager::addMeasure() {
  // open modal dialog
  //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(this->mainWindow));
  QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog());

  if (dialog->exec() != 0) {

    // not canceled, create measure
    boost::optional<BCLMeasure> measure = dialog->createMeasure();
    if (measure) {

      // open directory for editing
      QString path = QDir::toNativeSeparators(toQString(measure->directory()));
      QDesktopServices::openUrl(QUrl::fromLocalFile(path));

      updateMeasuresLists();

      // reload measure that has been updated
      measure = getMeasure(measure->uuid());
      OS_ASSERT(measure);

      // emit signal
      emit newMeasure(*measure);
    } else {
      // error, this should not happen
    }

  } else {
    // user canceled, do nothing
  }
}

void MeasureManager::duplicateSelectedMeasure() {
  OS_ASSERT(m_libraryController);
  QPointer<LibraryItem> item = m_libraryController->selectedItem();

  if (!item.isNull()) {
    boost::optional<BCLMeasure> bclMeasure = this->getMeasure(item->uuid());
    if (bclMeasure) {

      // check for updates in case measure being copied has changed
      checkForUpdates(bclMeasure->directory(), true);

      // open modal dialog
      //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure, this->mainWindow));
      QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure));

      if (dialog->exec() != 0) {

        // not canceled, create measure
        boost::optional<BCLMeasure> measure = dialog->createMeasure();
        if (measure) {

          // open directory for editing
          QString path = QDir::toNativeSeparators(toQString(measure->directory()));
          QDesktopServices::openUrl(QUrl::fromLocalFile(path));

          updateMeasuresLists();

          // emit signal
          emit newMeasure(*measure);
        } else {
          // error, this should not happen
        }

      } else {
        // user canceled, do nothing
      }
    }
  }
}

//QSharedPointer<measure::OSMeasureInfoGetter> MeasureManager::infoGetter() const
//{
//  return m_infoGetter;
//}

void MeasureManager::setLibraryController(const QSharedPointer<LocalLibraryController>& t_libraryController) {
  m_libraryController = t_libraryController;
}

}  // namespace openstudio
