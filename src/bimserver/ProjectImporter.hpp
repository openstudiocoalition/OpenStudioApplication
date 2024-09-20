/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef BIMSERVER_PROJECTIMPORTER_HPP
#define BIMSERVER_PROJECTIMPORTER_HPP

#include "BIMserverAPI.hpp"
#include "BIMserverConnection.hpp"

#include <openstudio/model/Model.hpp>

#include <QDialog>
#include <QListWidget>
#include <QStatusBar>
#include <QSettings>
#include <QEventLoop>
#include <QCloseEvent>
#include <QKeyEvent>

namespace openstudio {
namespace bimserver {

/// This shows a input dialog to gather project id for import
class BIMSERVER_API ProjectImporter : public QDialog
{
  Q_OBJECT

 public:
  /// Default constructor
  explicit ProjectImporter(QWidget* parent);

  /// Start importing IFC workflow
  boost::optional<model::Model> run();

  /// Virtual destructor
  ~ProjectImporter();

  /// Reimplemented the close event and guide it to the run() function
  void closeEvent(QCloseEvent* event) override;

  /// Reimplemented the key press event of ESC and guide it to the run() function
  void keyPressEvent(QKeyEvent* event) override;

 signals:
  /// OSM String is retrieved.
  void finished();

 public slots:

  /// Takes projectList from BIMserverConnection and prints out projects
  void processProjectList(const QStringList& projectList);
  /// Takes ifc list from BIMserverConnection and prints out ifc list
  void processIFCList(const QStringList& ifcList);
  /// process success cases for createProject, checkInIFC, and login
  void processSucessCases(const QString& successCase);
  /// process all failure cases if BIMserver outputs an exception. Print it
  void processFailureCases(const QString& failureCase);
  /// OSM string is retrieved
  void processOSMRetrieved(const QString& osmString);
  /// process if BIMserver is not connected.
  void processBIMserverErrors();

 private:
  BIMserverConnection* m_bimserverConnection;
  QSettings* m_settings;

  QString m_proID;
  QString m_ifcID;
  QListWidget* m_proList;
  QListWidget* m_ifcList;
  QStatusBar* m_statusBar;
  QEventLoop* m_waitForOSM;
  QString m_OSM;

  QPushButton* m_okButton;
  QPushButton* m_loadButton;
  QPushButton* m_selectButton;

 private slots:

  void okButton_clicked();
  void newButton_clicked();
  void loadButton_clicked();
  void selectButton_clicked();
  void settingButton_clicked();
  void app_ended();
};

}  // namespace bimserver
}  // namespace openstudio

#endif  // BIMSERVER_PROJECTIMPORTER_HPP
