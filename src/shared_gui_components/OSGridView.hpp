/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSGRIDVIEW_HPP
#define SHAREDGUICOMPONENTS_OSGRIDVIEW_HPP

#include <QTimer>
#include <QWidget>

#include "../openstudio_lib/OSItem.hpp"

#include <openstudio/model/ModelObject.hpp>

class QGridLayout;
class QHideEvent;
class QVBoxLayout;
class QLabel;
class QShowEvent;
class QString;
class QLayoutItem;
class OpenStudioLibFixture;

namespace openstudio {

class OSCellWrapper;
class OSCollapsibleView;
class OSDropZone;
class OSGridController;
class OSItem;
class GridCellLocation;
class GridCellInfo;

class OSGridView : public QWidget
{
  Q_OBJECT

 public:
  OSGridView(OSGridController* gridController, const QString& headerText, const QString& dropZoneText, bool useHeader, QWidget* parent = nullptr);

  virtual ~OSGridView();

  // return the QLayoutItem at a particular row and column
  QLayoutItem* itemAtPosition(int row, int column);

  void showDropZone(bool visible);

  void addLayoutToContentLayout(QLayout* layout);

  void addSpacingToContentLayout(int spacing);

 protected:
  virtual void hideEvent(QHideEvent* event) override;

  virtual void showEvent(QShowEvent* event) override;

  virtual void resizeEvent(QResizeEvent* event) override;

 signals:

  void dropZoneItemClicked(OSItem* item);

  void gridRowSelectionChanged(int numSelected, int numSelectable);

 public slots:

  //void requestRemoveRow(int row);

  void onAddRow(int row);

  void onRecreateAll();

  void onGridCellChanged(const GridCellLocation& location, const GridCellInfo& info);

 private:
  // For testing
  friend class OpenStudioLibFixture;

  // uses the OSGridController to create the OSCellWrapper for row, column
  void createCellWrapper(int row, int column);

  // delete all widgets
  void deleteAll();

  // add a row
  void addRow(int row);

  // recreate all widgets
  void recreateAll();

  // convert row to layout index
  constexpr int layoutIndex(int row) const;

  // get row in layout
  constexpr int rowInLayout(int row) const;

  // update column widths
  void updateColumnWidths();

  // get width for column
  int widthForColumn(int column) const;

  // construct a grid layout to our specs
  QGridLayout* makeGridLayout();

  // Add a widget, adding a new layout if necessary
  void addCellWrapper(OSCellWrapper* w, int row, int column);

  OSDropZone* m_dropZone;

  QVBoxLayout* m_contentLayout;

  std::vector<QGridLayout*> m_gridLayouts;

  std::vector<int> m_columnWidths;

  OSCollapsibleView* m_collapsibleView;

  OSGridController* m_gridController;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSGRIDVIEW_HPP
