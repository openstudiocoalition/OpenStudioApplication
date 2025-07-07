/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP
#define SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP

#include "OSConcepts.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <vector>

#include <QSharedPointer>
#include <QWidget>
#include <QVBoxLayout>

class QButtonGroup;
class QCheckBox;
class QColor;
class QLabel;
class QPaintEvent;
class OpenStudioLibFixture;

namespace openstudio {

class GridCellLocation;
class GridCellInfo;
class OSGridView;
class OSGridController;
class OSObjectSelector;
class OSWidgetHolder;

// An OSCellWrapper has one or more OSWidgetHolders, one for each subrow in a cell
class OSCellWrapper : public QWidget
{
  Q_OBJECT

 public:
  OSCellWrapper(OSGridView* gridView, QSharedPointer<BaseConcept> baseConcept, OSObjectSelector* objectSelector, int modelRow, int gridRow,
                int column);

  virtual ~OSCellWrapper();

  void addOSWidget(QWidget* widget, const boost::optional<model::ModelObject>& obj, const bool isSelector, const bool isParent);

  void setGridController(OSGridController* gridController);
  void setModelObject(const model::ModelObject& modelObject);
  void refresh();

  void setCellProperties(const GridCellLocation& location, const GridCellInfo& info);

 private slots:

  void onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void processNewModelObjects();

 signals:

  void rowNeedsStyle(int modelRow, int gridRow);

 private:
  // for testing
  friend class ::OpenStudioLibFixture;

  // Make the lowest level os widgets that corresponds to concepts (e.g. OSLineEdit2).
  // These will be put in container widgets to form the cell, regardless of the presence of sub rows.
  QWidget* createOSWidget(model::ModelObject t_mo, const QSharedPointer<BaseConcept>& t_baseConcept);

  void connectModelSignals();
  void disconnectModelSignals();
  void makeHeader();
  void updateStyle();

  OSGridView* m_gridView;
  QGridLayout* m_layout;
  std::vector<OSWidgetHolder*> m_holders;
  QSharedPointer<BaseConcept> m_baseConcept;
  OSObjectSelector* m_objectSelector;
  int m_modelRow = 0;
  int m_gridRow = 0;
  int m_column = 0;
  bool m_hasSubRows = false;
  int m_refreshCount = 0;
  bool m_header = false;
  bool m_visible = true;

  // only has these members if not a header cell
  boost::optional<model::ModelObject> m_modelObject;
  OSGridController* m_gridController;

  // set when connecting to a model signals
  boost::optional<model::Model> m_connectedmodel;

  // temp variable
  std::set<model::ModelObject> m_newModelObjects;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP
