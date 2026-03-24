/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP
#define OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP

#include "MainTabController.hpp"
#include "../shared_gui_components/OSListController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ElectricLoadCenterDistribution.hpp>
#include <openstudio/utilities/idf/Handle.hpp>

#include <QMap>
#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <boost/optional.hpp>

class QGraphicsScene;
class QGraphicsItem;

namespace openstudio {

class ELCDView;
class ELCDScene;
class ELCDUtilityGridPanel;
class ELCDMainPanelItem;
class ELCDDetailContainerItem;
class ELCDGeneratorItemView;
class GridLayoutItem;
class ELCDListController;
class OSItemId;

// ─── ElectricLoadCenterDistributionTabController ──────────────────────────────

class ElectricLoadCenterDistributionTabController : public MainTabController
{
  Q_OBJECT

 public:
  explicit ElectricLoadCenterDistributionTabController(const model::Model& model);
  virtual ~ElectricLoadCenterDistributionTabController();

 public slots:
  void zoomInOnELCD(const Handle& handle);
  void zoomOutToGridView();

 private slots:
  void refresh();
  void refreshNow();
  void onPowerInTransformerDrop(const OSItemId& itemId);
  void onPowerOutTransformerDrop(const OSItemId& itemId);
  void onPowerInTransformerClick();
  void onPowerOutTransformerClick();
  void onPowerInTransformerRemove();
  void onPowerOutTransformerRemove();

  // Detail view slots
  void onBussTypeChangeRequested(const QString& bussType);
  void onGenOpSchemeChangeRequested(const QString& scheme);
  void onValidateRequested();
  void onDetailGeneratorDrop(const OSItemId& itemId);
  void onDetailGeneratorRemove(const Handle& handle);
  void onDetailInverterDrop(const OSItemId& itemId);
  void onDetailInverterRemove();
  void onDetailStorageDrop(const OSItemId& itemId);
  void onDetailStorageRemove();
  void onDetailConverterDrop(const OSItemId& itemId);
  void onDetailConverterRemove();
  void onDetailLCPCTransformerDrop(const OSItemId& itemId);
  void onDetailLCPCTransformerRemove();
  void onDetailInverterClick();
  void onDetailStorageClick();
  void onDetailConverterClick();
  void onDetailLCPCTransformerClick();

  void onDetailGeneratorClick(const Handle& handle);
  void onDetailELCDInspect(const Handle& handle);
  void onDetailELCDNameChanged();
  void onDetailGeneratorNameChanged();

 private:
  void buildDetailScene(const model::ElectricLoadCenterDistribution& elcd);

  model::Model m_model;

  QPointer<ELCDView> m_elcdView;
  QPointer<GridLayoutItem> m_elcdGridView;

  QSharedPointer<QGraphicsScene> m_gridScene;
  QPointer<QGraphicsScene> m_detailScene;

  QSharedPointer<ELCDListController> m_listController;

  QPointer<ELCDUtilityGridPanel> m_utilityGridPanel;
  QPointer<ELCDMainPanelItem> m_mainPanelItem;

  boost::optional<model::ElectricLoadCenterDistribution> m_currentELCD;
  bool m_dirty = false;

  // Pointers to live-updatable detail view items (set in buildDetailScene)
  QPointer<ELCDDetailContainerItem> m_detailContainerItem;
  QMap<Handle, ELCDGeneratorItemView*> m_detailGeneratorViews;

  // Scene positions (set in constructor, reused in refreshNow)
  int m_kMainPanelX = 0;
  int m_kElcdGridX = 0;

  QVector<QGraphicsItem*> m_elcdConnectorItems;
};

// ─── ELCDListItem ─────────────────────────────────────────────────────────────

class ELCDListItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit ELCDListItem(const model::ElectricLoadCenterDistribution& elcd, OSListController* listController = nullptr);
  ~ELCDListItem() override;

  QString name() const;
  QString bussType() const;
  model::ElectricLoadCenterDistribution elcd() const;

 public slots:
  void remove();
  void zoomIn();

 private slots:
  void onNameChanged();

 private:
  model::ElectricLoadCenterDistribution m_elcd;
};

// ─── ELCDListDropZoneItem ─────────────────────────────────────────────────────

class ELCDListDropZoneItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit ELCDListDropZoneItem(OSListController* listController = nullptr);
  virtual ~ELCDListDropZoneItem() = default;
};

// ─── ELCDListController ───────────────────────────────────────────────────────

class ELCDListController : public OSListController
{
  Q_OBJECT

 public:
  explicit ELCDListController(ElectricLoadCenterDistributionTabController* tabController);

  ElectricLoadCenterDistributionTabController* tabController() const;

  QSharedPointer<OSListItem> itemAt(int i) override;
  int count() override;

  void reset();

 signals:
  void itemInsertedPrivate(int i);

 public:
  void notifyItemChanged(const model::ElectricLoadCenterDistribution& elcd);

 public slots:
  void createNewELCD();
  void addELCDFromDrop(const OSItemId& itemId);
  void removeELCD(model::ElectricLoadCenterDistribution& elcd);

 private slots:
  void onModelObjectAdd(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

 private:
  std::vector<model::ElectricLoadCenterDistribution> elcds() const;
  int elcdIndex(const model::ElectricLoadCenterDistribution& elcd) const;

  QPointer<ElectricLoadCenterDistributionTabController> m_tabController;
};

// ─── ELCDItemDelegate ─────────────────────────────────────────────────────────

class ELCDItemDelegate : public OSGraphicsItemDelegate
{
  Q_OBJECT;

 public:
  virtual ~ELCDItemDelegate() = default;

  QGraphicsObject* view(QSharedPointer<OSListItem> dataSource) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONTABCONTROLLER_HPP
