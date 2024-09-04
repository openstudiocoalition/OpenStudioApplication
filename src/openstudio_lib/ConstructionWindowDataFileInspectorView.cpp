/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ConstructionWindowDataFileInspectorView.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"

#include <openstudio/model/WindowDataFile.hpp>
#include <openstudio/model/WindowDataFile_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// ConstructionVC

ConstructionWindowDataFileInspectorView::ConstructionWindowDataFileInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, parent), m_isIP(isIP), m_nameEdit(nullptr), m_urlEdit(nullptr) {
  createLayout();
}

void ConstructionWindowDataFileInspectorView::createLayout() {
  auto* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  auto* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  // URL

  auto* label = new QLabel("URL: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  m_urlEdit = new OSLineEdit2();
  mainGridLayout->addWidget(m_urlEdit, 3, 0, 1, 3);

  // Stretch

  mainGridLayout->setRowStretch(100, 100);

  mainGridLayout->setColumnStretch(100, 100);
}

void ConstructionWindowDataFileInspectorView::onClearSelection() {
  detach();

  this->stackedWidget()->setCurrentIndex(0);
}

void ConstructionWindowDataFileInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  auto windowDataFile = modelObject.cast<model::WindowDataFile>();
  attach(windowDataFile);

  this->stackedWidget()->setCurrentIndex(1);
}

void ConstructionWindowDataFileInspectorView::onUpdate() {}

void ConstructionWindowDataFileInspectorView::attach(const openstudio::model::WindowDataFile& windowDataFile) {
  m_windowDataFile = windowDataFile;
  // m_nameEdit->bind(windowDataFile,"name");
  m_nameEdit->bind(
    *m_windowDataFile, OptionalStringGetter(std::bind(&model::WindowDataFile::name, m_windowDataFile.get_ptr(), true)),
    boost::optional<StringSetterOptionalStringReturn>(std::bind(&model::WindowDataFile::setName, m_windowDataFile.get_ptr(), std::placeholders::_1)));

  // m_urlEdit->bind(windowDataFile,"url");
  // Replaced to upgrade to OSLineEdit2, but fully removed at the suggestion of @danmacumber

  // m_urlEdit->bind(
  //   *m_windowDataFile,
  //   OptionalStringGetter(std::bind(&model::WindowDataFile::getURL, m_windowDataFile.get_ptr(),true)),
  //   boost::optional<StringSetter>(std::bind(&model::WindowDataFile::setURL, m_windowDataFile.get_ptr(),std::placeholders::_1))
  // );
}

void ConstructionWindowDataFileInspectorView::detach() {
  m_urlEdit->unbind();
  m_windowDataFile = boost::none;
}

}  // namespace openstudio
