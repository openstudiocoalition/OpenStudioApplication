/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OSAPP_UTILITIES_CLONEFIXUP_HPP
#define OSAPP_UTILITIES_CLONEFIXUP_HPP

#include <openstudio/model/ModelObject.hpp>

namespace openstudio {

/// ModelObject::clone() clones each child individually and reattaches it via setParent(), so true
/// parent-child edges are correctly remapped to point within the new subtree. But children are
/// cloned one at a time rather than as a batch sharing a single old->new handle table, so any
/// *lateral* object-list reference between two siblings in the cloned subtree (e.g.
/// CoilHeatingDesuperheater::heatingSource() pointing at a sibling cooling coil, or a
/// SetpointManager's node references) is left broken on the clone -- for some field/type
/// combinations still pointing at the original object, for others (empirically, heatingSource())
/// cleared to empty outright. This fixes that up generically, for any object type, rather than
/// special casing each lateral-reference field as it's discovered.
///
/// `original` is the subtree that was cloned; `clonedObject` is its clone. Re-links any lateral
/// reference in the clone that still points into (or should point into) the original subtree.
void fixupClonedReferences(const model::ModelObject& original, model::ModelObject& clonedObject);

}  // namespace openstudio

#endif  // OSAPP_UTILITIES_CLONEFIXUP_HPP
