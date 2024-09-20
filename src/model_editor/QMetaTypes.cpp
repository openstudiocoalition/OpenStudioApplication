/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "QMetaTypes.hpp"

namespace openstudio {
namespace detail {

// Note JM 2018-12-19: `Q_DECLARE_METATYPE` is enough to use a type inside a QVariant, but qRegisterMetaType is needed to use the type in
// *queued* signals/slots and to dynamically create objects of these types at runtime

int __iddobjectype_type = qRegisterMetaType<openstudio::IddObjectType>("openstudio::IddObjectType");
int __iddfiletype_type = qRegisterMetaType<openstudio::IddFileType>("openstudio::IddFileType");

int __ositemid_type = qRegisterMetaType<OSItemId>("OSItemId");
int __ositemid_vector_type = qRegisterMetaType<std::vector<OSItemId>>("std::vector<OSItemId>");

// qRegisterMetaType<openstudio::model::ModelObject>("openstudio::model::ModelObject"); // No default constructor!
// qRegisterMetaType<boost::optional<openstudio::model::ModelObject> >("boost::optional<openstudio::model::ModelObject>");
// qRegisterMetaType<std::vector<openstudio::model::ModelObject> >("std::vector<openstudio::model::ModelObject>" );

int __uuid_type = qRegisterMetaType<openstudio::UUID>("openstudio::UUID");

int __string_type = qRegisterMetaType<std::string>("std::string");
int __string_vector_type = qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");

int __optional_double_type = qRegisterMetaType<boost::optional<double>>("boost::optional<double>");
int __optional_unsigned_type = qRegisterMetaType<boost::optional<unsigned>>("boost::optional<unsigned>");
int __optional_int_type = qRegisterMetaType<boost::optional<int>>("boost::optional<int>");
int __optional_string_type = qRegisterMetaType<boost::optional<std::string>>("boost::optional<std::string>");

// qRegisterMetaType<openstudio::Attribute>("openstudio::Attribute");
//int __attribute_optional_type__ = qRegisterMetaType<boost::optional<openstudio::Attribute>>("boost::optional<openstudio::Attribute>");
//int __atribute_vector_type = qRegisterMetaType<std::vector<openstudio::Attribute>>("std::vector<openstudio::Attribute>");

int __quantity_type = qRegisterMetaType<openstudio::Quantity>("openstudio::Quantity");
int __optionalquantity_type = qRegisterMetaType<openstudio::OSOptionalQuantity>("openstudio::OSOptionalQuantity");

int __workspaceobject_type = qRegisterMetaType<std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>>();

}  // namespace detail
}  // namespace openstudio
