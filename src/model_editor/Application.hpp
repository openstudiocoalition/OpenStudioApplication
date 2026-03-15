/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_APPLICATION_HPP
#define MODELEDITOR_APPLICATION_HPP

#include "ModelEditorAPI.hpp"

#include "QMetaTypes.hpp"

#include <QApplication>

#include <boost/optional.hpp>

namespace openstudio {

/** Singleton application wide configuration management.
  */
class Application
{

 public:
  static Application& instance();

  Application(const Application& other) = delete;
  Application(Application&& other) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;

  /// get the QApplication, if no QApplication has been set this will create a default one
  QCoreApplication* application(bool gui = true);

  /// check if the application has been initialized
  bool hasApplication() const;

  /// check if the application has GUI enabled
  bool hasGUI() const;

  /// set the QApplication, this should be done before calling application(),
  /// no op if it has already been set.  Returns true if set succeeded.
  bool setApplication(QCoreApplication* qApplication);

  /// Process pending Qt events
  void processEvents();
  void processEvents(int maxTime);

  /// Check if application has given setting
  static bool hasSetting(const std::string& key);

  /// Remove setting
  static void removeSetting(const std::string& key);

  /// Check if the application is headless
  bool isDefaultInstance() const;

  /// Get the value of setting as given type, be careful when using getSettingValueAsBool
  /// you must first check if the optional is set and then check its value
  static boost::optional<bool> getSettingValueAsBool(const std::string& key);
  static boost::optional<int> getSettingValueAsInt(const std::string& key);
  static boost::optional<double> getSettingValueAsDouble(const std::string& key);
  static boost::optional<std::string> getSettingValueAsString(const std::string& key);

  /// Set application value to given value
  static void setSettingValue(const std::string& key, bool value);
  static void setSettingValue(const std::string& key, int value);
  static void setSettingValue(const std::string& key, double value);
  static void setSettingValue(const std::string& key, const std::string& value);

 private:
  /// private constructor
  Application();
  ~Application();

  /// QApplication handle
  QCoreApplication* m_qApplication;

  bool m_defaultInstance;
};

}  // namespace openstudio

#endif  // MODELEDITOR_APPLICATION_HPP
