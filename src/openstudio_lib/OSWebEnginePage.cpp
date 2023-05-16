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

#include "OSWebEnginePage.hpp"

#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QDesktopServices>
#include <QWebEngineCertificateError>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

namespace openstudio {

OSUrlRequestInterceptor::OSUrlRequestInterceptor(QObject* parent) : QWebEngineUrlRequestInterceptor(parent) {}
OSUrlRequestInterceptor::~OSUrlRequestInterceptor() = default;

void OSUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info) {
  static QByteArray userAgent;
  if (userAgent.isEmpty()) {
    userAgent.append("OpenStudioApp/");
    userAgent.append(OpenStudioApplicationVersionMajor());
    userAgent.append(".");
    userAgent.append(OpenStudioApplicationVersionMinor());
  }
  info.setHttpHeader("Accept-Language", "en-US,en;q=0.9,es;q=0.8,de;q=0.7");
  info.setHttpHeader("User-Agent", userAgent);
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
