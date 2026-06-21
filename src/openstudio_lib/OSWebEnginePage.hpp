/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSWEBENGINEPAGE_HPP
#define OPENSTUDIO_OSWEBENGINEPAGE_HPP

#include <QByteArray>
#include <QWebEnginePage>
#include <QWebEngineUrlRequestInfo>
#include <QWebEngineUrlRequestInterceptor>

#include <openstudio/utilities/core/Logger.hpp>

namespace openstudio {

class OSUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
 public:
  explicit OSUrlRequestInterceptor(QObject* parent = 0);
  virtual ~OSUrlRequestInterceptor();
  void interceptRequest(QWebEngineUrlRequestInfo& info) final;

 private:
  QByteArray m_userAgent;
};

/**
 * OSWebEnginePage is a QWebEnginePage subclass that adds OpenStudio-specific URL interception and
 * JavaScript bridge support. It is used by the Geometry tab's web view to communicate between the
 * FloorspaceJS editor and the C++ model layer.
 */
class OSWebEnginePage : public QWebEnginePage
{
  Q_OBJECT

 public:
  explicit OSWebEnginePage(QObject* parent = 0);
  virtual ~OSWebEnginePage();

 protected:
  virtual bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame) override;

 private:
  REGISTER_LOGGER("openstudio::OSWebEnginePage");
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSWEBENGINEPAGE_HPP
