/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_DEFAULTCONSTRUCTIONSETSCONTROLLER_HPP
#define OPENSTUDIO_DEFAULTCONSTRUCTIONSETSCONTROLLER_HPP

#include "ModelSubTabController.hpp"

namespace openstudio {

class DefaultConstructionSetsController : public ModelSubTabController
{
  Q_OBJECT

 public:
  explicit DefaultConstructionSetsController(const model::Model& model);

  virtual ~DefaultConstructionSetsController();

 protected:
  virtual void onAddObject(const openstudio::IddObjectType& iddObjectType) override;

  virtual void onCopyObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onRemoveObject(openstudio::model::ModelObject) override;

  virtual void onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) override;

  virtual void onPurgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  virtual void onDrop(const OSItemId& itemId) override;

  virtual void onInspectItem(OSItem* item) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_DEFAULTCONSTRUCTIONSETSCONTROLLER_HPP
