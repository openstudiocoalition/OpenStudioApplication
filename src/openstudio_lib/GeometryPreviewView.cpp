/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GeometryPreviewView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "MainWindow.hpp"

#include "../model_editor/Application.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ThreeJSForwardTranslator.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QWebEngineScriptCollection>
#include <QtConcurrent>
#include <QCheckBox>

using namespace std::placeholders;

namespace openstudio {

GeometryPreviewView::GeometryPreviewView(bool isIP, const openstudio::model::Model& model, QWidget* parent) : QWidget(parent) {
  // TODO: DLM implement units switching
  //connect(this, &GeometryPreviewView::toggleUnitsClicked, modelObjectInspectorView(), &ModelObjectInspectorView::toggleUnitsClicked);

  auto* layout = new QVBoxLayout;

  auto* webView = new PreviewWebView(isIP, model, this);
  layout->addWidget(webView);

  setLayout(layout);
}

GeometryPreviewView::~GeometryPreviewView() = default;

PreviewWebView::PreviewWebView(bool isIP, const model::Model& model, QWidget* t_parent)
  : QWidget(t_parent), m_isIP(isIP), m_model(model), m_progressBar(new ProgressBarWithError()), m_refreshBtn(new QPushButton("Refresh")) {

  openstudio::OSAppBase* app = OSAppBase::instance();
  OS_ASSERT(app);
  m_document = app->currentDocument();
  OS_ASSERT(m_document);

  auto* mainLayout = new QVBoxLayout;
  setLayout(mainLayout);

  connect(m_document.get(), &OSDocument::toggleUnitsClicked, this, &PreviewWebView::onUnitSystemChange);
  connect(m_refreshBtn, &QPushButton::clicked, this, &PreviewWebView::refreshClicked);

  auto* hLayout = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout);

  hLayout->addStretch();

  hLayout->addWidget(m_progressBar, 0, Qt::AlignVCenter);

  // set progress bar
  m_progressBar->setMinimum(0);
  m_progressBar->setMaximum(100);
  m_progressBar->setValue(0);
  m_progressBar->setVisible(true);

  hLayout->addWidget(m_refreshBtn, 0, Qt::AlignVCenter);
  m_refreshBtn->setVisible(true);

  m_view = new QWebEngineView(this);
  m_page = new OSWebEnginePage(m_view);
  m_view->setPage(m_page);  // note, view does not take ownership of page

  auto* mainWindow = OSAppBase::instance()->currentDocument()->mainWindow();
  const bool verboseOutput = mainWindow->geometryDiagnostics();
  m_geometryDiagnosticsBox = new QCheckBox();
  m_geometryDiagnosticsBox->setText("Geometry Diagnostics");
  m_geometryDiagnosticsBox->setChecked(verboseOutput);
  m_geometryDiagnosticsBox->setToolTip(
    "Enables adjacency issues. Enables checks for Surface/Space Convexity, due to this the ThreeJS export is slightly slower");
  connect(m_geometryDiagnosticsBox, &QCheckBox::clicked, mainWindow, &MainWindow::toggleGeometryDiagnostics);
  connect(m_geometryDiagnosticsBox, &QCheckBox::stateChanged, [this](int state) {
    if (state == Qt::Checked && !m_includeGeometryDiagnostics) {
      // Old m_json didn't contain the geometry diagnostics, so we need to include it, so we should set m_json to empty so the
      // ThreeJSForwardTranslator is called again
      m_json = QString();
    } else {
      // Any other case, the former m_json includes diagnostics, we only trigger the refresh which will reanimate the web page and potentially turn
      // off the Geometry diags datGUI
    }
    refreshClicked();
  });
  hLayout->addWidget(m_geometryDiagnosticsBox, 0, Qt::AlignVCenter);

  connect(m_view, &QWebEngineView::loadFinished, this, &PreviewWebView::onLoadFinished);
  connect(m_view, &QWebEngineView::renderProcessTerminated, this, &PreviewWebView::onRenderProcessTerminated);

