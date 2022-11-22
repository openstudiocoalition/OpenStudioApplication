/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
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