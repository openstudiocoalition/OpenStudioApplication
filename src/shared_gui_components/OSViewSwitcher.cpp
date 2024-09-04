/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSViewSwitcher.hpp"

#include <QBoxLayout>
#include <QPainter>
#include <QStackedWidget>
#include <QStyleOption>

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

OSViewSwitcher::OSViewSwitcher(QWidget* parent) : QWidget(parent), m_view(nullptr) {
  // This is weird. A QWidget with a QVBoxLayout that has a QStackedWidget (which is a convenience Widget that exposes a QStackedLayout)
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  m_stack = new QStackedWidget();

  layout->addWidget(m_stack);
}

OSViewSwitcher::~OSViewSwitcher() = default;

void OSViewSwitcher::setView(QWidget* view) {
  // Evan note: It's bad to play with null pointers
  // Unfortunately, the app crashes if you don't them
  if (!view) {
    //return;
  }
  // If the QStackWidget already has a widget, we remove it
  if (QWidget* widget = m_stack->currentWidget()) {
    m_stack->removeWidget(widget);
    // When we determine why there is a crash in Pat frm not
    // using a null pointer, we can again delte these widgets
    //widget->deleteLater();
  }
  OS_ASSERT(m_stack->count() == 0);

  // Store the view
  m_view = view;

  // TODO: this line triggers all of the warnings
  // Warning: QLayout: Attempting to add QLayout "" to openstudio::OSGridView "", which already has a layout ((null):0, (null))
  // qDebug() << m_view << "\n\n";
  // When this happens: m_view => openstudio::SpaceTypesTabView(0x7560610, name="BlueGradientWidget"

  // The QStackedWidget is empty before this function is called, so currentWidget() becomes the current widget.
  // Ownership of widget is passed on to the QStackedWidget.
  m_stack->addWidget(m_view);
}

QWidget* OSViewSwitcher::view() const {
  return m_view;
}

void OSViewSwitcher::clear() {
  if (m_view) {
    m_stack->removeWidget(m_view);
  }

  m_view = nullptr;
}

void OSViewSwitcher::paintEvent(QPaintEvent*) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}  // namespace openstudio
