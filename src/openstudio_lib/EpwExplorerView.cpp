/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "EpwExplorerView.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/utilities/core/PathHelpers.hpp>
#include <openstudio/utilities/filetypes/EpwFile.hpp>

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLabel>
#include <QPixmap>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineDownloadRequest>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>

namespace openstudio {

EpwExplorerView::EpwExplorerView(bool isIP, const model::Model& model, const QString& modelTempDir, QWidget* parent)
  : QWidget(parent), m_model(model), m_modelTempDir(modelTempDir), m_isIP(isIP) {
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  m_stack = new QStackedWidget(this);
  layout->addWidget(m_stack);

  // Index 0 — no weather file placeholder (matches Utility Bills style)
  auto* noFileLabel = new QLabel(this);
  noFileLabel->setPixmap(QPixmap(":/images/utility_calibration_warning.png"));
  noFileLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  m_stack->addWidget(noFileLabel);  // index 0

  // Index 1 — epwvis web view
  m_webView = new QWebEngineView(this);
  m_webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessFileUrls, true);
  // Suppress spurious touch-event warnings on macOS
  m_webView->setAttribute(Qt::WA_AcceptTouchEvents, false);
  connect(m_webView, &QWebEngineView::loadFinished, this, &EpwExplorerView::onPageLoadFinished);

  // Handle JS-triggered downloads (blob URLs, data: links with the download attribute)
  connect(m_webView->page()->profile(), &QWebEngineProfile::downloadRequested, this,
          [this](QWebEngineDownloadRequest* download) {
            const QString suggested = download->suggestedFileName();
            const QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
            const QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"), downloadsDir + "/" + suggested);
            if (savePath.isEmpty()) {
              download->cancel();
              return;
            }
            download->setDownloadDirectory(QFileInfo(savePath).absolutePath());
            download->setDownloadFileName(QFileInfo(savePath).fileName());
            download->accept();
          });
  m_stack->addWidget(m_webView);  // index 1

  loadWeatherFile();
}

void EpwExplorerView::loadWeatherFile() {
  boost::optional<model::WeatherFile> weatherFile = m_model.weatherFile();
  if (!weatherFile) {
    m_stack->setCurrentIndex(0);
    return;
  }

  const openstudio::path filesDir = toPath(m_modelTempDir) / toPath("resources/files/");
  boost::optional<EpwFile> epwFile = weatherFile->file(filesDir);
  if (!epwFile) {
    m_stack->setCurrentIndex(0);
    return;
  }

  QFile f(toQString(epwFile->path()));
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_stack->setCurrentIndex(0);
    return;
  }
  m_pendingEpwContent = QString::fromUtf8(f.readAll());

  m_stack->setCurrentIndex(1);
  m_webView->load(QUrl(QStringLiteral("qrc:///library/epw_explorer.html")));
}

void EpwExplorerView::onPageLoadFinished(bool ok) {
  if (!ok || m_pendingEpwContent.isEmpty()) {
    return;
  }
  m_pageLoaded = true;

  const QString unitSys = m_isIP ? QStringLiteral("IP") : QStringLiteral("SI");

  // JSON-encode the EPW content string so all special characters are safely escaped
  // for embedding as a JS string literal.
  QJsonArray arr;
  arr.append(QJsonValue(m_pendingEpwContent));
  const QString jsonArray = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
  // Strip the outer [ ] to get just the quoted string literal: "content..."
  const QString jsonStr = jsonArray.mid(1, jsonArray.length() - 2);

  m_webView->page()->runJavaScript(QStringLiteral("readRaw(%1, '%2')").arg(jsonStr, unitSys));
}

void EpwExplorerView::toggleUnits(bool isIP) {
  m_isIP = isIP;
  if (m_pageLoaded) {
    const QString unitSys = m_isIP ? QStringLiteral("IP") : QStringLiteral("SI");
    m_webView->page()->runJavaScript(
      QStringLiteral("unitSystem = '%1'; if (epw && epw.stationLocation) { updateCharts(epw, unitSystem); }").arg(unitSys));
  }
}

}  // namespace openstudio
