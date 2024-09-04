/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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
