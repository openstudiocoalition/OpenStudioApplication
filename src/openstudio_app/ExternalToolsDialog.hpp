/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_EXTERNALTOOLSDIALOG_HPP
#define OPENSTUDIO_EXTERNALTOOLSDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <openstudio/utilities/core/Filesystem.hpp>

class QListWidget;

namespace openstudio {

class ExternalToolsDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit ExternalToolsDialog(const openstudio::path& t_dviewPath);

  virtual ~ExternalToolsDialog() = default;

  /** The current externaltools paths */
  openstudio::path dviewPath() const;

  // openstudio::path otherToolPath() const;

 private:
  // Tool name should match the executable name
  void onChangeClicked(QLineEdit* t_lineEdit, const QString& toolName);

  QLineEdit* m_dviewPathLineEdit;

  // QLineEdit * m_otherToolPathLineEdit;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_EXTERNALTOOLSDIALOG_HPP
