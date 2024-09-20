/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSCOLLAPSIBLEVIEW_HPP
#define SHAREDGUICOMPONENTS_OSCOLLAPSIBLEVIEW_HPP

#include <QWidget>
#include <QPointer>
#include <QPushButton>

namespace openstudio {

class OSViewSwitcher;

// OSHeader is an empty QWidget that must be subclassed to show something interesting.
// The purpose of this class is to ensure that there is a toggle button to support OSCollapsibleView.
// It is the responsibility of the subclass to insert the toggle button into a layout.
class OSHeader : public QPushButton
{
  Q_OBJECT

 public:
  explicit OSHeader(QPushButton* button);

  virtual ~OSHeader() {}

  QPushButton* toggleButton;

 protected:
  void paintEvent(QPaintEvent*) override;
};

class OSCollapsibleView : public QWidget
{
  Q_OBJECT

 public:
  explicit OSCollapsibleView(bool alwaysExpanded = false, QWidget* parent = nullptr);

  virtual ~OSCollapsibleView() {}

  void setHeader(QWidget* header);

  void setContent(QWidget* content);

 public slots:

  void expand();

  void collapse();

  void setExpanded(bool expanded);

  bool isExpanded();

 private:
  QPointer<OSHeader> m_osHeader;

  OSViewSwitcher* m_headerContainer;

  OSViewSwitcher* m_contentContainer;

  bool m_alwaysExpanded;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSCOLLAPSIBLEVIEW_HPP
