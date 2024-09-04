/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GRIDITEM_HPP
#define OPENSTUDIO_GRIDITEM_HPP

#include <QGraphicsObject>
#include <QCoreApplication>
#include <QPixmap>
#include <QPointer>
#include <QPointF>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/Node.hpp>
#include <openstudio/model/Mixer.hpp>
#include <openstudio/model/Splitter.hpp>
#include <openstudio/model/Loop.hpp>
#include "OSItem.hpp"
#include "shared_gui_components/GraphicsItems.hpp"

class QMenu;

namespace openstudio {

class LoopScene;

class RemoveButtonItem;

class NodeContextButtonItem;

class ModelObjectGraphicsItem
  : public QGraphicsObject
  , public Nano::Observer
{
  Q_OBJECT;

 public:
  explicit ModelObjectGraphicsItem(QGraphicsItem* parent = nullptr);

  ~ModelObjectGraphicsItem() = default;

  void setEnableHighlight(bool highlight);

  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;

  void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  virtual void setModelObject(model::OptionalModelObject modelObject);

  model::OptionalModelObject modelObject();

  void setDeletable(bool deletable);

  bool draggable() const;
  void setDraggable(bool draggable);

 signals:

  void modelObjectSelected(model::OptionalModelObject&, bool readOnly);

  void removeModelObjectClicked(model::ModelObject&);

  void hvacComponentDropped(OSItemId itemid, model::HVACComponent& contextComp);

  void hvacComponentDropped(OSItemId itemid);

  void innerNodeClicked(model::ModelObject&);

 protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

  bool m_deleteAble;

  bool m_highlight;

  model::OptionalModelObject m_modelObject;

  RemoveButtonItem* m_removeButtonItem;

  bool m_enableHighlight;

  bool m_draggable;
  bool m_mouseDown;
  QPointF m_dragStartPosition;

 private slots:

  void onNameChange();

  void onRemoveButtonClicked();
};

class GridItem : public ModelObjectGraphicsItem
{
  Q_OBJECT;

 public:
  explicit GridItem(QGraphicsItem* parent = nullptr);

  virtual ~GridItem() = default;

  virtual QRectF boundingRect() const override;

  void setGridPos(int x, int y);

  int getXGridPos() const;

  int getYGridPos() const;

  void setHGridLength(int l);

  void setVGridLength(int l);

  int getHGridLength() const;

  int getVGridLength() const;

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  int m_hLength;

  int m_vLength;
};

class NodeContextButtonItem : public ButtonItem
{
  Q_OBJECT;

 public:
  explicit NodeContextButtonItem(GridItem* parent);

 signals:

  void removeModelObjectClicked(model::ModelObject&);

 private slots:

  void showContextMenu();

  void onRemoveSPMActionTriggered();

 private:
  QMenu* m_menu;
};

class SplitterItem : public GridItem
{
 public:
  explicit SplitterItem(QGraphicsItem* parent = nullptr);

  void setNumberBranches(int branches);

  int numberBranches();

  // SingleDuct1 means a terminal is attached to the demand inlet node 1 branch
  // SingleDuct2 means a terminal is attached to the demand inlet node 2 branch
  // DualDuct means there is a dual duct terminal
  enum TerminalType
  {
    SingleDuct1,
    SingleDuct2,
    DualDuct,
    None
  };

  // The size of the types vector is equal to the number of zones connected to the system
  // This is also expected to be equal to the number of branches
  void setTerminalTypes(const std::vector<TerminalType>& types);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  std::vector<TerminalType> m_terminalTypes;
  int m_numberBranches;
  int m_firstDuct1Index;
  int m_firstDuct2Index;
};

class MixerItem : public GridItem
{
 public:
  explicit MixerItem(QGraphicsItem* parent = nullptr);

  void setNumberBranches(int branches);

  int numberBranches();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  int m_numberBranches;
};

class SupplySplitterItem : public GridItem
{
 public:
  explicit SupplySplitterItem(QGraphicsItem* parent = nullptr);

  void setNumberBranches(int branches);

  int numberBranches();

  // See related method HorizontalBranchGroupItem::branchBaselineGridPositions
  // This method is used to get the splitter to line up with the horizontal lines in
  // the HorizontalBranchGroupItem.
  // SupplySideItem will coordinate this
  // The size should be equal to numberBranches
  void setBranchGridPositions(const std::vector<int>& branchGridPositions);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  std::vector<int> m_baselineBranchPositions;
};

class SupplyMixerItem : public GridItem
{
 public:
  explicit SupplyMixerItem(QGraphicsItem* parent = nullptr);

  void setNumberBranches(int branches);

