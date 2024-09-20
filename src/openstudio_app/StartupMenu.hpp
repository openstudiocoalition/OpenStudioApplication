/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_STARTUPMENU_HPP
#define OPENSTUDIO_STARTUPMENU_HPP

#include <QMenuBar>

class QMenu;

namespace openstudio {

class StartupMenu : public QMenuBar
{
  Q_OBJECT

 public:
  explicit StartupMenu(QWidget* parent = nullptr);

  virtual ~StartupMenu() {}

 signals:

  void exportClicked();

  void importClicked();

  void importgbXMLClicked();

  void importSDDClicked();

  void importIFCClicked();

  void loadFileClicked();

  void saveAsFileClicked();

  void saveFileClicked();

  void newClicked();

  void exitClicked();

  void helpClicked();

  void checkForUpdateClicked();

  void debugWebglClicked();

  void aboutClicked();

 private:
  QMenu* m_fileMenu;

  QMenu* m_viewMenu;

  QMenu* m_helpMenu;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_STARTUPMENU_HPP
