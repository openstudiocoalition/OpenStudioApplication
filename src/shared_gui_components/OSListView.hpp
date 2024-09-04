/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSLISTVIEW_HPP
#define SHAREDGUICOMPONENTS_OSLISTVIEW_HPP

#include "OSListController.hpp"
#include <QWidget>
#include <QSharedPointer>
#include <map>
#include <openstudio/utilities/core/Logger.hpp>

class QScrollArea;
class QVBoxLayout;
class QGraphicsObject;

namespace openstudio {

class OSListItem;
class OSItemDelegate;

/** The purpose of OSListView is to display a list of items in a vertical column.
 *  This class's primary responsibility is to visually arrange a list of things.
 *  Data is provided to OSListView by a subclass of OSListController.
 *  For each item provided by the OSListController, OSListView draws a view provided by OSItemDelegate.
 *  It should be uncommon to subclass OSListView.
 */
class OSListView : public QWidget
{
  Q_OBJECT

 public:
  explicit OSListView(bool scrollable = false, QWidget* parent = nullptr);

  virtual ~OSListView();

  void setDelegate(QSharedPointer<OSItemDelegate> delegate);

  void setListController(QSharedPointer<OSListController> listController);

  QSharedPointer<OSListController> listController() const;

  void setSpacing(int spacing);

  void setContentsMargins(int left, int top, int right, int bottom);

  void setHorizontalScrollBarAlwaysOn(bool alwaysOn);

  void setVerticalScrollBarAlwaysOn(bool alwaysOn);

 public slots:

  void refreshAllViews();

 protected:
  void paintEvent(QPaintEvent*) override;

 private slots:

  void insertItemView(int i);

  void removeItemView(int i);

  void removePair(QObject* object);

  void refreshItemView(int i);

 private:
  QSharedPointer<OSItemDelegate> m_delegate;

  QSharedPointer<OSListController> m_listController;

  QVBoxLayout* m_mainVLayout;

  // Use this to keep the OSListItem classes around for the life of the widget
  std::map<QObject*, QSharedPointer<OSListItem>> m_widgetItemPairs;

  bool m_scrollable;

  QScrollArea* m_scrollArea;

  REGISTER_LOGGER("openstudio.shared_gui_components.OSListView");
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSLISTVIEW_HPP
