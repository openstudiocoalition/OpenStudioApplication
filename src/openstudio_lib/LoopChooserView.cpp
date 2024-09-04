/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoopChooserView.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/WaterToAirComponent.hpp>
#include <openstudio/model/WaterToAirComponent_Impl.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/HVACComponent_Impl.hpp>
#include <openstudio/model/Loop.hpp>
#include <openstudio/model/Loop_Impl.hpp>
#include <openstudio/model/PlantLoop.hpp>
#include <openstudio/model/PlantLoop_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QScrollArea>

namespace openstudio {

LoopChooserView::LoopChooserView(QWidget* parent) : QWidget(parent) {
  auto* mainLayout = new QVBoxLayout();

  mainLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(mainLayout);

  auto* scrollArea = new QScrollArea();

  scrollArea->setWidgetResizable(true);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  mainLayout->addWidget(scrollArea);

  auto* scrollWidget = new QWidget();

  scrollArea->setWidget(scrollWidget);

  m_vLayout = new QVBoxLayout();

  m_vLayout->setContentsMargins(0, 0, 0, 0);

  scrollWidget->setLayout(m_vLayout);
}

void LoopChooserView::layoutModelObject(boost::optional<model::ModelObject>& modelObject) {
  m_component = boost::none;

  if (modelObject) {
    if (boost::optional<model::HVACComponent> comp = modelObject->optionalCast<model::HVACComponent>()) {
      m_component = comp;
    }
  }

  layoutView();
}

void LoopChooserView::layoutView() {
  this->setUpdatesEnabled(false);

  QLayoutItem* child;
  while ((child = m_vLayout->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }
  m_loopChooserItems.clear();

  if (m_component) {
    auto* label = new QLabel();
    label->setObjectName("IGHeader");
    label->setText(toQString(m_component->iddObject().name()));
    m_vLayout->addWidget(label);

    std::vector<model::PlantLoop> loops;
    loops = m_component->model().getConcreteModelObjects<model::PlantLoop>();

    for (auto it = loops.begin(); it < loops.end(); ++it) {
      auto* loopChooserItem = new LoopChooserItem(*it, this);
      m_loopChooserItems.push_back(loopChooserItem);
      m_vLayout->addWidget(loopChooserItem);
      loopChooserItem->setChecked(false);
    }
    m_vLayout->addStretch();

    boost::optional<model::PlantLoop> plantLoop;

    plantLoop = m_component->plantLoop();

    if (plantLoop) {
      loopChooserItemForLoop(plantLoop->name().get())->setChecked(true);
    }
  }

  for (int i = 0; i < m_vLayout->count(); i++) {
    if (QWidget* widget = m_vLayout->itemAt(i)->widget()) {
      widget->show();
    }
  }

  this->setUpdatesEnabled(true);
}

void LoopChooserView::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

LoopChooserItem* LoopChooserView::loopChooserItemForLoop(const std::string& loopName) {
  for (auto it = m_loopChooserItems.begin(); it < m_loopChooserItems.end(); ++it) {
    if ((*it)->loopName() == loopName) {
      return *it;
    }
  }
  return nullptr;
}

void LoopChooserView::onAddToLoopClicked(model::Loop& loop) {
  emit addToLoopClicked(loop, m_component);
}

void LoopChooserView::onRemoveFromLoopClicked(model::Loop& loop) {
  emit removeFromLoopClicked(loop, m_component);
}

LoopChooserItem::LoopChooserItem(model::Loop& loop, LoopChooserView* parent) : QWidget(parent), m_loopChooserView(parent), m_loop(loop) {
  auto* hLayout = new QHBoxLayout();

  m_checkBox = new QCheckBox();

  m_checkBox->setText(toQString(loop.name().get()));

  connect(m_checkBox, &QCheckBox::clicked, this, &LoopChooserItem::sendClickedSignal);
  connect(this, &LoopChooserItem::addToLoopClicked, m_loopChooserView, &LoopChooserView::onAddToLoopClicked);
  connect(this, &LoopChooserItem::removeFromLoopClicked, m_loopChooserView, &LoopChooserView::onRemoveFromLoopClicked);
  hLayout->addWidget(m_checkBox);

  setLayout(hLayout);
}

void LoopChooserItem::sendClickedSignal(bool checked) {
  if (checked) {
    emit addToLoopClicked(m_loop);

    m_loopChooserView->layoutView();
  } else {
    emit removeFromLoopClicked(m_loop);

    m_loopChooserView->layoutView();
  }
}

std::string LoopChooserItem::loopName() {
  return toString(m_checkBox->text());
}

void LoopChooserItem::setChecked(bool checked) {
  m_checkBox->setChecked(checked);
}

}  // namespace openstudio
