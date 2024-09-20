/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COMPONENTLIST_HPP
#define SHAREDGUICOMPONENTS_COMPONENTLIST_HPP

#include <QWidget>

class QVBoxLayout;
class QButtonGroup;

namespace openstudio {

class Component;

class ComponentList : public QWidget
{

  Q_OBJECT

 public:
  explicit ComponentList(QWidget* parent = nullptr);
  explicit ComponentList(const std::vector<Component*>& components, QWidget* parent = nullptr);
  virtual ~ComponentList() {}
  // return the checked component in the list, NULL if list is empty
  Component* checkedComponent() const;
  // return the first component in the list, NULL if list is empty
  Component* firstComponent();
  // return the last component in the list, NULL if list is empty
  Component* lastComponent();
  // return all components in the list
  std::vector<Component*> components();
  // add a component
  void addComponent(Component* component);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void createLayout();
  // set the components
  void setComponents(const std::vector<Component*>& components);

  QVBoxLayout* m_mainLayout;
  QButtonGroup* m_componentGroup;

 signals:
  void componentClicked(bool checked);

 private slots:
  void on_componentClicked(bool);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COMPONENTLIST_HPP
