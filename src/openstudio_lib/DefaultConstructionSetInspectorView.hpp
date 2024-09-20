/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_DEFAULTCONSTRUCTIONSETINSPECTORVIEW_HPP
#define OPENSTUDIO_DEFAULTCONSTRUCTIONSETINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"

#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/Model.hpp>

namespace openstudio {

class OSDropZone;
class OSLineEdit2;

class WallConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~WallConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class FloorConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~FloorConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class RoofConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~RoofConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class FixedWindowVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~FixedWindowVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class OperableWindowVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~OperableWindowVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class DoorVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~DoorVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class GlassDoorConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~GlassDoorConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class OverheadDoorConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~OverheadDoorConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SkylightConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SkylightConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class TubularDaylightDomeConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~TubularDaylightDomeConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class TubularDaylightDiffuserConstructionVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~TubularDaylightDiffuserConstructionVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class InteriorPartitionsVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~InteriorPartitionsVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SpaceShadingVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SpaceShadingVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingShadingVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~BuildingShadingVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class SiteShadingVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~SiteShadingVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class AdiabaticSurfaceVC : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~AdiabaticSurfaceVC() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;
  virtual std::vector<OSItemId> makeVector() override;
  virtual void onRemoveItem(OSItem* item) override;
  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;
  virtual void onDrop(const OSItemId& itemId) override;
};

class DefaultConstructionSetInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit DefaultConstructionSetInspectorView(const model::Model& model, QWidget* parent = nullptr);
  virtual ~DefaultConstructionSetInspectorView() {}

 protected:
  virtual void onClearSelection() override;
  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onUpdate() override;

 private:
  void attach(openstudio::model::DefaultConstructionSet& defaultConstructionSet);
  void detach();
  void configDropZones();

  OSLineEdit2* m_nameEdit;

  OSDropZone* m_exteriorWallConstructionDZ;
  OSDropZone* m_exteriorFloorConstructionDZ;
  OSDropZone* m_exteriorRoofConstructionDZ;
  OSDropZone* m_interiorWallConstructionDZ;
  OSDropZone* m_interiorFloorConstructionDZ;
  OSDropZone* m_interiorRoofConstructionDZ;
  OSDropZone* m_groundWallConstructionDZ;
  OSDropZone* m_groundFloorConstructionDZ;
  OSDropZone* m_groundRoofConstructionDZ;
  OSDropZone* m_exteriorFixedWindowDZ;
  OSDropZone* m_exteriorOperableWindowDZ;
  OSDropZone* m_exteriorDoorDZ;
  OSDropZone* m_glassDoorConstructionDZ;
  OSDropZone* m_overheadDoorConstructionDZ;
  OSDropZone* m_skylightConstructionDZ;
  OSDropZone* m_tubularDaylightDomeConstructionDZ;
  OSDropZone* m_tubularDaylightDiffuserConstructionDZ;
  OSDropZone* m_interiorFixedWindowDZ;
  OSDropZone* m_interiorOperableWindowDZ;
  OSDropZone* m_interiorDoorDZ;
  OSDropZone* m_interiorPartitionsDZ;
  OSDropZone* m_spaceShadingDZ;
  OSDropZone* m_buildingShadingDZ;
  OSDropZone* m_siteShadingDZ;
  OSDropZone* m_adiabaticSurfaceDZ;

  WallConstructionVC* m_exteriorWallConstructionVC;
  FloorConstructionVC* m_exteriorFloorConstructionVC;
  RoofConstructionVC* m_exteriorRoofConstructionVC;
  WallConstructionVC* m_interiorWallConstructionVC;
  FloorConstructionVC* m_interiorFloorConstructionVC;
  RoofConstructionVC* m_interiorRoofConstructionVC;
  WallConstructionVC* m_groundWallConstructionVC;
  FloorConstructionVC* m_groundFloorConstructionVC;
  RoofConstructionVC* m_groundRoofConstructionVC;
  FixedWindowVC* m_exteriorFixedWindowVC;
  OperableWindowVC* m_exteriorOperableWindowVC;
  DoorVC* m_exteriorDoorVC;
  GlassDoorConstructionVC* m_glassDoorConstructionVC;
  OverheadDoorConstructionVC* m_overheadDoorConstructionVC;
  SkylightConstructionVC* m_skylightConstructionVC;
  TubularDaylightDomeConstructionVC* m_tubularDaylightDomeConstructionVC;
  TubularDaylightDiffuserConstructionVC* m_tubularDaylightDiffuserConstructionVC;
  FixedWindowVC* m_interiorFixedWindowVC;
  OperableWindowVC* m_interiorOperableWindowVC;
  DoorVC* m_interiorDoorVC;
  InteriorPartitionsVC* m_interiorPartitionsVC;
  SpaceShadingVC* m_spaceShadingVC;
  BuildingShadingVC* m_buildingShadingVC;
  SiteShadingVC* m_siteShadingVC;
  AdiabaticSurfaceVC* m_adiabaticSurfaceVC;

  std::vector<ModelObjectVectorController*> m_vectorControllers;
  std::vector<OSDropZone*> m_dropZones;

  boost::optional<model::DefaultConstructionSet> m_defaultConstructionSet;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_DEFAULTCONSTRUCTIONSETINSPECTORVIEW_HPP
