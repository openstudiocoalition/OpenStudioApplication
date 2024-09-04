/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ANALYTICSHELPER_HPP
#define OPENSTUDIO_ANALYTICSHELPER_HPP

#include <QObject>

class QNetworkAccessManager;

namespace openstudio {

class AnalyticsHelper : public QObject
{
  Q_OBJECT

 public:
  explicit AnalyticsHelper(QObject* parent = nullptr);

  virtual ~AnalyticsHelper();

 public slots:

  void sendAnalytics(const QString& analyticsId, const std::string& contentType, const std::string& contentId);

 private:
  QNetworkAccessManager* m_networkAccessManager;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ANALYTICSHELPER_HPP
