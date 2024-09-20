/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_VRFCONTROLLER_HPP
#define OPENSTUDIO_VRFCONTROLLER_HPP

#include <QObject>
#include <QSharedPointer>
#include <boost/optional.hpp>
#include <openstudio/model/AirConditionerVariableRefrigerantFlow.hpp>
#include "../shared_gui_components/OSListController.hpp"

class QGraphicsScene;
class QGraphicsView;

namespace openstudio {

class OSItemId;
class VRFSystemListController;
class GridLayoutItem;
class VRFView;
class VRFSystemView;

class VRFController : public QObject
{
  Q_OBJECT

 public:
  VRFController();

  virtual ~VRFController();

  VRFView* vrfView() const;

  QSharedPointer<VRFSystemListController> vrfSystemListController() const;

 public slots:

  void zoomInOnSystem(const model::AirConditionerVariableRefrigerantFlow& system);

  void zoomOutToSystemGridView();

 private slots:

  void refresh();

  void refreshNow();

  void inspectOSItem(const OSItemId& itemid);

  void onVRFSystemViewDrop(const OSItemId& itemid);

  void onVRFSystemViewZoneDrop(const OSItemId& itemid);

  void onVRFTerminalViewDrop(const OSItemId& terminalId, const OSItemId& thermalZoneId);

  void onRemoveZoneClicked(const OSItemId& terminalId);

  void onRemoveTerminalClicked(const OSItemId& terminalId);

 private:
  QPointer<VRFView> m_vrfView;

  QPointer<GridLayoutItem> m_vrfSystemGridView;

  QPointer<VRFSystemView> m_detailView;

  QSharedPointer<QGraphicsScene> m_vrfGridScene;

  QSharedPointer<QGraphicsScene> m_detailScene;

  QSharedPointer<VRFSystemListController> m_vrfSystemListController;

  boost::optional<model::AirConditionerVariableRefrigerantFlow> m_currentSystem;

  bool m_dirty;
};

class VRFSystemListController : public OSListController
{
  Q_OBJECT

 public:
  explicit VRFSystemListController(VRFController* vrfController);

  VRFController* vrfController() const;

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  void reset();

 public slots:

  void addSystem(const OSItemId& itemid);

  void createNewSystem();

  void removeSystem(model::AirConditionerVariableRefrigerantFlow& vrfSystem);

 private:
  std::vector<model::AirConditionerVariableRefrigerantFlow> systems() const;

  int systemIndex(const model::AirConditionerVariableRefrigerantFlow& system) const;

  QPointer<VRFController> m_vrfController;
};

// A delegate to provide cells of the vrf system grid
class VRFSystemItemDelegate : public OSGraphicsItemDelegate
{
  Q_OBJECT;

 public:
  virtual ~VRFSystemItemDelegate() {}

  virtual QGraphicsObject* view(QSharedPointer<OSListItem> dataSource) override;
};

class VRFSystemListDropZoneItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit VRFSystemListDropZoneItem(OSListController* listController = nullptr);

  ~VRFSystemListDropZoneItem() {}
};

class VRFSystemListItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit VRFSystemListItem(const model::AirConditionerVariableRefrigerantFlow& vrfSystem, OSListController* listController = nullptr);

  virtual ~VRFSystemListItem() {}

  QString systemName() const;

  int numberOfConnectedZones() const;

  int numberOfConnectedTerminals() const;

 public slots:

  void remove();

  void zoomInOnSystem();

 private:
  model::AirConditionerVariableRefrigerantFlow m_vrfSystem;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_VRFCONTROLLER_HPP
