/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BASEDOCUMENT_HPP
#define SHAREDGUICOMPONENTS_BASEDOCUMENT_HPP

#include "OSItemId.hpp"

#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/Component.hpp>

#include <QString>
#include <boost/optional.hpp>

#include <string>
#include <utility>
#include <vector>

namespace openstudio {

/**
 * BaseDocument is the **pure virtual interface** through which shared_gui_components code accesses
 * the current open document. It lives in openstudio_shared_gui and has no dependency on
 * OSDocument or any other openstudio_lib symbol.
 *
 * OSDocument (in openstudio_lib) inherits from BaseDocument and provides the real
 * implementations. Every method is pure virtual — there are no default implementations.
 *
 * Dependency rule:
 *   shared_gui_components  →  BaseDocument  (this file, no openstudio_lib symbols)
 *   openstudio_lib         →  OSDocument    (implements BaseDocument)
 */
class BaseDocument
{
 public:
  virtual ~BaseDocument() {}

  /// Returns true if the item originated from the BCL (Building Component Library).
  virtual bool fromBCL(const OSItemId& itemId) const = 0;

  /// Returns true if the item originated from the component library.
  virtual bool fromComponentLibrary(const OSItemId& itemId) const = 0;

  /// Look up the IDD object type for an item (used to find the correct icon).
  virtual boost::optional<IddObjectType> getIddObjectType(const OSItemId& itemId) const = 0;

  /// Resolve an OSItemId to a model object in the current document.
  virtual boost::optional<model::ModelObject> getModelObject(const OSItemId& itemId) const = 0;

  /// Retrieve the BCL component identified by an OSItemId.
  virtual boost::optional<model::Component> getComponent(const OSItemId& itemId) const = 0;

  // -------------------------------------------------------------------------
  // Document state — safe SDK/Qt return types, no openstudio_lib dependency.
  // -------------------------------------------------------------------------

  /// Returns true if the document has unsaved changes.
  virtual bool modified() const = 0;

  /// Returns the string path to the location where the document is saved.
  /// Returns an empty string if the document has never been saved.
  virtual QString savePath() const = 0;

  /// Returns the path to the directory where model resources are stored.
  virtual QString modelTempDir() const = 0;

  /// Returns the live model associated with this document.
  virtual model::Model model() = 0;

  /// Returns the component library associated with this document.
  virtual model::Model componentLibrary() const = 0;

  /// Returns true if the OSItemId refers to an object in the live model.
  virtual bool fromModel(const OSItemId& itemId) const = 0;

  /// Search BCL components by attribute key/value pairs.
  virtual std::vector<BCLComponent> componentAttributeSearch(const std::vector<std::pair<std::string, std::string>>& pairs) const = 0;

  /// Mark the document as modified (unsaved changes).
  virtual void markAsModified() = 0;

  /// Mark the document as unmodified (no unsaved changes).
  virtual void markAsUnmodified() = 0;

  /// Disable the document UI (e.g. while a drop or run operation is in progress).
  virtual void disable() = 0;

  /// Re-enable the document UI.
  virtual void enable() = 0;

  /// Open the inspector sidebar panel.
  virtual void openSidebar() = 0;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BASEDOCUMENT_HPP
