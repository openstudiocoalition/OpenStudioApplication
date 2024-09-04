/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpaceTypeInspectorView.hpp"

#include "ModelObjectListView.hpp"
#include "SpaceTypesGridView.hpp"

#include <QPainter>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

namespace openstudio {

// SpaceTypeInspectorView

SpaceTypeInspectorView::SpaceTypeInspectorView(bool isIP, const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP) {

  m_gridView = new SpaceTypesGridView(this->m_isIP, this->m_model, this);
  stackedWidget()->addWidget(m_gridView);

  connect(m_gridView, &SpaceTypesGridView::dropZoneItemClicked, this, &SpaceTypeInspectorView::dropZoneItemClicked);

  connect(this, &SpaceTypeInspectorView::selectionCleared, m_gridView, &SpaceTypesGridView::selectionCleared);

  connect(this, &SpaceTypeInspectorView::toggleUnitsClicked, this, &SpaceTypeInspectorView::toggleUnits);

  connect(this, &SpaceTypeInspectorView::toggleUnitsClicked, m_gridView, &SpaceTypesGridView::toggleUnitsClicked);
}

std::set<model::ModelObject> SpaceTypeInspectorView::selectedObjects() const {
  return m_gridView->selectedObjects();
}

void SpaceTypeInspectorView::onClearSelection() {}

void SpaceTypeInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {}

void SpaceTypeInspectorView::onUpdate() {}

void SpaceTypeInspectorView::refresh() {}

void SpaceTypeInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
