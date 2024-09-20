/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelEditorFixture.hpp"

#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

#include "../QMetaTypes.hpp"
#include <QMetaType>
#include <QVariant>

// QModelIndex

using namespace openstudio;

TEST_F(ModelEditorFixture, IddObjectType_QVariant) {
  IddObjectType type(IddObjectType::Zone);
  QVariant variant = QVariant::fromValue(type);
  EXPECT_EQ("openstudio::IddObjectType", std::string(variant.typeName()));
  ASSERT_TRUE(variant.canConvert<IddObjectType>());
  auto type2 = variant.value<IddObjectType>();
  EXPECT_EQ(type.value(), type2.value());
}

//Q_DECLARE_METATYPE(openstudio::IddFileType)
//Q_DECLARE_METATYPE(openstudio::IddObjectType)

//#include "../openstudio_lib/OSItem.hpp"
//Q_DECLARE_METATYPE(openstudio::OSItemId)
//Q_DECLARE_METATYPE(std::vector<openstudio::OSItemId>)

//// #include <model/ModelObject.hpp>
//// Note JM 2018-12-13: Was already commented out
//// Q_DECLARE_METATYPE(openstudio::model::ModelObject); // no default constructor
//// Q_DECLARE_METATYPE(boost::optional<openstudio::model::ModelObject>);
//// Q_DECLARE_METATYPE(std::vector<openstudio::model::ModelObject>);

//#include <utilities/core/UUID.hpp>
//Q_DECLARE_METATYPE(openstudio::UUID);
TEST_F(ModelEditorFixture, UUID_QVariant) {
  Handle handle = createUUID();
  QVariant variant = QVariant::fromValue(handle);
  EXPECT_EQ("openstudio::UUID", std::string(variant.typeName()));
  ASSERT_TRUE(variant.canConvert<Handle>());
  auto handle2 = variant.value<Handle>();
  EXPECT_EQ(handle, handle2);
}

//#include <string>
//Q_DECLARE_METATYPE(std::string);
//Q_DECLARE_METATYPE(std::vector<std::string>);

//#include <boost/optional.hpp>

//// Original comment: declare these types so we can use them as properties
//Q_DECLARE_METATYPE(boost::optional<double>);
//Q_DECLARE_METATYPE(boost::optional<unsigned>);
//Q_DECLARE_METATYPE(boost::optional<int>);
//Q_DECLARE_METATYPE(boost::optional<std::string>);

//#include <utilities/data/Attribute.hpp>
////Q_DECLARE_METATYPE(openstudio::Attribute);
//Q_DECLARE_METATYPE(boost::optional<openstudio::Attribute>);
//Q_DECLARE_METATYPE(std::vector<openstudio::Attribute>);

//#include <utilities/units/Quantity.hpp>
//Q_DECLARE_METATYPE(openstudio::Quantity);

//#include <utilities/units/OSOptionalQuantity.hpp>
//Q_DECLARE_METATYPE(openstudio::OSOptionalQuantity);

//#include <utilities/idf/Workspace_Impl.hpp>
//Q_DECLARE_METATYPE(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>)
