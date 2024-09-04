/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SwimmingPoolIndoorFloorSurfaceDialog.hpp"

#include "../model_editor/Utilities.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/model/SwimmingPoolIndoor.hpp>
#include <openstudio/model/SwimmingPoolIndoor_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/ThermalZone.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

namespace openstudio {

SwimmingPoolIndoorFloorSurfaceDialog::SwimmingPoolIndoorFloorSurfaceDialog(model::SwimmingPoolIndoor pool, QWidget* parent)
  : OSDialog(false, parent), m_pool(std::move(pool)), m_model(m_pool.model()) {
  setWindowModality(Qt::ApplicationModal);

  okButton()->setText("Assign Surface");

  auto floorSurfaces = m_model.getConcreteModelObjects<model::Surface>();
  floorSurfaces.erase(std::remove_if(floorSurfaces.begin(), floorSurfaces.end(),
                                     [&](const auto& surface) -> bool { return !istringEqual("Floor", surface.surfaceType()); }),
                      floorSurfaces.end());

  if (floorSurfaces.empty()) {
    okButton()->setDisabled(true);
    auto* labelTitle = new QLabel(
      "SwimmingPool:Indoor requires a floor surface to be assigned.\nYou don't have any, so this object will not be added to your model.", this);
    labelTitle->setObjectName("H1");
    labelTitle->setStyleSheet("QLabel {color: #DD0A05; }");
    upperLayout()->addWidget(labelTitle);
    return;
  }

  auto* labelTitle = new QLabel("Select Floor Surface to assign Swimming Pool to", this);
  labelTitle->setObjectName("H1");
  upperLayout()->addWidget(labelTitle);

  auto* vertLayout = new QVBoxLayout();
  vertLayout->setContentsMargins(20, 10, 10, 10);
  vertLayout->setSpacing(20);
  upperLayout()->addLayout(vertLayout);

  {
    // Floor Surface dropdown
    auto* vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    auto* label = new QLabel("Floor Surface", this);
    label->setObjectName("H2");
    vLayout->addWidget(label);

    auto* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 10, 0);
    hLayout->setSpacing(5);

    m_floorSurfaceComboBox = new QComboBox(this);
    m_floorSurfaceComboBox->setObjectName("SwimmingPoolIndoorFloorSurfaceDialog");

    for (const auto& surface : floorSurfaces) {
      m_floorSurfaceComboBox->addItem(toQString(surface.nameString()), toQString(surface.handle()));
    }

    hLayout->addWidget(m_floorSurfaceComboBox);
    hLayout->addStretch();

    vLayout->addLayout(hLayout);

    vertLayout->addLayout(vLayout);
  }

  {
    // Space Name
    auto* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 10, 0);
    hLayout->setSpacing(5);

    auto* label = new QLabel("Space Name: ", this);
    label->setObjectName("H2");
    hLayout->addWidget(label);

    m_spaceName = new QLabel(this);
    m_spaceName->setObjectName("SwimmingPoolIndoorFloorSurfaceDialog");
    hLayout->addWidget(m_spaceName);
    hLayout->addStretch();

    vertLayout->addLayout(hLayout);
  }

  {
    // Thermal Zone Name
    auto* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 10, 0);
    hLayout->setSpacing(5);

    auto* label = new QLabel("Thermal Zone Name: ", this);
    label->setObjectName("H2");
    hLayout->addWidget(label);

    m_thermalZoneName = new QLabel(this);
    m_thermalZoneName->setObjectName("SwimmingPoolIndoorFloorSurfaceDialog");
    hLayout->addWidget(m_thermalZoneName);
    hLayout->addStretch();

    vertLayout->addLayout(hLayout);
  }

  vertLayout->addStretch();

  m_floorSurfaceComboBox->setCurrentIndex(0);
  onCurrentIndexChanged(0);

  connect(m_floorSurfaceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &SwimmingPoolIndoorFloorSurfaceDialog::onCurrentIndexChanged);
}

void SwimmingPoolIndoorFloorSurfaceDialog::onCurrentIndexChanged(int index) {
  OS_ASSERT(index >= 0);

  const UUID handle = toUUID(m_floorSurfaceComboBox->itemData(index).toString());
  m_floorSurface = m_model.getModelObject<model::Surface>(handle);
  OS_ASSERT(m_floorSurface);

  if (auto space_ = m_floorSurface->space()) {
    m_spaceName->setText(toQString(space_->nameString()));
    if (auto tz_ = space_->thermalZone()) {
      m_thermalZoneName->setText(toQString(tz_->nameString()));
    } else {
      m_thermalZoneName->setText("None");
    }

  } else {
    m_spaceName->setText("None");
    m_thermalZoneName->setText("None");
  }
}

boost::optional<model::Surface> SwimmingPoolIndoorFloorSurfaceDialog::floorSurface() const {
  return m_floorSurface;
}

void SwimmingPoolIndoorFloorSurfaceDialog::on_okButton(bool checked) {

  // In case there are no valid surfaces to assign to, the ok button is disabled anyways, so we can assign here directly
  OS_ASSERT(m_floorSurface);
  m_pool.setSurface(*m_floorSurface);

  OSDialog::on_okButton(checked);
}

}  // namespace openstudio
