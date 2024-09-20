/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_MEASUREBADGE_HPP
#define SHAREDGUICOMPONENTS_MEASUREBADGE_HPP

#include <QLabel>

#include <openstudio/utilities/bcl/BCLMeasure.hpp>

namespace openstudio {

class MeasureBadge : public QLabel
{
  Q_OBJECT

 public:
  void setMeasureBadgeType(MeasureBadgeType type);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_MEASUREBADGE_HPP
