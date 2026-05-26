/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SpacesSubsurfacesGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/DaylightingDeviceShelf.hpp>
#include <openstudio/model/DaylightingDeviceShelf_Impl.hpp>
#include <openstudio/model/InteriorPartitionSurface.hpp>
#include <openstudio/model/InteriorPartitionSurface_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/ShadingControl.hpp>
#include <openstudio/model/ShadingControl_Impl.hpp>
#include <openstudio/model/ShadingSurface.hpp>
#include <openstudio/model/ShadingSurface_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/Surface_Impl.hpp>
#include <openstudio/model/SubSurface.hpp>
#include <openstudio/model/SubSurface_Impl.hpp>
#include <openstudio/model/WindowPropertyFrameAndDivider.hpp>
#include <openstudio/model/WindowPropertyFrameAndDivider_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QCheckBox>
#include <QCoreApplication>

// SHADINGCONTROLS "Shading Controls"

// FRAMEANDDIVIDER "Frame and Divider"

namespace openstudio {

SpacesSubsurfacesGridView::SpacesSubsurfacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, displayAdditionalProps, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSubSurfaceTypeFilter();
  showOutsideBoundaryConditionFilter();
  showSpaceNameFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController =
    new SpacesSubsurfacesGridController(isIP, displayAdditionalProps, tr("Space"), IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, tr("Space"), tr("Drop\nSpace"), false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  clearSelection();
}

SpacesSubsurfacesGridController::SpacesSubsurfacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                                 IddObjectType iddObjectType, const model::Model& model,
                                                                 const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void SpacesSubsurfacesGridView::removeObject(openstudio::model::ModelObject modelObject) {
  boost::optional<model::SubSurface> subSurface = modelObject.optionalCast<model::SubSurface>();
  if (subSurface) {
    boost::optional<model::SubSurface> adjacentSubSurface = subSurface->adjacentSubSurface();
    if (adjacentSubSurface) {
      adjacentSubSurface->remove();
    }
  }
  modelObject.remove();
}

void SpacesSubsurfacesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesSubsurfacesGridView::onClearSelection() {
  clearSelection();
}

void SpacesSubsurfacesGridView::clearSelection() {
  m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

void SpacesSubsurfacesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      tr("Subsurface Name"), tr("Parent Surface Name"), tr("Subsurface Type"),
      tr("Multiplier"),      tr("Construction"),        tr("Outside Boundary Condition Object"),
      //tr("Shading Surface Name"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      tr("Subsurface Name"), tr("Parent Surface Name"), tr("Shading Control"), tr("Shading Type"),
      //tr("Construction with Shading Name"),
      //tr("Shading Device Material Name"),
      tr("Shading Control Type"), tr("Schedule Name"),
      //tr("Setpoint"),                        IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //tr("Shading Control Is Scheduled"),       IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //tr("Glare Control Is Active"),            IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //tr("Type of Slat Angle Control for Blinds"), IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //tr("Slat Angle Schedule Name"),           IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //SETPOINT2,                       IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Shading Controls"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      tr("Subsurface Name"),
      tr("Parent Surface Name"),
      tr("Frame and Divider"),
      tr("Frame Width"),
      tr("Frame Outside Projection"),
      tr("Frame Inside Projection"),
      tr("Frame Conductance"),
      tr("Frame - Edge Glass Conductance to Center - Of - Glass Conductance"),
      tr("Frame Solar Absorptance"),
      tr("Frame Visible Absorptance"),
      tr("Frame Thermal Hemispherical Emissivity"),
      tr("Divider Type"),
      tr("Divider Width"),
      tr("Number of Horizontal Dividers"),
      tr("Number of Vertical Dividers"),
      tr("Divider Outside Projection"),
      tr("Divider Inside Projection"),
      tr("Divider Conductance"),
      tr("Ratio of Divider - Edge Glass Conductance to Center - Of - Glass Conductance"),
      tr("Divider Solar Absorptance"),
      tr("Divider Visible Absorptance"),
      tr("Divider Thermal Hemispherical Emissivity"),
      tr("Outside Reveal Depth"),
      tr("Outside Reveal Solar Absorptance"),
      tr("Inside Sill Depth"),
      tr("Inside Sill Solar Absorptance"),
      tr("Inside Reveal Depth"),
      tr("Inside Reveal Solar Absorptance"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Frame and Divider"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      tr("Subsurface Name"),
      tr("Parent Surface Name"),
      tr("Daylighting Shelf Name"),
      //tr("Window Name"),
      tr("Inside Shelf Name"),
      tr("Outside Shelf Name"),
      tr("View Factor to Outside Shelf"),
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(tr("Daylighting Shelves"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesSubsurfacesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesSubsurfacesGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {tr("Display Name"), tr("CAD Object ID")});
  }
  // always show name and selected columns
  fields.insert(fields.begin(), {tr("Space Name"), tr("All")});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == tr("Space Name")) {
      addParentNameLineEditColumn(Heading(tr("Space Name"), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else if (field == tr("Display Name")) {
      addNameLineEditColumn(Heading(tr("Display Name"), false, false),                       // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::displayName),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setDisplayName)  // setter
      );
    } else if (field == tr("CAD Object ID")) {
      addNameLineEditColumn(Heading(tr("CAD Object ID"), false, false),                      // heading
                            false,                                                           // isInspectable
                            false,                                                           // isLocked
                            DisplayNameAdapter<model::Space>(&model::Space::cadObjectId),    // getter
                            DisplayNameAdapter<model::Space>(&model::Space::setCADObjectId)  // setter
      );
    } else {

      std::function<std::vector<model::ModelObject>(const model::Space&)> allSurfaces([](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        auto surfaces = t_space.surfaces();
        allModelObjects.insert(allModelObjects.end(), surfaces.begin(), surfaces.end());
        return allModelObjects;
      });

      std::function<std::vector<model::ModelObject>(const model::Space&)> allSubSurfaces([allSurfaces](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        for (const auto& surface : allSurfaces(t_space)) {
          auto subSurfaces = surface.cast<model::Surface>().subSurfaces();
          allModelObjects.insert(allModelObjects.end(), subSurfaces.begin(), subSurfaces.end());
        }
        return allModelObjects;
      });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allSubsurfaceSurfaces(
        [allSurfaces](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          for (const auto& surface : allSurfaces(t_space)) {
            auto subSurfaces = surface.cast<model::Surface>().subSurfaces();
            for (const auto& subSurface : subSurfaces) {
              // TODO: What is this?! loop on surface.subsurfaces, then check subsurface.surface? Isn't that redundant?
              auto surf = subSurface.surface();
              if (surf) {
                allModelObjects.push_back(*surf);
              } else {
                allModelObjects.emplace_back();
              }
            }
          }
          return allModelObjects;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allDaylightingDeviceShelfs(
        [allSubSurfaces](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          std::vector<boost::optional<model::DaylightingDeviceShelf>> allDaylightingDeviceShelfs;
          for (const auto& subSurface : allSubSurfaces(t_space)) {
            auto daylightingDeviceShelf = subSurface.cast<model::SubSurface>().daylightingDeviceShelf();
            if (daylightingDeviceShelf) {
              allDaylightingDeviceShelfs.push_back(daylightingDeviceShelf);
            } else {
              allDaylightingDeviceShelfs.push_back(boost::optional<model::DaylightingDeviceShelf>());
            }
          }
          allModelObjects.insert(allModelObjects.end(), allDaylightingDeviceShelfs.begin(), allDaylightingDeviceShelfs.end());

          return allModelObjects;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allWindowPropertyFrameAndDividers(
        [allSubSurfaces](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          std::vector<boost::optional<model::WindowPropertyFrameAndDivider>> allWindowPropertyFrameAndDividers;
          for (const auto& subSurface : allSubSurfaces(t_space)) {
            auto windowPropertyFrameAndDivider = subSurface.cast<model::SubSurface>().windowPropertyFrameAndDivider();
            if (windowPropertyFrameAndDivider) {
              allWindowPropertyFrameAndDividers.push_back(windowPropertyFrameAndDivider);
            } else {
              allWindowPropertyFrameAndDividers.push_back(boost::optional<model::WindowPropertyFrameAndDivider>());
            }
          }
          allModelObjects.insert(allModelObjects.end(), allWindowPropertyFrameAndDividers.begin(), allWindowPropertyFrameAndDividers.end());

          return allModelObjects;
        });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allShadingControls(
        [allSubSurfaces](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          std::vector<boost::optional<model::ShadingControl>> allShadingControls;
          for (const auto& subSurface : allSubSurfaces(t_space)) {
          // TODO: temporary workaround, see Shading Control Enhancements #239
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
            auto shadingControl = subSurface.cast<model::SubSurface>().shadingControl();
#if defined(_MSC_VER)
#  pragma warning(pop)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif
            if (shadingControl) {
              allShadingControls.push_back(shadingControl);
            } else {
              allShadingControls.push_back(boost::optional<model::ShadingControl>());
            }
          }
          allModelObjects.insert(allModelObjects.end(), allShadingControls.begin(), allShadingControls.end());

          return allModelObjects;
        });

      if (field == tr("All")) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip(tr("Check to select all rows"));
        connect(checkbox.data(), &OSSelectAllCheckBox::checkStateChanged, this, &SpacesSubsurfacesGridController::onSelectAllStateChanged);
        connect(this, &SpacesSubsurfacesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(tr("All"), false, false, checkbox), tr("Check to select this row").toStdString(), DataSource(allSubSurfaces, true));
      }

      else if (field == tr("Parent Surface Name")) {
        addNameLineEditColumn(
          Heading(tr("Parent Surface Name"), true, false), false, false, CastNullAdapter<model::Surface>(&model::Surface::name),
          CastNullAdapter<model::Surface>(&model::Surface::setName),
          boost::optional<std::function<void(model::Surface*)>>(std::function<void(model::Surface*)>([](model::Surface* t_s) { t_s->remove(); })),
          boost::optional<std::function<bool(model::Surface*)>>(),  // isDefaulted
          DataSource(allSubsurfaceSurfaces, true));
      } else if (field == tr("Subsurface Name")) {
        addNameLineEditColumn(Heading(tr("Subsurface Name"), true, false), false, false, CastNullAdapter<model::SubSurface>(&model::SubSurface::name),
                              CastNullAdapter<model::SubSurface>(&model::SubSurface::setName),
                              boost::optional<std::function<void(model::SubSurface*)>>(
                                std::function<void(model::SubSurface*)>([](model::SubSurface* t_s) { t_s->remove(); })),
                              boost::optional<std::function<bool(model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == tr("Subsurface Type")) {
        addComboBoxColumn<std::string, model::SubSurface>(
          Heading(tr("Subsurface Type")),
          std::function<std::string(const std::string&)>(static_cast<std::string (*)(const std::string&)>(&openstudio::toString)),
          std::function<std::vector<std::string>()>(&model::SubSurface::validSubSurfaceTypeValues),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::subSurfaceType),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::setSubSurfaceType),
          boost::optional<std::function<void(model::SubSurface*)>>(
            CastNullAdapter<model::SubSurface>(&model::SubSurface::resetSubSurfaceType)),  // New since 3.1.0
          boost::optional<std::function<bool(model::SubSurface*)>>(
            CastNullAdapter<model::SubSurface>(&model::SubSurface::isSubSurfaceTypeDefaulted)),  // New since 3.1.0
          DataSource(allSubSurfaces, true));
      } else if (field == tr("Multiplier")) {
        addValueEditColumn(Heading(tr("Multiplier")), NullAdapter(&model::SubSurface::multiplier), NullAdapter(&model::SubSurface::setMultiplier),
                           boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetMultiplier)),
                           boost::optional<std::function<bool(model::SubSurface*)>>(NullAdapter(&model::SubSurface::isMultiplierDefaulted)),
                           DataSource(allSubSurfaces, true));
      } else if (field == tr("Construction")) {
        setConstructionColumn(6);
        addDropZoneColumn(Heading(tr("Construction")), CastNullAdapter<model::SubSurface>(&model::SubSurface::construction),
                          CastNullAdapter<model::SubSurface>(&model::SubSurface::setConstruction),
                          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetConstruction)),
                          boost::optional<std::function<bool(model::SubSurface*)>>(NullAdapter(&model::SubSurface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(),
                          DataSource(allSubSurfaces, true));
      } else if (field == tr("Outside Boundary Condition Object")) {
        std::function<bool(model::SubSurface*, const model::SubSurface&)> setter([](model::SubSurface* t_subSurface, const model::SubSurface& t_arg) {
          auto copy = t_arg;
          return t_subSurface->setAdjacentSubSurface(copy);
        });

        addDropZoneColumn(
          Heading(tr("Outside Boundary Condition Object"), true, false), CastNullAdapter<model::SubSurface>(&model::SubSurface::adjacentSubSurface),
          setter, boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetAdjacentSubSurface)),
          boost::optional<std::function<bool(model::SubSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == tr("Shading Surface Name")) {

      } else if (field == tr("Shading Control")) {

        // TODO: temporary workaround, see Shading Control Enhancements #239
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        std::function<bool(model::SubSurface*, const model::ShadingControl&)> setter(
          [](model::SubSurface* t_surface, const model::ShadingControl& t_arg) {
            return const_cast<model::ShadingControl&>(t_arg).addSubSurface(*t_surface);
          });

        addDropZoneColumn(Heading(tr("Shading Control")), CastNullAdapter<model::SubSurface>(&model::SubSurface::shadingControl), setter,
                          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetShadingControl)),
                          boost::optional<std::function<bool(model::SubSurface*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(),
                          DataSource(allSubSurfaces, true));
#if defined(_MSC_VER)
#  pragma warning(pop)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif
      } else if (field == tr("Shading Type")) {
        addComboBoxColumn<std::string, model::ShadingControl>(
          Heading(tr("Shading Type"), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ShadingControl::shadingTypeValues),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingType),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingType),
          boost::optional<std::function<void(model::ShadingControl*)>>(), DataSource(allShadingControls, true));
      } else if (field == tr("Construction with Shading Name")) {
        // ShadingControl
        //boost::optional<Construction> construction() const;
      } else if (field == tr("Shading Device Material Name")) {
        // ShadingControl
        //boost::optional<ShadingMaterial> shadingMaterial() const;
      } else if (field == tr("Shading Control Type")) {
        addComboBoxColumn<std::string, model::ShadingControl>(
          Heading(tr("Shading Control Type"), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ShadingControl::shadingControlTypeValues),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingControlType),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingControlType),
          boost::optional<std::function<void(model::ShadingControl*)>>(NullAdapter(&model::ShadingControl::resetShadingControlType)),
          boost::optional<std::function<bool(model::ShadingControl*)>>(NullAdapter(&model::ShadingControl::isShadingControlTypeDefaulted)),
          DataSource(allShadingControls, true));
      } else if (field == tr("Schedule Name")) {
        std::function<bool(model::ShadingControl*, const model::Schedule&)> setter(
          [](model::ShadingControl* t_shadingControl, const model::Schedule& t_arg) {
            auto copy = t_arg;
            return t_shadingControl->setSchedule(copy);
          });

        addDropZoneColumn(
          Heading(tr("Schedule Name"), true, false), NullAdapter(&model::ShadingControl::schedule), setter,
          boost::optional<std::function<void(model::ShadingControl*)>>(CastNullAdapter<model::ShadingControl>(&model::ShadingControl::resetSchedule)),
          boost::optional<std::function<bool(model::ShadingControl*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingControl*)>>(), DataSource(allShadingControls, true));
      } else if (field == tr("Frame and Divider")) {
        addDropZoneColumn(
          Heading(tr("Frame and Divider")), CastNullAdapter<model::SubSurface>(&model::SubSurface::windowPropertyFrameAndDivider),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::setWindowPropertyFrameAndDivider),
          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetWindowPropertyFrameAndDivider)),
          boost::optional<std::function<bool(model::SubSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == tr("Frame Width")) {
        addValueEditColumn(Heading(tr("Frame Width"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameWidth),
                           NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameWidth),
                           boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameWidth)),
                           boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameWidthDefaulted)),
                           DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Outside Projection")) {
        addValueEditColumn(
          Heading(tr("Frame Outside Projection"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameOutsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameOutsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameOutsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Inside Projection")) {
        addValueEditColumn(
          Heading(tr("Frame Inside Projection"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameInsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameInsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameInsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameInsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Conductance")) {
        addValueEditColumn(
          Heading(tr("Frame Conductance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame - Edge Glass Conductance to Center - Of - Glass Conductance")) {
        addValueEditColumn(
          Heading(tr("Frame - Edge Glass Conductance to Center - Of - Glass Conductance"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::ratioOfFrameEdgeGlassConductanceToCenterOfGlassConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::resetRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Solar Absorptance")) {
        addValueEditColumn(
          Heading(tr("Frame Solar Absorptance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Visible Absorptance")) {
        addValueEditColumn(
          Heading(tr("Frame Visible Absorptance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameVisibleAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameVisibleAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameVisibleAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameVisibleAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Frame Thermal Hemispherical Emissivity")) {
        addValueEditColumn(
          Heading(tr("Frame Thermal Hemispherical Emissivity"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::frameThermalHemisphericalEmissivity),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameThermalHemisphericalEmissivity),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameThermalHemisphericalEmissivity)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isFrameThermalHemisphericalEmissivityDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Type")) {
        addComboBoxColumn<std::string, model::WindowPropertyFrameAndDivider>(
          Heading(tr("Divider Type"), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::WindowPropertyFrameAndDivider::dividerTypeValues),
          CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::dividerType),
          CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::setDividerType),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            NullAdapter(&model::WindowPropertyFrameAndDivider::resetDividerType)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            NullAdapter(&model::WindowPropertyFrameAndDivider::isDividerTypeDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Width")) {
        addValueEditColumn(Heading(tr("Divider Width"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerWidth),
                           NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerWidth),
                           boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerWidth)),
                           boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerWidthDefaulted)),
                           DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Number of Horizontal Dividers")) {
        addValueEditColumn(
          Heading(tr("Number of Horizontal Dividers"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::numberOfHorizontalDividers),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setNumberOfHorizontalDividers),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetNumberOfHorizontalDividers)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isNumberOfHorizontalDividersDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Number of Vertical Dividers")) {
        addValueEditColumn(
          Heading(tr("Number of Vertical Dividers"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::numberOfVerticalDividers),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setNumberOfVerticalDividers),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetNumberOfVerticalDividers)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isNumberOfVerticalDividersDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Outside Projection")) {
        addValueEditColumn(
          Heading(tr("Divider Outside Projection"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerOutsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerOutsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerOutsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerOutsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Inside Projection")) {
        addValueEditColumn(
          Heading(tr("Divider Inside Projection"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerInsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerInsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerInsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerInsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Conductance")) {
        addValueEditColumn(
          Heading(tr("Divider Conductance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Ratio of Divider - Edge Glass Conductance to Center - Of - Glass Conductance")) {
        addValueEditColumn(
          Heading(tr("Ratio of Divider - Edge Glass Conductance to Center - Of - Glass Conductance"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::ratioOfDividerEdgeGlassConductanceToCenterOfGlassConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::resetRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Solar Absorptance")) {
        addValueEditColumn(
          Heading(tr("Divider Solar Absorptance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Visible Absorptance")) {
        addValueEditColumn(
          Heading(tr("Divider Visible Absorptance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerVisibleAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerVisibleAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerVisibleAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerVisibleAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Divider Thermal Hemispherical Emissivity")) {
        addValueEditColumn(
          Heading(tr("Divider Thermal Hemispherical Emissivity"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::dividerThermalHemisphericalEmissivity),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerThermalHemisphericalEmissivity),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerThermalHemisphericalEmissivity)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isDividerThermalHemisphericalEmissivityDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Outside Reveal Depth")) {
        addValueEditColumn(
          Heading(tr("Outside Reveal Depth"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::outsideRevealDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setOutsideRevealDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetOutsideRevealDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isOutsideRevealDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Outside Reveal Solar Absorptance")) {
        addValueEditColumn(
          Heading(tr("Outside Reveal Solar Absorptance"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::outsideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setOutsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetOutsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isOutsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Inside Sill Depth")) {
        addValueEditColumn(
          Heading(tr("Inside Sill Depth"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideSillDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideSillDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideSillDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideSillDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Inside Sill Solar Absorptance")) {
        addValueEditColumn(
          Heading(tr("Inside Sill Solar Absorptance"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideSillSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideSillSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideSillSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideSillSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Inside Reveal Depth")) {
        addValueEditColumn(
          Heading(tr("Inside Reveal Depth"), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Inside Reveal Solar Absorptance")) {
        addValueEditColumn(
          Heading(tr("Inside Reveal Solar Absorptance"), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == tr("Window Name")) {
      } else if (field == tr("Daylighting Shelf Name")) {
        addNameLineEditColumn(Heading(tr("Daylighting Shelf Name")), false, false,
                              CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::name),
                              CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::setName),
                              boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                              boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(), DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == tr("Inside Shelf Name")) {

        std::function<bool(model::DaylightingDeviceShelf*, const model::InteriorPartitionSurface&)> setter(
          [](model::DaylightingDeviceShelf* t_shelf, const model::InteriorPartitionSurface& t_arg) { return t_shelf->setInsideShelf(t_arg); });

        addDropZoneColumn(Heading(tr("Inside Shelf Name")),
                          CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::insideShelf), setter,
                          boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::DaylightingDeviceShelf*)>>(),
                          DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == tr("Outside Shelf Name")) {

        std::function<bool(model::DaylightingDeviceShelf*, const model::ShadingSurface&)> setter(
          [](model::DaylightingDeviceShelf* t_shelf, const model::ShadingSurface& t_arg) { return t_shelf->setOutsideShelf(t_arg); });

        addDropZoneColumn(Heading(tr("Outside Shelf Name")),
                          CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::outsideShelf), setter,
                          boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::DaylightingDeviceShelf*)>>(),
                          DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == tr("View Factor to Outside Shelf")) {
        addValueEditColumn(
          Heading(tr("View Factor to Outside Shelf")), NullAdapter(&model::DaylightingDeviceShelf::viewFactortoOutsideShelf),
          NullAdapter(&model::DaylightingDeviceShelf::setViewFactortoOutsideShelf),
          //boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::resetViewFactortoOutsideShelf)),
          //boost::optional<std::function<bool(model::DaylightingDeviceShelf *)>>(),
          DataSource(allDaylightingDeviceShelfs, true));
      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString SpacesSubsurfacesGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesSubsurfacesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesSubsurfacesGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesSubsurfacesGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
