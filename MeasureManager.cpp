/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include "MeasureManager.hpp"

#include "BaseApp.hpp"
#include "BCLMeasureDialog.hpp"
#include "LocalLibraryController.hpp"
#include "ProcessEventsProgressBar.hpp"

#include "BuildingComponentDialog.hpp"
#include "OSDialog.hpp"

#include "../measure/OSArgument.hpp"

#include "../model/Model.hpp"

#include "../utilities/core/ApplicationPathHelpers.hpp"
#include "../utilities/core/Application.hpp"
#include "../utilities/core/Assert.hpp"
#include "../utilities/core/PathHelpers.hpp"
#include "../utilities/core/RubyException.hpp"
#include "../utilities/bcl/BCLMeasure.hpp"
#include "../utilities/bcl/RemoteBCL.hpp"
#include "../utilities/filetypes/WorkflowJSON.hpp"
#include "../utilities/filetypes/WorkflowStep.hpp"
#include "../utilities/filetypes/WorkflowStep_Impl.hpp"

#include <jsoncpp/json.h>

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

namespace openstudio {

MeasureManager::MeasureManager(BaseApp *t_app)
  : m_app(t_app)
{
  m_networkAccessManager = new QNetworkAccessManager(this);
}

QUrl MeasureManager::url() const
{
  return m_url;
}

void MeasureManager::setUrl(const QUrl& url)
{
  m_url = url;
}

void MeasureManager::saveTempModel()
{
  boost::optional<model::Model> model = m_app->currentModel();
  boost::optional<path> tempDir = m_app->tempDir();

  if (!model){
    return;
  }
  if (!tempDir){
    return;
  }

  m_tempModelPath = tempDir.get() / toPath("temp_measure_manager.osm");

  model->save(m_tempModelPath, true);

  QUrl url(m_url);
  url.setPath("/reset");

  std::string url_s = m_url.toString().toStdString();

  QString data("{}");

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "json");

  QNetworkAccessManager manager;

  QNetworkReply* reply = manager.post(request, data.toUtf8());

  while (reply->isRunning()){
    Application::instance().processEvents();
  }

  QString replyString = reply->readAll();
  std::string s = replyString.toStdString();

  bool result = true;
  if (reply->error() != QNetworkReply::NoError){
    result = false;
  }

  delete reply;
}


std::pair<bool,std::string> MeasureManager::updateMeasure(const BCLMeasure &t_measure)
{
  std::pair<bool,std::string> result(true,"");
  try {
    measure::OSArgumentVector args = getArguments(t_measure);

    // DLM: TODO, I guess we should cache arguments here?
   // bool differentVersions = t_project.updateMeasure(t_measure, args);
   // 
   // if (!differentVersions) {
   //   OptionalBCLMeasure existingMeasure = t_project.getMeasureByUUID(t_measure.uuid());
   //   if (existingMeasure) {
   //     LOG(Debug, "Measure hasn't changed, but args were reloaded, forcing argument resetting");
   //     t_project.registerArguments(*existingMeasure,args);
   //     t_project.analysis().problem().updateMeasure(*existingMeasure, args, false);
   //   }
   // } 
  } catch ( const RubyException &e ) {
    std::stringstream ss;
    ss << "An error occurred while updating measure '" << t_measure.displayName() << "':" << std::endl;
    ss << "  " << e.what();
    LOG(Error, ss.str());
    result = std::pair<bool,std::string>(false,ss.str());
  }

  return result;
}

