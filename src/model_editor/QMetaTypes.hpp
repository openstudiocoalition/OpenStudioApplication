/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_QMETATYPES
#define MODELEDITOR_QMETATYPES

#include <QMetaType>

#include <QModelIndex>
Q_DECLARE_METATYPE(QModelIndex)

#include <openstudio/utilities/idd/IddEnums.hpp>
Q_DECLARE_METATYPE(openstudio::IddFileType)
Q_DECLARE_METATYPE(openstudio::IddObjectType)

#include "../openstudio_lib/OSItem.hpp"
Q_DECLARE_METATYPE(openstudio::OSItemId)
Q_DECLARE_METATYPE(std::vector<openstudio::OSItemId>)

// #include <model/ModelObject.hpp>
// Note JM 2018-12-13: Was already commented out
// Q_DECLARE_METATYPE(openstudio::model::ModelObject); // no default constructor
// Q_DECLARE_METATYPE(boost::optional<openstudio::model::ModelObject>);
// Q_DECLARE_METATYPE(std::vector<openstudio::model::ModelObject>);

#include <openstudio/utilities/core/UUID.hpp>
Q_DECLARE_METATYPE(openstudio::UUID);

#include <string>
Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(std::vector<std::string>);

#include <boost/optional.hpp>
Q_DECLARE_METATYPE(boost::optional<double>);
Q_DECLARE_METATYPE(boost::optional<unsigned>);
Q_DECLARE_METATYPE(boost::optional<int>);
Q_DECLARE_METATYPE(boost::optional<std::string>);

#include <openstudio/utilities/data/Attribute.hpp>
//Q_DECLARE_METATYPE(openstudio::Attribute);
//Q_DECLARE_METATYPE(boost::optional<openstudio::Attribute>);
//Q_DECLARE_METATYPE(std::vector<openstudio::Attribute>);

#include <openstudio/utilities/units/Quantity.hpp>
Q_DECLARE_METATYPE(openstudio::Quantity);

#include <openstudio/utilities/units/OSOptionalQuantity.hpp>
Q_DECLARE_METATYPE(openstudio::OSOptionalQuantity);

#include <openstudio/utilities/idf/Workspace_Impl.hpp>
Q_DECLARE_METATYPE(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>)

#endif  // MODELEDITOR_QMETATYPES
