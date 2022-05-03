/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Space Name"
#define SELECTED "All"

// ALL GRID BUTTONS
#define SURFACENAME "Parent Surface Name"  // read only
#define SUBSURFACENAME "Subsurface Name"

// GENERAL "General"
#define SUBSURFACETYPE "Subsurface Type"
#define MULTIPLIER "Multiplier"
#define CONSTRUCTION "Construction"
#define OUTSIDEBOUNDARYCONDITIONOBJECT "Outside Boundary Condition Object"  // Dan note: needs model method for suggestions
#define SHADINGSURFACENAME "Shading Surface Name"                           // read only

// SHADINGCONTROLS "Shading Controls"
#define SHADINGCONTROLNAME "Shading Control"
#define SHADINGTYPE "Shading Type"
#define CONSTRUCTIONWITHSHADINGNAME "Construction with Shading Name"
#define SHADINGDEVICEMATERIALNAME "Shading Device Material Name"
#define SHADINGCONTROLTYPE "Shading Control Type"
#define SCHEDULENAME "Schedule Name"
#define SETPOINT "Setpoint"
#define SHADINGCONTROLISSCHEDULED "Shading Control Is Scheduled"
#define GLARECONTROLISACTIVE "Glare Control Is Active"
#define TYPEOFSLATANGLECONTROLFORBLINDS "Type of Slat Angle Control for Blinds"
#define SLATANGLESCHEDULENAME "Slat Angle Schedule Name"
#define SETPOINT2 "Setpoint 2"

// FRAMEANDDIVIDER "Frame and Divider"
#define FRAMEANDDIVIDERNAME "Frame and Divider"
#define FRAMEWIDTH "Frame Width"
#define FRAMEOUTSIDEPROJECTION "Frame Outside Projection"
#define FRAMEINSIDEPROJECTION "Frame Inside Projection"
#define FRAMECONDUCTANCE "Frame Conductance"
#define FRAMEEDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE "Frame - Edge Glass Conductance to Center - Of - Glass Conductance"
#define FRAMESOLARABSORPTANCE "Frame Solar Absorptance"
#define FRAMEVISIBLEABSORPTANCE "Frame Visible Absorptance"
#define FRAMETHERMALHEMISPHERICALEMISSIVITY "Frame Thermal Hemispherical Emissivity"
#define DIVIDERTYPE "Divider Type"
#define DIVIDERWIDTH "Divider Width"
#define NUMBEROFHORIZONTALDIVIDERS "Number of Horizontal Dividers"
#define NUMBEROFVERTICALDIVIDERS "Number of Vertical Dividers"
#define DIVIDEROUTSIDEPROJECTION "Divider Outside Projection"
#define DIVIDERINSIDEPROJECTION "Divider Inside Projection"
#define DIVIDERCONDUCTANCE "Divider Conductance"
#define RATIOOFDIVIDEREDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE "Ratio of Divider - Edge Glass Conductance to Center - Of - Glass Conductance"
#define DIVIDERSOLARABSORPTANCE "Divider Solar Absorptance"
#define DIVIDERVISIBLEABSORPTANCE "Divider Visible Absorptance"
#define DIVIDERTHERMALHEMISPHERICALEMISSIVITY "Divider Thermal Hemispherical Emissivity"
#define OUTSIDEREVEALDEPTH "Outside Reveal Depth"
#define OUTSIDEREVEALSOLARABSORPTANCE "Outside Reveal Solar Absorptance"
#define INSIDESILLDEPTH "Inside Sill Depth"
#define INSIDESILLSOLARABSORPTANCE "Inside Sill Solar Absorptance"
#define INSIDEREVEALDEPTH "Inside Reveal Depth"
#define INSIDEREVEALSOLARABSORPTANCE "Inside Reveal Solar Absorptance"

// DAYLIGHTINGSHELVES "Daylighting Shelves"
#define DAYLIGHTINGSHELFNAME "Daylighting Shelf Name"
#define WINDOWNAME "Window Name"
#define INSIDESHELFNAME "Inside Shelf Name"    // Dan note: drop down need a model method for suggestions
#define OUTSIDESHELFNAME "Outside Shelf Name"  // Dan note: drop down need a model method for suggestions
#define VIEWFACTORTOOUTSIDESHELF "View Factor to Outside Shelf"

