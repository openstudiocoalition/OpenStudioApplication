/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelEditorFixture.hpp"

#include "../InspectorDialog.hpp"
#include "../TestButton.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/Building.hpp>
#include <openstudio/model/Building_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/Lights.hpp>
#include <openstudio/model/Lights_Impl.hpp>
#include <openstudio/model/LightsDefinition.hpp>
#include <openstudio/model/LightsDefinition_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>

#include <openstudio/utilities/idf/WorkspaceObjectWatcher.hpp>

#include <QObject>

using namespace openstudio::model;
using namespace openstudio;

TEST_F(ModelEditorFixture, InspectorDialog_EmptyModel) {
  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog());

  EXPECT_TRUE(inspectorDialog->setIddObjectType(Space::iddObjectType()));
  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, inspectorDialog.get(), &InspectorDialog::onPushButtonNew);

  Model model = inspectorDialog->model();
  EXPECT_EQ(0u, model.numObjects());

  button->doClick();

  ASSERT_EQ(1u, model.objects(true).size());
  EXPECT_TRUE(model.objects(true)[0].optionalCast<Space>());
}

TEST_F(ModelEditorFixture, InspectorDialog_Remove1Object) {
  Model model;
  Space space1(model);
  Space space2(model);
  EXPECT_EQ(2u, model.numObjects());

  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog(model));

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, inspectorDialog.get(), &InspectorDialog::onPushButtonDelete);

  EXPECT_TRUE(inspectorDialog->setIddObjectType(Space::iddObjectType()));
  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());

  std::vector<Handle> handles;
  handles.push_back(space1.handle());
  EXPECT_TRUE(inspectorDialog->setSelectedObjectHandles(handles));
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(space1.handle(), inspectorDialog->selectedObjectHandles()[0]);

  button->doClick();

  ASSERT_EQ(1u, model.numObjects());
  EXPECT_EQ(space2.handle(), model.objects(true)[0].handle());

  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(space2.handle(), inspectorDialog->selectedObjectHandles()[0]);
}

TEST_F(ModelEditorFixture, InspectorDialog_Copy1Object) {
  Model model;
  Space space1(model);
  Space space2(model);
  ASSERT_EQ(2u, model.numObjects());
  EXPECT_EQ(space1.handle(), model.objects(true)[0].handle());
  EXPECT_EQ(space2.handle(), model.objects(true)[1].handle());

  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog(model));

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, inspectorDialog.get(), &InspectorDialog::onPushButtonCopy);

  EXPECT_TRUE(inspectorDialog->setIddObjectType(Space::iddObjectType()));
  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());

  std::vector<Handle> handles;
  handles.push_back(space1.handle());
  EXPECT_TRUE(inspectorDialog->setSelectedObjectHandles(handles));
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(space1.handle(), inspectorDialog->selectedObjectHandles()[0]);

  button->doClick();

  ASSERT_EQ(3u, model.numObjects());
  EXPECT_EQ(space1.handle(), model.objects(true)[0].handle());
  EXPECT_EQ(space2.handle(), model.objects(true)[1].handle());
  EXPECT_TRUE(model.objects(true)[2].optionalCast<Space>());

  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(model.objects(true)[2].handle(), inspectorDialog->selectedObjectHandles()[0]);
}

TEST_F(ModelEditorFixture, InspectorDialog_ModelObjectRemove) {
  Model model;
  LightsDefinition definition(model);
  Lights lights(definition);
  ASSERT_EQ(2u, model.numObjects());
  EXPECT_EQ(definition.handle(), model.objects(true)[0].handle());
  EXPECT_EQ(lights.handle(), model.objects(true)[1].handle());

  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog(model));

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, inspectorDialog.get(), &InspectorDialog::onPushButtonDelete);

  EXPECT_TRUE(inspectorDialog->setIddObjectType(LightsDefinition::iddObjectType()));
  EXPECT_EQ(LightsDefinition::iddObjectType(), inspectorDialog->iddObjectType());

  std::vector<Handle> handles;
  handles.push_back(definition.handle());
  EXPECT_TRUE(inspectorDialog->setSelectedObjectHandles(handles));
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(definition.handle(), inspectorDialog->selectedObjectHandles()[0]);

  button->doClick();

  ASSERT_EQ(0u, model.numObjects());
}

TEST_F(ModelEditorFixture, InspectorDialog_SignalsOnIddObjectTypeChange) {
  Model model;
  Space space(model);
  ThermalZone thermalZone(model);

  ASSERT_TRUE(model.numObjects() >= 2);
  EXPECT_EQ(space.handle(), model.objects(true)[0].handle());
  EXPECT_EQ(thermalZone.handle(), model.objects(true)[1].handle());

  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog(model));

  EXPECT_TRUE(inspectorDialog->setIddObjectType(Space::iddObjectType()));
  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(space.handle(), inspectorDialog->selectedObjectHandles()[0]);

  WorkspaceObjectWatcher spaceWatcher(space);
  EXPECT_FALSE(spaceWatcher.dirty());

  WorkspaceObjectWatcher thermalZoneWatcher(thermalZone);
  EXPECT_FALSE(thermalZoneWatcher.dirty());

  EXPECT_TRUE(inspectorDialog->setIddObjectType(ThermalZone::iddObjectType()));
  EXPECT_EQ(ThermalZone::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(thermalZone.handle(), inspectorDialog->selectedObjectHandles()[0]);

  EXPECT_FALSE(spaceWatcher.dirty());
  EXPECT_TRUE(thermalZoneWatcher.dirty());  // created new field for humidistat

  spaceWatcher.clearState();
  thermalZoneWatcher.clearState();

  EXPECT_FALSE(spaceWatcher.dirty());
  EXPECT_FALSE(thermalZoneWatcher.dirty());

  EXPECT_TRUE(inspectorDialog->setIddObjectType(Space::iddObjectType()));
  EXPECT_EQ(Space::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(space.handle(), inspectorDialog->selectedObjectHandles()[0]);

  EXPECT_FALSE(spaceWatcher.dirty());
  EXPECT_FALSE(thermalZoneWatcher.dirty());

  EXPECT_TRUE(inspectorDialog->setIddObjectType(ThermalZone::iddObjectType()));
  EXPECT_EQ(ThermalZone::iddObjectType(), inspectorDialog->iddObjectType());
  ASSERT_EQ(1u, inspectorDialog->selectedObjectHandles().size());
  EXPECT_EQ(thermalZone.handle(), inspectorDialog->selectedObjectHandles()[0]);

  EXPECT_FALSE(spaceWatcher.dirty());
  EXPECT_FALSE(thermalZoneWatcher.dirty());
}

TEST_F(ModelEditorFixture, InspectorDialog_SketchUpPlugin) {
  std::shared_ptr<InspectorDialog> inspectorDialog(new InspectorDialog(InspectorDialogClient::SketchUpPlugin));
}