  int numberBranches();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  int m_numberBranches;
};

class OneThreeNodeItem : public GridItem
{
 public:
  explicit OneThreeNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private:
  QPointer<NodeContextButtonItem> m_contextButton;
};

// This is a "dual path" squeezed into the space of a single GridItem
// Used after the zone splitter, going into the zone terminal
class OneThreeDualDuctItem : public GridItem
{
 public:
  explicit OneThreeDualDuctItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

  void setModelObject2(model::OptionalModelObject modelObject);

 private:
  boost::optional<model::ModelObject> m_modelObject2;
};

class FourFiveNodeItem : public GridItem
{
 public:
  explicit FourFiveNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class TwoFiveNodeItem : public GridItem
{
 public:
  explicit TwoFiveNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class OneThreeStraightItem : public GridItem
{
 public:
  // if dual is true there are two parallel lines
  explicit OneThreeStraightItem(QGraphicsItem* parent = nullptr, bool dualDuct = false);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 private:
  bool m_dualDuct;
};

class OneThreeDualDuctMixerItem : public GridItem
{
 public:
  // if dual is true there are two parallel lines
  explicit OneThreeDualDuctMixerItem(QGraphicsItem* parent = nullptr, bool dualDuct = false);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class SupplyPlenumItem : public GridItem
{
 public:
  explicit SupplyPlenumItem(const model::ModelObject& modelObject, QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private:
  void setModelObjectInternal(model::OptionalModelObject modelObject);
  QColor m_color;
};

class ReturnPlenumItem : public GridItem
{
 public:
  explicit ReturnPlenumItem(const model::ModelObject& modelObject, QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private:
  void setModelObjectInternal(model::OptionalModelObject modelObject);
  QColor m_color;
};

class LinkItem : public QGraphicsObject
{
  Q_OBJECT;

 public:
  explicit LinkItem(QGraphicsItem* parent = nullptr);

  virtual ~LinkItem() {}

  QRectF boundingRect() const override;

 signals:

  void mouseClicked();

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  bool m_mouseDown;

  bool m_isHovering;
};

class OneThreeWaterToAirItem : public GridItem
{
  Q_OBJECT;

 public:
  explicit OneThreeWaterToAirItem(QGraphicsItem* parent = nullptr);

  virtual ~OneThreeWaterToAirItem() {}

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private slots:

  void onLinkItemClicked();

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 private:
  bool m_showLinks;
};

class OneThreeWaterToWaterItem : public GridItem
{
  Q_OBJECT;

 public:
  explicit OneThreeWaterToWaterItem(QGraphicsItem* parent = nullptr);

  virtual ~OneThreeWaterToWaterItem() {}

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private slots:

  void onLinkItemClicked();

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 private:
  bool m_showLinks;
};

//class OneThreeWaterToAirItem : public GridItem
//{
//  Q_OBJECT;
//
//  public:
//
//  OneThreeWaterToAirItem(QGraphicsItem * parent = 0 );
//
//  void paint(QPainter *painter,
//             const QStyleOptionGraphicsItem *option,
//             QWidget *widget = 0);
//
//  void setModelObject( model::OptionalModelObject modelObject );
//
//  private slots:
//
//  void onLinkItemClicked();
//
//  protected:
//
//  void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
//
//  private:
//
//  bool m_showLinks;
//};

class TwoFourStraightItem : public GridItem
{
 public:
  explicit TwoFourStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class OASupplyStraightItem : public GridItem
{
 public:
  explicit OASupplyStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class OAReliefStraightItem : public GridItem
{
 public:
  explicit OAReliefStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class OAStraightNodeItem : public GridItem
{
 public:
  explicit OAStraightNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private:
  QPointer<NodeContextButtonItem> m_contextButton;
};

class OASupplyBranchItem : public GridItem
{
 public:
  OASupplyBranchItem(const std::vector<model::ModelObject>& supplyModelObjects, const std::vector<model::ModelObject>& reliefModelObjects,
                     QGraphicsItem* parent = nullptr);

  void layout();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  std::vector<GridItem*> m_gridItems;

  std::vector<OASupplyStraightItem*> m_paddingItems;
};

class OAReliefBranchItem : public GridItem
{
 public:
  OAReliefBranchItem(const std::vector<model::ModelObject>& reliefModelObjects, const std::vector<model::ModelObject>& supplyModelObjects,
                     QGraphicsItem* parent = nullptr);

  void layout();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  std::vector<GridItem*> m_gridItems;

  std::vector<OASupplyStraightItem*> m_paddingItems;
};

class OAEndNodeItem : public GridItem
{
 public:
  explicit OAEndNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class OAAirToAirItem : public GridItem
{
 public:
  explicit OAAirToAirItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class FourTwoStraightItem : public GridItem
{
 public:
  explicit FourTwoStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class TwoFourNodeItem : public GridItem
{
 public:
  explicit TwoFourNodeItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void setModelObject(model::OptionalModelObject modelObject) override;

 private:
  QPointer<NodeContextButtonItem> m_contextButton;
};

class OneFourStraightItem : public GridItem
{
 public:
  explicit OneFourStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class ThreeFourStraightItem : public GridItem
{
 public:
  explicit ThreeFourStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class TwoThreeStraightItem : public GridItem
{
 public:
  explicit TwoThreeStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class OneTwoStraightItem : public GridItem
{
 public:
  explicit OneTwoStraightItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class OAMixerItem : public GridItem
{
 public:
  explicit OAMixerItem(QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

// These are used inside dual duct items that combine two paths inside one GridItem
class HalfHeightItem : public ModelObjectGraphicsItem
{
 public:
  explicit HalfHeightItem(QGraphicsItem* parent = nullptr);

  virtual ~HalfHeightItem() {}

  virtual QRectF boundingRect() const override;
};

class HalfHeightOneThreeStraightItem : public HalfHeightItem
{
 public:
  explicit HalfHeightOneThreeStraightItem(QGraphicsItem* parent = nullptr);

  virtual ~HalfHeightOneThreeStraightItem() {}

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class HalfHeightOneThreeNodeItem : public HalfHeightItem
{
 public:
  explicit HalfHeightOneThreeNodeItem(QGraphicsItem* parent = nullptr);

  virtual ~HalfHeightOneThreeNodeItem() {}

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class HorizontalBranchItem : public GridItem
{
 public:
  // Dual duct means there will be two parallel paths within one item
  explicit HorizontalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent = nullptr, bool dualDuct = false);

  // This signature is always used on a dual duct
  // modelObjectsBeforeTerminal is what it says, the outer pair for the two parallel paths
  // After the terminal the paths come togethor (the dual duct terminal is a mixer) and we
  // have a single linear series of components from then on to the zone mixer
  HorizontalBranchItem(std::pair<std::vector<model::ModelObject>, std::vector<model::ModelObject>> modelObjectsBeforeTerminal,
                       const std::vector<model::ModelObject>& modelObjectsAfterTerminal, QGraphicsItem* parent = nullptr);

  static std::vector<GridItem*> itemFactory(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent);

  void setPadding(unsigned);

  void setText(const QString& text);

  void setHasTwoRightSidePipes(bool dualRight);

  unsigned padding();

  void layout();

  void setIsDropZone(bool isDropZone);

  void setModelObject(model::OptionalModelObject modelObject) override;

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

 private:
  std::vector<GridItem*> m_gridItems;

  std::vector<OneThreeStraightItem*> m_paddingItems;

  bool m_isDropZone;

  QString m_text;

  bool m_hasDualTwoRightSidePipes;

  bool m_dualDuct;
};

class HorizontalBranchGroupItem : public GridItem
{
 public:
  HorizontalBranchGroupItem(model::Splitter& splitter, model::Mixer& mixer, QGraphicsItem* parent = nullptr);

  HorizontalBranchGroupItem(model::Splitter& splitter, std::vector<model::Node>& supplyOutletNodes, QGraphicsItem* parent);

  void layout();

  void setShowDropZone(bool showDropZone);

  unsigned numberOfBranches() const;

  // HorizontalBranchGroupItem instances normally have a series of parallel branches
  // that are each 1 grid unit in height. If we have a dual duct there might be oa systems on the branches
  // and this messes up the assumption about each branch being 1 grid unit high.
  // This method will return the "baseline" (ie where the horizontal black line is) grid pos
  // for each branch.
  std::vector<int> branchBaselineGridPositions() const;

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  model::Splitter m_splitter;
  std::vector<HorizontalBranchItem*> m_branchItems;
  HorizontalBranchItem* m_dropZoneBranchItem;
};

class VerticalBranchItem : public GridItem
{
 public:
  explicit VerticalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent = nullptr);

  void setPadding(unsigned);

  void layout();

 protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  std::vector<GridItem*> m_gridItems;

  std::vector<TwoFourStraightItem*> m_paddingItems;
};

class ReverseVerticalBranchItem : public GridItem
{
 public:
  explicit ReverseVerticalBranchItem(const std::vector<model::ModelObject>& modelObjects, QGraphicsItem* parent = nullptr);

  void setPadding(unsigned);

  void layout();

 private:
  std::vector<GridItem*> m_gridItems;

  std::vector<FourTwoStraightItem*> m_paddingItems;
};

// This is the thing on the demand side of a dual duct
// where the second stream intersects the first forming a tee.
// The streams are not physically combined but visual they combine,
// into the same grid items (ie one grid item show both streams).
class DualDuctTee : public GridItem
{
 public:
  explicit DualDuctTee(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

// This is the elbow used on the demand side of a dual duct
class TwoThreeStraightItem2 : public GridItem
{
 public:
  explicit TwoThreeStraightItem2(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class DemandSideItem : public GridItem
{
 public:
  DemandSideItem(QGraphicsItem* parent, const std::vector<model::Node>& demandInletNodes, const model::Node& demandOutletNode);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void layout();

  void setPadding(unsigned padding);

  unsigned padding();

  int numberBranches();

 private:
  std::vector<model::Node> m_demandInletNodes;

  model::Node m_demandOutletNode;

  HorizontalBranchItem* m_inletBranch;

  HorizontalBranchItem* m_outletBranch;

  HorizontalBranchGroupItem* m_zoneBranches;

  SplitterItem* m_splitterItem;

  MixerItem* m_mixerItem;

  // A TwoFourStraightItem if single duct
  // or a TwoFourStraightItem2 if a dual duct
  GridItem* m_rightElbow;

  DualDuctTee* m_rightElbow2;

  OneTwoStraightItem* m_leftElbow;

  VerticalBranchItem* m_leftVertical;

  VerticalBranchItem* m_rightVertical;

  VerticalBranchItem* m_rightVertical2;

  TwoFourNodeItem* m_inletNode;

  TwoFourNodeItem* m_inletNode2;

  TwoFourNodeItem* m_outletNode;

  unsigned m_padding;

  OneThreeStraightItem* m_inletSpacer;

  OneThreeStraightItem* m_outletSpacer;
};

class OASystemItem : public GridItem
{
 public:
  explicit OASystemItem(model::AirLoopHVACOutdoorAirSystem& oaSystem, QGraphicsItem* parent = nullptr);

 protected:
  void layout();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  OAMixerItem* m_oaMixerItem;

  OASupplyBranchItem* m_oaBranch;

  OAReliefBranchItem* m_reliefBranch;

  OAEndNodeItem* m_oaNodeItem;

  OAEndNodeItem* m_reliefNodeItem;
};

class SupplySideItem : public GridItem
{
 public:
  SupplySideItem(QGraphicsItem* parent, model::Node& supplyInletNode, std::vector<model::Node>& supplyOutletNodes);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void layout();

  void setPadding(unsigned padding);

  unsigned padding();

  int numberBranches();

 private:
  model::Node m_supplyInletNode;

  std::vector<model::Node> m_supplyOutletNodes;

  HorizontalBranchItem* m_outletBranchItem;

  HorizontalBranchItem* m_inletBranchItem;

  HorizontalBranchGroupItem* m_mainBranchGroupItem;

  TwoFourNodeItem* m_inletNodeItem;

  TwoFourNodeItem* m_outletNodeItem;

  TwoFourNodeItem* m_outletNodeItem2;

  OneFourStraightItem* m_leftElbowItem;

  VerticalBranchItem* m_leftVerticalItem;

  ThreeFourStraightItem* m_rightElbowItem;

  ThreeFourStraightItem* m_rightElbowItem2;

  VerticalBranchItem* m_rightVerticalItem;

  VerticalBranchItem* m_rightVerticalItem2;

  SupplySplitterItem* m_splitterItem;

  SupplyMixerItem* m_mixerItem;

  unsigned m_padding;

  OneThreeStraightItem* m_inletSpacer;

  OneThreeStraightItem* m_outletSpacer;

  HorizontalBranchItem* m_dualDuctHorizontalSpace;
};

class SystemCenterItem : public GridItem
{
 public:
  SystemCenterItem(QGraphicsItem* parent, const model::Loop& loop);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  bool m_supplyDualDuct;
  bool m_demandDualDuct;
};

class SystemItem : public GridItem
{
 public:
  SystemItem(const model::Loop& loop, LoopScene* loopScene);

  virtual ~SystemItem();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  QColor plenumColor(const Handle& plenumHandle);

 private:
  model::Loop m_loop;

  LoopScene* m_loopScene;

  SupplySideItem* m_supplySideItem;

  DemandSideItem* m_demandSideItem;

  SystemCenterItem* m_systemCenterItem;

  std::map<Handle, int> m_plenumIndexMap;
  std::map<Handle, QColor> m_plenumColorMap;

  int plenumIndex(const Handle& plenumHandle);
};

class OneTwoFourStraightItem : public GridItem
{
 public:
  explicit OneTwoFourStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

class TwoThreeFourStraightItem : public GridItem
{
 public:
  explicit TwoThreeFourStraightItem(QGraphicsItem* parent = nullptr);

 protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GRIDITEM_HPP
