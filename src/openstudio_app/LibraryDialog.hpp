/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LIBRARYDIALOG_HPP
#define OPENSTUDIO_LIBRARYDIALOG_HPP

#include <QDialog>
#include <vector>
#include <openstudio/utilities/core/Filesystem.hpp>

class QListWidget;

namespace openstudio {

class LibraryDialog : public QDialog
{
  Q_OBJECT

 public:
  LibraryDialog(const std::vector<openstudio::path>& paths, const std::vector<openstudio::path>& defaultPaths, const openstudio::path& resourcesPath);

  virtual ~LibraryDialog(){};

  /** The current list of library paths */
  std::vector<openstudio::path> paths() const;

 private:
  void onAdd();
  void onRemove();
  void onRestore();

  QListWidget* m_list;
  std::vector<openstudio::path> m_defaultPaths;
  openstudio::path m_resourcesPath;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LIBRARYDIALOG_HPP
