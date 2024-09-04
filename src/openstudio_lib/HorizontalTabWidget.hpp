/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_HORIZONTALTABWIDGET_HPP
#define OPENSTUDIO_HORIZONTALTABWIDGET_HPP

#include <QWidget>
#include <vector>

class QStackedWidget;

class QPushButton;

class QPaintEvent;

namespace openstudio {

class HorizontalTabWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit HorizontalTabWidget(QWidget* parent = nullptr);

  virtual ~HorizontalTabWidget() {}

  void addTab(QWidget* widget, int id, const QString& label);

  void hideTab(QWidget* widget, bool hide);

 public slots:

  void setCurrentId(int id);

  void setCurrentWidget(QWidget* widget);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private slots:

  void select();

  void setCurrentIndex(int index);

 private:
  QStackedWidget* m_pageStack;

  QWidget* m_tabBar;

  QWidget* m_tabBarLine;

  std::vector<QString> m_selectedPixmaps;

  std::vector<QString> m_neighborSelectedPixmaps;

  std::vector<QString> m_unSelectedPixmaps;

  std::vector<QPushButton*> m_tabButtons;

  std::vector<int> m_ids;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_HORIZONTALTABWIDGET_HPP
