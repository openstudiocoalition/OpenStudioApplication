/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENT_HPP
#define SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENT_HPP

#include <QAbstractButton>

#include "Component.hpp"

class QVBoxLayout;

namespace openstudio {

class CollapsibleComponentHeader;
class ComponentList;

class CollapsibleComponent : public QAbstractButton
{
  Q_OBJECT

 public:
  CollapsibleComponent(CollapsibleComponentHeader* collapsibleComponentHeader, ComponentList* componentList, QWidget* parent = nullptr);
  virtual ~CollapsibleComponent() {}
  bool expanded() const;
  void setExpanded(bool expanded);
  CollapsibleComponentHeader* collapsibleComponentHeader() const;
  ComponentList* componentList() const;
  void setText(const QString& text);
  void setNumResults(int numResults);
  void setNumPages(int numPages);
  void firstPage();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void createLayout();

  CollapsibleComponentHeader* m_collapsibleComponentHeader;
  ComponentList* m_componentList;
  QVBoxLayout* m_mainLayout;

 signals:
  void headerClicked(bool);
  void componentClicked(bool);
  void getComponentsByPage(int pageNum);

 private slots:
  void on_headerClicked(bool checked);
  void on_componentClicked(bool checked);
  void on_getComponentsByPage(int pageIdx);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENT_HPP
