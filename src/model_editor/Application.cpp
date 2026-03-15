/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "Application.hpp"

// TODO: JM 2019-03-28 Do I also need to make a specific version of getOpenStudioModuleDirectory?
#include <openstudio/utilities/core/ApplicationPathHelpers.hpp>

#include <openstudio/utilities/core/String.hpp>

#include "Utilities.hpp"

#include <QSettings>

namespace openstudio {

Application& Application::instance() {
  static Application instance;
  return instance;
}

Application::Application() : m_qApplication(nullptr), m_defaultInstance(false) {}

Application::~Application() {
  if (m_qApplication) {
    m_qApplication->quit();
  }
}

/// get the QApplication, if no QApplication has been set this will create a default one
QCoreApplication* Application::application(bool gui) {
  if (!m_qApplication) {

    if (QCoreApplication::instance()) {

      m_qApplication = QCoreApplication::instance();

    } else {

      QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
      QCoreApplication::setAttribute(Qt::AA_PluginApplication, true);

      // dir containing the current module, can be openstudio.so or openstudio.exe
      openstudio::path openstudioModuleDirPath = getOpenStudioModuleDirectory();

      // Add the current module path to the backup plugin search location
      QCoreApplication::addLibraryPath(toQString(openstudioModuleDirPath));

      openstudio::path openstudioPossibleBinDirPath = openstudioModuleDirPath / openstudio::toPath("platforms/");
      QCoreApplication::addLibraryPath(toQString(openstudioPossibleBinDirPath));

      // DLM: the code below is pretty kludgy, it depends on installation of the OpenStudio Application components
      openstudioPossibleBinDirPath = openstudioModuleDirPath / openstudio::toPath("../bin/");
      QCoreApplication::addLibraryPath(toQString(openstudioPossibleBinDirPath));

      openstudioPossibleBinDirPath = openstudioModuleDirPath / openstudio::toPath("../bin/platforms/");
      QCoreApplication::addLibraryPath(toQString(openstudioPossibleBinDirPath));

      // Make the ruby path the default plugin search location
      //#if defined(Q_OS_DARWIN)
      //      openstudio::path p = getApplicationRunDirectory().parent_path().parent_path().parent_path() / toPath("Ruby/openstudio");
      //      QCoreApplication::addLibraryPath(toQString(p));
      //#elif defined(Q_OS_WIN)
      //      openstudio::path p = getApplicationRunDirectory().parent_path() / toPath("Ruby/openstudio");
      //      QCoreApplication::addLibraryPath(toQString(p));
      //#endif

      static char* argv[] = {nullptr};
      static int argc = sizeof(argv) / sizeof(char*) - 1;

      // Load the qpa plugin
      if (gui) {
        m_qApplication = new QApplication(argc, argv);
        dynamic_cast<QApplication*>(m_qApplication)->setQuitOnLastWindowClosed(false);
      } else {
        m_qApplication = new QCoreApplication(argc, argv);
      }

      m_defaultInstance = true;
    }
  }

  return m_qApplication;
}

bool Application::hasApplication() const {
  return (m_qApplication != nullptr);
}

bool Application::hasGUI() const {
  if (hasApplication()) {
    return dynamic_cast<QApplication*>(m_qApplication);
  }
  return false;
}

/// set the QApplication, this should be done before calling application(), no op if it has already been set
bool Application::setApplication(QCoreApplication* qApplication) {
  if (!m_qApplication) {
    m_qApplication = qApplication;
    return true;
  }
  return false;
}

void Application::processEvents() {
  application()->sendPostedEvents();
  application()->processEvents(QEventLoop::AllEvents);
}

void Application::processEvents(int maxTime) {
  application()->sendPostedEvents();
  application()->processEvents(QEventLoop::AllEvents, maxTime);
}

bool Application::hasSetting(const std::string& key) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  return settings.contains(toQString(key));
}

void Application::removeSetting(const std::string& key) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.remove(toQString(key));
}

bool Application::isDefaultInstance() const {
  return m_defaultInstance;
}

boost::optional<bool> Application::getSettingValueAsBool(const std::string& key) {
  boost::optional<bool> result;

  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);

  if (!settings.contains(toQString(key))) {
    return boost::none;
  }

  QVariant value = settings.value(toQString(key));
  if (!value.isNull()) {
    if (value.canConvert<bool>()) {
      result = value.value<bool>();
    }
  }

  return result;
}

boost::optional<int> Application::getSettingValueAsInt(const std::string& key) {
  boost::optional<int> result;

  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);

  if (!settings.contains(toQString(key))) {
    return boost::none;
  }

  QVariant value = settings.value(toQString(key));
  if (!value.isNull()) {
    if (value.canConvert<int>()) {
      result = value.value<int>();
    }
  }

  return result;
}

boost::optional<double> Application::getSettingValueAsDouble(const std::string& key) {
  boost::optional<double> result;

  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);

  if (!settings.contains(toQString(key))) {
    return boost::none;
  }

  QVariant value = settings.value(toQString(key));
  if (!value.isNull()) {
    if (value.canConvert<double>()) {
      result = value.value<double>();
    }
  }

  return result;
}

boost::optional<std::string> Application::getSettingValueAsString(const std::string& key) {
  boost::optional<std::string> result;

  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);

  if (!settings.contains(toQString(key))) {
    return boost::none;
  }

  QVariant value = settings.value(toQString(key));
  if (!value.isNull()) {
    if (value.canConvert<std::string>()) {
      result = value.value<std::string>();
    }
  }

  return result;
}

void Application::setSettingValue(const std::string& key, bool value) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue(toQString(key), QVariant::fromValue(value));
}

void Application::setSettingValue(const std::string& key, int value) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue(toQString(key), QVariant::fromValue(value));
}

void Application::setSettingValue(const std::string& key, double value) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue(toQString(key), QVariant::fromValue(value));
}

void Application::setSettingValue(const std::string& key, const std::string& value) {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue(toQString(key), QVariant::fromValue(value));
}

}  // namespace openstudio
