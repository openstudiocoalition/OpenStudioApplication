/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "FloorSurfaceDialog.hpp"

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

FloorSurfaceDialog::FloorSurfaceDialog(model::SwimmingPoolIndoor pool, QWidget* parent)
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
    m_floorSurfaceComboBox->setObjectName("FloorSurfaceDialog");

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
    m_spaceName->setObjectName("FloorSurfaceDialog");
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
    m_thermalZoneName->setObjectName("FloorSurfaceDialog");
    hLayout->addWidget(m_thermalZoneName);
    hLayout->addStretch();

    vertLayout->addLayout(hLayout);
  }

  vertLayout->addStretch();

  m_floorSurfaceComboBox->setCurrentIndex(0);
  onCurrentIndexChanged(0);

  connect(m_floorSurfaceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &FloorSurfaceDialog::onCurrentIndexChanged);
}

void FloorSurfaceDialog::onCurrentIndexChanged(int index) {
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

boost::optional<model::Surface> FloorSurfaceDialog::floorSurface() const {
  return m_floorSurface;
}

void FloorSurfaceDialog::on_okButton(bool checked) {

  // In case there are no valid surfaces to assign to, the ok button is disabled anyways, so we can assign here directly
  OS_ASSERT(m_floorSurface);
  m_pool.setSurface(*m_floorSurface);

  OSDialog::on_okButton(checked);
}

}  // namespace openstudio
