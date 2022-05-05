/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
