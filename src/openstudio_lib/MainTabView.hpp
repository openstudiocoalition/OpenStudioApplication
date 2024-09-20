/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MAINTABVIEW_HPP
#define OPENSTUDIO_MAINTABVIEW_HPP

#include <QPointer>
#include <QWidget>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <vector>

class QLabel;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;

namespace openstudio {

class OSViewSwitcher;

class MainTabView
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  enum TabType
  {
    MAIN_TAB,
    SUB_TAB,
    GRIDVIEW_SUB_TAB
  };

  MainTabView(const QString& tabLabel, TabType tabType, QWidget* parent = nullptr);

  virtual ~MainTabView();

  void setTabType(TabType tabType);

  ///! Use this method only if your tab will *NOT* have sub tabs
  bool addTabWidget(QWidget* widget);

  ///! Use this method only if your tab will have sub tabs
  bool addSubTab(const QString& subTabLabel, int id);

  void setSubTab(QWidget* widget);

  // Returns the id of the current sub tab.
  // Returns -1 if there are no sub tabs.
  //int subTabId() const;

  // Returns the index of the current sub tab.
  // Returns -1 if there are no sub tabs.
  //int subTabIndex() const;

  // Public method for setting the current sub tab.
  bool selectSubTabByIndex(int index);

  QPointer<OSViewSwitcher> m_editView;

 protected:
  void setCurrentIndex(int index);
  void setCurrentWidget(QWidget* widget);
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  QLabel* m_tabLabel = nullptr;
  QWidget* m_mainWidget = nullptr;
  QWidget* m_currentInnerWidget = nullptr;
  QVBoxLayout* m_innerLayout = nullptr;

  std::vector<QString> m_selectedPixmaps;
  std::vector<QString> m_neighborSelectedPixmaps;
  std::vector<QString> m_unSelectedPixmaps;
  std::vector<QPushButton*> m_tabButtons;
  std::vector<int> m_ids;

  TabType m_tabType;

 signals:

  void tabSelected(int id);
  void toggleUnitsClicked(bool displayIP);

 private slots:

  void select();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINTABVIEW_HPP
