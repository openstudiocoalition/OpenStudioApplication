/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelEditorFixture.hpp"

#include "../ModalDialogs.hpp"
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

#include <openstudio/utilities/idd/OS_Lights_FieldEnums.hxx>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QObject>

using namespace openstudio::model;
using namespace openstudio;

TEST_F(ModelEditorFixture, ModalDialogs_EmptyModel_Cancel) {
  Model model;

  std::vector<IddObjectType> typesToDisplay;
  typesToDisplay.push_back(Space::iddObjectType());

  std::shared_ptr<ModelObjectSelectorDialog> modelObjectSelectorDialog(new ModelObjectSelectorDialog(typesToDisplay, model));

  ModelObjectSelectorDialogWatcher watcher(modelObjectSelectorDialog);
  EXPECT_FALSE(watcher.selectedModelObject());

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, modelObjectSelectorDialog.get(), &ModelObjectSelectorDialog::onPushButtonCancel);

  button->doClick();

  EXPECT_FALSE(watcher.selectedModelObject());
}

TEST_F(ModelEditorFixture, ModalDialogs_EmptyModel_Ok) {
  Model model;

  std::vector<IddObjectType> typesToDisplay;
  typesToDisplay.push_back(Space::iddObjectType());

  std::shared_ptr<ModelObjectSelectorDialog> modelObjectSelectorDialog(new ModelObjectSelectorDialog(typesToDisplay, model));

  ModelObjectSelectorDialogWatcher watcher(modelObjectSelectorDialog);
  EXPECT_FALSE(watcher.selectedModelObject());

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, modelObjectSelectorDialog.get(), &ModelObjectSelectorDialog::onPushButtonOK);

  button->doClick();

  EXPECT_FALSE(watcher.selectedModelObject());
}

TEST_F(ModelEditorFixture, ModalDialogs_Cancel) {
  Model model;
  Space space1(model);
  Space space2(model);

  std::vector<IddObjectType> typesToDisplay;
  typesToDisplay.push_back(Space::iddObjectType());

  std::shared_ptr<ModelObjectSelectorDialog> modelObjectSelectorDialog(new ModelObjectSelectorDialog(typesToDisplay, model));

  ModelObjectSelectorDialogWatcher watcher(modelObjectSelectorDialog);
  ASSERT_TRUE(watcher.selectedModelObject());
  EXPECT_EQ(space1.handle(), watcher.selectedModelObject()->handle());

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, modelObjectSelectorDialog.get(), &ModelObjectSelectorDialog::onPushButtonCancel);

  button->doClick();

  EXPECT_FALSE(watcher.selectedModelObject());
}

TEST_F(ModelEditorFixture, ModalDialogs_Ok) {
  Model model;
  Space space1(model);
  Space space2(model);

  std::vector<IddObjectType> typesToDisplay;
  typesToDisplay.push_back(Space::iddObjectType());

  std::shared_ptr<ModelObjectSelectorDialog> modelObjectSelectorDialog(new ModelObjectSelectorDialog(typesToDisplay, model));

  ModelObjectSelectorDialogWatcher watcher(modelObjectSelectorDialog);
  ASSERT_TRUE(watcher.selectedModelObject());
  EXPECT_EQ(space1.handle(), watcher.selectedModelObject()->handle());

  std::shared_ptr<TestButton> button(new TestButton);

  QObject::connect(button.get(), &TestButton::clicked, modelObjectSelectorDialog.get(), &ModelObjectSelectorDialog::onPushButtonOK);

  button->doClick();

  ASSERT_TRUE(watcher.selectedModelObject());
  EXPECT_EQ(space1.handle(), watcher.selectedModelObject()->handle());
}

TEST_F(ModelEditorFixture, ModalDialogs_EnsureSpaceLoadDefinition_EmptyModel) {
  Model model;
  EXPECT_TRUE(model.addObject(IdfObject(IddObjectType::OS_Lights)));

  ASSERT_EQ(1u, model.getModelObjects<Lights>().size());
  Lights light = model.getModelObjects<Lights>()[0];

  // does not have definition
  EXPECT_FALSE(light.getTarget(OS_LightsFields::LightsDefinitionName));
  EXPECT_THROW(light.definition(), openstudio::Exception);

  ensureSpaceLoadDefinition(light);

  ASSERT_TRUE(light.getTarget(OS_LightsFields::LightsDefinitionName));
  ASSERT_NO_THROW(light.definition());
  ASSERT_NO_THROW(light.lightsDefinition());
}

/* THIS IS A BLOCKING TEST
TEST_F(ModelEditorFixture, ModalDialogs_EnsureSpaceLoadDefinition)
{
  Model model;
  LightsDefinition definition1(model);
  LightsDefinition definition2(model);
  EXPECT_TRUE(model.addObject(IdfObject(IddObjectType::OS_Lights)));

  ASSERT_EQ(1u, model.getModelObjects<Lights>().size());
  Lights light = model.getModelObjects<Lights>()[0];

  // does not have definition
  EXPECT_FALSE(light.getTarget(OS_LightsFields::LightsDefinitionName));
  EXPECT_THROW(light.definition(), openstudio::Exception);

  ensureSpaceLoadDefinition(light);

  ASSERT_TRUE(light.getTarget(OS_LightsFields::LightsDefinitionName));
  ASSERT_NO_THROW(light.definition());
  ASSERT_NO_THROW(light.lightsDefinition());
}
*/

TEST_F(ModelEditorFixture, ModalDialogs_EnsureSpaceLoadDefinition_WithDefinition) {
  Model model;
  LightsDefinition definition1(model);
  LightsDefinition definition2(model);
  Lights light(definition1);

  ensureSpaceLoadDefinition(light);

  ASSERT_TRUE(light.getTarget(OS_LightsFields::LightsDefinitionName));
  ASSERT_NO_THROW(light.definition());
  ASSERT_NO_THROW(light.lightsDefinition());
}
