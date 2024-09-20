/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioAppFixture.hpp"

#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <QDir>
#include <QFileInfo>

using namespace openstudio;

#define TEST_UNITS(a, b, c)        \
  EXPECT_TRUE(convert(1.0, a, b)); \
  EXPECT_TRUE(convert(1.0, a, c));

TEST_F(OpenStudioAppFixture, Units) {
  // test unit conversions in the app
  // grep OSQuantityEdit2
  TEST_UNITS("", "", "");
  TEST_UNITS("m", "m", "ft");
  TEST_UNITS("m^2", "m^2", "ft^2");
  TEST_UNITS("m", "m", "in");
  TEST_UNITS("deg", "deg", "deg");
  TEST_UNITS("W/m*K", "W/m*K", "Btu*in/hr*ft^2*R");
  TEST_UNITS("W/m*K", "W/m*K", "Btu/hr*ft*R");
  TEST_UNITS("W/m*K^2", "W/m*K^2", "Btu*in/hr*ft^2*R^2");
  TEST_UNITS("W/m^2*K", "W/m^2*K", "Btu/ft^2*hr*R");
  TEST_UNITS("m^2*K/W", "m^2*K/W", "ft^2*hr*R/Btu");
  TEST_UNITS("W", "W", "W");
  TEST_UNITS("W/m^2", "W/m^2", "W/m^2");
  TEST_UNITS("W/m^2", "W/m^2", "W/ft^2");
  TEST_UNITS("W/person", "W/person", "W/person");
  TEST_UNITS("m^3/s*W", "m^3/s*W", "ft^3/min*W");
  TEST_UNITS("s/m", "s/m", "s/in");
  TEST_UNITS("kg/m^3", "kg/m^3", "lb/ft^3");
  TEST_UNITS("J/kg*K", "J/kg*K", "Btu/lb*R");
  TEST_UNITS("s/m", "s/m", "s/in");
  TEST_UNITS("people/m^2", "people/m^2", "people/ft^2");
  TEST_UNITS("m^2/person", "m^2/person", "ft^2/person");
  TEST_UNITS("m^3/s*W", "L/s*W", "ft^3*hr/min*Btu");
  TEST_UNITS("W", "W", "Btu/hr");
  TEST_UNITS("W/m^2", "W/m^2", "Btu/hr*ft^2");
  TEST_UNITS("W/person", "W/person", "Btu/hr*person");
  TEST_UNITS("m^3/s", "m^3/s", "gal/min");
  TEST_UNITS("g/m*s", "g/m*s", "g/m*s");
  TEST_UNITS("g/m*s*K", "g/m*s*K", "g/m*s*K");
  TEST_UNITS("J/kg*K", "J/kg*K", "Btu/lb*R");
  TEST_UNITS("J/kg*K^2", "J/kg*K^2", "Btu/lb*R^2");
  TEST_UNITS("C", "C", "F");
  TEST_UNITS("K", "K", "R");
  TEST_UNITS("1/m", "1/m", "1/ft");
  TEST_UNITS("1/m", "1/m", "1/in");
}
