/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MAINRIGHTCOLUMNCONTROLLER_HPP
#define OPENSTUDIO_MAINRIGHTCOLUMNCONTROLLER_HPP

#include "../shared_gui_components/OSQObjectController.hpp"
#include <boost/smart_ptr.hpp>
#include <openstudio/model/Model.hpp>
#include "../shared_gui_components/EditController.hpp"
#include "../shared_gui_components/LocalLibraryController.hpp"

#include <QSharedPointer>

class QStackedWidget;

namespace openstudio {

class EditController;
class HorizontalTabWidget;
class InspectorController;
class LocalLibraryController;
class SystemItem;
class OSItem;

class MainRightColumnController : public OSQObjectController
{
  Q_OBJECT

 public:
  MainRightColumnController(const model::Model& model, const openstudio::path& resourcesPath);

  virtual ~MainRightColumnController(){};

  HorizontalTabWidget* mainRightColumnView() const;

  QSharedPointer<LocalLibraryController> measureLibraryController();

  void hideMyModelTab(bool hide);

  bool isMyModelTabHidden() {
    return m_myModelTabIsHidden;
  }

  std::shared_ptr<InspectorController> inspectorController() {
    return m_inspectorController;
  };

  enum RightColumnTabID
  {
    MY_MODEL,
    LIBRARY,
    EDIT
  };

  // Methods to store and retrieve SystemItem objects.
  // These are used to get plenum colors and potentially other tasks.
  void registerSystemItem(const Handle& systemHandle, SystemItem* systemItem);
  void unregisterSystemItem(const Handle& systemHandle);
  SystemItem* systemItem(const Handle& systemHandle) const;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void itemRemoveClicked(OSItem*);

 public slots:

  void configureForSiteSubTab(int subTabID);

  void configureForSchedulesSubTab(int subTabID);

  void configureForConstructionsSubTab(int subTabID);

  void configureForGeometrySubTab(int subTabID);

  void configureForLoadsSubTab(int subTabID);

  void configureForSpaceTypesSubTab(int subTabID);

  void configureForFacilitySubTab(int subTabID);

  void configureForSpacesSubTab(int subTabID);

  void configureForThermalZonesSubTab(int subTabID);

  void configureForHVACSystemsSubTab(int subTabID);

  void configureForOutputVariablesSubTab(int subTabID);

  void configureForSimulationSettingsSubTab(int subTabID);

  void configureForScriptsSubTab(int subTabID);

  void configureForRunSimulationSubTab(int subTabID);

  void configureForResultsSummarySubTab(int subTabID);

  void inspectModelObject(model::OptionalModelObject& modelObject, bool readOnly);

  void inspectModelObjectByItem(OSItem* item, bool readOnly);

  void toggleUnits(bool displayIP);

  void chooseEditTab();

  QSharedPointer<LocalLibraryController> measuresLibraryController();

  QSharedPointer<EditController> measuresEditController();

 private:
  REGISTER_LOGGER("openstudio.openstudio_lib.MainRightColumnController");

  void setMyModelView(QWidget* widget);

  void setLibraryView(QWidget* widget);

  void setEditView(QWidget* widget);

  HorizontalTabWidget* m_horizontalTabWidget;

  std::shared_ptr<InspectorController> m_inspectorController;

  QStackedWidget* m_myModelView;

  QStackedWidget* m_libraryView;

  QStackedWidget* m_editView;

  std::map<Handle, SystemItem*> m_systemItemMap;

  model::Model m_model;

  openstudio::path m_resourcesPath;

  QSharedPointer<LocalLibraryController> m_measureLibraryController;

  QSharedPointer<EditController> m_measureEditController;

  bool m_myModelTabIsHidden;

  OSItem* m_item = nullptr;

 private slots:

  void onItemRemoveClicked(OSItem*);

  void emitItemRemoveClicked(OSItem*);

  void onRemoveButtonClicked(bool checked);

  void onWorkspaceObjectRemoved();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MAINRIGHTCOLUMNCONTROLLER_HPP
