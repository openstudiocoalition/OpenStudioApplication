/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ModelSubTabView.hpp"

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectItem.hpp"
#include "ModelObjectListView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSItemSelector.hpp"
#include "OSItemSelectorButtons.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <iostream>

#include <QButtonGroup>
#include <QCalendarWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

namespace openstudio {

ModelSubTabView::ModelSubTabView(OSItemSelector* itemSelector, ModelObjectInspectorView* modelObjectInspectorView, bool showGridViewLayout,
                                 QWidget* parent)
  : SubTabView(itemSelector, modelObjectInspectorView, showGridViewLayout, parent), m_modelObjectInspectorView(modelObjectInspectorView) {
  connect(this, &ModelSubTabView::dropZoneItemClicked, this, &ModelSubTabView::onDropZoneItemClicked);
  connect(modelObjectInspectorView, &ModelObjectInspectorView::dropZoneItemClicked, this, &ModelSubTabView::dropZoneItemClicked);
}

ModelObjectInspectorView* ModelSubTabView::modelObjectInspectorView() {
  return m_modelObjectInspectorView;
}

void ModelSubTabView::onDropZoneItemClicked(OSItem* item) {
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

}  // namespace openstudio
