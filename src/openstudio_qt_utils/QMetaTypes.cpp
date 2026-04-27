/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "QMetaTypes.hpp"

namespace openstudio {
namespace detail {

// Note JM 2018-12-19: `Q_DECLARE_METATYPE` is enough to use a type inside a QVariant, but qRegisterMetaType is needed to use the type in
// *queued* signals/slots and to dynamically create objects of these types at runtime

[[maybe_unused]] int iddobjecttype_meta_type_id = qRegisterMetaType<openstudio::IddObjectType>("openstudio::IddObjectType");
[[maybe_unused]] int iddfiletype_meta_type_id = qRegisterMetaType<openstudio::IddFileType>("openstudio::IddFileType");

[[maybe_unused]] int uuid_meta_type_id = qRegisterMetaType<openstudio::UUID>("openstudio::UUID");

[[maybe_unused]] int string_meta_type_id = qRegisterMetaType<std::string>("std::string");
[[maybe_unused]] int string_vector_meta_type_id = qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");

[[maybe_unused]] int optional_double_meta_type_id = qRegisterMetaType<boost::optional<double>>("boost::optional<double>");
[[maybe_unused]] int optional_unsigned_meta_type_id = qRegisterMetaType<boost::optional<unsigned>>("boost::optional<unsigned>");
[[maybe_unused]] int optional_int_meta_type_id = qRegisterMetaType<boost::optional<int>>("boost::optional<int>");
[[maybe_unused]] int optional_string_meta_type_id = qRegisterMetaType<boost::optional<std::string>>("boost::optional<std::string>");

[[maybe_unused]] int quantity_meta_type_id = qRegisterMetaType<openstudio::Quantity>("openstudio::Quantity");
[[maybe_unused]] int optional_quantity_meta_type_id = qRegisterMetaType<openstudio::OSOptionalQuantity>("openstudio::OSOptionalQuantity");

[[maybe_unused]] int workspaceobject_impl_meta_type_id = qRegisterMetaType<std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>>();

}  // namespace detail
}  // namespace openstudio
