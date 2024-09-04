/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_INTERNALMASSINSPECTORVIEW_HPP
#define OPENSTUDIO_INTERNALMASSINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/InternalMassDefinition.hpp>

namespace openstudio {

class OSLineEdit2;

class OSQuantityEdit2;

class OSDropZone;

class ModelObjectVectorController;

class InternalMassDefinitionInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  InternalMassDefinitionInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~InternalMassDefinitionInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void attach(const openstudio::model::InternalMassDefinition& internalMassDefinition);

  void detach();

  void refresh();

  OSLineEdit2* m_nameEdit;

  OSQuantityEdit2* m_surfaceAreaEdit;

  OSQuantityEdit2* m_surfaceAreaPerSpaceFloorAreaEdit;

  OSQuantityEdit2* m_surfaceAreaPerPersonEdit;

  OSDropZone* m_ConstructionDropZone;

  ModelObjectVectorController* m_ConstructionVectorController;

  bool m_isIP;

  boost::optional<model::InternalMassDefinition> m_internalMassDefinition;

  class ConstructionVectorController : public ModelObjectVectorController
  {
   public:
    boost::optional<model::InternalMassDefinition> internalMassDefinition();

   protected:
    std::vector<OSItemId> makeVector() override;

    void onChangeRelationship(const openstudio::model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

    void onRemoveItem(OSItem* item) override;

    void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;

    void onDrop(const OSItemId& itemId) override;
  };

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_INTERNALMASSINSPECTORVIEW_HPP
