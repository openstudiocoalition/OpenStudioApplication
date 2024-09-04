/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_BUILDINGINSPECTORVIEW_HPP
#define OPENSTUDIO_BUILDINGINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"

#include <openstudio/model/Building.hpp>

class QColor;
class QComboBox;
class QLabel;
class QPushButton;

namespace openstudio {

class OSComboBox2;
class OSDropZone;
class OSIntegerEdit2;
class OSLineEdit2;
class OSQuantityEdit2;
class OSQuantityEdit2;
class OSSwitch2;

class BuildingSpaceTypeVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~BuildingSpaceTypeVectorController() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingDefaultConstructionSetVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~BuildingDefaultConstructionSetVectorController() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingDefaultScheduleSetVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  virtual ~BuildingDefaultScheduleSetVectorController() {}

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  BuildingInspectorView(bool isIP, bool displayAdditionalProps, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~BuildingInspectorView() {}

 signals:

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 protected slots:

  void editStandardsTemplate(const QString& text);
  void standardsTemplateChanged(const QString& text);

  void editStandardsBuildingType(const QString& text);
  void standardsBuildingTypeChanged(const QString& text);

 private:
  void attach(openstudio::model::Building& building);

  void detach();

  void populateStandardsTemplates();
  void populateStandardsBuildingTypes();

  boost::optional<openstudio::model::Building> m_building;
  BuildingDefaultConstructionSetVectorController* m_defaultConstructionSetVectorController = nullptr;
  BuildingDefaultScheduleSetVectorController* m_defaultScheduleSetVectorController = nullptr;
  BuildingSpaceTypeVectorController* m_spaceTypeVectorController = nullptr;
  OSDropZone* m_defaultConstructionSetDropZone = nullptr;
  OSDropZone* m_defaultScheduleSetDropZone = nullptr;
  OSDropZone* m_spaceTypeDropZone = nullptr;
  OSIntegerEdit2* m_numberAboveGroundStories = nullptr;
  OSIntegerEdit2* m_numberLivingUnits = nullptr;
  OSIntegerEdit2* m_numberStories = nullptr;
  OSLineEdit2* m_nameEdit = nullptr;
  QLabel* m_displayNamelabel = nullptr;
  OSLineEdit2* m_displayNameEdit = nullptr;
  QLabel* m_cadObjectIdLabel = nullptr;
  OSLineEdit2* m_cadObjectIdEdit = nullptr;
  OSQuantityEdit2* m_northAxisEdit = nullptr;
  OSQuantityEdit2* m_floorToCeilingHeight = nullptr;
  OSQuantityEdit2* m_floorToFloorHeight = nullptr;
  OSSwitch2* m_relocatable = nullptr;
  QComboBox* m_standardsTemplateComboBox = nullptr;
  QComboBox* m_standardsBuildingTypeComboBox = nullptr;
  bool m_isIP;
  bool m_displayAdditionalProps;

 public slots:

  void toggleUnits(bool displayIP) override;

  void toggleDisplayAdditionalProps(bool displayAdditionalProps) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_BUILDINGSTORYINSPECTORVIEW_HPP
