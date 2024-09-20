/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYEDITORVIEW_HPP
#define OPENSTUDIO_GEOMETRYEDITORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"
#include "OSWebEnginePage.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/DefaultConstructionSet.hpp>

#include <openstudio/utilities/geometry/FloorplanJS.hpp>

#include "../shared_gui_components/ProgressBarWithError.hpp"

#include <QWidget>
#include <QDialog>
#include <QWebEngineView>

class QComboBox;
class QPushButton;
class QTimer;

namespace openstudio {

class GeometryEditorView : public QWidget
{
  Q_OBJECT

 public:
  GeometryEditorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~GeometryEditorView();

 private:
};

// debug widget
class DebugWebView : public QDialog
{
  Q_OBJECT;

 public:
  explicit DebugWebView(const QString& debugPort, QWidget* parent = nullptr);
  virtual ~DebugWebView();

 private:
  QWebEngineView* m_view;
};

// main widget

class BaseEditor : public QObject
{
  Q_OBJECT;

 public:
  BaseEditor(bool isIP, const openstudio::model::Model& model, QWebEngineView* view, QWidget* t_parent = nullptr);
  virtual ~BaseEditor();

  bool editorLoaded() const;
  bool javascriptRunning() const;
  bool blockUpdateTimerSignals(bool block);

  model::Model exportModel() const;
  std::map<UUID, UUID> exportModelHandleMapping() const;

 public slots:
  virtual void loadEditor() = 0;
  virtual void doExport() = 0;
  virtual void saveExport() = 0;
  virtual void translateExport() = 0;
  virtual void updateModel(const openstudio::model::Model& model) = 0;
  virtual void checkForUpdate() = 0;

  virtual void onChanged();

 signals:

  bool changed();

 protected:
  bool m_editorLoaded;
  bool m_javascriptRunning;
  unsigned m_versionNumber;

  bool m_isIP;
  openstudio::model::Model m_model;
  QWebEngineView* m_view;

  QVariant m_export;
  model::Model m_exportModel;
  std::map<UUID, UUID> m_exportModelHandleMapping;

  std::shared_ptr<OSDocument> m_document;
  QTimer* m_checkForUpdateTimer;
};

class FloorspaceEditor : public BaseEditor
{
  Q_OBJECT;

 public:
  FloorspaceEditor(const openstudio::path& floorplanPath, bool isIP, const openstudio::model::Model& model, QWebEngineView* view,
                   QWidget* t_parent = nullptr);
  virtual ~FloorspaceEditor();

 public slots:
  virtual void loadEditor() override;
  virtual void doExport() override;
  virtual void saveExport() override;
  virtual void translateExport() override;
  virtual void updateModel(const openstudio::model::Model& model) override;
  virtual void checkForUpdate() override;

 private:
  std::string m_originalBuildingName;
  std::string m_originalSiteName;
  boost::optional<openstudio::model::DefaultConstructionSet> m_originalDefaultConstructionSet;
  openstudio::path m_floorplanPath;
  boost::optional<FloorplanJS> m_floorplan;
};

class GbXmlEditor : public BaseEditor
{
  Q_OBJECT;

 public:
  GbXmlEditor(const openstudio::path& gbXmlPath, bool isIP, const openstudio::model::Model& model, QWebEngineView* view, QWidget* t_parent = nullptr);
  virtual ~GbXmlEditor();

 public slots:
  virtual void loadEditor() override;
  virtual void doExport() override;
  virtual void saveExport() override;
  virtual void translateExport() override;
  virtual void updateModel(const openstudio::model::Model& model) override;
  virtual void checkForUpdate() override;

 private:
  openstudio::path m_gbXmlPath;
  QString m_gbXML;
};

class IdfEditor : public BaseEditor
{
  Q_OBJECT;

 public:
  IdfEditor(const openstudio::path& idfPath, bool forceConvert, bool isIP, const openstudio::model::Model& model, QWebEngineView* view,
            QWidget* t_parent = nullptr);
  virtual ~IdfEditor();

 public slots:
  virtual void loadEditor() override;
  virtual void doExport() override;
  virtual void saveExport() override;
  virtual void translateExport() override;
  virtual void updateModel(const openstudio::model::Model& model) override;
  virtual void checkForUpdate() override;

 private:
  openstudio::path m_idfPath;
  QString m_jdf;
};

class OsmEditor : public BaseEditor
{
  Q_OBJECT;

 public:
  OsmEditor(const openstudio::path& osmPath, bool isIP, const openstudio::model::Model& model, QWebEngineView* view, QWidget* t_parent = nullptr);
  virtual ~OsmEditor();

 public slots:
  virtual void loadEditor() override;
  virtual void doExport() override;
  virtual void saveExport() override;
  virtual void translateExport() override;
  virtual void updateModel(const openstudio::model::Model& model) override;
  virtual void checkForUpdate() override;

 private:
  openstudio::path m_osmPath;
};

// EditorWebView is the main UI widget, it decides which BaseEditor to instantiate
class EditorWebView : public QWidget
{
  Q_OBJECT;

 public:
  EditorWebView(bool isIP, const openstudio::model::Model& model, QWidget* t_parent = nullptr);
  virtual ~EditorWebView();

 public slots:
  void onUnitSystemChange(bool t_isIP);

 private slots:
  void geometrySourceChanged(const QString& text);
  void newImportClicked();
  void refreshClicked();
  void saveClickedBlocking(const openstudio::path&);
  void previewClicked();
  void mergeClicked();
  void debugClicked();
  void previewExport();
  void mergeExport();
  void onChanged();

  void onLoadFinished(bool ok);
  void onLoadProgress(int progress);
  void onLoadStarted();
  void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);

 private:
  REGISTER_LOGGER("openstudio::EditorWebView");

  openstudio::path floorplanPath() const;
  openstudio::path gbXmlPath() const;
  openstudio::path idfPath() const;
  openstudio::path osmPath() const;

  BaseEditor* m_baseEditor;

  bool m_isIP;
  bool m_mergeWarn;

  model::Model m_model;

  QString m_debugPort;

  QComboBox* m_geometrySourceComboBox;
  QPushButton* m_newImportGeometry;
  ProgressBarWithError* m_progressBar;
  QPushButton* m_refreshBtn;
  QPushButton* m_previewBtn;
  QPushButton* m_mergeBtn;
  QPushButton* m_debugBtn;

  QWebEngineView* m_view;
  OSWebEnginePage* m_page;
  std::shared_ptr<OSDocument> m_document;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYEDITORVIEW_HPP
