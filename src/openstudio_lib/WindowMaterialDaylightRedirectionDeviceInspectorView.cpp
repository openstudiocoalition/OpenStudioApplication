/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "WindowMaterialDaylightRedirectionDeviceInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/DaylightRedirectionDevice.hpp>
#include <openstudio/model/DaylightRedirectionDevice_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// WindowMaterialDaylightRedirectionDeviceInspectorView

WindowMaterialDaylightRedirectionDeviceInspectorView::WindowMaterialDaylightRedirectionDeviceInspectorView(bool isIP,
                                                                                                           const openstudio::model::Model& model,
                                                                                                           QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP) {
  createLayout();
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::createLayout() {
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

  // Daylight Redirection Device Type

  label = new QLabel("Daylight Redirection Device Type: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_daylightRedirectionDeviceType = new OSComboBox2();
  mainGridLayout->addWidget(m_daylightRedirectionDeviceType, row, 0, 1, 3);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();  // call parent implementation
  detach();
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto material = modelObject.cast<model::DaylightRedirectionDevice>();
  attach(material);
  refresh();
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::onUpdate() {
  refresh();
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::attach(openstudio::model::DaylightRedirectionDevice& daylightRedirectionDevice) {
  m_material = daylightRedirectionDevice;

  m_nameEdit->bind(*m_material, OptionalStringGetter(std::bind(&model::DaylightRedirectionDevice::name, m_material.get_ptr(), true)),
                   boost::optional<StringSetterOptionalStringReturn>(
                     std::bind(&model::DaylightRedirectionDevice::setName, m_material.get_ptr(), std::placeholders::_1)));

  m_daylightRedirectionDeviceType->bind<std::string>(
    *m_material, static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
    std::bind(&openstudio::model::DaylightRedirectionDevice::daylightRedirectionDeviceTypeValues),
    StringGetter(std::bind(&openstudio::model::DaylightRedirectionDevice::daylightRedirectionDeviceType, m_material.get_ptr())),
    StringSetter(
      std::bind(&openstudio::model::DaylightRedirectionDevice::setDaylightRedirectionDeviceType, m_material.get_ptr(), std::placeholders::_1)),
    NoFailAction(std::bind(&model::DaylightRedirectionDevice::resetDaylightRedirectionDeviceType, m_material.get_ptr())));

  m_standardsInformationWidget->attach(daylightRedirectionDevice);

  this->stackedWidget()->setCurrentIndex(1);
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::detach() {
  m_nameEdit->unbind();
  m_daylightRedirectionDeviceType->unbind();

  this->stackedWidget()->setCurrentIndex(0);

  m_standardsInformationWidget->detach();

  m_material.reset();
}

void WindowMaterialDaylightRedirectionDeviceInspectorView::refresh() {}

}  // namespace openstudio
