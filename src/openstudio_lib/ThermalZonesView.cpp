/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ThermalZonesView.hpp"

#include "ModelObjectListView.hpp"
#include "ThermalZonesGridView.hpp"

#include <QPainter>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

ThermalZonesView::ThermalZonesView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : ModelSubTabView(new ModelObjectListView(IddObjectType::OS_ThermalZone, model, true, false, parent),
                    new ThermalZoneView(isIP, displayAdditionalProps, model, parent), true, parent) {

  connect(itemSelector(), &OSItemSelector::selectionCleared, inspectorView(), &OSInspectorView::clearSelection);

  connect(inspectorView(), &OSInspectorView::dropZoneItemClicked, this, &ThermalZonesView::dropZoneItemClicked);
}

ThermalZoneView::ThermalZoneView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP), m_displayAdditionalProps(displayAdditionalProps) {

  m_thermalZonesGridView = new ThermalZonesGridView(this->m_isIP, this->m_displayAdditionalProps, this->m_model, this);
  this->stackedWidget()->addWidget(m_thermalZonesGridView);

  connect(m_thermalZonesGridView, &ThermalZonesGridView::dropZoneItemClicked, this, &ThermalZoneView::dropZoneItemClicked);

  connect(this, &ThermalZoneView::selectionCleared, m_thermalZonesGridView, &ThermalZonesGridView::selectionCleared);

  connect(this, &ThermalZoneView::toggleUnitsClicked, this, &ThermalZoneView::toggleUnits);

  connect(this, &ThermalZoneView::toggleUnitsClicked, m_thermalZonesGridView, &ThermalZonesGridView::toggleUnitsClicked);

  connect(this, &ThermalZoneView::toggleDisplayAdditionalPropsClicked, this, &ThermalZoneView::toggleDisplayAdditionalProps);

  connect(this, &ThermalZoneView::toggleDisplayAdditionalPropsClicked, m_thermalZonesGridView,
          &ThermalZonesGridView::toggleDisplayAdditionalPropsClicked);

  refresh();
}

std::set<model::ModelObject> ThermalZoneView::selectedObjects() const {
  return m_thermalZonesGridView->selectedObjects();
}

void ThermalZoneView::onClearSelection() {}

void ThermalZoneView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {}

void ThermalZoneView::onUpdate() {}

void ThermalZoneView::refresh() {}

void ThermalZoneView::toggleUnits(bool isIP) {
  m_isIP = isIP;
}

void ThermalZoneView::toggleDisplayAdditionalProps(bool displayAdditionalProps) {
  m_displayAdditionalProps = displayAdditionalProps;
}

}  // namespace openstudio
