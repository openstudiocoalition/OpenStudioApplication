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

#include "AnalyticsHelper.hpp"
#include "AnalyticsHelperSecrets.hxx"

#include "../model_editor/Application.hpp"
#include "../model_editor/Utilities.hpp"
#include "../utilities/OpenStudioApplicationPathHelpers.hpp"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrlQuery>

namespace openstudio {

AnalyticsHelper::AnalyticsHelper(QObject* parent) : QObject(parent), m_networkAccessManager(new QNetworkAccessManager(this)) {
  m_networkAccessManager->setAutoDeleteReplies(true);
}

AnalyticsHelper::~AnalyticsHelper() {
  // cancel all futures
  delete m_networkAccessManager;
}

void AnalyticsHelper::sendAnalytics(const QString& analyticsId, int verticalTabIndex) {

  // https://developers.google.com/analytics/devguides/collection/protocol/ga4/sending-events
  QUrlQuery query(QString::fromUtf8("https://www.google-analytics.com/mp/collect?"));
  query.addQueryItem("api_secret", apiSecret());
  query.addQueryItem("measurement_id", measurementId());

  QNetworkRequest request(query.query());
  request.setRawHeader("Content-Type", "application/json");

  QString tab = QString("Tab-%1").arg(verticalTabIndex);

  QJsonObject selectContentParams;
  selectContentParams["content_type"] = "OSAppTab";
  selectContentParams["content_id"] = tab;

  QJsonObject selectContentEvent;
  selectContentEvent["name"] = "select_content";
  selectContentEvent["params"] = selectContentParams;

  QJsonArray events;
  events.append(selectContentEvent);

  QJsonObject json;
  json["client_id"] = analyticsId;
  json["non_personalized_ads"] = true;
  json["events"] = events;

  QJsonDocument doc(json);
  QByteArray data = doc.toJson();
  m_networkAccessManager->post(request, data);

  // for debugging
  //auto reply = m_networkAccessManager->post(request, data);
  //while (reply->isRunning() && !reply->isFinished())
  //{
  //  Application::instance().processEvents();
  //}
  //auto error = reply->error();
  //auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  //delete reply;
}

}  // namespace openstudio
