/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "HVACTemplateHelperDialog.hpp"
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>

namespace openstudio {

HVACTemplateHelperDialog::HVACTemplateHelperDialog(const model::Model& model, QWidget* parent) : QDialog(parent), m_model(model) {
  setMinimumSize(200, 200);

  setSizeGripEnabled(false);

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setSpacing(20);
  setLayout(mainVLayout);

  auto* message = new QLabel("Select zones to apply system to.");
  mainVLayout->addWidget(message);

  auto* divider1 = new QFrame();
  divider1->setFrameShape(QFrame::HLine);
  divider1->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(divider1);

  auto* scrollArea = new QScrollArea();
  mainVLayout->addWidget(scrollArea);

  auto* scrollWidget = new QWidget();
  scrollArea->setWidget(scrollWidget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  auto* scrollLayout = new QVBoxLayout();
  scrollLayout->setSpacing(20);
  scrollWidget->setLayout(scrollLayout);

  m_buttonGroup = new QButtonGroup();
  m_buttonGroup->setExclusive(false);

  m_zones = m_model.getConcreteModelObjects<model::ThermalZone>();

  int i = 1;

  for (auto it = m_zones.begin(); it < m_zones.end(); ++it) {
    auto* checkBox = new QCheckBox();

    checkBox->setText(QString::fromStdString(it->name().get()));

    m_buttonGroup->addButton(checkBox, i);

    scrollLayout->addWidget(checkBox);

    i++;
  }

  auto* divider2 = new QFrame();
  divider2->setFrameShape(QFrame::HLine);
  divider2->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(divider2);

  mainVLayout->addStretch();

  auto* hLayout = new QHBoxLayout();
  mainVLayout->addLayout(hLayout);

  hLayout->addStretch();

  auto* cancelButton = new QPushButton();
  cancelButton->setText("Cancel");
  hLayout->addWidget(cancelButton);
  connect(cancelButton, &QPushButton::clicked, this, &HVACTemplateHelperDialog::reject);

  auto* okButton = new QPushButton();
  okButton->setText("OK");
  hLayout->addWidget(okButton);
  connect(okButton, &QPushButton::clicked, this, &HVACTemplateHelperDialog::accept);
}

std::vector<model::ThermalZone> HVACTemplateHelperDialog::selectedZones() {
  std::vector<model::ThermalZone> result;

  int i = 1;

  for (auto it = m_zones.begin(); it < m_zones.end(); ++it) {
    auto* checkBox = qobject_cast<QCheckBox*>(m_buttonGroup->button(i));

    if (checkBox->isChecked()) {
      result.push_back(*it);
    }

    i++;
  }

  return result;
}

}  // namespace openstudio
