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

#ifndef OPENSTUDIO_SERVICEWATERGRIDITEMS_HPP
#define OPENSTUDIO_SERVICEWATERGRIDITEMS_HPP

#include <QGraphicsObject>
#include <QCoreApplication>
#include <QPixmap>
#include <openstudio/model/ModelObject.hpp>
#include "OSItem.hpp"
#include "GridItem.hpp"

namespace openstudio {

class ServiceWaterScene;

class WaterUseConnectionsDetailScene;

class ServiceWaterItem : public GridItem
{
 public:
  explicit ServiceWaterItem(ServiceWaterScene* serviceWaterScene);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class WaterUseConnectionsDetailItem : public GridItem
{
 public:
  explicit WaterUseConnectionsDetailItem(WaterUseConnectionsDetailScene* waterUseConnectionsDetailScene);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class WaterUseConnectionsDropZoneItem : public HorizontalBranchItem
{
 public:
  explicit WaterUseConnectionsDropZoneItem(QGraphicsItem* parent = nullptr);
};

class WaterUseEquipmentDropZoneItem : public HorizontalBranchItem
{
 public:
  explicit WaterUseEquipmentDropZoneItem(QGraphicsItem* parent = nullptr);
};

class SewerItem : public GridItem
{
 public:
  explicit SewerItem(QGraphicsItem* parent = nullptr);

  ButtonItem* sewerButton() const;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  ButtonItem* m_sewerButton;
};

class MainsSupplyItem : public GridItem
{
 public:
  explicit MainsSupplyItem(QGraphicsItem* parent = nullptr);

  ButtonItem* mainsSupplyButton() const;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  ButtonItem* m_mainsSupplyButton;
};

class HotWaterSupplyItem : public GridItem
{
  Q_OBJECT;

 public:
  explicit HotWaterSupplyItem(QGraphicsItem* parent = nullptr);

  virtual ~HotWaterSupplyItem() {}

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  ButtonItem* m_hotWaterSupplyButton;

 private slots:

  void onHotWaterSupplyButtonClicked();
};

class DoubleOneThreeStraightItem : public GridItem
{
 public:
  explicit DoubleOneThreeStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class DoubleThreeFourStraightItem : public GridItem
{
 public:
  explicit DoubleThreeFourStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class DoubleTwoFourStraightItem : public GridItem
{
 public:
  explicit DoubleTwoFourStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class DoubleTwoThreeStraightItem : public GridItem
{
 public:
  explicit DoubleTwoThreeStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class DoubleTwoThreeFourStraightItem : public GridItem
{
 public:
  explicit DoubleTwoThreeFourStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class HotWaterJunctionItem : public DoubleTwoFourStraightItem
{
 public:
  explicit HotWaterJunctionItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class ColdWaterJunctionItem : public GridItem
{
 public:
  explicit ColdWaterJunctionItem(int isElbow, QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  bool m_isElbow;
};

class WaterUseConnectionsItem : public GridItem
{
 public:
  explicit WaterUseConnectionsItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class WaterUseEquipmentItem : public GridItem
{
 public:
  explicit WaterUseEquipmentItem(QGraphicsItem* parent = nullptr);

  void setModelObject(model::OptionalModelObject modelObject) override;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class MakeupWaterItem : public GridItem
{
  Q_OBJECT;

 public:
  explicit MakeupWaterItem(QGraphicsItem* parent = nullptr);

  virtual ~MakeupWaterItem() {}

  ButtonItem* mainsSupplyButton() const;

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private slots:

  void onHotWaterSupplyButtonClicked();

 private:
  ButtonItem* m_mainsSupplyButton;

  ButtonItem* m_hotWaterSupplyButton;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SERVICEWATERGRIDITEMS_HPP
