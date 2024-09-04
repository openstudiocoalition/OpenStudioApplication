/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_VERTICALTABWIDGET_HPP
#define OPENSTUDIO_VERTICALTABWIDGET_HPP

#include <QWidget>

#include <vector>

class QPushButton;

namespace openstudio {

class MainTabView;

class OSViewSwitcher;

class VerticalTabWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit VerticalTabWidget(QWidget* parent = nullptr);

  virtual ~VerticalTabWidget() {}

  void addTabButton(int id, QString toolTip, const QString& selectedImagePath, const QString& unSelectedImagePath, const QString& disabledImagePath);

  void enableTabButton(int id, bool enable);

  void refreshTabButtons();

  void setView(MainTabView* view, int id);

  MainTabView* view() const;

  int verticalTabIndex();

 signals:

  void tabSelected(int id);

 public slots:

  void setCurrentIndex(int index);

 private slots:

  void select();

 private:
  int getIndex(int id);

  OSViewSwitcher* m_viewSwitcher = nullptr;

  QWidget* m_tabBar;

  std::vector<QString> m_selectedPixmaps;

  std::vector<QString> m_unSelectedPixmaps;

  std::vector<QString> m_disabledPixmaps;

  std::vector<QPushButton*> m_tabButtons;

  std::vector<int> m_ids;

  int currentIndex = -1;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_VERTICALTABWIDGET_HPP
