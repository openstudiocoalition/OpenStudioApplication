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
#include <openstudio/model/PlanarSurface.hpp>
#include <openstudio/model/PlanarSurface_Impl.hpp>
#include <openstudio/model/BuildingStory.hpp>
#include <openstudio/model/BuildingStory_Impl.hpp>
#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>
#include <openstudio/model/SubSurface.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/model/ThreeJSForwardTranslator.hpp>

#include <algorithm>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QJsonArray>
#include <QJsonDocument>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QWebEngineScriptCollection>
#include <QWebChannel>
#include <QtConcurrent>
#include <QCheckBox>

using namespace std::placeholders;

namespace openstudio {

GeometryBridge::GeometryBridge(model::Model& model, QObject* parent) : QObject(parent), m_model(model) {}

void GeometryBridge::reverseSurfaceVertices(const QString& surfaceName) {
  if (auto surface = m_model.getModelObjectByName<model::PlanarSurface>(surfaceName.toStdString())) {
    auto vertices = surface->vertices();
    std::reverse(vertices.begin(), vertices.end());
    surface->setVertices(vertices);
    emit modelChanged();
  }
}

void GeometryBridge::triangulateSurface(const QString& surfaceName) {
  auto planarSurface_ = m_model.getModelObjectByName<model::PlanarSurface>(surfaceName.toStdString());
  if (!planarSurface_) {
    return;
  }

  // Skip the case where the Surface has SubSurfaces, since we'd have to deal with triangulation of the subSurfaces(s) then intersection etc
  if (auto surface_ = planarSurface_->optionalCast<model::Surface>()) {
    if (!surface_->subSurfaces().empty()) {
      return;
    }
  }
  const auto triangles = planarSurface_->triangulation();
  if (triangles.size() < 2) {
    return;
  }

  const std::string origName = planarSurface_->nameString();
  for (int i = 0; const auto& points : triangles) {
    ++i;
    auto clone = planarSurface_->clone().cast<model::PlanarSurface>();
    clone.setVertices(points);
    clone.setName(origName + " (triangulation " + std::to_string(i) + ")");
  }
  planarSurface_->remove();

  emit modelChanged();
}

void GeometryBridge::setSunExposure(const QString& surfaceName, const QString& value) {
  if (auto surface = m_model.getModelObjectByName<model::Surface>(surfaceName.toStdString())) {
    surface->setSunExposure(value.toStdString());
    emit modelChanged();
  }
}

void GeometryBridge::setWindExposure(const QString& surfaceName, const QString& value) {
  if (auto surface = m_model.getModelObjectByName<model::Surface>(surfaceName.toStdString())) {
    surface->setWindExposure(value.toStdString());
    emit modelChanged();
  }
}

void GeometryBridge::setOutsideBoundaryCondition(const QString& surfaceName, const QString& value) {
  if (auto surface = m_model.getModelObjectByName<model::Surface>(surfaceName.toStdString())) {
    surface->setOutsideBoundaryCondition(value.toStdString());
    emit modelChanged();
  }
}

void GeometryBridge::setConstruction(const QString& surfaceName, const QString& constructionName) {
  if (auto surface = m_model.getModelObjectByName<model::PlanarSurface>(surfaceName.toStdString())) {
    if (constructionName.isEmpty()) {
      surface->resetConstruction();
    } else if (auto construction = m_model.getModelObjectByName<model::ConstructionBase>(constructionName.toStdString())) {
      surface->setConstruction(*construction);
    }
    emit modelChanged();
  }
}

void GeometryBridge::setThermalZone(const QString& spaceName, const QString& thermalZoneName) {
  if (auto space = m_model.getModelObjectByName<model::Space>(spaceName.toStdString())) {
    if (thermalZoneName.isEmpty()) {
      space->resetThermalZone();
    } else if (auto thermalZone = m_model.getModelObjectByName<model::ThermalZone>(thermalZoneName.toStdString())) {
      space->setThermalZone(*thermalZone);
    }
    emit modelChanged();
  }
}

void GeometryBridge::setBuildingStory(const QString& spaceName, const QString& buildingStoryName) {
  if (auto space = m_model.getModelObjectByName<model::Space>(spaceName.toStdString())) {
    if (buildingStoryName.isEmpty()) {
      space->resetBuildingStory();
    } else if (auto story = m_model.getModelObjectByName<model::BuildingStory>(buildingStoryName.toStdString())) {
      space->setBuildingStory(*story);
    }
    emit modelChanged();
  }
}

void GeometryBridge::setSpaceType(const QString& spaceName, const QString& spaceTypeName) {
  if (auto space = m_model.getModelObjectByName<model::Space>(spaceName.toStdString())) {
    if (spaceTypeName.isEmpty()) {
      space->resetSpaceType();
    } else if (auto spaceType = m_model.getModelObjectByName<model::SpaceType>(spaceTypeName.toStdString())) {
      space->setSpaceType(*spaceType);
    }
    emit modelChanged();
  }
}

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