  // Debug: switch to true. if false, code isn't even compiled since if-constexpr is used
  constexpr bool isDebug_ = false;
  if constexpr (isDebug_) {
    connect(m_view, &QWebEngineView::loadStarted, this, []() { qDebug() << "Loading started"; });
    connect(m_view, &QWebEngineView::loadProgress, this,
            [](int progress) { qDebug() << "PreviewWebView::onLoadProgress: " << progress; });  // &PreviewWebView::onLoadProgress);
    connect(m_page, &QWebEnginePage::loadStarted, this, []() { qDebug() << "Page Loading Started"; });
    connect(m_page, &QWebEnginePage::loadProgress, this, [](int progress) { qDebug() << "Page Loading Progress: " << progress; });
    connect(m_page, &QWebEnginePage::loadFinished, this, [](bool ok) { qDebug() << "Page Loading Finished: " << ok; });
  }

  // Force QWebEngineView to fill the rest of the space
  m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_view->setContextMenuPolicy(Qt::NoContextMenu);

  //mainLayout->addWidget(m_view, 10, Qt::AlignTop);
  mainLayout->addWidget(m_view);

  QUrl previewURL("qrc:///library/geometry_preview.html");
  m_view->load(previewURL);
}

PreviewWebView::~PreviewWebView() = default;

void PreviewWebView::refreshClicked() {
  // qDebug() << "refreshClicked";

  m_progressBar->setError(false);

  m_view->triggerPageAction(QWebEnginePage::ReloadAndBypassCache);
}

void PreviewWebView::onUnitSystemChange(bool t_isIP) {
  LOG(Debug, "onUnitSystemChange " << t_isIP << " reloading results");
  m_isIP = t_isIP;
}

// void callWithResult(const QString& result) {
//   QString x = result;
// }

void PreviewWebView::onLoadFinished(bool ok) {
  const QString title = m_view->title();
  // qDebug() << "onLoadFinished, ok=" << ok << ", title=" << title;
  if (ok) {
    m_progressBar->setValue(10);
  } else {
    m_progressBar->setValue(100);
    m_progressBar->setError(true);
    return;
  }

  if (m_json.isEmpty()) {
    std::function<void(double)> updatePercentage = std::bind(&PreviewWebView::onTranslateProgress, this, _1);
    //ThreeScene scene = modelToThreeJS(m_model.clone(true).cast<model::Model>(), true, updatePercentage); // triangulated

    // qDebug() << "ThreeJSForwardTranslator";

    model::ThreeJSForwardTranslator ft;
    m_includeGeometryDiagnostics = m_geometryDiagnosticsBox->isChecked();
    ft.setIncludeGeometryDiagnostics(m_includeGeometryDiagnostics);
    const ThreeScene scene = ft.modelToThreeJS(m_model, true, updatePercentage);  // triangulated
    const std::string json = scene.toJSON(false);                                 // no pretty print
    m_json = QString::fromStdString(json);
  } else {
    m_progressBar->setValue(90);
  }

  // disable doc
  m_document->disable();

  // call init and animate
  const QString javascript = QString("runFromJSON(%1, %2);").arg(m_json, m_geometryDiagnosticsBox->isChecked() ? "true" : "false");
  m_view->page()->runJavaScript(javascript, [this](const QVariant& v) { onJavaScriptFinished(v); });

  //javascript = QString("os_data.metadata.version");
  //m_view->page()->runJavaScript(javascript, [](const QVariant &v) { callWithResult(v.toString()); });
}

//void PreviewWebView::onLoadProgress(int progress)
//{
//}

//void PreviewWebView::onLoadStarted()
//{
//}

void PreviewWebView::onTranslateProgress(double percentage) {
  m_progressBar->setValue(10 + 0.8 * percentage);
  OSAppBase::instance()->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
}

void PreviewWebView::onJavaScriptFinished(const QVariant& v) {
  m_document->enable();
  m_progressBar->setValue(100);
}

void PreviewWebView::onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode) {
  // qDebug() << "RenderProcessTerminationStatus: terminationStatus= " << terminationStatus << "exitCode=" << exitCode;
  m_progressBar->setValue(100);
  m_progressBar->setError(true);
}

}  // namespace openstudio
