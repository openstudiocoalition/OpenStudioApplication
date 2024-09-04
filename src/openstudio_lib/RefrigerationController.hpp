/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_REFRIGERATIONCONTROLLER_HPP
#define OPENSTUDIO_REFRIGERATIONCONTROLLER_HPP

#include <QObject>
#include <boost/optional.hpp>
#include "../shared_gui_components/OSListController.hpp"
#include "../model_editor/QMetaTypes.hpp"
#include <openstudio/model/RefrigerationSystem.hpp>

class QGraphicsScene;
class QGraphicsView;
class QGraphicsObject;
class QWidget;

namespace openstudio {

class GridLayoutItem;
class RefrigerationSystemView;
class RefrigerationSystemListController;
class RefrigerationScene;
class RefrigerationSystemDetailView;
class OSItemId;
class RefrigerationView;
class NoRefrigerationView;
// TODO class RefrigerationGridView;

class RefrigerationController : public QObject
{
  Q_OBJECT

 public:
  RefrigerationController();

  virtual ~RefrigerationController();

  RefrigerationView* refrigerationView() const;

  NoRefrigerationView* noRefrigerationView() const;

  // TODO RefrigerationGridView * refrigerationGridView() const;

  QSharedPointer<RefrigerationSystemListController> refrigerationSystemListController() const;

  static boost::optional<model::RefrigerationSystem> cascadeSystem(const model::RefrigerationCondenserCascade& condenser);
  static boost::optional<model::RefrigerationSystem> supplySystem(const model::RefrigerationCondenserCascade& condenser);

  void refreshRefrigerationSystemView(RefrigerationSystemView* systemView, boost::optional<model::RefrigerationSystem>& system);

 public slots:

  void zoomInOnSystem(const Handle& handle);

  void zoomInOnSystem(const model::RefrigerationSystem& refrigerationSystem);

  void zoomOutToSystemGridView();

 private slots:

  void refresh();

  void refreshNow();

  void onCondenserViewDrop(const OSItemId& itemid);

  void onCompressorViewDrop(const OSItemId& itemid);

  void onSubCoolerViewDrop(const OSItemId& itemid);

  void onSHXViewDrop(const OSItemId& itemid);

  void onCasesViewDrop(const OSItemId& itemid);

  void onSecondaryViewDrop(const OSItemId& itemid);

  void removeCondenser(const OSItemId& itemid);

  void removeCompressor(const OSItemId& itemid);

  void removeCase(const OSItemId& itemid);

  void removeSubCooler(const OSItemId& itemid);

  void removeSubCoolerLiquidSuction(const OSItemId& itemid);

  void removeLoad(const Handle&);

  void inspectOSItem(const OSItemId& itemid);

 private:
  QPointer<RefrigerationView> m_refrigerationView;

  QPointer<GridLayoutItem> m_refrigerationSystemGridView;

  QPointer<RefrigerationSystemView> m_detailView;

  QSharedPointer<RefrigerationSystemListController> m_refrigerationSystemListController;

  QSharedPointer<QGraphicsScene> m_refrigerationGridScene;

  QPointer<QGraphicsScene> m_refrigerationScene;

  QPointer<NoRefrigerationView> m_noRefrigerationView;

  bool m_dirty;

  boost::optional<model::RefrigerationSystem> m_currentSystem;
};

class RefrigerationSystemListController : public OSListController
{
  Q_OBJECT

 public:
  explicit RefrigerationSystemListController(RefrigerationController* refrigerationController);
  RefrigerationController* refrigerationController() const;

  QSharedPointer<OSListItem> itemAt(int i) override;
  int count() override;

  void reset();

 signals:

  void itemInsertedPrivate(int i);

 public slots:

  void addSystem(const OSItemId& itemid);
  void createNewSystem();
  void removeSystem(model::RefrigerationSystem& refrigerationSystem);

 private slots:

  void onModelObjectAdd(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

 private:
  std::vector<model::RefrigerationSystem> systems() const;
  int systemIndex(const model::RefrigerationSystem& system) const;
  QPointer<RefrigerationController> m_refrigerationController;
};

// A delegate to provide cells of the refrigeration system grid
class RefrigerationSystemItemDelegate : public OSGraphicsItemDelegate
{
  Q_OBJECT;

 public:
  virtual ~RefrigerationSystemItemDelegate() = default;

  virtual QGraphicsObject* view(QSharedPointer<OSListItem> dataSource) override;
};

class RefrigerationSystemListDropZoneItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit RefrigerationSystemListDropZoneItem(OSListController* listController = nullptr);

  virtual ~RefrigerationSystemListDropZoneItem() = default;
};

class RefrigerationSystemListItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit RefrigerationSystemListItem(const model::RefrigerationSystem& refrigerationSystem, OSListController* listController = nullptr);
  virtual ~RefrigerationSystemListItem() = default;

  QString systemName() const;
  model::RefrigerationSystem system() const;

 public slots:

  void remove();
  void zoomInOnSystem();

 private:
  model::RefrigerationSystem m_refrigerationSystem;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_REFRIGERATIONCONTROLLER_HPP
