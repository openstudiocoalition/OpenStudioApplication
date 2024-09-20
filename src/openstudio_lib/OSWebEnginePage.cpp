/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSWebEnginePage.hpp"

#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QDesktopServices>
#include <QWebEngineCertificateError>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include <fmt/core.h>

namespace openstudio {

OSUrlRequestInterceptor::OSUrlRequestInterceptor(QObject* parent)
  : QWebEngineUrlRequestInterceptor(parent),
    m_userAgent(
      QByteArray::fromStdString(fmt::format("OpenStudioApp/{}.{}", OpenStudioApplicationVersionMajor(), OpenStudioApplicationVersionMinor()))) {}

OSUrlRequestInterceptor::~OSUrlRequestInterceptor() = default;

void OSUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info) {
  info.setHttpHeader("Accept-Language", "en-US,en;q=0.9,es;q=0.8,de;q=0.7");
  info.setHttpHeader("User-Agent", m_userAgent);
}

OSWebEnginePage::OSWebEnginePage(QObject* parent) : QWebEnginePage(parent) {
  auto* interceptor = new OSUrlRequestInterceptor(this->profile());
  this->profile()->setUrlRequestInterceptor(interceptor);

  settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);
  settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessFileUrls, true);
  settings()->setAttribute(QWebEngineSettings::WebAttribute::AllowRunningInsecureContent, true);
  settings()->setAttribute(QWebEngineSettings::WebAttribute::AllowGeolocationOnInsecureOrigins, true);
  settings()->setAttribute(QWebEngineSettings::WebAttribute::SpatialNavigationEnabled, true);
}

OSWebEnginePage::~OSWebEnginePage() = default;

bool OSWebEnginePage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame) {
  if (type == QWebEnginePage::NavigationTypeLinkClicked) {
    // QString s = url.toString();
    // open links in system browser rather than embedded view
    QDesktopServices::openUrl(url);
    return false;
  }
  return true;
}

}  // namespace openstudio
