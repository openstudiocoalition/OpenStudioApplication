/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "PageNavigator.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QPushButton>

namespace openstudio {

PageNavigator::PageNavigator(int numPages, int numPagesListed, QWidget* parent)
  : QWidget(parent),
    m_numPagesListed(numPagesListed),
    m_numPages(numPages),
    m_checkedTextColor(QColor(Qt::black)),
    m_uncheckedTextColor(QColor(104, 104, 104)),
    m_buttonGroup(nullptr),
    m_buttonLayout(nullptr) {
  if (m_numPages < m_numPagesListed) {
    m_numPagesListed = m_numPages;
  }

  createLayout();
}

PageNavigator::~PageNavigator() = default;

void PageNavigator::createLayout() {
  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(2);

  setLayout(layout);

  m_buttonGroup = new QButtonGroup(this);
  connect(m_buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, &PageNavigator::on_buttonClicked);

  QPushButton* button = nullptr;

  ///! button first page
  button = new QPushButton(this);
  button->setIcon(QIcon(":/images/fast_reverse.png"));
  button->setFixedSize(40, 20);
  button->setCheckable(false);
  connect(button, &QPushButton::clicked, this, &PageNavigator::on_firstPage);
  layout->addWidget(button);

  ///! button previous page
  button = new QPushButton(this);
  button->setIcon(QIcon(":/images/reverse.png"));
  button->setFixedSize(40, 20);
  button->setCheckable(false);
  connect(button, &QPushButton::clicked, this, &PageNavigator::on_previousPage);
  layout->addWidget(button);

  ///! buttons for pages listed
  m_buttonLayout = new QHBoxLayout();
  m_buttonLayout->setContentsMargins(0, 0, 0, 0);
  m_buttonLayout->setSpacing(2);
  layout->addLayout(m_buttonLayout);
  makeNumberedButtons();

  ///! button next page
  button = new QPushButton(this);
  button->setIcon(QIcon(":/images/forward.png"));
  button->setFixedSize(40, 20);
  button->setCheckable(false);
  connect(button, &QPushButton::clicked, this, &PageNavigator::on_nextPage);
  layout->addWidget(button);

  ///! button last page
  button = new QPushButton(this);
  button->setIcon(QIcon(":/images/fast_forward.png"));
  button->setFixedSize(40, 20);
  button->setCheckable(false);
  connect(button, &QPushButton::clicked, this, &PageNavigator::on_lastPage);
  layout->addWidget(button);
}

void PageNavigator::makeNumberedButtons() {
  for (QAbstractButton* button : m_buttonGroup->buttons().toVector()) {
    m_buttonGroup->removeButton(button);
    if (button) {
      delete button;
      button = nullptr;
    }
  }

  int id = 0;
  QString num;
  for (int i = 0; i < m_numPagesListed; ++i) {
    auto* button = new QPushButton();
    button->setText(num.setNum(i + 1));
    button->setFixedSize(40, 20);
    button->setCheckable(true);
    m_buttonLayout->addWidget(button);
    m_buttonGroup->addButton(button);
    m_buttonGroup->setId(button, id++);
  }
  if (m_buttonGroup->button(0)) {
    m_buttonGroup->button(0)->setChecked(true);
  }
  OS_ASSERT(m_numPagesListed == m_buttonGroup->buttons().size());
}

void PageNavigator::setButtonPageNumbers(int lowestPageNum) {
  OS_ASSERT(lowestPageNum > 0 && lowestPageNum <= m_numPages);
  QString num;
  for (int i = 0; i < m_numPagesListed; ++i) {
    QAbstractButton* button = m_buttonGroup->button(i);
    button->setText(num.setNum(lowestPageNum + i));
    if (i == m_numPages) {
      break;
    }
  }
}

void PageNavigator::firstPage() {
  on_firstPage(true);
}

void PageNavigator::setNumPages(int numPages) {
  m_numPages = numPages;
  setNumPagesListed();
}

void PageNavigator::setNumPagesListed(int numPagesListed) {
  m_numPagesListed = numPagesListed;

  // check that the correct number of pages are listed
  if (m_numPages < m_numPagesListed) {
    m_numPagesListed = m_numPages;
  }

  // adjust the number of buttons used, if need be
  if (m_buttonGroup->buttons().size() != m_numPagesListed) {
    makeNumberedButtons();
  }
}

///! Slots

void PageNavigator::on_buttonClicked(QAbstractButton* button) {
  /// Get the new page of components (idx zero)
  emit getComponentsByPage(button->text().toInt() - 1);
}

void PageNavigator::on_firstPage(bool checked) {
  // Verify that there are pages to work with
  if (m_numPagesListed == 0) {
    return;
  }

  /// for efficiency, check if the first page is already being displayed
  if (m_buttonGroup->checkedButton() && m_buttonGroup->checkedButton()->text().toInt() == 1) {
    return;
  }

  /// Get the page number held by the first button
  if (m_buttonGroup->button(0)) {
    int firstPageNum = m_buttonGroup->button(0)->text().toInt();

    /// page num = 1, page idx = 0
    m_buttonGroup->button(0)->setChecked(true);

    /// for efficiency, check if the first page nums are already being displayed
    if (firstPageNum != 1) {
      setButtonPageNumbers(1);
    }

    emit getComponentsByPage(0);
  }
}

void PageNavigator::on_previousPage(bool checked) {
  // Verify that there are pages to work with
  if (m_numPagesListed == 0) {
    return;
  }

  /// If the first button of the group is selected...
  if (m_buttonGroup->checkedId() == 0) {
    ///...and if it can still be decreased...
    if (m_buttonGroup->button(0)->text().toInt() > 1) {
      /// ...subtract 1 from the page numbers listed...
      setButtonPageNumbers(m_buttonGroup->button(0)->text().toInt() - 1);
      /// ...and get the new page of components (idx zero)...
      emit getComponentsByPage(m_buttonGroup->checkedButton()->text().toInt() - 1);
    }
    /// ...and be done
    return;
  }

  /// If greater than the first button of the group is selected...
  if (m_buttonGroup->checkedId() > 0) {
    /// ...decrease the selected button...
    m_buttonGroup->button(m_buttonGroup->checkedId() - 1)->setChecked(true);
    /// ...and get the new page of components (idx zero)...
    emit getComponentsByPage(m_buttonGroup->checkedButton()->text().toInt() - 1);
    /// ...and be done
    return;
  }

  /// You should never get here
  OS_ASSERT(false);
  return;
}

void PageNavigator::on_nextPage(bool checked) {
  // Verify that there are pages to work with
  if (m_numPagesListed == 0) {
    return;
  }

  /// If the last button of the group is selected...
  if (m_buttonGroup->checkedId() == m_numPagesListed - 1) {
    ///...and if it can still be increased...
    if (m_buttonGroup->button(m_numPagesListed - 1)->text().toInt() < m_numPages) {
      /// ...add 1 to the page numbers listed...
      setButtonPageNumbers(m_buttonGroup->button(1)->text().toInt());
      /// ...and get the new page of components (idx zero)...
      emit getComponentsByPage(m_buttonGroup->checkedButton()->text().toInt() - 1);
    }
    /// ...and be done
    return;
  }

  /// If less than the last button of the group is selected...
  if (m_buttonGroup->checkedId() < m_numPagesListed - 1) {
    /// ...advance the selected button...
    m_buttonGroup->button(m_buttonGroup->checkedId() + 1)->setChecked(true);
    /// ...and get the new page of components (idx zero)...
    emit getComponentsByPage(m_buttonGroup->checkedButton()->text().toInt() - 1);
    /// ...and be done
    return;
  }

  /// You should never get here
  OS_ASSERT(false);
  return;
}

void PageNavigator::on_lastPage(bool checked) {
  // Verify that there are pages to work with
  if (m_numPagesListed == 0) {
    return;
  }

  /// for efficiency, check if the last page is already being displayed
  if (m_buttonGroup->checkedButton() && m_buttonGroup->checkedButton()->text().toInt() == m_numPages) {
    return;
  }

  /// Get the page number held by the last button
  if (m_buttonGroup->button(m_numPagesListed - 1)) {
    int lastPageNum = m_buttonGroup->button(m_numPagesListed - 1)->text().toInt();

    /// page num = m_numPages, page idx = m_numPages - 1
    m_buttonGroup->button(m_numPagesListed - 1)->setChecked(true);

    /// for efficiency, check if the last page nums are already being displayed
    if (lastPageNum != m_numPages) {
      setButtonPageNumbers(m_numPages - m_numPagesListed + 1);
    }

    emit getComponentsByPage(m_numPages - 1);
  }
}

}  // namespace openstudio
