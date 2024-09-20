/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_PATHWATCHER_HPP
#define MODELEDITOR_PATHWATCHER_HPP

#include "ModelEditorAPI.hpp"

#include <openstudio/utilities/core/Path.hpp>

#include <QObject>
#include <QString>

// forward declarations
class QTimer;

/** Class for watching a file for changes, directories are not supported 
  **/
class MODELEDITOR_API PathWatcher : public QObject
{

  Q_OBJECT;

 public:
  /// constructor with path

  /// if path is not a directory it is assumed to be a regular file which may or may not exist at construction,
  /// a timer is used to periodically check for changes to the file
  /// msec is the timer delay to check for updates to the file,
  /// an execption is thrown if path is a directory
  explicit PathWatcher(const openstudio::path& p, int msec = 1000);

  /// virtual destructor
  virtual ~PathWatcher();

  /// is the watcher currently enabled
  bool enabled() const;

  /// enable the watcher
  void enable();

  // disable watcher, returns true if it was enabled
  bool disable();

  /// path that is being watched
  openstudio::path path() const;

  /// true if path has been changed
  bool dirty() const;

  /// reset state to clean
  void clearState();

  /// for files, called when watched file is added if watcher enabled
  virtual void onPathAdded();

  /// for files, called when watched file is modified if watcher enabled
  virtual void onPathChanged();

  /// for files, called when watched file is removed if watcher enabled
  virtual void onPathRemoved();

 public slots:

  /// called when file is modified or removed
  void fileChanged(const QString& path);

  /// periodically check for changes
  void checkFile();

 private:
  /// impl
  std::shared_ptr<QTimer> m_timer;

  bool m_enabled;
  bool m_exists;
  bool m_dirty;
  std::string m_checksum;
  openstudio::path m_path;
  int m_msec;
};

#endif  // MODELEDITOR_PATHWATCHER_HPP
