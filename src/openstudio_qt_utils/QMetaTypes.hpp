/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIOQTUTILS_QMETATYPES_HPP
#define OPENSTUDIOQTUTILS_QMETATYPES_HPP

#include <QMetaType>

#include <QModelIndex>
Q_DECLARE_METATYPE(QModelIndex)

#include <openstudio/utilities/idd/IddEnums.hpp>
Q_DECLARE_METATYPE(openstudio::IddFileType)
Q_DECLARE_METATYPE(openstudio::IddObjectType)

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

#include <openstudio/utilities/units/Quantity.hpp>
Q_DECLARE_METATYPE(openstudio::Quantity);

#include <openstudio/utilities/units/OSOptionalQuantity.hpp>
Q_DECLARE_METATYPE(openstudio::OSOptionalQuantity);

#include <openstudio/utilities/idf/Workspace_Impl.hpp>
Q_DECLARE_METATYPE(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>)

#endif  // OPENSTUDIOQTUTILS_QMETATYPES_HPP
