/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "../CloneFixup.hpp"

#include <openstudio/model/AirLoopHVAC.hpp>
#include <openstudio/model/AirLoopHVAC_Impl.hpp>
#include <openstudio/model/CoilCoolingDXSingleSpeed.hpp>
#include <openstudio/model/CoilCoolingDXSingleSpeed_Impl.hpp>
#include <openstudio/model/CoilHeatingDesuperheater.hpp>
#include <openstudio/model/CoilHeatingDesuperheater_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Node.hpp>

using namespace openstudio;
using namespace openstudio::model;

TEST(CloneFixup, RemapsLateralReferenceOnCloneSubtree) {
  Model model;
  AirLoopHVAC airLoopHVAC(model);
  CoilCoolingDXSingleSpeed coolingCoil(model);
  CoilHeatingDesuperheater desuperheaterCoil(model);
  ASSERT_TRUE(desuperheaterCoil.setHeatingSource(coolingCoil));

  Node coolingCoilNode = airLoopHVAC.supplyOutletNode();
  ASSERT_TRUE(coolingCoil.addToNode(coolingCoilNode));
  Node desuperheaterNode = airLoopHVAC.supplyOutletNode();
  ASSERT_TRUE(desuperheaterCoil.addToNode(desuperheaterNode));

  ModelObject original = airLoopHVAC;
  ModelObject clonedObject = airLoopHVAC.clone(model);
  fixupClonedReferences(original, clonedObject);

  auto clonedLoop = clonedObject.cast<AirLoopHVAC>();
  boost::optional<CoilHeatingDesuperheater> clonedDesuperheater;
  boost::optional<CoilCoolingDXSingleSpeed> clonedCoolingCoil;
  for (const ModelObject& comp : clonedLoop.supplyComponents()) {
    if (auto d = comp.optionalCast<CoilHeatingDesuperheater>()) {
      clonedDesuperheater = d;
    } else if (auto c = comp.optionalCast<CoilCoolingDXSingleSpeed>()) {
      clonedCoolingCoil = c;
    }
  }
  ASSERT_TRUE(clonedDesuperheater);
  ASSERT_TRUE(clonedCoolingCoil);

  // The clone's heatingSource must point at the *cloned* cooling coil, not the original.
  boost::optional<ModelObject> clonedHeatingSource = clonedDesuperheater->heatingSource();
  ASSERT_TRUE(clonedHeatingSource);
  EXPECT_EQ(clonedHeatingSource->handle(), clonedCoolingCoil->handle());
  EXPECT_NE(clonedHeatingSource->handle(), coolingCoil.handle());

  // The original must be untouched.
  boost::optional<ModelObject> originalHeatingSource = desuperheaterCoil.heatingSource();
  ASSERT_TRUE(originalHeatingSource);
  EXPECT_EQ(originalHeatingSource->handle(), coolingCoil.handle());
}

TEST(CloneFixup, NoLateralReferenceIsANoOp) {
  Model model;
  AirLoopHVAC airLoopHVAC(model);
  CoilCoolingDXSingleSpeed coolingCoil(model);

  Node coolingCoilNode = airLoopHVAC.supplyOutletNode();
  ASSERT_TRUE(coolingCoil.addToNode(coolingCoilNode));

  ModelObject original = airLoopHVAC;
  ModelObject clonedObject = airLoopHVAC.clone(model);
  EXPECT_NO_THROW(fixupClonedReferences(original, clonedObject));

  auto clonedLoop = clonedObject.cast<AirLoopHVAC>();
  bool foundClonedCoil = false;
  for (const ModelObject& comp : clonedLoop.supplyComponents()) {
    if (comp.optionalCast<CoilCoolingDXSingleSpeed>()) {
      foundClonedCoil = true;
    }
  }
  EXPECT_TRUE(foundClonedCoil);
}
