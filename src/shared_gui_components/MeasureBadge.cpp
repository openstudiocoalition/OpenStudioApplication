/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MeasureBadge.hpp"

#include <openstudio/utilities/core/Assert.hpp>

namespace openstudio {

void MeasureBadge::setMeasureBadgeType(MeasureBadgeType type) {
  if (type == MeasureBadgeType::BCLMeasure) {
    setText("BCL");
    setStyleSheet("QLabel { color: #4B7DB0; font-size: 12px; }");
  } else if (type == MeasureBadgeType::MyMeasure) {
    setText("My");
    setStyleSheet("QLabel { color: #882255; font-size: 12px; }");
  } else if (type == MeasureBadgeType::OSMeasure) {
    setText("OS");
    setStyleSheet("QLabel { color: #4B7DB0; font-size: 12px; }");
  } else {
    OS_ASSERT(false);
  }
}

}  // namespace openstudio
