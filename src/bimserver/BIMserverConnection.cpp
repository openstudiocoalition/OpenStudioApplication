/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BIMserverConnection.hpp"

#include "../model_editor/Application.hpp"
#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/System.hpp>
#include <openstudio/utilities/core/Path.hpp>
#include <openstudio/utilities/core/FilesystemHelpers.hpp>
#include <openstudio/utilities/core/StringHelpers.hpp>

#include <QString>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <iostream>
#include <boost/none.hpp>

namespace openstudio {
namespace bimserver {

BIMserverConnection::BIMserverConnection(QObject* parent, const QString& bimserverAddr, const QString& bimserverPort)
  : QObject(parent),
    m_networkManager(new QNetworkAccessManager),
    m_bimserverURL(QUrl(QString("http://" + bimserverAddr + ":" + bimserverPort + "/json"))),
    m_operationDone(true),
    m_loginSuccess(true),
    m_createProjectSuccess(true),
    m_deleteProjectSuccess(true),
    m_checkInIFCSuccess(true) {}

BIMserverConnection::~BIMserverConnection() {
  delete m_networkManager;
}

void BIMserverConnection::login(QString username, QString password) {

  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;
  if (!username.isEmpty() && !password.isEmpty()) {
    m_username = std::move(username);
    m_password = std::move(password);
    sendLoginRequest();
  } else {
    emit errorOccured(QString("Username or password empty!"));
  }
}

void BIMserverConnection::sendLoginRequest() {
  //construct login json
  QJsonObject parameters;
  parameters["username"] = QJsonValue(m_username);
  parameters["password"] = QJsonValue(m_password);
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1AuthInterface"));
  request["method"] = QJsonValue(QString("login"));
  request["parameters"] = parameters;
  QJsonObject jsonRequest;
  jsonRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(jsonRequest);

  QByteArray jsonByteArray = doc.toJson();

  //setup network connection

  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processLoginRequest);
  m_networkManager->post(qNetworkRequest, jsonByteArray);
}

void BIMserverConnection::processLoginRequest(QNetworkReply* rep) {
  if (rep) {  //extract token from login Request.
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject responseObj = responseDoc.object();
    QJsonObject response = responseObj["response"].toObject();

    //if error occurs, emit signals. Otherwise, set m_token
    if (!containsError(response)) {

      m_token = response["result"].toString();
      m_loginSuccess = true;
      m_operationDone = true;
      emit operationSucceeded(QString("login"));

    } else {
      // if login contains any kinds of error, such as username/password incorrect, it is a bimserver error
      emit bimserverError();
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::getAllProjects() {
  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;
  sendGetAllProjectsRequest();
}

void BIMserverConnection::sendGetAllProjectsRequest() {
  QJsonObject parameters;
  parameters["onlyTopLevel"] = QJsonValue(true);
  parameters["onlyActive"] = QJsonValue(true);
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("getAllProjects"));
  request["parameters"] = parameters;
  QJsonObject jsonRequest;

  jsonRequest["token"] = QJsonValue(m_token);
  jsonRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(jsonRequest);

  QByteArray downloadJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processGetAllProjectsRequest);
  m_networkManager->post(qNetworkRequest, downloadJson);
}

void BIMserverConnection::processGetAllProjectsRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject responseObj = responseDoc.object();

    QJsonObject response = responseObj["response"].toObject();

    if (!containsError(response)) {
      QJsonArray result = response["result"].toArray();
      QStringList projectList;
      for (const auto& value : result) {
        QJsonObject ifcProject = value.toObject();
        int oid = ifcProject["oid"].toInt();
        QString projectID = toQString(openstudio::string_conversions::number(oid));
        QString projectName = ifcProject["name"].toString();
        QString project = projectID.append(":").append(projectName);
        projectList.append(project);
      }
      m_projectList = projectList;
      m_operationDone = true;
      emit listAllProjects(projectList);
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::download(QString revisionID) {

  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;
  if (!revisionID.isEmpty()) {
    m_roid = std::move(revisionID);
    sendGetSerializerRequest();
  } else {
    emit errorOccured(QString("Revision ID is empty"));
  }
}

void BIMserverConnection::sendGetSerializerRequest() {
  QJsonObject parameters;
  parameters["serializerName"] = QJsonValue(QString("Osm"));
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("getSerializerByName"));
  request["parameters"] = parameters;
  QJsonObject jsonRequest;
  jsonRequest["token"] = QJsonValue(m_token);
  jsonRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(jsonRequest);

  QByteArray jsonByteArray = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processGetSerializerRequest);
  m_networkManager->post(qNetworkRequest, jsonByteArray);
}

void BIMserverConnection::processGetSerializerRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject downloadResponse = responseDoc.object();
    QJsonObject response = downloadResponse["response"].toObject();

