/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GridViewSubTab.hpp"

#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItemSelectorButtons.hpp"

#include <model/ModelObject.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QBoxLayout>
#include <QScrollArea>

namespace openstudio {

GridViewSubTab::GridViewSubTab(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : QWidget(parent),
    m_model(model),
    m_isIP(isIP),
    m_displayAdditionalProps(displayAdditionalProps),
    m_scrollLayout(new QVBoxLayout()),
    m_itemSelectorButtons(new OSItemSelectorButtons()) {

  // ***** Main Layout *****
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);

  // ***** Scroll *****
  m_scrollLayout->setContentsMargins(0, 0, 0, 0);
  m_scrollLayout->setSpacing(0);

  auto* scrollWidget = new QWidget();
  scrollWidget->setObjectName("ScrollWidget");
  scrollWidget->setStyleSheet("QWidget#ScrollWidget { background: transparent; }");
  scrollWidget->setLayout(m_scrollLayout);

  auto* scrollArea = new QScrollArea();
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->setFrameStyle(QFrame::NoFrame);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setWidget(scrollWidget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setBackgroundRole(QPalette::NoRole);
  mainLayout->addWidget(scrollArea);

  // Item Selector Buttons
  m_itemSelectorButtons->hideDropZone();
  mainLayout->addWidget(m_itemSelectorButtons, 0, Qt::AlignBottom);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::addClicked, this, &GridViewSubTab::onAddClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::copyClicked, this, &GridViewSubTab::onCopyClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::removeClicked, this, &GridViewSubTab::onRemoveClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::purgeClicked, this, &GridViewSubTab::onPurgeClicked);

  connect(this, &GridViewSubTab::toggleUnitsClicked, this, &GridViewSubTab::toggleUnits);
  connect(this, &GridViewSubTab::toggleDisplayAdditionalPropsClicked, this, &GridViewSubTab::toggleDisplayAdditionalProps);
}

void GridViewSubTab::setGridView(OSGridView* gridView) {
  m_scrollLayout->addWidget(gridView, 0, Qt::AlignTop);

  connect(gridView, &OSGridView::dropZoneItemClicked, this, &GridViewSubTab::onDropZoneItemClicked);

  connect(gridView, &OSGridView::gridRowSelectionChanged, this, &GridViewSubTab::onGridRowSelectionChanged);

  connect(this, &GridViewSubTab::selectionCleared, m_gridController, &OSGridController::onSelectionCleared);

  connect(this, &GridViewSubTab::toggleUnitsClicked, m_gridController, &OSGridController::toggleUnitsClicked);
  connect(this, &GridViewSubTab::toggleUnitsClicked, m_gridController, &OSGridController::onToggleUnits);

  connect(this, &GridViewSubTab::toggleDisplayAdditionalPropsClicked, m_gridController, &OSGridController::toggleDisplayAdditionalPropsClicked);
  connect(this, &GridViewSubTab::toggleDisplayAdditionalPropsClicked, m_gridController, &OSGridController::onToggleDisplayAdditionalProps);
}

void GridViewSubTab::setGridController(OSGridController* gridController) {
  connect(this, &GridViewSubTab::toggleUnitsClicked, gridController, &OSGridController::onToggleUnits);
  connect(this, &GridViewSubTab::toggleDisplayAdditionalPropsClicked, gridController, &OSGridController::onToggleDisplayAdditionalProps);
}

void GridViewSubTab::onAddClicked() {
  const auto& selObjs = m_gridController->selectedObjects();

  // Always make at least one
  if (selObjs.empty()) {
    addObject(m_gridController->iddObjectType());
  } else {
    for (const auto& obj : selObjs) {
      addObject(obj);
    }
  }
}

void GridViewSubTab::onCopyClicked() {
  const auto& selObjs = m_gridController->selectedObjects();

  for (const auto& obj : selObjs) {
    if (!obj.handle().isNull()) {
      copyObject(obj);
    }
  }
}

void GridViewSubTab::onRemoveClicked() {
  for (const auto& obj : m_gridController->selectedObjects()) {
    removeObject(obj);
  }

  // No selected rows will remain after a delete, so clear selection
  onClearSelection();
}

void GridViewSubTab::onPurgeClicked() {
  purgeObjects(m_gridController->iddObjectType());

  // No selected rows will remain after a delete, so clear selection
  onClearSelection();
}

void GridViewSubTab::addObject(const model::ModelObject& modelObject) {
  addObject(modelObject.iddObjectType());
}

void GridViewSubTab::copyObject(const openstudio::model::ModelObject& modelObject) {
  modelObject.clone(this->m_model);
}

void GridViewSubTab::removeObject(openstudio::model::ModelObject modelObject) {
  modelObject.remove();
}

std::set<model::ModelObject> GridViewSubTab::selectedObjects() const {
  return m_gridController->selectedObjects();
}

void GridViewSubTab::onDropZoneItemClicked(OSItem* item) {
  std::shared_ptr<OSDocument> currentDocument = OSAppBase::instance()->currentDocument();
  if (currentDocument) {
    if (!item) {
      emit dropZoneItemSelected(item, false);
    } else {
      // Note: perhaps passing this here offers more flexibility in the future when determining readOnly
      emit dropZoneItemSelected(item, item->itemId().isDefaulted());
    }
  }
}

void GridViewSubTab::toggleUnits(bool isIP) {
  m_isIP = isIP;
}

void GridViewSubTab::toggleDisplayAdditionalProps(bool displayAdditionalProps) {
  m_displayAdditionalProps = displayAdditionalProps;
}

void GridViewSubTab::onGridRowSelectionChanged(int numSelected, int /*numSelectable*/) {
  if (numSelected == 0) {
    onClearSelection();
  } else {
    onSelectItem();
  }
}

}  // namespace openstudio
