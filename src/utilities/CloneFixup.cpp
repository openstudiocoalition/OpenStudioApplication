/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "CloneFixup.hpp"

#include <openstudio/model/AirLoopHVACOutdoorAirSystem.hpp>
#include <openstudio/model/AirLoopHVACOutdoorAirSystem_Impl.hpp>
#include <openstudio/model/Loop.hpp>
#include <openstudio/model/Loop_Impl.hpp>
#include <openstudio/model/ParentObject.hpp>
#include <openstudio/model/ParentObject_Impl.hpp>

#include <openstudio/utilities/idd/IddObject.hpp>
#include <openstudio/utilities/idf/Handle.hpp>
#include <openstudio/utilities/idf/WorkspaceObject.hpp>

#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <utility>
#include <vector>

namespace openstudio {

namespace {
// ParentObject::children() is true ownership (e.g. a UnitarySystem's fan/coils) but does *not*
// include HVACComponents placed on a Loop's supply/demand branches -- a UnitarySystem sitting on
// an AirLoopHVAC's supply branch is reachable via supplyComponents(), not children(). The same
// gap exists one level deeper: equipment on an AirLoopHVACOutdoorAirSystem's outdoor-air/relief
// branches (e.g. an evaporative cooler, or a SetpointManager:MixedAir sitting on one of those
// nodes) is reachable only via oaComponents()/reliefComponents(), not children() either. Cloning
// a whole loop (the "copy system" toolbar action) needs all of these, or the branch equipment --
// and anything lateral-referenced from inside it -- is never visited at all.
//
// Returned as separate groups, each matched and recursed into independently: cloning a loop does
// not carry over the connected thermal zones on the demand side, so original vs. clone
// demandComponents() can legitimately have different sizes. Treating everything as one combined
// list would let a benign demand-side mismatch abort recursion into the other groups too, where
// the equipment (and any lateral references inside it) *does* line up 1:1 with the original.
std::vector<std::vector<model::ModelObject>> childSubtreeObjectGroups(const model::ModelObject& object) {
  std::vector<std::vector<model::ModelObject>> groups;
  if (boost::optional<model::Loop> loop = object.optionalCast<model::Loop>()) {
    groups.push_back(loop->supplyComponents());
    groups.push_back(loop->demandComponents());
  }
  if (boost::optional<model::AirLoopHVACOutdoorAirSystem> oaSystem = object.optionalCast<model::AirLoopHVACOutdoorAirSystem>()) {
    groups.push_back(oaSystem->oaComponents());
    groups.push_back(oaSystem->reliefComponents());
  }
  if (boost::optional<model::ParentObject> parent = object.optionalCast<model::ParentObject>()) {
    groups.push_back(parent->children());
  }
  return groups;
}

// Walks `original` and `clone` in parallel -- childSubtreeObjectGroups() order is deterministic
// and preserved by clone() -- appending every matched (original, clone) pair in the subtree,
// including the roots themselves, to `pairs`. A single recursive walk here is shared by both the
// handle-map build and the reference remap below, rather than each re-implementing it.
void collectClonedObjectPairs(const model::ModelObject& original, const model::ModelObject& clone,
                              std::vector<std::pair<model::ModelObject, model::ModelObject>>& pairs) {
  pairs.emplace_back(original, clone);

  std::vector<std::vector<model::ModelObject>> originalGroups = childSubtreeObjectGroups(original);
  std::vector<std::vector<model::ModelObject>> cloneGroups = childSubtreeObjectGroups(clone);
  for (size_t g = 0; g < originalGroups.size() && g < cloneGroups.size(); ++g) {
    const std::vector<model::ModelObject>& originalChildren = originalGroups[g];
    const std::vector<model::ModelObject>& cloneChildren = cloneGroups[g];
    if (originalChildren.size() == cloneChildren.size()) {
      for (size_t i = 0; i < originalChildren.size(); ++i) {
        collectClonedObjectPairs(originalChildren[i], cloneChildren[i], pairs);
      }
    }
  }
}

// For every lateral object-list field on `original` whose target was itself cloned (i.e. has an
// entry in handleMap), forces the corresponding field on `clonedObject` to point at that clone.
// Fields are read from `original`, not from `clonedObject`: clone() doesn't necessarily leave a
// lateral reference pointing at the stale original object -- for CoilHeatingDesuperheater's
// heatingSource() it clears the field outright -- so the only reliable source of "what this
// field is supposed to point at" is the original.
void remapLateralReferences(const model::ModelObject& original, model::ModelObject& clonedObject,
                            const std::unordered_map<Handle, model::ModelObject, boost::hash<boost::uuids::uuid>>& handleMap) {
  IddObject iddObject = original.iddObject();
  for (unsigned index = 0; index < original.numFields(); ++index) {
    if (iddObject.objectLists(index).empty()) {
      continue;
    }
    boost::optional<WorkspaceObject> originalTarget = original.getTarget(index);
    if (!originalTarget) {
      continue;
    }
    auto it = handleMap.find(originalTarget->handle());
    if (it == handleMap.end()) {
      continue;
    }
    boost::optional<WorkspaceObject> clonedTarget = clonedObject.getTarget(index);
    if (clonedTarget && clonedTarget->handle() == it->second.handle()) {
      continue;
    }
    clonedObject.setPointer(index, it->second.handle());
  }
}
}  // namespace

void fixupClonedReferences(const model::ModelObject& original, const model::ModelObject& clonedObject) {
  std::vector<std::pair<model::ModelObject, model::ModelObject>> pairs;
  collectClonedObjectPairs(original, clonedObject, pairs);

  std::unordered_map<Handle, model::ModelObject, boost::hash<boost::uuids::uuid>> handleMap;
  for (const auto& pair : pairs) {
    handleMap.emplace(pair.first.handle(), pair.second);
  }

  for (auto& pair : pairs) {
    remapLateralReferences(pair.first, pair.second, handleMap);
  }
}

}  // namespace openstudio
