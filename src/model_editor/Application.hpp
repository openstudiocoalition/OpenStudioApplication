/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_APPLICATION_HPP
#define MODELEDITOR_APPLICATION_HPP

#include "ModelEditorAPI.hpp"

#include <openstudio/utilities/core/Singleton.hpp>

#include "QMetaTypes.hpp"

#include <QApplication>

#include <boost/optional.hpp>

namespace openstudio {

/** Singleton application wide configuration management.
  */
class ApplicationSingleton
{

  friend class Singleton<ApplicationSingleton>;

 public:
  /// get the QApplication, if no QApplication has been set this will create a default one
  QCoreApplication* application(bool gui = true);

  /// check if the application has been initialized
  bool hasApplication() const;

  /// check if the application has GUI enabled
  bool hasGUI() const;

  /// set the QApplication, this should be done before calling application(),
  /// no op if it has already been set.  Returns true if set succeeded.
  bool setApplication(QCoreApplication* qApplication);

  /// get the QWidget wrapper around SketchUp window
  /// initialized by call to application, only implemented for windows
  QWidget* sketchUpWidget();

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

  ~ApplicationSingleton();

 private:
  /// private constructor
  ApplicationSingleton();

  /// QApplication handle
  QCoreApplication* m_qApplication;

  /// QWidget wrapper around SketchUp window
  QWidget* m_sketchUpWidget;

  bool m_defaultInstance;
};

typedef openstudio::Singleton<ApplicationSingleton> Application;

#if _WIN32 || _MSC_VER
/// Explicitly instantiate and export ApplicationSingleton Singleton template instance
/// so that the same instance is shared between the DLL's that link to this dll
MODELEDITOR_TEMPLATE_EXT template class openstudio::Singleton<ApplicationSingleton>;
#endif
}  // namespace openstudio

#endif  // MODELEDITOR_APPLICATION_HPP
