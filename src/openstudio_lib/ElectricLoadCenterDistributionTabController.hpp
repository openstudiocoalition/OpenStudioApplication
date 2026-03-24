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

#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <boost/optional.hpp>

class QGraphicsScene;
class QGraphicsLineItem;

namespace openstudio {

class ELCDView;
class ELCDScene;
class ELCDUtilityGridPanel;
class ELCDMainPanelItem;
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

 private:
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

  // Scene positions (set in constructor, reused in refreshNow)
  int m_kMainPanelX = 0;
  int m_kElcdGridX = 0;

  QVector<QGraphicsLineItem*> m_elcdConnectorLines;
};

// ─── ELCDListItem ─────────────────────────────────────────────────────────────

class ELCDListItem : public OSListItem
{
  Q_OBJECT

 public:
  explicit ELCDListItem(const model::ElectricLoadCenterDistribution& elcd, OSListController* listController = nullptr);
  virtual ~ELCDListItem() = default;

  QString name() const;
  QString bussType() const;
  model::ElectricLoadCenterDistribution elcd() const;

 public slots:
  void remove();
  void zoomIn();

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
