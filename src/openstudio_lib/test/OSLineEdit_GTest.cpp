/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

// OSLineEdit is defined in shared_gui_components, but this test lives in openstudio_lib/test/
// because it requires OpenStudioLibFixture, which provides a live OSAppBase, OSDocument, and
// model::Model. A BaseApp mock would be needed to host it in shared_gui_components tests.
#include "OpenStudioLibFixture.hpp"

#include "../../shared_gui_components/OSLineEdit.hpp"
#include "../../shared_gui_components/OSConcepts.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>

#include <QComboBox>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, OSLineEdit) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), WorkspaceObjectNameLess());
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();

  ASSERT_EQ(4u, spaces.size());
  ASSERT_EQ(1u, spaceTypes.size());

  processEvents();
}
