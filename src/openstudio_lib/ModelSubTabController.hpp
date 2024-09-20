/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELSUBTABCONTROLLER_HPP
#define OPENSTUDIO_MODELSUBTABCONTROLLER_HPP

#include "SubTabController.hpp"
#include "ModelSubTabView.hpp"

#include <openstudio/model/Model.hpp>

#include <QObject>

namespace openstudio {

class OSItem;
class OSItemId;
class SubTabView;

namespace model {
class Component;
}

class ModelSubTabController : public SubTabController
{
  Q_OBJECT

 public:
  ModelSubTabController(ModelSubTabView* subTabView, const model::Model& model);

  virtual ~ModelSubTabController() = default;

 signals:

  void modelObjectSelected(model::OptionalModelObject& modelObject, bool readOnly);

  void dropZoneItemSelected(OSItem* item, bool readOnly);

  void dropZoneItemClicked(OSItem* item);

  void toggleUnitsClicked(bool displayIP);

 protected:
  openstudio::model::Model model() const;

  bool fromModel(const OSItemId& itemId) const;

  bool fromComponentLibrary(const OSItemId& itemId) const;

  boost::optional<model::ModelObject> getModelObject(const OSItemId& itemId) const;

  boost::optional<model::Component> getComponent(const OSItemId& itemId) const;

  virtual void onAddObject(const openstudio::IddObjectType& iddObjectType) = 0;

  virtual void onAddObject(const openstudio::model::ModelObject& modelObject) {}

  virtual void onCopyObject(const openstudio::model::ModelObject& modelObject) = 0;

  virtual void onRemoveObject(openstudio::model::ModelObject modelObject) = 0;

  virtual void onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId) = 0;

  virtual void onPurgeObjects(const openstudio::IddObjectType& iddObjectType) = 0;

  virtual void onDrop(const OSItemId& itemId) override = 0;

  virtual void onAddItem() override;

  virtual void onCopyItem() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem* item, const OSItemId& replacementItemId) override;

  virtual void onPurgeItems() override;

 private slots:

 private:
  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  openstudio::IddObjectType currentIddObjectType() const;

  openstudio::model::Model m_model;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELSUBTABCONTROLLER_HPP
