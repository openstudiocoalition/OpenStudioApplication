/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "PathWatcher.hpp"
#include "Application.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Checksum.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QTimer>

/// constructor
PathWatcher::PathWatcher(const openstudio::path& p, int msec)
  : m_enabled(true), m_exists(openstudio::filesystem::exists(p)), m_dirty(false), m_checksum(openstudio::checksum(p)), m_path(p), m_msec(msec) {
  // make sure a QApplication exists
  openstudio::Application::instance().application(false);
  openstudio::Application::instance().processEvents();

  const bool isDirectory =
    (openstudio::filesystem::is_directory(p) || openstudio::toString(p.filename()) == "." || openstudio::toString(p.filename()) == "/");
  if (isDirectory) {

    LOG_FREE_AND_THROW("openstudio.PathWatcher", "Watching Directory '" << openstudio::toString(p) << "' is not supported");

  } else {
    m_timer = std::shared_ptr<QTimer>(new QTimer());
    connect(m_timer.get(), &QTimer::timeout, this, &PathWatcher::checkFile);
    m_timer->start(m_msec);

    // DLM: do not use QFileSystemWatcher to watch individual files, was acting glitchy
    //connected = this->connect(m_impl.get(), SIGNAL(fileChanged(const QString&)), SLOT(fileChanged(const QString&)));
    //OS_ASSERT(connected);
    //m_impl->addPath(openstudio::toQString(p));
  }
}

PathWatcher::~PathWatcher() = default;

bool PathWatcher::enabled() const {
  return m_enabled;
}

void PathWatcher::enable() {
  m_enabled = true;

  if (!m_timer->isActive()) {
    m_timer->start(m_msec);
  }
}

bool PathWatcher::disable() {
  if (m_timer->isActive()) {
    m_timer->stop();
  }

  bool result = m_enabled;
  m_enabled = false;
  return result;
}

openstudio::path PathWatcher::path() const {
  return m_path;
}

bool PathWatcher::dirty() const {
  return m_dirty;
}

void PathWatcher::clearState() {
  m_exists = openstudio::filesystem::exists(m_path);
  m_dirty = false;
  m_checksum = openstudio::checksum(m_path);
}

void PathWatcher::onPathAdded() {}

void PathWatcher::onPathChanged() {}

void PathWatcher::onPathRemoved() {}

void PathWatcher::fileChanged(const QString& path) {
  checkFile();
}

void PathWatcher::checkFile() {
  bool exists = openstudio::filesystem::exists(m_path);
  std::string checksum = openstudio::checksum(m_path);

  if (checksum == "00000000") {
    exists = false;
  }

  if (m_exists && exists) {

    // first check checksum
    if (checksum != m_checksum) {
      m_dirty = true;
      m_checksum = checksum;

      // regular change
      if (m_enabled) {
        onPathChanged();
      }
    }

  } else if (m_exists && !exists) {

    // used to exist, now does not
    m_dirty = true;
    m_exists = exists;
    m_checksum = openstudio::checksum(m_path);

    if (m_enabled) {
      onPathRemoved();
    }

  } else if (!m_exists && exists) {

    // did not exist, now does
    m_dirty = true;
    m_exists = exists;
    m_checksum = openstudio::checksum(m_path);

    if (m_enabled) {
      onPathAdded();
    }

  } else {
    // !m_exists && !exists
    // no change
  }
}
