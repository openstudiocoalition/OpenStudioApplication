/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESSUBTABGRIDVIEW_HPP
#define OPENSTUDIO_SPACESSUBTABGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

class QComboBox;
class QLineEdit;

namespace openstudio {

class SpacesSubsurfacesGridController;

class SpacesSubtabGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  SpacesSubtabGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SpacesSubtabGridView() = default;

  QLineEdit* m_thermalZoneNameFilter = nullptr;

  QLineEdit* m_spaceNameFilter = nullptr;

  QComboBox* m_storyFilter = nullptr;

  QComboBox* m_thermalZoneFilter = nullptr;

  QComboBox* m_spaceTypeFilter = nullptr;

  QComboBox* m_subSurfaceTypeFilter = nullptr;

  QComboBox* m_loadTypeFilter = nullptr;

  QComboBox* m_windExposureFilter = nullptr;

  QComboBox* m_sunExposureFilter = nullptr;

  QComboBox* m_outsideBoundaryConditionFilter = nullptr;

  QComboBox* m_surfaceTypeFilter = nullptr;

  QComboBox* m_interiorPartitionGroupFilter = nullptr;

 protected:
  REGISTER_LOGGER("openstudio.SpacesSubtabGridView");

  virtual void addObject(const openstudio::IddObjectType& iddObjectType) override;

  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  void showStoryFilter();

  void showThermalZoneFilter();

  void showSpaceTypeFilter();

  void showSubSurfaceTypeFilter();

  void showSpaceNameFilter();

  void showWindExposureFilter();

  void showSunExposureFilter();

  void showOutsideBoundaryConditionFilter();

  void showSurfaceTypeFilter();

  void showInteriorPartitionGroupFilter();

  void showLoadTypeFilter();

  void initializeStoryFilter();

  void initializeThermalZoneFilter();

  void initializeSpaceTypeFilter();

  void initializeSubSurfaceTypeFilter();

  void initializeLoadTypeFilter();

  void initializeWindExposureFilter();

  void initializeSunExposureFilter();

  void initializeOutsideBoundaryConditionFilter();

  void initializeSurfaceTypeFilter();

  void initializeInteriorPartitionGroupFilter();

  void filterChanged();

  // All these sets will store the objects that DO NEED to be hidden
  // These objects should be the selector type objects
  // So if the set is empty -> show all
  std::set<openstudio::model::ModelObject> m_objectsFilteredByStory;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByThermalZone;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySpaceType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySubSurfaceType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySpaceName;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByWindExposure;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySunExposure;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByOutsideBoundaryCondition;

  std::set<openstudio::model::ModelObject> m_objectsFilteredBySurfaceType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByInteriorPartitionGroup;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByLoadType;

  QGridLayout* m_filterGridLayout = nullptr;

  std::vector<model::ModelObject> m_spacesModelObjects = std::vector<model::ModelObject>();

 protected slots:

  virtual void spaceNameFilterChanged();

  virtual void subSurfaceTypeFilterChanged(const QString& text);

  virtual void storyFilterChanged(const QString& text);

  virtual void thermalZoneFilterChanged(const QString& text);

  virtual void spaceTypeFilterChanged(const QString& text);

  virtual void loadTypeFilterChanged(const QString& text);

  virtual void windExposureFilterChanged(const QString& text);

  virtual void sunExposureFilterChanged(const QString& text);

  virtual void outsideBoundaryConditionFilterChanged(const QString& text);

  virtual void surfaceTypeFilterChanged(const QString& text);

  virtual void interiorPartitionGroupFilterChanged(const QString& text);

 private:
  // All of the subtabs except the main SpacesSpace subtab will return true
  // We use this in filterChanged
  virtual bool hasSubRows() {
    return true;
  };
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACESSUBTABGRIDVIEW_HPP
