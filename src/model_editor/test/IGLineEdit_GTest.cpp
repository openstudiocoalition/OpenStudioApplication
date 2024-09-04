/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelEditorFixture.hpp"

#include "../IGLineEdit.hpp"
#include "../InspectorGadget.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/Building.hpp>
#include <openstudio/model/Building_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/Lights.hpp>
#include <openstudio/model/Lights_Impl.hpp>
#include <openstudio/model/LightsDefinition.hpp>
#include <openstudio/model/LightsDefinition_Impl.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/model/ThermalZone_Impl.hpp>

#include <openstudio/utilities/idf/WorkspaceObjectWatcher.hpp>

#include <QObject>

using namespace openstudio::model;
using namespace openstudio;

TEST_F(ModelEditorFixture, IGLineEdit_Real) {
  InspectorGadget gadget;
  QString val;
  auto* text = new IGLineEdit(val, &gadget, &gadget);
  auto* valid = new QRegularExpressionValidator(QRegularExpression("-?[0-9]*\\.?[0-9]*([eE][-+]?[0-9]*)?"), text);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);

  text->setValidator(valid);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("0"), val);

  val = QString("hamster");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("0"), val);

  val = QString("autosize");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("autosize"), val);

  text->setMin(1.0);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  val = QString("0.0");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  text->setMax(2.0);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  text->setDefault(1.5);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1.5"), val);
}

TEST_F(ModelEditorFixture, IGLineEdit_Int) {
  InspectorGadget gadget;
  QString val;
  auto* text = new IGLineEdit(val, &gadget, &gadget);
  auto* valid = new QIntValidator(text);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);

  text->setValidator(valid);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("0"), val);

  val = QString("hamster");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("0"), val);

  val = QString("autosize");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("autosize"), val);

  text->setMin(1.0);
  valid->setBottom(1);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  val = QString("0.0");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  text->setMax(3.0);
  valid->setTop(3);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("1"), val);

  text->setDefault(2);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("2"), val);
}

TEST_F(ModelEditorFixture, IGLineEdit_Text) {
  InspectorGadget gadget;
  QString val;
  auto* text = new IGLineEdit(val, &gadget, &gadget);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);

  val = QString("hamster");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("hamster"), val);

  val = QString("autosize");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("autosize"), val);

  text->setMin(1.0);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);

  val = QString("0.0");
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString("0.0"), val);

  text->setMax(2.0);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);

  text->setDefault(1.5);

  val.clear();
  text->setText(val);
  EXPECT_TRUE(text->checkValue(val));
  EXPECT_EQ(QString(""), val);
}