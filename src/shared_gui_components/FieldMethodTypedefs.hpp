/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_FIELDMETHODTYPEDEFS_HPP
#define SHAREDGUICOMPONENTS_FIELDMETHODTYPEDEFS_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <boost/optional.hpp>

#include <vector>

namespace openstudio {

using BoolGetter = std::function<bool()>;
using DoubleGetter = std::function<double()>;
using IntGetter = std::function<int()>;
using StringGetter = std::function<std::string()>;
using UnsignedGetter = std::function<unsigned int()>;
using ModelObjectGetter = std::function<model::ModelObject()>;

using OptionalDoubleGetter = std::function<boost::optional<double>()>;
using OptionalIntGetter = std::function<boost::optional<int>()>;
using OptionalStringGetter = std::function<boost::optional<std::string>()>;
using OptionalStringGetterBoolArg = std::function<boost::optional<std::string>(bool)>;
using OptionalUnsignedGetter = std::function<boost::optional<unsigned int>()>;
using OptionalModelObjectGetter = std::function<boost::optional<model::ModelObject>()>;

using BoolSetter = std::function<void(bool)>;
using BoolSetterBoolReturn = std::function<bool(bool)>;
using DoubleSetter = std::function<bool(double)>;
using DoubleSetterVoidReturn = std::function<void(double)>;
using IntSetter = std::function<bool(int)>;
using StringSetter = std::function<bool(std::string)>;
using StringSetterVoidReturn = std::function<void(std::string)>;
using StringSetterOptionalStringReturn = std::function<boost::optional<std::string>(const std::string&)>;
using UnsignedSetter = std::function<bool(unsigned int)>;
using ModelObjectSetter = std::function<bool(const model::ModelObject&)>;
using ModelObjectIsDefaulted = std::function<bool(const model::ModelObject&)>;
using ModelObjectIsReadOnly = std::function<bool(const model::ModelObject&)>;
using OtherModelObjects = std::function<std::vector<model::ModelObject>(const model::ModelObject&)>;

using NoFailAction = std::function<void()>;
using BasicQuery = std::function<bool()>;

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_FIELDMETHODTYPEDEFS_HPP
