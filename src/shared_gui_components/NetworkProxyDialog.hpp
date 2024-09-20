/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_NETWORKPROXYDIALOG_HPP
#define SHAREDGUICOMPONENTS_NETWORKPROXYDIALOG_HPP

#include <QNetworkProxy>
#include <QDialog>
#include <QSettings>
#include <QNetworkReply>

#include <openstudio/utilities/core/Logger.hpp>

class QComboBox;
class QLineEdit;

namespace openstudio {
class NetworkProxyDialog : public QDialog
{
  Q_OBJECT;
  REGISTER_LOGGER("openstudio.NetworkProxyDialog");

 public:
  // Saves to / loads from the QSettings object passed in
  NetworkProxyDialog(QSettings& t_settings, QWidget* t_parent);
  virtual ~NetworkProxyDialog() {}

  QNetworkProxy createProxy();

  static QNetworkProxy createProxy(const QSettings& t_settings);
  bool testProxy(QNetworkProxy& t_proxy);
  static bool testProxy(QNetworkProxy& t_proxy, QWidget* t_parent);
  void setSettings(QSettings& t_settings) const;

 private slots:
  void saveClicked();
  void cancelClicked();
  void testClicked();
  void proxyTypeChanged(int index);

 private:
  static std::pair<QNetworkReply::NetworkError, QString> testProxyConnection(const QNetworkProxy& t_proxy, QWidget* t_parent);

  QSettings& m_settings;

  QComboBox* m_proxyType;
  QLineEdit* m_hostName;
  QLineEdit* m_port;
  QLineEdit* m_user;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_NETWORKPROXYDIALOG_HPP