    if (!containsError(response)) {
      QJsonObject result = response["result"].toObject();
      m_serializerOid = toQString(openstudio::string_conversions::number(result["oid"].toInt()));
      sendDownloadRequest();
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::sendDownloadRequest() {
  QJsonObject parameters;
  parameters["roid"] = QJsonValue(m_roid);
  parameters["serializerOid"] = QJsonValue(m_serializerOid);
  parameters["showOwn"] = QJsonValue(false);
  parameters["sync"] = QJsonValue(false);
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("download"));
  request["parameters"] = parameters;
  QJsonObject jsonRequest;
  jsonRequest["token"] = QJsonValue(m_token);
  jsonRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(jsonRequest);

  QByteArray jsonByteArray = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processDownloadRequest);
  m_networkManager->post(qNetworkRequest, jsonByteArray);
}

void BIMserverConnection::processDownloadRequest(QNetworkReply* rep) {
  //extract token from login Request
  if (rep) {
    QByteArray responseArray = rep->readAll();
    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject downloadResponse = responseDoc.object();
    QJsonObject response = downloadResponse["response"].toObject();

    if (!containsError(response)) {
      m_actionId = toQString(openstudio::string_conversions::number(response["result"].toInt()));
      sendGetProgressRequest(m_actionId, QString("download"));
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::sendGetDownloadDataRequest() {
  QJsonObject parameters;
  parameters["actionId"] = QJsonValue(m_actionId);
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("getDownloadData"));
  request["parameters"] = parameters;
  QJsonObject getDownloadDataRequest;
  getDownloadDataRequest["token"] = QJsonValue(m_token);
  getDownloadDataRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(getDownloadDataRequest);

  QByteArray getDownloadDataJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processGetDownloadDataRequest);
  m_networkManager->post(qNetworkRequest, getDownloadDataJson);
}

void BIMserverConnection::processGetDownloadDataRequest(QNetworkReply* rep) {
  //extract token from login Request

  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject downloadResponse = responseDoc.object();
    QJsonObject response = downloadResponse["response"].toObject();

    if (!containsError(response)) {
      QJsonObject result = response["result"].toObject();
      QString file = result["file"].toString();
      //decode the response
      QByteArray byteArray;
      byteArray.append(file.toStdString().c_str());
      QString OSMFile = QByteArray::fromBase64(byteArray);

      m_osmModel = OSMFile;
      m_operationDone = true;
      emit osmStringRetrieved(OSMFile);
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

//
void BIMserverConnection::createProject(const QString& projectName) {

  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;

  if (!projectName.isEmpty()) {
    sendCreateProjectRequest(projectName);
  } else {
    emit errorOccured(QString("Project Name is empty!"));
  }
}

void BIMserverConnection::sendCreateProjectRequest(const QString& projectName) {

  QJsonObject parameters;
  parameters["projectName"] = projectName;
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("addProject"));
  request["parameters"] = parameters;
  QJsonObject createProjectRequest;
  createProjectRequest["token"] = QJsonValue(m_token);
  createProjectRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(createProjectRequest);

  QByteArray createProjectRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processCreateProjectRequest);
  m_networkManager->post(qNetworkRequest, createProjectRequestJson);
}

void BIMserverConnection::processCreateProjectRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject createProjectResponse = responseDoc.object();
    QJsonObject response = createProjectResponse["response"].toObject();

    if (!containsError(response)) {
      m_createProjectSuccess = true;
      m_operationDone = true;
      emit operationSucceeded(QString("createProject"));
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::deleteProject(const QString& projectID) {

  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;

  if (!projectID.isEmpty()) {
    sendDeleteProjectRequest(projectID);
  } else {
    emit errorOccured(QString("Project ID is empty!"));
  }
}

void BIMserverConnection::sendDeleteProjectRequest(const QString& projectID) {

  QJsonObject parameters;
  parameters["poid"] = projectID;
  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("deleteProject"));
  request["parameters"] = parameters;
  QJsonObject deleteProjectRequest;
  deleteProjectRequest["token"] = QJsonValue(m_token);
  deleteProjectRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(deleteProjectRequest);

  QByteArray deleteProjectRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processDeleteProjectRequest);
  m_networkManager->post(qNetworkRequest, deleteProjectRequestJson);
}

