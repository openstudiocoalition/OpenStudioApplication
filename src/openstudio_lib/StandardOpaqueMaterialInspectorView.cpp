/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "StandardOpaqueMaterialInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <sstream>

namespace openstudio {

StandardOpaqueMaterialInspectorView::StandardOpaqueMaterialInspectorView(const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent) {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->insertWidget(0, hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->insertWidget(1, visibleWidget);

  this->stackedWidget()->setCurrentIndex(0);

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 1);
  mainVLayout->setSpacing(0);
  visibleWidget->setLayout(mainVLayout);

  m_mainLabel = new QLabel(this);
  m_mainLabel->setWordWrap(true);
  m_mainLabel->setText("StandardOpaqueMaterialInspectorView: Initial");
  m_mainLabel->setAlignment(Qt::AlignHCenter);
  m_mainLabel->setAlignment(Qt::AlignVCenter);
  mainVLayout->addWidget(m_mainLabel);
}

void StandardOpaqueMaterialInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  this->stackedWidget()->setCurrentIndex(0);
}

void StandardOpaqueMaterialInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  std::stringstream ss;
  ss << "StandardOpaqueMaterialInspectorView:" << std::endl;
  ss << modelObject;
  m_mainLabel->setText(toQString(ss.str()));

  this->stackedWidget()->setCurrentIndex(1);
}

void StandardOpaqueMaterialInspectorView::onUpdate() {
  boost::optional<openstudio::model::ModelObject> modelObject = this->modelObject();
  OS_ASSERT(modelObject);

  std::stringstream ss;
  ss << "StandardOpaqueMaterialInspectorView:" << std::endl;
  ss << *modelObject;
  m_mainLabel->setText(toQString(ss.str()));
}

}  // namespace openstudio
