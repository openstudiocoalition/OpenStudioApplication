/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COLORPALETTES_HPP
#define SHAREDGUICOMPONENTS_COLORPALETTES_HPP

#include <array>

#include <QColor>

namespace openstudio {

namespace ColorPalettes {

static constexpr std::array<QColor, 13> schedule_rules_colors{{
  // {r, g, b},     // **approximative** color name (found closest match from SVG color keyword name from World Wide Web Consortium)
  {170, 68, 153},   // darkorchid
  {51, 34, 136},    // darkslateblue
  {17, 119, 51},    // forestgreen
  {153, 153, 51},   // olivedrab
  {221, 204, 119},  // burlywood
  {204, 102, 119},  // indianred
  {136, 34, 85},    // brown
  {68, 170, 153},   // cadetblue
  {102, 153, 204},  // cornflowerblue
  {102, 17, 0},     // maroon
  {170, 68, 102},   // indianred
  {80, 80, 80},     // darkslategray
  {136, 204, 238},  // skyblue

}};

}  // namespace ColorPalettes

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COLORPALETTES_HPP