namespace openstudio {

SpacesSubsurfacesGridView::SpacesSubsurfacesGridView(bool isIP, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSubSurfaceTypeFilter();
  showOutsideBoundaryConditionFilter();
  showSpaceNameFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesSubsurfacesGridController(isIP, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);

  onClearSelection();
}

SpacesSubsurfacesGridController::SpacesSubsurfacesGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                                 const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void SpacesSubsurfacesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void SpacesSubsurfacesGridView::onClearSelection() {
  m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

void SpacesSubsurfacesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      SUBSURFACENAME, SURFACENAME, SUBSURFACETYPE, MULTIPLIER, CONSTRUCTION, OUTSIDEBOUNDARYCONDITIONOBJECT,
      //  SHADINGSURFACENAME,
    };
    addCategoryAndFields(QString("General"), std::move(fields));
  }

  {
    std::vector<QString> fields{
      SUBSURFACENAME, SURFACENAME, SHADINGCONTROLNAME, SHADINGTYPE,
      //  CONSTRUCTIONWITHSHADINGNAME,
      //  SHADINGDEVICEMATERIALNAME,
      SHADINGCONTROLTYPE, SCHEDULENAME,
      //  SETPOINT,                        IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //  SHADINGCONTROLISSCHEDULED,       IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //  GLARECONTROLISACTIVE,            IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //  TYPEOFSLATANGLECONTROLFORBLINDS, IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //  SLATANGLESCHEDULENAME,           IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
      //  SETPOINT2,                       IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
    };
    addCategoryAndFields(QString("Shading Controls"), std::move(fields));
  }

  {
    std::vector<QString> fields{
      SUBSURFACENAME,
      SURFACENAME,
      FRAMEANDDIVIDERNAME,
      FRAMEWIDTH,
      FRAMEOUTSIDEPROJECTION,
      FRAMEINSIDEPROJECTION,
      FRAMECONDUCTANCE,
      FRAMEEDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE,
      FRAMESOLARABSORPTANCE,
      FRAMEVISIBLEABSORPTANCE,
      FRAMETHERMALHEMISPHERICALEMISSIVITY,
      DIVIDERTYPE,
      DIVIDERWIDTH,
      NUMBEROFHORIZONTALDIVIDERS,
      NUMBEROFVERTICALDIVIDERS,
      DIVIDEROUTSIDEPROJECTION,
      DIVIDERINSIDEPROJECTION,
      DIVIDERCONDUCTANCE,
      RATIOOFDIVIDEREDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE,
      DIVIDERSOLARABSORPTANCE,
      DIVIDERVISIBLEABSORPTANCE,
      DIVIDERTHERMALHEMISPHERICALEMISSIVITY,
      OUTSIDEREVEALDEPTH,
      OUTSIDEREVEALSOLARABSORPTANCE,
      INSIDESILLDEPTH,
      INSIDESILLSOLARABSORPTANCE,
      INSIDEREVEALDEPTH,
      INSIDEREVEALSOLARABSORPTANCE,
    };
    addCategoryAndFields(QString("Frame and Divider"), std::move(fields));
  }

  {
    std::vector<QString> fields{
      SUBSURFACENAME,
      SURFACENAME,
      DAYLIGHTINGSHELFNAME,
      //  WINDOWNAME,
      INSIDESHELFNAME,
      OUTSIDESHELFNAME,
      VIEWFACTORTOOUTSIDESHELF,
    };
    addCategoryAndFields(QString("Daylighting Shelves"), std::move(fields));
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesSubsurfacesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesSubsurfacesGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::Space>(&model::Space::name),
                                  CastNullAdapter<model::Space>(&model::Space::setName));
    } else {

      std::function<std::vector<model::ModelObject>(const model::Space&)> allSurfaces([](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        auto surfaces = t_space.surfaces();
        allModelObjects.insert(allModelObjects.end(), surfaces.begin(), surfaces.end());
        return allModelObjects;
      });

      std::function<std::vector<model::ModelObject>(const model::Space&)> allSubSurfaces([allSurfaces](const model::Space& t_space) {
        std::vector<model::ModelObject> allModelObjects;
        for (auto surface : allSurfaces(t_space)) {
          auto subSurfaces = surface.cast<model::Surface>().subSurfaces();
          allModelObjects.insert(allModelObjects.end(), subSurfaces.begin(), subSurfaces.end());
        }
        return allModelObjects;
      });

      std::function<std::vector<boost::optional<model::ModelObject>>(const model::Space&)> allSubsurfaceSurfaces(
        [allSurfaces](const model::Space& t_space) {
          std::vector<boost::optional<model::ModelObject>> allModelObjects;
          for (auto surface : allSurfaces(t_space)) {
            auto subSurfaces = surface.cast<model::Surface>().subSurfaces();
            for (auto subSurface : subSurfaces) {
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
          for (auto subSurface : allSubSurfaces(t_space)) {
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
          for (auto subSurface : allSubSurfaces(t_space)) {
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
          for (auto subSurface : allSubSurfaces(t_space)) {
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

      if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpacesSubsurfacesGridController::onSelectAllStateChanged);
        connect(this, &SpacesSubsurfacesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row", DataSource(allSubSurfaces, true));
      }

      else if (field == SURFACENAME) {
        addNameLineEditColumn(
          Heading(QString(SURFACENAME), true, false), false, false, CastNullAdapter<model::Surface>(&model::Surface::name),
          CastNullAdapter<model::Surface>(&model::Surface::setName),
          boost::optional<std::function<void(model::Surface*)>>(std::function<void(model::Surface*)>([](model::Surface* t_s) { t_s->remove(); })),
          boost::optional<std::function<bool(model::Surface*)>>(), DataSource(allSubsurfaceSurfaces, true));
      } else if (field == SUBSURFACENAME) {
        addNameLineEditColumn(Heading(QString(SUBSURFACENAME), true, false), false, false,
                              CastNullAdapter<model::SubSurface>(&model::SubSurface::name),
                              CastNullAdapter<model::SubSurface>(&model::SubSurface::setName),
                              boost::optional<std::function<void(model::SubSurface*)>>(
                                std::function<void(model::SubSurface*)>([](model::SubSurface* t_s) { t_s->remove(); })),
                              boost::optional<std::function<bool(model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == SUBSURFACETYPE) {
        addComboBoxColumn<std::string, model::SubSurface>(
          Heading(QString(SUBSURFACETYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::SubSurface::validSubSurfaceTypeValues),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::subSurfaceType),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::setSubSurfaceType),
          boost::optional<std::function<void(model::SubSurface*)>>(
            CastNullAdapter<model::SubSurface>(&model::SubSurface::resetSubSurfaceType)),  // New since 3.1.0
          boost::optional<std::function<bool(model::SubSurface*)>>(
            CastNullAdapter<model::SubSurface>(&model::SubSurface::isSubSurfaceTypeDefaulted)),  // New since 3.1.0
          boost::optional<DataSource>(allSubSurfaces, true));
      } else if (field == MULTIPLIER) {
        addValueEditColumn(Heading(QString(MULTIPLIER)), NullAdapter(&model::SubSurface::multiplier), NullAdapter(&model::SubSurface::setMultiplier),
                           boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetMultiplier)),
                           boost::optional<std::function<bool(model::SubSurface*)>>(NullAdapter(&model::SubSurface::isMultiplierDefaulted)),
                           DataSource(allSubSurfaces, true));
      } else if (field == CONSTRUCTION) {
        setConstructionColumn(6);
        addDropZoneColumn(Heading(QString(CONSTRUCTION)), CastNullAdapter<model::SubSurface>(&model::SubSurface::construction),
                          CastNullAdapter<model::SubSurface>(&model::SubSurface::setConstruction),
                          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetConstruction)),
                          boost::optional<std::function<bool(model::SubSurface*)>>(NullAdapter(&model::SubSurface::isConstructionDefaulted)),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(),
                          DataSource(allSubSurfaces, true));
      } else if (field == OUTSIDEBOUNDARYCONDITIONOBJECT) {
        std::function<bool(model::SubSurface*, const model::SubSurface&)> setter([](model::SubSurface* t_subSurface, const model::SubSurface& t_arg) {
          auto copy = t_arg;
          return t_subSurface->setAdjacentSubSurface(copy);
        });

        addDropZoneColumn(
          Heading(QString(OUTSIDEBOUNDARYCONDITIONOBJECT), true, false), CastNullAdapter<model::SubSurface>(&model::SubSurface::adjacentSubSurface),
          setter, boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetAdjacentSubSurface)),
          boost::optional<std::function<bool(model::SubSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == SHADINGSURFACENAME) {

      } else if (field == SHADINGCONTROLNAME) {

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

        addDropZoneColumn(Heading(QString(SHADINGCONTROLNAME)), CastNullAdapter<model::SubSurface>(&model::SubSurface::shadingControl), setter,
                          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetShadingControl)),
                          boost::optional<std::function<bool(model::SubSurface*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(),
                          DataSource(allSubSurfaces, true));
#if defined(_MSC_VER)
#  pragma warning(pop)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif
      } else if (field == SHADINGTYPE) {
        addComboBoxColumn<std::string, model::ShadingControl>(
          Heading(QString(SHADINGTYPE), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ShadingControl::shadingTypeValues),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingType),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingType),
          boost::optional<std::function<void(model::ShadingControl*)>>(), DataSource(allShadingControls, true));
      } else if (field == CONSTRUCTIONWITHSHADINGNAME) {
        // ShadingControl
        //boost::optional<Construction> construction() const;
      } else if (field == SHADINGDEVICEMATERIALNAME) {
        // ShadingControl
        //boost::optional<ShadingMaterial> shadingMaterial() const;
      } else if (field == SHADINGCONTROLTYPE) {
        addComboBoxColumn<std::string, model::ShadingControl>(
          Heading(QString(SHADINGCONTROLTYPE), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::ShadingControl::shadingControlTypeValues),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingControlType),
          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingControlType),
          boost::optional<std::function<void(model::ShadingControl*)>>(NullAdapter(&model::ShadingControl::resetShadingControlType)),
          boost::optional<std::function<bool(model::ShadingControl*)>>(NullAdapter(&model::ShadingControl::isShadingControlTypeDefaulted)),
          DataSource(allShadingControls, true));
      } else if (field == SCHEDULENAME) {
        std::function<bool(model::ShadingControl*, const model::Schedule&)> setter(
          [](model::ShadingControl* t_shadingControl, const model::Schedule& t_arg) {
            auto copy = t_arg;
            return t_shadingControl->setSchedule(copy);
          });

        addDropZoneColumn(
          Heading(QString(SCHEDULENAME), true, false), NullAdapter(&model::ShadingControl::schedule), setter,
          boost::optional<std::function<void(model::ShadingControl*)>>(CastNullAdapter<model::ShadingControl>(&model::ShadingControl::resetSchedule)),
          boost::optional<std::function<bool(model::ShadingControl*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingControl*)>>(), DataSource(allShadingControls, true));
      } else if (field == FRAMEANDDIVIDERNAME) {
        addDropZoneColumn(
          Heading(QString(FRAMEANDDIVIDERNAME)), CastNullAdapter<model::SubSurface>(&model::SubSurface::windowPropertyFrameAndDivider),
          CastNullAdapter<model::SubSurface>(&model::SubSurface::setWindowPropertyFrameAndDivider),
          boost::optional<std::function<void(model::SubSurface*)>>(NullAdapter(&model::SubSurface::resetWindowPropertyFrameAndDivider)),
          boost::optional<std::function<bool(model::SubSurface*)>>(),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::SubSurface*)>>(), DataSource(allSubSurfaces, true));
      } else if (field == FRAMEWIDTH) {
        addValueEditColumn(Heading(QString(FRAMEWIDTH), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameWidth),
                           NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameWidth),
                           boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameWidth)),
                           boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameWidthDefaulted)),
                           DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMEOUTSIDEPROJECTION) {
        addValueEditColumn(
          Heading(QString(FRAMEOUTSIDEPROJECTION), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameOutsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameOutsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameOutsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMEINSIDEPROJECTION) {
        addValueEditColumn(
          Heading(QString(FRAMEINSIDEPROJECTION), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameInsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameInsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameInsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameInsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMECONDUCTANCE) {
        addValueEditColumn(
          Heading(QString(FRAMECONDUCTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMEEDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE) {
        addValueEditColumn(
          Heading(QString(FRAMEEDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::ratioOfFrameEdgeGlassConductanceToCenterOfGlassConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::resetRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMESOLARABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(FRAMESOLARABSORPTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMEVISIBLEABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(FRAMEVISIBLEABSORPTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::frameVisibleAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameVisibleAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameVisibleAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isFrameVisibleAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == FRAMETHERMALHEMISPHERICALEMISSIVITY) {
        addValueEditColumn(
          Heading(QString(FRAMETHERMALHEMISPHERICALEMISSIVITY), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::frameThermalHemisphericalEmissivity),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setFrameThermalHemisphericalEmissivity),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetFrameThermalHemisphericalEmissivity)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isFrameThermalHemisphericalEmissivityDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERTYPE) {
        addComboBoxColumn<std::string, model::WindowPropertyFrameAndDivider>(
          Heading(QString(DIVIDERTYPE), true, false), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
          std::function<std::vector<std::string>()>(&model::WindowPropertyFrameAndDivider::dividerTypeValues),
          CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::dividerType),
          CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::setDividerType),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            NullAdapter(&model::WindowPropertyFrameAndDivider::resetDividerType)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            NullAdapter(&model::WindowPropertyFrameAndDivider::isDividerTypeDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERWIDTH) {
        addValueEditColumn(Heading(QString(DIVIDERWIDTH), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerWidth),
                           NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerWidth),
                           boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerWidth)),
                           boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
                             CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerWidthDefaulted)),
                           DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == NUMBEROFHORIZONTALDIVIDERS) {
        addValueEditColumn(
          Heading(QString(NUMBEROFHORIZONTALDIVIDERS), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::numberOfHorizontalDividers),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setNumberOfHorizontalDividers),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetNumberOfHorizontalDividers)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isNumberOfHorizontalDividersDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == NUMBEROFVERTICALDIVIDERS) {
        addValueEditColumn(
          Heading(QString(NUMBEROFVERTICALDIVIDERS), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::numberOfVerticalDividers),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setNumberOfVerticalDividers),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetNumberOfVerticalDividers)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isNumberOfVerticalDividersDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDEROUTSIDEPROJECTION) {
        addValueEditColumn(
          Heading(QString(DIVIDEROUTSIDEPROJECTION), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerOutsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerOutsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerOutsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerOutsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERINSIDEPROJECTION) {
        addValueEditColumn(
          Heading(QString(DIVIDERINSIDEPROJECTION), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerInsideProjection),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerInsideProjection),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerInsideProjection)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerInsideProjectionDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERCONDUCTANCE) {
        addValueEditColumn(
          Heading(QString(DIVIDERCONDUCTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == RATIOOFDIVIDEREDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE) {
        addValueEditColumn(
          Heading(QString(RATIOOFDIVIDEREDGEGLASSCONDUCTANCETOCENTEROFGLASSCONDUCTANCE), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::ratioOfDividerEdgeGlassConductanceToCenterOfGlassConductance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::resetRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERSOLARABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(DIVIDERSOLARABSORPTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERVISIBLEABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(DIVIDERVISIBLEABSORPTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::dividerVisibleAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerVisibleAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerVisibleAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isDividerVisibleAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == DIVIDERTHERMALHEMISPHERICALEMISSIVITY) {
        addValueEditColumn(
          Heading(QString(DIVIDERTHERMALHEMISPHERICALEMISSIVITY), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::dividerThermalHemisphericalEmissivity),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setDividerThermalHemisphericalEmissivity),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetDividerThermalHemisphericalEmissivity)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(CastNullAdapter<model::WindowPropertyFrameAndDivider>(
            &model::WindowPropertyFrameAndDivider::isDividerThermalHemisphericalEmissivityDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == OUTSIDEREVEALDEPTH) {
        addValueEditColumn(
          Heading(QString(OUTSIDEREVEALDEPTH), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::outsideRevealDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setOutsideRevealDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetOutsideRevealDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isOutsideRevealDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == OUTSIDEREVEALSOLARABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(OUTSIDEREVEALSOLARABSORPTANCE), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::outsideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setOutsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetOutsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isOutsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == INSIDESILLDEPTH) {
        addValueEditColumn(
          Heading(QString(INSIDESILLDEPTH), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideSillDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideSillDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideSillDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideSillDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == INSIDESILLSOLARABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(INSIDESILLSOLARABSORPTANCE), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideSillSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideSillSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideSillSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideSillSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == INSIDEREVEALDEPTH) {
        addValueEditColumn(
          Heading(QString(INSIDEREVEALDEPTH), true, false), NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealDepth),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealDepth),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealDepth)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealDepthDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == INSIDEREVEALSOLARABSORPTANCE) {
        addValueEditColumn(
          Heading(QString(INSIDEREVEALSOLARABSORPTANCE), true, false),
          NullAdapter(&model::WindowPropertyFrameAndDivider::insideRevealSolarAbsorptance),
          NullAdapter(&model::WindowPropertyFrameAndDivider::setInsideRevealSolarAbsorptance),
          boost::optional<std::function<void(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::resetInsideRevealSolarAbsorptance)),
          boost::optional<std::function<bool(model::WindowPropertyFrameAndDivider*)>>(
            CastNullAdapter<model::WindowPropertyFrameAndDivider>(&model::WindowPropertyFrameAndDivider::isInsideRevealSolarAbsorptanceDefaulted)),
          DataSource(allWindowPropertyFrameAndDividers, true));
      } else if (field == WINDOWNAME) {
      } else if (field == DAYLIGHTINGSHELFNAME) {
        addNameLineEditColumn(Heading(QString(DAYLIGHTINGSHELFNAME)), false, false,
                              CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::name),
                              CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::setName),
                              boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                              boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(), DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == INSIDESHELFNAME) {

        std::function<bool(model::DaylightingDeviceShelf*, const model::InteriorPartitionSurface&)> setter(
          [](model::DaylightingDeviceShelf* t_shelf, const model::InteriorPartitionSurface& t_arg) { return t_shelf->setInsideShelf(t_arg); });

        addDropZoneColumn(Heading(QString(INSIDESHELFNAME)),
                          CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::insideShelf), setter,
                          boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::DaylightingDeviceShelf*)>>(),
                          DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == OUTSIDESHELFNAME) {

        std::function<bool(model::DaylightingDeviceShelf*, const model::ShadingSurface&)> setter(
          [](model::DaylightingDeviceShelf* t_shelf, const model::ShadingSurface& t_arg) { return t_shelf->setOutsideShelf(t_arg); });

        addDropZoneColumn(Heading(QString(OUTSIDESHELFNAME)),
                          CastNullAdapter<model::DaylightingDeviceShelf>(&model::DaylightingDeviceShelf::outsideShelf), setter,
                          boost::optional<std::function<void(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<bool(model::DaylightingDeviceShelf*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::DaylightingDeviceShelf*)>>(),
                          DataSource(allDaylightingDeviceShelfs, true));
      } else if (field == VIEWFACTORTOOUTSIDESHELF) {
        addValueEditColumn(
          Heading(QString(VIEWFACTORTOOUTSIDESHELF)), NullAdapter(&model::DaylightingDeviceShelf::viewFactortoOutsideShelf),
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
  if (!this->hasHorizontalHeader()) return;

  OSGridController::checkSelectedFields();
}

void SpacesSubsurfacesGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesSubsurfacesGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
