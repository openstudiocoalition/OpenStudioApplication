/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_EPWEXPLORERVIEW_HPP
#define OPENSTUDIO_EPWEXPLORERVIEW_HPP

#include <openstudio/model/Model.hpp>

#include <QWidget>

class QStackedWidget;
class QWebEngineView;

namespace openstudio {

class EpwExplorerView : public QWidget
{
  Q_OBJECT

 public:
  EpwExplorerView(bool isIP, const model::Model& model, const QString& modelTempDir, QWidget* parent = nullptr);
  ~EpwExplorerView() override = default;

 public slots:
  void toggleUnits(bool isIP);

 private slots:
  void onPageLoadFinished(bool ok);

 private:
  void loadWeatherFile();

  model::Model m_model;
  QString m_modelTempDir;
  bool m_isIP;

  QStackedWidget* m_stack = nullptr;
  QWebEngineView* m_webView = nullptr;

  // EPW file content held until the page finishes loading
  QString m_pendingEpwContent;
  bool m_pageLoaded = false;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_EPWEXPLORERVIEW_HPP
