/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BCLComponentItem.hpp"

#include "OSItem.hpp"

#include "../shared_gui_components/MeasureBadge.hpp"

#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>

#include <QLabel>

namespace openstudio {

OSItemId bclComponentToItemId(const openstudio::BCLComponent& component) {
  OSItemId itemId(QString::fromStdString(component.uid()), OSItemId::BCL_SOURCE_ID, false);

  return itemId;
}

BCLComponentItem::BCLComponentItem(const BCLComponent& component, OSItemType type, QWidget* parent)
  : OSItem(bclComponentToItemId(component), type, parent), m_bclComponent(component) {
  setText(QString::fromStdString(m_bclComponent.name()));

  m_measureBadge->setMeasureBadgeType(MeasureBadgeType::BCLMeasure);
}

bool BCLComponentItem::equal(const openstudio::OSItem* item) const {
  if (item->itemId().sourceId() == OSItemId::BCL_SOURCE_ID) {
    if (item->itemId().itemId() == itemId().itemId()) {
      return true;
    }
  }

  return false;
}

}  // namespace openstudio
