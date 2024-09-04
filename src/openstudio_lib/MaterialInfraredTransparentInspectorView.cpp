/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MaterialInfraredTransparentInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/InfraredTransparentMaterial.hpp>
#include <openstudio/model/InfraredTransparentMaterial_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// MaterialInfraredTransparentInspectorView

MaterialInfraredTransparentInspectorView::MaterialInfraredTransparentInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP) {
  createLayout();
}

void MaterialInfraredTransparentInspectorView::createLayout() {
  auto* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();

  QLabel* label = nullptr;

  // Name

  label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_nameEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 3);

  ++row;

  // Standards Information

  m_standardsInformationWidget = new StandardsInformationMaterialWidget(m_isIP, mainGridLayout, row);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void MaterialInfraredTransparentInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void MaterialInfraredTransparentInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto infraredTransparentMaterial = modelObject.cast<model::InfraredTransparentMaterial>();
  attach(infraredTransparentMaterial);
  refresh();
}

void MaterialInfraredTransparentInspectorView::onUpdate() {
  refresh();
}

void MaterialInfraredTransparentInspectorView::attach(openstudio::model::InfraredTransparentMaterial& infraredTransparentMaterial) {
  m_infraredTransparentMaterial = infraredTransparentMaterial;

  // m_nameEdit->bind(infraredTransparentMaterial,"name");
  m_nameEdit->bind(*m_infraredTransparentMaterial,
                   OptionalStringGetter(std::bind(&model::InfraredTransparentMaterial::name, m_infraredTransparentMaterial.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::InfraredTransparentMaterial::setName, m_infraredTransparentMaterial.get_ptr(), std::placeholders::_1)));

  m_standardsInformationWidget->attach(infraredTransparentMaterial);

  this->stackedWidget()->setCurrentIndex(1);
}

void MaterialInfraredTransparentInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();

  m_infraredTransparentMaterial = boost::none;

  m_standardsInformationWidget->detach();
}

void MaterialInfraredTransparentInspectorView::refresh() {}

}  // namespace openstudio
