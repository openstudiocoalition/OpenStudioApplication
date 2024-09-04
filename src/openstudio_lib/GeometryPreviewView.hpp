/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYPREVIEWVIEW_HPP
#define OPENSTUDIO_GEOMETRYPREVIEWVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"
#include "OSWebEnginePage.hpp"

#include <openstudio/model/Model.hpp>

#include "../shared_gui_components/ProgressBarWithError.hpp"

#include <QWidget>
#include <QWebEngineView>

class QCheckBox;
class QComboBox;
class QPushButton;

namespace openstudio {

class OSDocument;

class GeometryPreviewView : public QWidget
{
  Q_OBJECT

 public:
  GeometryPreviewView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~GeometryPreviewView();

 private:
};

// main widget

class PreviewWebView : public QWidget
{
  Q_OBJECT;

 public:
  PreviewWebView(bool isIP, const openstudio::model::Model& model, QWidget* t_parent = nullptr);
  virtual ~PreviewWebView();

 public slots:
  void onUnitSystemChange(bool t_isIP);

 private slots:
  void refreshClicked();

  // DLM: for debugging
  void onLoadFinished(bool ok);
  //void 	onLoadProgress(int progress);
  //void 	onLoadStarted();
  void onTranslateProgress(double percentage);
  void onJavaScriptFinished(const QVariant& v);
  void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);

 private:
  REGISTER_LOGGER("openstudio::PreviewWebView");

  bool m_isIP;
  model::Model m_model;

  ProgressBarWithError* m_progressBar;
  QCheckBox* m_geometryDiagnosticsBox;
  bool m_includeGeometryDiagnostics;
  QPushButton* m_refreshBtn;

  QWebEngineView* m_view;
  OSWebEnginePage* m_page;
  std::shared_ptr<OSDocument> m_document;

  QString m_json;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYPREVIEWVIEW_HPP
