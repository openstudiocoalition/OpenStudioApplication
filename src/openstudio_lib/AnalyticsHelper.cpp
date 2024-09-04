/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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

void AnalyticsHelper::sendAnalytics(const QString& analyticsId, const std::string& contentType, const std::string& contentId) {

  // https://developers.google.com/analytics/devguides/collection/protocol/ga4/sending-events
  QUrlQuery query(QString::fromUtf8("https://www.google-analytics.com/mp/collect?"));
  query.addQueryItem("api_secret", apiSecret());
  query.addQueryItem("measurement_id", measurementId());

  QNetworkRequest request(query.query());
  request.setRawHeader("Content-Type", "application/json");

  QJsonObject selectContentParams;
  selectContentParams["content_type"] = QString::fromStdString(contentType);
  selectContentParams["content_id"] = QString::fromStdString(contentId);

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
