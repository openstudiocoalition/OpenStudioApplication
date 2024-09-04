/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef BIMSERVER_BIMSERVERCONNECTION_HPP
#define BIMSERVER_BIMSERVERCONNECTION_HPP

#include "BIMserverAPI.hpp"

#include <openstudio/utilities/core/Logger.hpp>

#include <QString>
#include <QUrl>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <boost/optional.hpp>

namespace openstudio {

class ProgressBar;

namespace model {
class Model;
class ModelObject;
class Surface;
}  // namespace model

namespace bimserver {

/// This provides utilities to connect to BIMserver
class BIMSERVER_API BIMserverConnection : public QObject
{
  Q_OBJECT

 public:
  /// Default constructor
  BIMserverConnection(QObject* parent, const QString& bimserverAddr, const QString& bimserverPort);
  /// destructor
  ~BIMserverConnection();

  //@}
  /** @name Unblocking class members */
  //@{

  /// login with username and password
  void login(QString username, QString password);
  /// download the osm model
  void download(QString revisionID);
  /// get all projects
  void getAllProjects();
  /// create new project
  void createProject(const QString& projectName);
  /// delete a project
  void deleteProject(const QString& projectID);
  /// check in new ifc file
  void checkInIFCFile(QString projectID, QString IFCFilePath);
  /// get all revisions of IFC files of a project
  void getIFCRevisionList(const QString& projectID);

  //@}
  /** @name Blocking class members */
  //@{

  /// Login, Blocked
  bool loginBlocked(const QString& username, const QString& password, int timeout);
  /// download the osm model, Blocked
  boost::optional<QString> downloadBlocked(const QString& projectID, int timeout);
  /// get all projects, Blocked
  boost::optional<QStringList> getAllProjectsBlocked(int timeout);
  /// create new project, Blocked
  bool createProjectBlocked(const QString& projectName, int timeout);
  /// delete a project, Blocked
  bool deleteProjectBlocked(const QString& projectID, int timeout);
  /// check in new ifc file, Blocked
  bool checkInIFCFileBlocked(const QString& projectID, const QString& IFCFilePath, int timeout);
  /// get all revisions of IFC files of a project, Blocked
  boost::optional<QStringList> getIFCRevisionListBlocked(const QString& projectID, int timeout);

 signals:
  /// send the retrieved osmString to GUI
  void osmStringRetrieved(QString osmString);

  ///send the list of all projects to GUI
  void listAllProjects(QStringList projectList);

  ///send the list of all ifc revisions
  void listAllIFCRevisions(QStringList ifcRevisionList);

  ///emit error occurs signal
  void errorOccured(QString errorMessage);

  ///operationSucceeded() will send one of the following QString:
  void operationSucceeded(QString successMessage);

  ///emit error if BIMserver is not setup correctly.
  void bimserverError();

 private slots:
  //slots used by this class only
  /// log in to BIMserver
  void processLoginRequest(QNetworkReply* rep);
  /// get all projects
  void processGetAllProjectsRequest(QNetworkReply* rep);
  /// get serializer id
  void processGetSerializerRequest(QNetworkReply* rep);
  /// get download action id
  void processDownloadRequest(QNetworkReply* rep);
  /// get download data
  void processGetDownloadDataRequest(QNetworkReply* rep);
  /// create new project
  void processCreateProjectRequest(QNetworkReply* rep);
  /// Delete a project
  void processDeleteProjectRequest(QNetworkReply* rep);
  /// get ifc deserializer
  void processGetDeserializerRequest(QNetworkReply* rep);
  /// checkIn new IFC
  void processCheckInIFCRequest(QNetworkReply* rep);
  /// get ifc revision list
  void processGetProjectByIDRequest(QNetworkReply* rep);
  /// get download progress
  void processGetProgressRequest();

 private:
  REGISTER_LOGGER("openstudio.BIMserverConnection");

  void sendLoginRequest();
  void sendGetAllProjectsRequest();
  void sendGetSerializerRequest();
  void sendDownloadRequest();
  void sendGetDownloadDataRequest();
  void sendCreateProjectRequest(const QString& projectName);
  void sendDeleteProjectRequest(const QString& projectID);
  void sendGetDeserializerRequest();
  void sendCheckInIFCRequest(const QString& IFCFilePath);
  void sendGetProjectByIDRequest(const QString& projectID);
  void sendGetProgressRequest(const QString& topicId, const QString& action);

  bool containsError(const QJsonObject& responseMessage);
  void emitErrorMessage(const QJsonObject& responseMessage);

  bool waitForLock(int msec) const;

  QNetworkAccessManager* m_networkManager;
  QUrl m_bimserverURL;
  QString m_username;
  QString m_password;
  QString m_token;
  QString m_serializerOid;
  QString m_deserializerOid;
  QString m_roid;
  QString m_actionId;
  QString m_poid;
  QString m_filePath;
  bool m_operationDone;

  /// Variables for the blocking calls
  bool m_loginSuccess;
  boost::optional<QString> m_osmModel;
  boost::optional<QStringList> m_projectList;
  bool m_createProjectSuccess;
  bool m_deleteProjectSuccess;
  bool m_checkInIFCSuccess;
  boost::optional<QStringList> m_ifcList;
};

}  // namespace bimserver
}  // namespace openstudio

#endif  // BIMSERVER_BIMSERVERCONNECTION_HPP