void BIMserverConnection::processDeleteProjectRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject deleteProjectResponse = responseDoc.object();
    QJsonObject response = deleteProjectResponse["response"].toObject();

    if (!containsError(response)) {
      m_deleteProjectSuccess = true;
      m_operationDone = true;
      emit operationSucceeded(QString("deleteProject"));
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::checkInIFCFile(QString projectID, QString IFCFilePath) {

  if (!m_operationDone) {
    emit errorOccured(QString("The last process has not end yet. Please wait and try again."));
    return;
  }
  m_operationDone = false;

  if (!projectID.isEmpty() && !IFCFilePath.isEmpty()) {
    m_poid = std::move(projectID);
    m_filePath = std::move(IFCFilePath);
    sendGetDeserializerRequest();
  } else {
    emit errorOccured(QString("Please provide valid projectID and IFC file path!"));
  }
}

void BIMserverConnection::sendGetDeserializerRequest() {
  QJsonObject parameters;
  parameters["extension"] = QJsonValue(QString("ifc"));

  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("getSuggestedDeserializerForExtension"));
  request["parameters"] = parameters;

  QJsonObject getDeserializerRequest;
  getDeserializerRequest["token"] = QJsonValue(m_token);
  getDeserializerRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(getDeserializerRequest);
  QByteArray getDeserializerRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processGetDeserializerRequest);
  m_networkManager->post(qNetworkRequest, getDeserializerRequestJson);
}

void BIMserverConnection::processGetDeserializerRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject getDeserializerResponse = responseDoc.object();
    QJsonObject response = getDeserializerResponse["response"].toObject();

    if (!containsError(response)) {
      QJsonObject result = response["result"].toObject();
      m_deserializerOid = toQString(openstudio::string_conversions::number(result["oid"].toInt()));
      sendCheckInIFCRequest(m_filePath);
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::sendCheckInIFCRequest(const QString& IFCFilePath) {
  const auto path = openstudio::toPath(IFCFilePath.toStdString());
  openstudio::filesystem::ifstream file(path);
  if (!file.is_open()) {
    emit errorOccured(QString("Cannot open file, please verify and try again"));
    return;
  }

  QJsonObject parameters;
  parameters["poid"] = QJsonValue(m_poid);
  parameters["comment"] = QJsonValue(QString(""));
  parameters["deserializerOid"] = QJsonValue(m_deserializerOid);
  // filesystem::file_size returns a uintmax_t really.
  parameters["fileSize"] = QJsonValue(toQString(openstudio::string_conversions::number(std::uint64_t(openstudio::filesystem::file_size(path)))));
  parameters["fileName"] = QJsonValue(toQString(toString(path.stem())));
  //encode file into Base64
  std::vector<char> data = openstudio::filesystem::read(file);
  QByteArray fileArrayEncoded = QByteArray(data.data(), data.size()).toBase64();
  QString fileEncoded(fileArrayEncoded);
  parameters["data"] = QJsonValue(fileEncoded);
  parameters["merge"] = QJsonValue(false);
  parameters["sync"] = QJsonValue(false);

  QJsonObject request;
  request["interface"] = QJsonValue(QString("ServiceInterface"));
  request["method"] = QJsonValue(QString("checkin"));
  request["parameters"] = parameters;

  QJsonObject checkInIFCRequest;
  checkInIFCRequest["token"] = QJsonValue(m_token);
  checkInIFCRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(checkInIFCRequest);
  QByteArray checkInIFCRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processCheckInIFCRequest);
  m_networkManager->post(qNetworkRequest, checkInIFCRequestJson);
}

