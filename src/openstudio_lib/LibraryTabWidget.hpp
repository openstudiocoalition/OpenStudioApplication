/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LIBRARYTABWIDGET_HPP
#define OPENSTUDIO_LIBRARYTABWIDGET_HPP

#include <QWidget>
#include <vector>

class QStackedWidget;

class QPushButton;

namespace openstudio {

class LibraryTabWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit LibraryTabWidget(QWidget* parent = nullptr);

  virtual ~LibraryTabWidget() {}

  void showRemoveButton();

  void hideRemoveButton();

  void addTab(QWidget* widget, const QString& selectedImagePath, const QString& unSelectedImagePath);

  /* This method, given a tab widget, will change the currentIndex to be the following if there is one, or zero otherwise
   * It will hide the tab by hide the corresponding button.
   * Useful to hide the LoopChooserView for components that don't have a water coil for eg. */
  void hideTab(QWidget* widget, bool hide = true);

 signals:

  void removeButtonClicked(bool);

 public slots:

  void setCurrentIndex(int index);

  void setCurrentWidget(QWidget* widget);

 private slots:

  void select();

 private:
  QStackedWidget* m_pageStack;

  QWidget* m_tabBar;

  std::vector<QString> m_selectedPixmaps;

  std::vector<QString> m_neighborSelectedPixmaps;

  std::vector<QString> m_unSelectedPixmaps;

  std::vector<QPushButton*> m_tabButtons;

  QPushButton* m_removeButton = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LIBRARYTABWIDGET_HPP
