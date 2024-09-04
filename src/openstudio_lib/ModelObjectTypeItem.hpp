/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTTYPEITEM_HPP
#define OPENSTUDIO_MODELOBJECTTYPEITEM_HPP

#include "OSCollapsibleItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class OSCollapsibleItemHeader;
class ModelObjectListView;

class ModelObjectTypeItem : public OSCollapsibleItem
{
  Q_OBJECT

 public:
  ModelObjectTypeItem(OSCollapsibleItemHeader* collapsibleItemHeader, ModelObjectListView* modelObjectListView, QWidget* parent = nullptr);

  virtual ~ModelObjectTypeItem() {}

  IddObjectType iddObjectType() const;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTTYPEITEM_HPP
