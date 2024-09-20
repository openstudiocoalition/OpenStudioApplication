/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <openstudio/nano/nano_signal_slot.hpp>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QButtonGroup>
#include <QResizeEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QRadioButton>

#include <iostream>

namespace openstudio {

ModelObjectInspectorView::ModelObjectInspectorView(const openstudio::model::Model& model, bool addScrollArea, QWidget* parent)
  : OSInspectorView(addScrollArea, parent), m_model(model) {
  connect(this, &ModelObjectInspectorView::toggleUnitsClicked, this, &ModelObjectInspectorView::toggleUnits);
}

void ModelObjectInspectorView::update() {
  /** Nano Signal onChange originally modified a slot directly inside
   *  QWidget called update(). This is fine, except for when we need
   *  to automatically disconnect. Now this nano signal calls this slot,
   *  And this slot automatically emits the QSignal that will call
   *  call QWidget::update().
   */
  // this->onChange.nano_emit();
  //emit onChange();
}

void ModelObjectInspectorView::selectModelObject(const openstudio::model::ModelObject& modelObject) {
  if (m_modelObject) {
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onChange.disconnect<ModelObjectInspectorView, &ModelObjectInspectorView::update>(this);
  }

  m_modelObject = modelObject;

  m_modelObject->getImpl<model::detail::ModelObject_Impl>()->onChange.connect<ModelObjectInspectorView, &ModelObjectInspectorView::update>(this);

  onSelectModelObject(*m_modelObject);
}

void ModelObjectInspectorView::onClearSelection() {
  if (m_modelObject) {
    m_modelObject->getImpl<model::detail::ModelObject_Impl>()->onChange.disconnect<ModelObjectInspectorView, &ModelObjectInspectorView::update>(this);
  }

  m_modelObject.reset();
}

void ModelObjectInspectorView::onSelectItem(OSItem* item) {
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
  OS_ASSERT(modelObjectItem);
  selectModelObject(modelObjectItem->modelObject());
}

std::set<model::ModelObject> ModelObjectInspectorView::selectedObjects() const {
  // Default implementation. The tabs with grid views need to do their own thing here
  return std::set<model::ModelObject>();
}

boost::optional<openstudio::model::ModelObject> ModelObjectInspectorView::modelObject() const {
  return m_modelObject;
}

void ModelObjectInspectorView::toggleUnits(bool displayIP) {}

void ModelObjectInspectorView::toggleDisplayAdditionalProps(bool displayAdditionalProps) {}

DefaultInspectorView::DefaultInspectorView(const model::Model& model, QWidget* parent) : ModelObjectInspectorView(model, true, parent) {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->insertWidget(0, hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->insertWidget(1, visibleWidget);

  this->stackedWidget()->setCurrentIndex(0);

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(7, 7, 7, 7);
  mainVLayout->setSpacing(7);

  auto* underConstructionLabel = new QLabel();
  underConstructionLabel->setPixmap(QPixmap(":/images/coming_soon_building_summary.png"));
  underConstructionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mainVLayout->addWidget(underConstructionLabel);

  visibleWidget->setLayout(mainVLayout);

  underConstructionLabel = new QLabel();
  underConstructionLabel->setPixmap(QPixmap(":/images/coming_soon_building_summary.png"));
  underConstructionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  this->stackedWidget()->addWidget(underConstructionLabel);

  m_mainLabel = new QLabel(this);
  m_mainLabel->setWordWrap(true);
  m_mainLabel->setText("Initial");
  m_mainLabel->setAlignment(Qt::AlignHCenter);
  m_mainLabel->setAlignment(Qt::AlignVCenter);

  mainVLayout->addWidget(underConstructionLabel);
  mainVLayout->addWidget(m_mainLabel);
}

void DefaultInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  this->stackedWidget()->setCurrentIndex(0);
}

void DefaultInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  std::stringstream ss;
  ss << modelObject;
  m_mainLabel->setText(toQString(ss.str()));

  this->stackedWidget()->setCurrentIndex(1);
}

void DefaultInspectorView::onUpdate() {
  boost::optional<openstudio::model::ModelObject> modelObject = this->modelObject();
  OS_ASSERT(modelObject);

  std::stringstream ss;
  ss << *modelObject;
  m_mainLabel->setText(toQString(ss.str()));
}

}  // namespace openstudio
