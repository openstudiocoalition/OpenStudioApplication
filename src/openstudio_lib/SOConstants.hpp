/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SOCONSTANTS_HPP
#define OPENSTUDIO_SOCONSTANTS_HPP

namespace openstudio {

enum AddToModelEnum
{
  ADDTOMODEL_THERMALZONE,
  ADDTOMODEL_AIRLOOPHVAC,
  ADDTOMODEL_DUAL_AIRLOOPHVAC,
  ADDTOMODEL_SYSTEM_TYPE_1,
  ADDTOMODEL_SYSTEM_TYPE_2,
  ADDTOMODEL_SYSTEM_TYPE_3,
  ADDTOMODEL_SYSTEM_TYPE_4,
  ADDTOMODEL_SYSTEM_TYPE_5,
  ADDTOMODEL_SYSTEM_TYPE_6,
  ADDTOMODEL_SYSTEM_TYPE_7,
  ADDTOMODEL_SYSTEM_TYPE_8,
  ADDTOMODEL_SYSTEM_TYPE_9,
  ADDTOMODEL_SYSTEM_TYPE_10,
  ADDTOMODEL_PLANTLOOP,
  ADDTOMODEL_SHWLOOP
};

enum NewFromTemplateEnum
{
  NEWFROMTEMPLATE_EMPTY,
  NEWFROMTEMPLATE_WIZARD
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SOCONSTANTS_HPP
