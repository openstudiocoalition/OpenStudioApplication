/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpaceTypesView.hpp"

#include "ModelObjectListView.hpp"
#include "OSItem.hpp"
#include "SpaceTypeInspectorView.hpp"

#include "../openstudio_lib/OSItem.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyleOption>
#include <QVBoxLayout>

#include <sstream>

#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

SpaceTypesView::SpaceTypesView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(new ModelObjectListView(IddObjectType::OS_SpaceType, model, true, false, parent), new SpaceTypeInspectorView(isIP, model, parent),
                    true,  // Note: "true" creates a GridView SubTabView
                    parent) {
  auto* modelObjectListView = qobject_cast<ModelObjectListView*>(this->itemSelector());
  OS_ASSERT(modelObjectListView);
  modelObjectListView->setItemsDraggable(false);

  connect(itemSelector(), &OSItemSelector::selectionCleared, inspectorView(), &OSInspectorView::clearSelection);

  connect(inspectorView(), &OSInspectorView::dropZoneItemClicked, this, &SpaceTypesView::dropZoneItemClicked);
}

}  // namespace openstudio
