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

#include "GeometryPreviewView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"

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

    model::ThreeJSForwardTranslator ft;
    const ThreeScene scene = ft.modelToThreeJS(m_model, true, updatePercentage);  // triangulated
    const std::string json = scene.toJSON(false);                                 // no pretty print
    m_json = QString::fromStdString(json);
  } else {
    m_progressBar->setValue(90);
  }

  // disable doc
  m_document->disable();

  // call init and animate
  const QString javascript = QString("runFromJSON(%1);").arg(m_json);
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
