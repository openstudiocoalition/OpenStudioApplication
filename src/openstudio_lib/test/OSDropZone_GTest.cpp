/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioLibFixture.hpp"

#include "../OSDropZone.hpp"
#include "../../shared_gui_components/OSConcepts.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/SpaceType.hpp>
#include <openstudio/model/SpaceType_Impl.hpp>

#include <QComboBox>

#include <memory>

using namespace openstudio;

TEST_F(OpenStudioLibFixture, OSDropZone) {

  model::Model model = model::exampleModel();
  auto spaces = model.getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), WorkspaceObjectNameLess());
  auto spaceTypes = model.getConcreteModelObjects<model::SpaceType>();

  ASSERT_EQ(4u, spaces.size());
  ASSERT_EQ(1u, spaceTypes.size());

  auto space1 = spaces[0];

  Heading heading("Heading", true, true);

  QSharedPointer<DropZoneConcept> dropZoneConcept(
    new DropZoneConceptImpl<model::SpaceType, model::Space>(heading, &model::Space::spaceType, &model::Space::setSpaceType,
                                                            boost::optional<std::function<void(model::Space*)>>(&model::Space::resetSpaceType),
                                                            boost::optional<std::function<bool(model::Space*)>>(&model::Space::isSpaceTypeDefaulted),
                                                            boost::optional<std::function<std::vector<model::ModelObject>(const model::Space*)>>()));

  auto dropZone = std::make_shared<OSDropZone2>();
  dropZone->bind(space1, OptionalModelObjectGetter(std::bind(&DropZoneConcept::get, dropZoneConcept.data(), space1)),
                 ModelObjectSetter(std::bind(&DropZoneConcept::set, dropZoneConcept.data(), space1, std::placeholders::_1)),
                 NoFailAction(std::bind(&DropZoneConcept::reset, dropZoneConcept.data(), space1)),
                 ModelObjectIsDefaulted(std::bind(&DropZoneConcept::isDefaulted, dropZoneConcept.data(), space1)));

  processEvents();
}