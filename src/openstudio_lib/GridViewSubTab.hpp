/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GRIDVIEWSUBTAB_HPP
#define OPENSTUDIO_GRIDVIEWSUBTAB_HPP

#include "../shared_gui_components/OSGridController.hpp"
#include "../shared_gui_components/OSGridView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QWidget>

class QScrollArea;
class QVBoxLayout;
class OpenStudioLibFixture;

namespace openstudio {

class OSGridController;

class OSGridView;

class OSItemSelectorButtons;

class GridViewSubTab : public QWidget
{
  Q_OBJECT

 public:
  GridViewSubTab(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~GridViewSubTab() = default;

 protected:
  void setGridView(OSGridView* gridView);

  void setGridController(OSGridController* gridController);

  virtual void addObject(const openstudio::IddObjectType& iddObjectType) = 0;

  virtual void addObject(const openstudio::model::ModelObject& modelObject);

  virtual void copyObject(const openstudio::model::ModelObject& modelObject);

  virtual void removeObject(openstudio::model::ModelObject modelObject);

  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) = 0;

  virtual std::set<model::ModelObject> selectedObjects() const;

  model::Model m_model;

  OSGridView* m_gridView = nullptr;

  OSGridController* m_gridController = nullptr;

  bool m_isIP;

  bool m_displayAdditionalProps;

  QVBoxLayout* m_scrollLayout = nullptr;

  OSItemSelectorButtons* m_itemSelectorButtons = nullptr;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void dropZoneItemSelected(OSItem* item, bool readOnly);

  void selectionCleared();

 public slots:

  void onDropZoneItemClicked(OSItem* item);

  void onGridRowSelectionChanged(int numSelected, int numSelectable);

 protected slots:

  void onAddClicked();

  void onCopyClicked();

  void onRemoveClicked();

  void onPurgeClicked();

  void toggleUnits(bool isIP);

  void toggleDisplayAdditionalProps(bool displayAdditionalProps);

  virtual void onSelectItem() = 0;

  virtual void onClearSelection() = 0;

 private:
  // For testing
  friend class ::OpenStudioLibFixture;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GRIDVIEWSUBTAB_HPP