void BIMserverConnection::processCheckInIFCRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject checkInIFCResponse = responseDoc.object();
    QJsonObject response = checkInIFCResponse["response"].toObject();

    if (!containsError(response)) {
      QString topicId = toQString(openstudio::string_conversions::number(response["result"].toInt()));
      sendGetProgressRequest(topicId, "checkInIFC");
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::getIFCRevisionList(const QString& projectID) {
  if (!projectID.isEmpty()) {
    sendGetProjectByIDRequest(projectID);
  } else {
    emit errorOccured(QString("Project ID empty!"));
  }
}

void BIMserverConnection::sendGetProjectByIDRequest(const QString& projectID) {
  QJsonObject parameters;
  parameters["poid"] = QJsonValue(projectID);

  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1ServiceInterface"));
  request["method"] = QJsonValue(QString("getAllRevisionsOfProject"));
  request["parameters"] = parameters;

  QJsonObject getIFCRevisionListRequest;
  getIFCRevisionListRequest["token"] = QJsonValue(m_token);
  getIFCRevisionListRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(getIFCRevisionListRequest);

  QByteArray getIFCRevisionListRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  // disconnect all signals from m_networkManager to this
  disconnect(m_networkManager, nullptr, this, nullptr);
  connect(m_networkManager, &QNetworkAccessManager::finished, this, &BIMserverConnection::processGetProjectByIDRequest);
  m_networkManager->post(qNetworkRequest, getIFCRevisionListRequestJson);
}

void BIMserverConnection::processGetProjectByIDRequest(QNetworkReply* rep) {
  if (rep) {
    QByteArray responseArray = rep->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject responseObj = responseDoc.object();

    QJsonObject response = responseObj["response"].toObject();

    if (!containsError(response)) {
      QJsonArray result = response["result"].toArray();
      QStringList revisionList;
      for (const auto& value : result) {
        QJsonObject ifcRevision = value.toObject();
        QString revision = toQString(openstudio::string_conversions::number(ifcRevision["oid"].toInt()));
        double time = ifcRevision["date"].toDouble();
        QDateTime timestamp;
        timestamp.setSecsSinceEpoch(static_cast<qint64>(time / 1000 + 0.5));
        // TODO: Why is this unused?
        // QString project = revision.append(":").append(timestamp.toString(QLocale::system().dateTimeFormat(QLocale::ShortFormat)));

        revisionList.append(revision);
      }
      m_ifcList = revisionList;
      m_operationDone = true;
      emit listAllIFCRevisions(revisionList);
    } else {
      emitErrorMessage(response);
    }

  } else {
    emit bimserverError();
  }
}

void BIMserverConnection::sendGetProgressRequest(const QString& topicId, const QString& action) {
  QJsonObject parameters;
  parameters["topicId"] = QJsonValue(topicId);

  QJsonObject request;
  request["interface"] = QJsonValue(QString("Bimsie1NotificationRegistryInterface"));
  request["method"] = QJsonValue(QString("getProgress"));
  request["parameters"] = parameters;

  QJsonObject getProgressRequest;
  getProgressRequest["token"] = QJsonValue(m_token);
  getProgressRequest["request"] = request;

  QJsonDocument doc;
  doc.setObject(getProgressRequest);

  QByteArray getProgressRequestJson = doc.toJson();

  //setup network connection
  QNetworkRequest qNetworkRequest(m_bimserverURL);
  qNetworkRequest.setRawHeader("Content-Type", "application/json");

  QNetworkReply* reply = m_networkManager->post(qNetworkRequest, getProgressRequestJson);
  reply->setProperty("sender", QVariant(action));
  disconnect(m_networkManager, nullptr, this, nullptr);
  disconnect(reply, nullptr, this, nullptr);
  connect(reply, &QNetworkReply::finished, this, &BIMserverConnection::processGetProgressRequest);
}

void BIMserverConnection::processGetProgressRequest() {

  auto* reply = qobject_cast<QNetworkReply*>(sender());

  if (reply) {
    QByteArray responseArray = reply->readAll();

    QJsonDocument responseDoc = QJsonDocument::fromJson(responseArray);
    QJsonObject downloadResponse = responseDoc.object();
    QJsonObject response = downloadResponse["response"].toObject();

    if (response["errors"].toArray().isEmpty()) {
      QString sender = reply->property("sender").toString();

      if (sender == "download") {
        sendGetDownloadDataRequest();
      } else if (sender == "checkInIFC") {
        m_checkInIFCSuccess = true;
        m_operationDone = true;
        emit operationSucceeded(QString("checkInIFC"));
      }
    } else {
      QJsonArray error = response["errors"].toArray();
      QString errorMessage;
      for (const auto& value : error) {
        errorMessage = errorMessage + value.toString() + QString("\n");
      }
      emit errorOccured(errorMessage);
    }

    reply->deleteLater();

  } else {
    emit errorOccured(QString("No Response from BIMserver"));
  }
}

//BIMserver error reporting
bool BIMserverConnection::containsError(const QJsonObject& responseMessage) {

  if (responseMessage.isEmpty()) {
    return true;
  } else {
    return responseMessage.contains("exception");
  }
}

void BIMserverConnection::emitErrorMessage(const QJsonObject& responseMessage) {

  if (responseMessage.isEmpty()) {
    emit bimserverError();
  } else {
    QJsonObject except = responseMessage["exception"].toObject();
    QString errorMessage = except["message"].toString();
    emit errorOccured(errorMessage);
  }
}

bool BIMserverConnection::loginBlocked(const QString& username, const QString& password, int timeout) {
  m_loginSuccess = false;
  login(username, password);
  waitForLock(timeout);
  return m_loginSuccess;
}

boost::optional<QString> BIMserverConnection::downloadBlocked(const QString& projectID, int timeout) {
  m_osmModel = boost::none;
  download(projectID);
  waitForLock(timeout);
  return m_osmModel;
}

boost::optional<QStringList> BIMserverConnection::getAllProjectsBlocked(int timeout) {
  m_projectList = boost::none;
  getAllProjects();
  waitForLock(timeout);
  return m_projectList;
}

bool BIMserverConnection::createProjectBlocked(const QString& projectName, int timeout) {
  m_createProjectSuccess = false;
  createProject(projectName);
  waitForLock(timeout);
  return m_createProjectSuccess;
}

bool BIMserverConnection::deleteProjectBlocked(const QString& projectID, int timeout) {
  m_deleteProjectSuccess = false;
  deleteProject(projectID);
  waitForLock(timeout);
  return m_deleteProjectSuccess;
}

bool BIMserverConnection::checkInIFCFileBlocked(const QString& projectID, const QString& IFCFilePath, int timeout) {
  m_checkInIFCSuccess = false;
  checkInIFCFile(projectID, IFCFilePath);
  waitForLock(timeout);
  return m_checkInIFCSuccess;
}

boost::optional<QStringList> BIMserverConnection::getIFCRevisionListBlocked(const QString& projectID, int timeout) {
  m_ifcList = boost::none;
  getIFCRevisionList(projectID);
  waitForLock(timeout);
  return m_ifcList;
}

bool BIMserverConnection::waitForLock(int msec) const {
  int msecPerLoop = 20;
  int numTries = msec / msecPerLoop;
  int current = 0;
  while (true) {

    // if we can get the lock then the download is complete
    if (m_operationDone) {
      return true;
    }

    Application::instance().processEvents(msecPerLoop);

    if (current > numTries) {
      LOG(Error, "waitForLock timeout");
      break;
    }

    ++current;
  }

  return false;
}

}  // namespace bimserver
}  // namespace openstudio
