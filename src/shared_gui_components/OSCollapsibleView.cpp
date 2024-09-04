/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSCollapsibleView.hpp"
#include "OSViewSwitcher.hpp"
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QPushButton>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

OSHeader::OSHeader(QPushButton* button) : QPushButton() {
  this->setFlat(true);
  toggleButton = button;
}

void OSHeader::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

OSCollapsibleView::OSCollapsibleView(bool alwaysExpanded, QWidget* parent) : QWidget(parent), m_alwaysExpanded(alwaysExpanded) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(0);
  setLayout(mainVLayout);

  m_headerContainer = new OSViewSwitcher();
  mainVLayout->addWidget(m_headerContainer);

  m_contentContainer = new OSViewSwitcher();
  mainVLayout->addWidget(m_contentContainer);
  m_contentContainer->hide();
}

void OSCollapsibleView::setHeader(QWidget* header) {
  m_headerContainer->setView(header);

  m_osHeader = qobject_cast<OSHeader*>(header);

  if (m_osHeader) {
    connect(m_osHeader->toggleButton, &QPushButton::toggled, this, &OSCollapsibleView::setExpanded);
  }
}

void OSCollapsibleView::setContent(QWidget* content) {
  m_contentContainer->setView(content);
}

void OSCollapsibleView::expand() {
  setExpanded(true);
}

void OSCollapsibleView::collapse() {
  setExpanded(false);
}

void OSCollapsibleView::setExpanded(bool expanded) {
  if (m_alwaysExpanded || expanded) {
    if (m_osHeader) {
      m_osHeader->toggleButton->blockSignals(true);
      m_osHeader->toggleButton->setChecked(true);
      m_osHeader->toggleButton->blockSignals(false);
    }

    m_contentContainer->show();
  } else {

    if (m_osHeader) {
      m_osHeader->toggleButton->blockSignals(true);
      m_osHeader->toggleButton->setChecked(false);
      m_osHeader->toggleButton->blockSignals(false);
    }

    m_contentContainer->hide();
  }
}

bool OSCollapsibleView::isExpanded() {
  return !m_contentContainer->isHidden();
}

}  // namespace openstudio