  auto* hLayout = new QHBoxLayout();
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

  auto* channel = new QWebChannel(m_page);
  m_bridge = new GeometryBridge(m_model, this);
  channel->registerObject(QStringLiteral("bridge"), m_bridge);
  m_page->setWebChannel(channel);
  connect(m_bridge, &GeometryBridge::modelChanged, this, [this]() {
    m_json = QString();
    // Save current view settings and camera state to sessionStorage before the page reload
    m_view->page()->runJavaScript("saveViewStateToSessionStorage();", [this](const QVariant& /*v*/) { refreshClicked(); });
  });

  auto* mainWindow = OSAppBase::instance()->currentDocument()->mainWindow();
  const bool verboseOutput = mainWindow->geometryDiagnostics();
  m_geometryDiagnosticsBox = new QCheckBox();
  m_geometryDiagnosticsBox->setText("Geometry Diagnostics");
  m_geometryDiagnosticsBox->setChecked(verboseOutput);
  m_geometryDiagnosticsBox->setToolTip(
    "Enables adjacency issues. Enables checks for Surface/Space Convexity, due to this the ThreeJS export is slightly slower");
  connect(m_geometryDiagnosticsBox, &QCheckBox::clicked, mainWindow, &MainWindow::toggleGeometryDiagnostics);
  connect(m_geometryDiagnosticsBox, &QCheckBox::checkStateChanged, [this](Qt::CheckState state) {
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

  // build lists of available names for the JS context menu
  QJsonArray spaceTypeNamesArray;
  for (const auto& st : m_model.getConcreteModelObjects<model::SpaceType>()) {
    spaceTypeNamesArray.append(QString::fromStdString(st.nameString()));
  }
  const QString spaceTypeNamesJson = QJsonDocument(spaceTypeNamesArray).toJson(QJsonDocument::Compact);

  QJsonArray constructionNamesArray;
  for (const auto& c : m_model.getModelObjects<model::ConstructionBase>()) {
    constructionNamesArray.append(QString::fromStdString(c.nameString()));
  }
  const QString constructionNamesJson = QJsonDocument(constructionNamesArray).toJson(QJsonDocument::Compact);

  QJsonArray thermalZoneNamesArray;
  for (const auto& tz : m_model.getConcreteModelObjects<model::ThermalZone>()) {
    thermalZoneNamesArray.append(QString::fromStdString(tz.nameString()));
  }
  const QString thermalZoneNamesJson = QJsonDocument(thermalZoneNamesArray).toJson(QJsonDocument::Compact);

  QJsonArray buildingStoryNamesArray;
  for (const auto& bs : m_model.getConcreteModelObjects<model::BuildingStory>()) {
    buildingStoryNamesArray.append(QString::fromStdString(bs.nameString()));
  }
  const QString buildingStoryNamesJson = QJsonDocument(buildingStoryNamesArray).toJson(QJsonDocument::Compact);

  // call init and animate
  const QString javascript = QString("var availableSpaceTypeNames = %1; var availableConstructionNames = %2;"
                                     " var availableThermalZoneNames = %3; var availableBuildingStoryNames = %4;"
                                     " runFromJSON(%5, %6);")
                               .arg(spaceTypeNamesJson, constructionNamesJson, thermalZoneNamesJson, buildingStoryNamesJson, m_json,
                                    m_geometryDiagnosticsBox->isChecked() ? "true" : "false");
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
