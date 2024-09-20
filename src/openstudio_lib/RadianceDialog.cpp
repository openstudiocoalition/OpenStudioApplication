/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "RadianceDialog.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace openstudio {

RadianceDialog::RadianceDialog(bool isIP, QWidget* radianceInspector, QWidget* parent) : OSDialog(parent), m_radianceInspector(radianceInspector) {
  setWindowModality(Qt::ApplicationModal);
  createLayoutInternal();
}

void RadianceDialog::createLayout() {
  createLayoutInternal();
}

void RadianceDialog::createLayoutInternal() {
  okButton()->setText("Apply");

  QLabel* label = nullptr;

  label = new QLabel("Custom Radiance Parameter Settings", this);
  label->setObjectName("H1");
  upperLayout()->addWidget(label);

  auto* vertLayout = new QVBoxLayout();
  vertLayout->setContentsMargins(20, 10, 10, 10);
  vertLayout->setSpacing(20);
  upperLayout()->addLayout(vertLayout);

  vertLayout->addWidget(m_radianceInspector);

  vertLayout->addStretch();
}

// ***** SLOTS *****

void RadianceDialog::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
