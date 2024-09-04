/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTLIST_HPP
#define SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTLIST_HPP

#include "Component.hpp"

class QButtonGroup;
class QHBoxLayout;
class QVBoxLayout;

namespace openstudio {

class Component;
class CollapsibleComponent;
class CollapsibleComponentHeader;

class CollapsibleComponentList : public QWidget
{
  Q_OBJECT

 public:
  explicit CollapsibleComponentList(QWidget* parent = nullptr);
  explicit CollapsibleComponentList(const std::vector<CollapsibleComponent*>& collapsibleComponents, QWidget* parent = nullptr);
  virtual ~CollapsibleComponentList() {}
  // return the checked component in the list, NULL if list is empty
  Component* checkedComponent() const;
  // return the checked CollapsibleComponent in the list, NULL if list is empty
  CollapsibleComponent* checkedCollapsibleComponent() const;
  // return the first CollapsibleComponent in the list, NULL if list is empty
  CollapsibleComponent* firstCollapsibleComponent();
  // return the last CollapsibleComponent in the list, NULL if list is empty
  CollapsibleComponent* lastCollapsibleComponent();
  // return all collapsibleComponents in the list
  std::vector<CollapsibleComponent*> collapsibleComponents();
  // return all components in the list
  std::vector<Component*> components();
  // add a collapsibleComponent
  void addCollapsibleComponent(CollapsibleComponent* collapsibleComponent);
  // set the title text
  void setText(const QString& text);
  // set the page nav's numeric value displayed of BCL results
  void setNumResults(int numResults);
  // set the page nav's numeric value displayed of BCL pages
  void setNumPages(int numPages);
  ///! set the page nav to display first page (pageIdx = 0)
  void firstPage();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void createLayout();
  // set the CollapsibleComponents
  void setCollapsibleComponents(const std::vector<CollapsibleComponent*>& collapsibleComponents);

  QVBoxLayout* m_mainLayout;
  QHBoxLayout* m_contentLayout;
  QButtonGroup* m_collapsibleComponentGroup;

 signals:
  void headerClicked(bool checked);
  void componentClicked(bool checked);
  void collapsibleComponentClicked(bool checked);
  void getComponentsByPage(int pageNum);

 private slots:
  void on_headerClicked(bool checked);
  void on_componentClicked(bool checked);
  void on_collapsibleComponentClicked(bool checked);
  void on_getComponentsByPage(int pageIdx);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTLIST_HPP