BCLMeasure MeasureManager::insertReplaceMeasure(const UUID &t_id)
{
  // DLM: TODO, hook up workflowJSON changed signal to doc dirty, same place as conncet model signal
  // DLM: TODO, emit signal to rebuild WorkflowController

  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();

  boost::optional<BCLMeasure> measure = getMeasure(t_id);
  OS_ASSERT(measure);

  boost::optional<BCLMeasure> existingMeasure = workflowJSON.getBCLMeasureByUUID(t_id);

  if (existingMeasure && (existingMeasure->versionUUID() != measure->versionUUID()))
  {
    QDialog dialog(m_app->mainWidget(), Qt::WindowFlags(Qt::Dialog | Qt::WindowTitleHint));
    auto mainContentVLayout = new QVBoxLayout();
    dialog.setWindowTitle(QCoreApplication::applicationName());

    dialog.setLayout(mainContentVLayout);
    QLabel *label = new QLabel("A modified copy of this measure is already being used in this project.");
    QRadioButton *replace = new QRadioButton("Replace all instances of this measure in this project with this one.");
    replace->setChecked(true);
    QRadioButton *copy = new QRadioButton("Create a new instance using this project's copy of this measure.");
    mainContentVLayout->addWidget(label);
    mainContentVLayout->addWidget(replace);
    mainContentVLayout->addWidget(copy);

    auto buttons = new QHBoxLayout();

    QPushButton *cancel = new QPushButton("Cancel");
    QPushButton *apply = new QPushButton("Apply");

    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(apply);

    connect(cancel, &QPushButton::pressed, &dialog, &QDialog::reject);

    connect(apply, &QPushButton::pressed, &dialog, &QDialog::accept);
    
    mainContentVLayout->addLayout(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
      if (replace->isChecked())
      {
        LOG(Info, "User chose to replace existing instances with new version of measure");
        std::pair<bool,std::string> updateResult = updateMeasure(*measure);
        if (updateResult.first)
        {
          boost::optional<BCLMeasure> updatedMeasure = getMeasure(t_id);
          OS_ASSERT(updatedMeasure);
          return *updatedMeasure;
        } else {
          QMessageBox::critical(m_app->mainWidget(), QString("Error Updating Measure"), QString::fromStdString(updateResult.second));
          throw std::runtime_error("Unknown error occurred when calling project.updateMeasure, false was returned");
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
  if (!projectmeasure){
    LOG(Info, "WorkflowJSON::addMeasure failed");
    throw std::runtime_error("Failed to add measure to the project");
  }
  return *projectmeasure;
}

std::vector<measure::OSArgument> MeasureManager::getArguments(const BCLMeasure &t_measure)
{
  QUrl url(m_url);
  url.setPath("/compute_arguments");

  std::string url_s = m_url.toString().toStdString();

  QString data = QString("{\"measure_dir\": \"") + toQString(t_measure.directory()) + QString("\", \"osm_path\": \"") + toQString(m_tempModelPath) + QString("\"}");

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "json");

  QNetworkAccessManager manager;

  QNetworkReply* reply = manager.post(request, data.toUtf8());

  while (reply->isRunning()){
    Application::instance().processEvents();
  }

  QString replyString = reply->readAll();
  std::string s = replyString.toStdString();

  bool test = true;
  if (reply->error() != QNetworkReply::NoError){
    // TODO: handle error
    test = false;
  }
  delete reply;

  std::vector<measure::OSArgument> result;
  if (test){
    Json::Reader reader;
    Json::Value json;
    bool parsingSuccessful = reader.parse(s, json);
    if (parsingSuccessful){

      Json::Value arguments = json.get("arguments",  Json::Value(Json::arrayValue));

      Json::ArrayIndex n = arguments.size();
      for (Json::ArrayIndex i = 0; i < n; ++i){

        Json::Value argument = arguments[i];

        try{
          std::string typeString = argument.get("type", Json::Value("")).asString();
          measure::OSArgumentType type(typeString);
          boost::optional<measure::OSArgument> osArgument = getArgument(type, argument);
          if (osArgument){
            result.push_back(*osArgument);
          } else{
            // TODO: handle error
          }
        } catch (const std::exception& ){
          // TODO: handle error
          continue;
        }
      }

    } else{
      // TODO: handle error
    }
  }


  return result;
}

boost::optional<measure::OSArgument> MeasureManager::getArgument(const measure::OSArgumentType& type, const Json::Value& argument)
{
  std::string name = argument.get("name", Json::Value("")).asString();
  bool required = argument.get("required", Json::Value(false)).asBool();
  bool modelDependent = argument.get("model_dependent", Json::Value(false)).asBool();

  boost::optional<measure::OSArgument> result;
  if (type.value() == measure::OSArgumentType::Boolean){
    result = measure::OSArgument::makeBoolArgument(name, required, modelDependent);

    if (argument.isMember("default_value")){
      bool defaultValue = argument.get("default_value", Json::Value(false)).asBool();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Double){
    result = measure::OSArgument::makeDoubleArgument(name, required, modelDependent);

    if (argument.isMember("default_value")){
      double defaultValue = argument.get("default_value", Json::Value(0.0)).asDouble();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Quantity){
    result = measure::OSArgument::makeQuantityArgument(name, required, modelDependent);

    if (argument.isMember("default_value")){
      double defaultValue = argument.get("default_value", Json::Value(0.0)).asDouble();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Integer){
    result = measure::OSArgument::makeIntegerArgument(name, required, modelDependent);

    if (argument.isMember("default_value")){
      double defaultValue = argument.get("default_value", Json::Value(0.0)).asInt();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::String){
    result = measure::OSArgument::makeStringArgument(name, required, modelDependent);

    if (argument.isMember("default_value")){
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Choice){

    Json::Value choiceValues = argument.get("choice_values",  Json::Value(Json::arrayValue));
    Json::Value choiceDisplayNames = argument.get("choice_display_names",  Json::Value(Json::arrayValue));

    Json::ArrayIndex n = choiceValues.size();
    if (n != choiceDisplayNames.size()){
      choiceDisplayNames = choiceValues;
    }

    std::vector<std::string> choices;
    std::vector<std::string> displayNames;
    for (Json::ArrayIndex i = 0; i < n; ++i){
      choices.push_back(choiceValues[i].asString());
      displayNames.push_back(choiceDisplayNames[i].asString());
    }

    result = measure::OSArgument::makeChoiceArgument(name, choices, displayNames, required, modelDependent);

    if (argument.isMember("default_value")){
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }

  } else if (type.value() == measure::OSArgumentType::Path){

    bool isRead = argument.get("is_read",  Json::Value(false)).asBool();
    std::string extension = argument.get("extension",  Json::Value("*")).asString();

    result = measure::OSArgument::makePathArgument(name, isRead, extension, required, modelDependent);

    if (argument.isMember("default_value")){
      std::string defaultValue = argument.get("default_value", Json::Value("")).asString();
      result->setDefaultValue(defaultValue);
    }
        
  } else if (type.value() == measure::OSArgumentType::Separator){

    result = measure::OSArgument::makeSeparatorArgument(name, modelDependent);

  }
  OS_ASSERT(result);

  if (argument.isMember("display_name")){
    std::string displayName = argument.get("display_name", Json::Value("")).asString();
    result->setDisplayName(displayName);
  }

  if (argument.isMember("description")){
    std::string description = argument.get("description", Json::Value("")).asString();
    result->setDescription(description);
  }

  if (argument.isMember("units")){
    std::string units = argument.get("units", Json::Value("")).asString();
    result->setUnits(units);
  }

  return result;
}

//std::string MeasureManager::suggestMeasureGroupName(const BCLMeasure &t_measure)
//{
//  std::string baseName = t_measure.displayName();
//
//  baseName.append(" Group");
//
//  std::set<std::string> allNames;
//  if( boost::optional<analysisdriver::SimpleProject> project = m_app->project() ){
//    analysis::Analysis analysis = project->analysis();
//    analysis::Problem problem = analysis.problem();
//    for (const analysis::InputVariable& variable : problem.variables()) {
//      allNames.insert(variable.name());
//      allNames.insert(variable.displayName());
//    }
//  }
//
//  std::string result = baseName;
//  int i = 1;
//  while (allNames.find(result) != allNames.end()){
//    result = baseName + " " + QString::number(i).toStdString();
//    i++;
//  }
//
//  return result;
//}

//std::string MeasureManager::suggestMeasureName(const BCLMeasure &t_measure, bool t_fixed)
//{
//  std::string baseName = t_measure.displayName();
//
//  if (!t_fixed)
//  {
//    baseName.append(" Alternative");
//  }
//
//  std::set<std::string> allNames;
//  if( boost::optional<analysisdriver::SimpleProject> project = m_app->project() ){
//    analysis::Analysis analysis = project->analysis();
//    analysis::Problem problem = analysis.problem();
//    for (const analysis::InputVariable& variable : problem.variables()) {
//      boost::optional<analysis::MeasureGroup> discreteVariable = variable.optionalCast<analysis::MeasureGroup>();
//      if (discreteVariable){
//        for (const analysis::Measure& measure : discreteVariable->measures(false)) {
//          allNames.insert(measure.name());
//          allNames.insert(measure.displayName());
//        }
//      }
//    }
//  }
//
//  std::string result = baseName;
//  int i = 1;
//  while (allNames.find(result) != allNames.end()){
//    result = baseName + " " + QString::number(i).toStdString();
//    i++;
//  }
//
//  return result;
//}

void MeasureManager::updateMeasures(bool t_showMessage)
{
  std::vector<BCLMeasure> measures;

  WorkflowJSON workflowJSON = m_app->currentModel()->workflowJSON();
  for (const auto& step : workflowJSON.workflowSteps()){
    if (step.optionalCast<MeasureStep>()){
      boost::optional<BCLMeasure> measure = workflowJSON.getBCLMeasure(step.cast<MeasureStep>());
      if (measure){
        measures.push_back(*measure);
      }
    }
  }

  auto progress = new ProcessEventsProgressBar();
  progress->setMaximum(std::numeric_limits<double>::max());

  size_t loc = 0;
  std::vector<std::string> failMessages;
  for (const auto & measure : measures)
  {
    progress->setValue(loc);
    std::pair<bool,std::string> updateResult = updateMeasure(measure);
    if (!updateResult.first) {
      failMessages.push_back(updateResult.second);
    }
    ++loc;
  }
  progress->setValue(measures.size());

  delete progress;

  if (t_showMessage)
  {
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

      auto messageBox = new QMessageBox(m_app->mainWidget());
      messageBox->setWindowTitle(QString("Measures Updated"));
      messageBox->setText(toQString(ss.str()));
      messageBox->setDetailedText(errors);
      //messageBox->setMinimumWidth(330);
      //messageBox->setSizeGripEnabled(true);

      // DLM: there is a bug in QMessageBox where setMinimumWidth is not used
      // http://www.qtcentre.org/threads/22298-QMessageBox-Controlling-the-width?p=113348#post113348
      auto horizontalSpacer = new QSpacerItem(330, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
      QGridLayout* layout = static_cast<QGridLayout*>(messageBox->layout());
      layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
      
      messageBox->exec();
      delete messageBox;
    }
    else {
      QMessageBox::information(m_app->mainWidget(), QString("Measures Updated"), toQString(ss.str()));
    }
  }
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

bool MeasureManager::checkForUpdates(const openstudio::path& measureDir, bool force)
{
  QUrl url(m_url);
  url.setPath("/update_measures");

  std::string url_s = m_url.toString().toStdString();

  QString data = QString("{\"measures_dir\": \"") + toQString(measureDir) + QString("\"}");

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "json");

  QNetworkAccessManager manager;

  QNetworkReply* reply = manager.post(request, data.toUtf8());

  while (reply->isRunning()){
    Application::instance().processEvents();
  }

  QString replyString = reply->readAll();
  std::string s = replyString.toStdString();

  bool result = true;
  if (reply->error() != QNetworkReply::NoError){
    result = false;
  }

  delete reply;

  return result;
}

void MeasureManager::downloadBCLMeasures()
{
  auto remoteBCL = new RemoteBCL();
  int numUpdates = remoteBCL->checkForMeasureUpdates();
  if (numUpdates == 0){
    QMessageBox::information(m_app->mainWidget(), "Measures Updated", "All measures are up-to-date.");
  }else{
    std::vector<BCLSearchResult> updates = remoteBCL->measuresWithUpdates();
    QStringList measureNames;
    for (const BCLSearchResult& update : updates) {
      measureNames.push_back(QString(0x2022) + " " + toQString(update.name()));
    }
    remoteBCL->updateMeasures();
    QMessageBox msg(m_app->mainWidget());
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle("Measures Updated");
    if (numUpdates == 1){
      msg.setText("1 measure has been updated.                   ");
    }else{
      msg.setText(QString::number(numUpdates) + " measures have been updated.                   ");
    }
    msg.setDetailedText(measureNames.join("\n"));
    msg.exec();
  }

  delete remoteBCL;
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

bool MeasureManager::isManagedMeasure(const UUID & id) const
{
  if (m_managedMeasures.find(id) != m_managedMeasures.end()){
    return true;
  }
  return false;
}

std::vector<BCLMeasure> MeasureManager::openstudioMeasures() const
{
  std::vector<BCLMeasure> result;

  for (const auto & measure : m_openstudioMeasures)
  {
    result.push_back(measure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::bclMeasures() const
{
  std::vector<BCLMeasure> result;

  for(const auto & bclMeasure : m_bclMeasures)
  {
    result.push_back(bclMeasure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::myMeasures() const
{
  std::vector<BCLMeasure> result;

  for(const auto & measure : m_myMeasures )
  {
    result.push_back(measure.second);
  }

  return result;
}

void MeasureManager::updateMeasuresLists()
{
  openstudio::path userMeasuresDir = BCLMeasure::userMeasuresDir();

  auto updateUserMeasures = true;
  if (isNetworkPath(userMeasuresDir) && !isNetworkPathAvailable(userMeasuresDir)) {
    updateUserMeasures = false;
  }

  updateMeasuresLists(updateUserMeasures);
}

void MeasureManager::updateMeasuresLists(bool updateUserMeasures)
{
  m_managedMeasures.clear();
  m_openstudioMeasures.clear();
  m_myMeasures.clear();
  m_bclMeasures.clear();

  // call the measure manager in the CLI to update all measure dirs
  checkForUpdates(BCLMeasure::patApplicationMeasuresDir(), false);

  // DLM: todo, update LocalBCL
  //LocalBCL::libraryPath();
  //checkForUpdates(BCLMeasure::patApplicationMeasures(), false);

  if (updateUserMeasures) {
    checkForUpdates(BCLMeasure::userMeasuresDir(), false);
  }

  // measures managed by the applications
  // DLM: currently these do not exist
  /*  
  BCLMeasure alternativeModelMeasure = BCLMeasure::alternativeModelMeasure();
  m_managedMeasures.insert(std::pair<UUID, BCLMeasure>(alternativeModelMeasure.uuid(), alternativeModelMeasure));

  BCLMeasure reportRequestMeasure = BCLMeasure::reportRequestMeasure();
  m_managedMeasures.insert(std::pair<UUID, BCLMeasure>(reportRequestMeasure.uuid(), reportRequestMeasure));

  BCLMeasure standardReportMeasure = BCLMeasure::standardReportMeasure();
  m_managedMeasures.insert(std::pair<UUID, BCLMeasure>(standardReportMeasure.uuid(), standardReportMeasure));

  BCLMeasure calibrationReportMeasure = BCLMeasure::calibrationReportMeasure();
  m_managedMeasures.insert(std::pair<UUID, BCLMeasure>(calibrationReportMeasure.uuid(), calibrationReportMeasure));
  */
  
  std::vector<BCLMeasure> openstudioMeasures = BCLMeasure::patApplicationMeasures();
  for (auto & measure : openstudioMeasures)
  {
    auto it = m_openstudioMeasures.find(measure.uuid());
    if (it != m_openstudioMeasures.end()){
      // duplicate measure detected, user copy and paste
      LOG(Error, "UUID of built in measure at '" << measure.directory() << "' conflicts with other built in measure, measure at '" << it->second.directory() << "' will be used instead");
    }else{
      m_openstudioMeasures.insert(std::pair<UUID, BCLMeasure>(measure.uuid(), measure));
    }
  }

  if (updateUserMeasures) {
   std::vector<BCLMeasure> userMeasures = BCLMeasure::userMeasures();
    for( auto & measure : userMeasures )
    {
      bool updateUUID = false;
      if (m_myMeasures.find(measure.uuid()) != m_myMeasures.end()){
        updateUUID = true;
      }
      if (m_bclMeasures.find(measure.uuid()) != m_bclMeasures.end()){
        updateUUID = true;
      }
      if (m_openstudioMeasures.find(measure.uuid()) != m_openstudioMeasures.end()){
        updateUUID = true;
      }

      if (updateUUID){
        // duplicate measure detected, manual copy and paste likely cause
        // assign measure a new UUID here and save
        measure.changeUID();
        measure.incrementVersionId();
        measure.save();
      }

      m_myMeasures.insert(std::pair<UUID,BCLMeasure>(measure.uuid(),measure));
    }
  }
 
  std::vector<BCLMeasure> localBCLMeasures = BCLMeasure::localBCLMeasures();
  for( auto & measure : localBCLMeasures )
  {
    auto it = m_bclMeasures.find(measure.uuid());
    if (it != m_bclMeasures.end()){
      // duplicate measure detected
      LOG(Error, "UUID of bcl measure at '" << measure.directory() << "' conflicts with other bcl measure, measure at '" << it->second.directory() << "' will be used instead");
    }else{
      m_bclMeasures.insert(std::pair<UUID,BCLMeasure>(measure.uuid(),measure));
    }
  }

  if (m_libraryController)
  {
    m_libraryController->reset();
  }
}

void MeasureManager::addMeasure()
{
  // open modal dialog
  //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(this->mainWindow));
  QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog());

  if (dialog->exec()){

    // not canceled, create measure
    boost::optional<BCLMeasure> measure = dialog->createMeasure();
    if (measure){

      // open directory for editing
      QString path = QDir::toNativeSeparators(toQString(measure->directory()));
      QDesktopServices::openUrl(QUrl("file:///" + path));

      updateMeasuresLists();

      // emit signal
      emit newMeasure(*measure);
    }else{
      // error, this should not happen
    }

  }else{
    // user canceled, do nothing
  }
}

void MeasureManager::duplicateSelectedMeasure()
{
  OS_ASSERT(m_libraryController);
  QPointer<LibraryItem> item = m_libraryController->selectedItem();

  if( !item.isNull() )
  {
    boost::optional<BCLMeasure> bclMeasure = this->getMeasure(item->uuid());
    if (bclMeasure){

      // check for updates in case measure being copied has changed
      // do not save bclMeasure 
      // DLM: todo
      //checkForUpdates(*bclMeasure, true);

      // open modal dialog
      //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure, this->mainWindow));
      QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure));

      if (dialog->exec()){

        // not canceled, create measure
        boost::optional<BCLMeasure> measure = dialog->createMeasure();
        if (measure){

          // open directory for editing
          QString path = QDir::toNativeSeparators(toQString(measure->directory()));
          QDesktopServices::openUrl(QUrl("file:///" + path));

          updateMeasuresLists();

          // emit signal
          emit newMeasure(*measure);
        }else{
          // error, this should not happen
        }

      }else{
        // user canceled, do nothing
      }
    }
  }
}

bool MeasureManager::isMeasureSelected()
{
  if (m_libraryController){
    QPointer<LibraryItem> item = m_libraryController->selectedItem();
    return !item.isNull();
  }
  return false;
}

//QSharedPointer<measure::OSMeasureInfoGetter> MeasureManager::infoGetter() const
//{
//  return m_infoGetter;
//}

std::vector<BCLMeasure> MeasureManager::combinedMeasures(bool includeOpenStudioMeasures) const
{
  std::vector<BCLMeasure> result;
  std::set<UUID> resultUUIDs;
  
  if (includeOpenStudioMeasures){
    // insert openstudio measures
    for (auto it = m_openstudioMeasures.begin(), itend = m_openstudioMeasures.end(); it != itend; ++it){
      if (resultUUIDs.find(it->first) == resultUUIDs.end()){
        resultUUIDs.insert(it->first);
        result.push_back(it->second);
      }else{
        LOG(Error, "UUID of built in measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
      }
    }
  }

  // insert my measures
  for (auto it = m_myMeasures.begin(), itend = m_myMeasures.end(); it != itend; ++it){
    if (resultUUIDs.find(it->first) == resultUUIDs.end()){
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    }else{
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

   // insert bcl measures
  for (auto it = m_bclMeasures.begin(), itend = m_bclMeasures.end(); it != itend; ++it){
    if (resultUUIDs.find(it->first) == resultUUIDs.end()){
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    }else{
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

  return result;
}

boost::optional<BCLMeasure> MeasureManager::getMeasure(const UUID & id)
{
  boost::optional<BCLMeasure> result;

  std::map<UUID,BCLMeasure>::iterator it;
  
  // search openstudio measures
  it = m_openstudioMeasures.find(id);
  if (it != m_openstudioMeasures.end()) {
    if (result){
      LOG(Error, "UUID of built in measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  // search my measures
  it = m_myMeasures.find(id);
  if( it != m_myMeasures.end() ) { 
    if (result){
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  // search bcl measures
  it = m_bclMeasures.find(id);
  if( it != m_bclMeasures.end() ) {     
    if (result){
      LOG(Error, "UUID of bcl measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  return result;
}

void MeasureManager::setLibraryController(const QSharedPointer<LocalLibraryController> &t_controller)
{
  m_libraryController = t_controller;
}

} // openstudio


