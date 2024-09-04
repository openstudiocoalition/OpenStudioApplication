/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelObjectTreeWidget.hpp"
#include "ModelObjectTreeItems.hpp"
#include "OSAppBase.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QPainter>
#include <QTimer>

namespace openstudio {

ModelObjectTreeWidget::ModelObjectTreeWidget(const model::Model& model, QWidget* parent) : OSItemSelector(parent), m_model(model) {
  m_vLayout = new QVBoxLayout();
  m_vLayout->setContentsMargins(0, 7, 0, 0);
  m_vLayout->setSpacing(7);
  setLayout(m_vLayout);

  m_treeWidget = new QTreeWidget(parent);
  m_treeWidget->setStyleSheet("QTreeWidget { border: none; border-top: 1px solid black; }");
  m_treeWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);

  m_vLayout->addWidget(m_treeWidget);

  // model.getImpl<model::detail::Model_Impl>().get()->addWorkspaceObjectPtr.connect<ModelObjectTreeWidget, &ModelObjectTreeWidget::objectAdded>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectAddedPtr, this, &ModelObjectTreeWidget::objectAdded, Qt::QueuedConnection);

  //model.getImpl<model::detail::Model_Impl>().get()->removeWorkspaceObjectPtr.connect<ModelObjectTreeWidget, &ModelObjectTreeWidget::objectRemoved>(this);
  connect(OSAppBase::instance(), &OSAppBase::workspaceObjectRemovedPtr, this, &ModelObjectTreeWidget::objectRemoved, Qt::QueuedConnection);
}

OSItem* ModelObjectTreeWidget::selectedItem() const {
  // todo: something
  return nullptr;
}

QTreeWidget* ModelObjectTreeWidget::treeWidget() const {
  return m_treeWidget;
}

QVBoxLayout* ModelObjectTreeWidget::vLayout() const {
  return m_vLayout;
}

openstudio::model::Model ModelObjectTreeWidget::model() const {
  return m_model;
}

void ModelObjectTreeWidget::objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                        const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  onObjectAdded(impl->getObject<model::ModelObject>(), iddObjectType, handle);
}

void ModelObjectTreeWidget::objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl,
                                          const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) {
  onObjectRemoved(impl->getObject<model::ModelObject>(), iddObjectType, handle);
}

void ModelObjectTreeWidget::refresh() {
  int N = m_treeWidget->topLevelItemCount();
  for (int i = 0; i < N; ++i) {
    QTreeWidgetItem* treeItem = m_treeWidget->topLevelItem(i);
    auto* modelObjectTreeItem = dynamic_cast<ModelObjectTreeItem*>(treeItem);
    if (modelObjectTreeItem) {
      if (!modelObjectTreeItem->isDirty()) {
        modelObjectTreeItem->makeDirty();
        QTimer::singleShot(0, modelObjectTreeItem, &ModelObjectTreeItem::refresh);
      }
    }
  }
}

}  // namespace openstudio
