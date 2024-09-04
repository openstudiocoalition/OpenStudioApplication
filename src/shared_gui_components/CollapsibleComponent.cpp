/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "CollapsibleComponent.hpp"
#include "CollapsibleComponentHeader.hpp"
#include "ComponentList.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

namespace openstudio {

CollapsibleComponent::CollapsibleComponent(CollapsibleComponentHeader* collapsibleComponentHeader, ComponentList* componentList, QWidget* parent)
  : QAbstractButton(parent), m_collapsibleComponentHeader(collapsibleComponentHeader), m_componentList(componentList), m_mainLayout(nullptr) {

  OS_ASSERT(m_collapsibleComponentHeader);
  OS_ASSERT(m_componentList);

  setObjectName("CollapsibleComponent");

  m_mainLayout = new QVBoxLayout();
  m_mainLayout->setSpacing(0);
  m_mainLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(m_mainLayout);

  // collapsible header
  m_mainLayout->addWidget(m_collapsibleComponentHeader);

  connect(collapsibleComponentHeader, &CollapsibleComponentHeader::clicked, this, &CollapsibleComponent::headerClicked);

  connect(collapsibleComponentHeader, &CollapsibleComponentHeader::clicked, this, &CollapsibleComponent::on_headerClicked);

  connect(collapsibleComponentHeader, &CollapsibleComponentHeader::getComponentsByPage, this, &CollapsibleComponent::getComponentsByPage);

  connect(collapsibleComponentHeader, &CollapsibleComponentHeader::getComponentsByPage, this, &CollapsibleComponent::on_getComponentsByPage);

  // component list
  m_mainLayout->addWidget(m_componentList);

  connect(componentList, &ComponentList::componentClicked, this, &CollapsibleComponent::componentClicked);

  connect(componentList, &ComponentList::componentClicked, this, &CollapsibleComponent::on_componentClicked);

  m_mainLayout->addStretch();
}

CollapsibleComponentHeader* CollapsibleComponent::collapsibleComponentHeader() const {
  return m_collapsibleComponentHeader;
}

ComponentList* CollapsibleComponent::componentList() const {
  return m_componentList;
}

bool CollapsibleComponent::expanded() const {
  return this->m_collapsibleComponentHeader->isChecked();
}

void CollapsibleComponent::setExpanded(bool expanded) {
  this->m_collapsibleComponentHeader->setChecked(expanded);
  this->m_componentList->setVisible(expanded);
}

void CollapsibleComponent::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CollapsibleComponent::setText(const QString& text) {
  m_collapsibleComponentHeader->setText(text);
}

void CollapsibleComponent::setNumResults(int numResults) {
  m_collapsibleComponentHeader->setNumResults(numResults);
}

void CollapsibleComponent::setNumPages(int numPages) {
  m_collapsibleComponentHeader->setNumPages(numPages);
}

void CollapsibleComponent::firstPage() {
  m_collapsibleComponentHeader->firstPage();
}

///! SLOTS

void CollapsibleComponent::on_headerClicked(bool checked) {
  setExpanded(checked);
}

void CollapsibleComponent::on_componentClicked(bool checked) {}

void CollapsibleComponent::on_getComponentsByPage(int pageIdx) {}

}  // namespace openstudio
