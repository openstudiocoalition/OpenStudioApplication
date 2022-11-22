/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "SpacesDaylightingGridView.hpp"

#include "OSDropZone.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/DaylightingControl.hpp>
#include <openstudio/model/DaylightingControl_Impl.hpp>
#include <openstudio/model/GlareSensor.hpp>
#include <openstudio/model/GlareSensor_Impl.hpp>
#include <openstudio/model/Space.hpp>
#include <openstudio/model/Space_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_Space_FieldEnums.hxx>

#include <QCheckBox>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Space Name"
#define SELECTED "All"

// DAYLIGHTINGCONTROLS "Daylighting Controls"
#define THERMALZONENAME "Thermal Zone Name"  // read only
#define SPACENAME "Space Name"
#define POSITIONXCOORDINATE "Position X - Coordinate"
#define POSITIONYCOORDINATE "Position Y - Coordinate"
#define POSITIONZCOORDINATE "Position Z - Coordinate"
#define ROTATIONAROUNDXAXIS "Rotation Around X - Axis"
#define ROTATIONAROUNDYAXIS "Rotation Around Y - Axis"
#define ROTATIONAROUNDZAXIS "Rotation Around Z - Axis"
#define ILLUMINANCESETPOINT "Illuminance Setpoint"
#define LIGHTINGCONTROLTYPE "Lighting Control Type"
#define MINIMUMINPUTPOWERFRACTIONFORCONTINUOUSDIMMINGCONTROL "Minimum Input Power Fraction for Continuous Dimming Control"
#define MINIMUMLIGHTOUTPUTFRACTIONFORCONTINUOUSDIMMINGCONTROL "Minimum Light Output Fraction for Continuous Dimming Control"
#define NUMBEROFSTEPPEDCONTROLSTEPS "Number of Stepped Control Steps"
#define PROBABILITYLIGHTINGWILLBERESETWHENNEEDEDINMANUALSTEPPEDCONTROL "Probability Lighting will be Reset When Needed in Manual Stepped Control"
#define NUMBEROFDAYLIGHTINGVIEWS "Number of Daylighting Views"
#define MAXIMUMALLOWABLEDISCOMFORTGLAREINDEX "Maximum Allowable Discomfort Glare Index"

// ILLUMINANCEMAPS "Illuminance Maps"
#define SPACENAME "Space Name"
//#define NAME "Name"
#define ORIGINXCOORDINATE "Origin X - Coordinate"
#define ORIGINYCOORDINATE "Origin Y - Coordinate"
#define ORIGINZCOORDINATE "Origin Z - Coordinate"
#define ROTATIONAROUNDXAXIS "Rotation Around X - Axis"
#define ROTATIONAROUNDYAXIS "Rotation Around Y - Axis"
#define ROTATIONAROUNDZAXIS "Rotation Around Z - Axis"
#define XLENGTH "X Length"
#define NUMBEROFXGRIDPOINTS "Number of X Grid Points"
#define YLENGTH "Y Length"
#define NUMBEROFYGRIDPOINTS "Number of Y Grid Points"

// GLARESENSORS "Glare Sensors"
#define SPACENAME "Space Name"
//#define NAME "Name"
#define POSITIONXCOORDINATE "Position X - Coordinate"
#define POSITIONYCOORDINATE "Position Y - Coordinate"
#define POSITIONZCOORDINATE "Position Z - Coordinate"
#define ROTATIONAROUNDXAXIS "Rotation Around X - Axis"
#define ROTATIONAROUNDYAXIS "Rotation Around Y - Axis"
#define ROTATIONAROUNDZAXIS "Rotation Around Z - Axis"
#define NUMBEROFGLAREVIEWVECTORS "Number of Glare View Vectors"
#define MAXIMUMALLOWABLEDAYLIGHTGLAREPROBABILITY "Maximum Allowable Daylight Glare Probability"

namespace openstudio {

SpacesDaylightingGridView::SpacesDaylightingGridView(bool isIP, const model::Model& model, QWidget* parent)
  : SpacesSubtabGridView(isIP, model, parent) {
  showStoryFilter();
  showThermalZoneFilter();
  showSpaceTypeFilter();
  showSpaceNameFilter();

  m_filterGridLayout->setRowStretch(m_filterGridLayout->rowCount(), 100);
  m_filterGridLayout->setColumnStretch(m_filterGridLayout->columnCount(), 100);

  m_gridController = new SpacesDaylightingGridController(isIP, "Space", IddObjectType::OS_Space, model, m_spacesModelObjects);
  m_gridView = new OSGridView(m_gridController, "Space", "Drop\nSpace", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  m_gridView->addLayoutToContentLayout(m_filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
  m_gridView->showDropZone(false);
}

SpacesDaylightingGridController::SpacesDaylightingGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                                 const model::Model& model, const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void SpacesDaylightingGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      //THERMALZONENAME,
      //SPACENAME,
      //POSITIONXCOORDINATE,
      //POSITIONYCOORDINATE,
      //POSITIONZCOORDINATE,
      //ROTATIONAROUNDXAXIS,
      //ROTATIONAROUNDYAXIS,
      //ROTATIONAROUNDZAXIS,
      //ILLUMINANCESETPOINT,
      //LIGHTINGCONTROLTYPE,
      //MINIMUMINPUTPOWERFRACTIONFORCONTINUOUSDIMMINGCONTROL,
      //MINIMUMLIGHTOUTPUTFRACTIONFORCONTINUOUSDIMMINGCONTROL,
      //NUMBEROFSTEPPEDCONTROLSTEPS,
      //PROBABILITYLIGHTINGWILLBERESETWHENNEEDEDINMANUALSTEPPEDCONTROL,
      //NUMBEROFDAYLIGHTINGVIEWS,
      //MAXIMUMALLOWABLEDISCOMFORTGLAREINDEX,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Daylighting Controls"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      //SPACENAME,
      //NAME,
      //ORIGINXCOORDINATE,
      //ORIGINYCOORDINATE,
      //ORIGINZCOORDINATE,
      //ROTATIONAROUNDXAXIS,
      //ROTATIONAROUNDYAXIS,
      //ROTATIONAROUNDZAXIS,
      //XLENGTH,
      //NUMBEROFXGRIDPOINTS,
      //YLENGTH,
      //NUMBEROFYGRIDPOINTS,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Illuminance Maps"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  {
    std::vector<QString> fields{
      //SPACENAME,
      //NAME,
      //POSITIONXCOORDINATE,
      //POSITIONYCOORDINATE,
      //POSITIONZCOORDINATE,
      //ROTATIONAROUNDXAXIS,
      //ROTATIONAROUNDYAXIS,
      //ROTATIONAROUNDZAXIS,
      //NUMBEROFGLAREVIEWVECTORS,
      //MAXIMUMALLOWABLEDAYLIGHTGLAREPROBABILITY,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("Glare Sensors"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void SpacesDaylightingGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void SpacesDaylightingGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addNameLineEditColumn(Heading(QString(NAME), false, false), false, false, CastNullAdapter<model::Space>(&model::Space::name),
                            CastNullAdapter<model::Space>(&model::Space::setName));
    } else {
      if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &SpacesDaylightingGridController::onSelectAllStateChanged);
        connect(this, &SpacesDaylightingGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
        //addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row",
        //  DataSource(
        //  allLoads,
        //  true
        //  )
        //  );
      }

      else if (field == THERMALZONENAME) {

      }

      else if (field == SPACENAME) {

      }

      else if (field == POSITIONXCOORDINATE) {

      }

      else if (field == POSITIONYCOORDINATE) {

      }

      else if (field == POSITIONZCOORDINATE) {

      }

      else if (field == ROTATIONAROUNDXAXIS) {

      }

      else if (field == ROTATIONAROUNDYAXIS) {

      }

      else if (field == ROTATIONAROUNDZAXIS) {

      }

      else if (field == ILLUMINANCESETPOINT) {

      }

      else if (field == LIGHTINGCONTROLTYPE) {

      }

      else if (field == MINIMUMINPUTPOWERFRACTIONFORCONTINUOUSDIMMINGCONTROL) {
        //DaylightingControl
        //double minimumInputPowerFractionforContinuousDimmingControl() const;
        //bool setMinimumInputPowerFractionforContinuousDimmingControl(double minimumInputPowerFractionforContinuousDimmingControl);
        //void resetMinimumInputPowerFractionforContinuousDimmingControl();
        //bool isMinimumInputPowerFractionforContinuousDimmingControlDefaulted() const;

      }

      else if (field == MINIMUMLIGHTOUTPUTFRACTIONFORCONTINUOUSDIMMINGCONTROL) {
        //DaylightingControl
        //  double minimumLightOutputFractionforContinuousDimmingControl() const;
        //bool setMinimumLightOutputFractionforContinuousDimmingControl(double minimumLightOutputFractionforContinuousDimmingControl);
        //void resetMinimumLightOutputFractionforContinuousDimmingControl();
        //bool isMinimumLightOutputFractionforContinuousDimmingControlDefaulted() const;

      }

      else if (field == NUMBEROFSTEPPEDCONTROLSTEPS) {
        //DaylightingControl
        //  int numberofSteppedControlSteps() const;
        //void setNumberofSteppedControlSteps(int numberofSteppedControlSteps);
        //void resetNumberofSteppedControlSteps();
        //bool isNumberofSteppedControlStepsDefaulted() const;

      }

      else if (field == PROBABILITYLIGHTINGWILLBERESETWHENNEEDEDINMANUALSTEPPEDCONTROL) {
        //DaylightingControl
        //  double probabilityLightingwillbeResetWhenNeededinManualSteppedControl() const;
        //bool setProbabilityLightingwillbeResetWhenNeededinManualSteppedControl(double probabilityLightingwillbeResetWhenNeededinManualSteppedControl);
        //void resetProbabilityLightingwillbeResetWhenNeededinManualSteppedControl();
        //bool isProbabilityLightingwillbeResetWhenNeededinManualSteppedControlDefaulted() const;

      }

      else if (field == NUMBEROFDAYLIGHTINGVIEWS) {
        //DaylightingControl
        //  int numberofDaylightingViews() const;
        //bool setNumberofDaylightingViews(int numberofDaylightingViews);
        //void resetNumberofDaylightingViews();
        //bool isNumberofDaylightingViewsDefaulted() const;

      }

      else if (field == MAXIMUMALLOWABLEDISCOMFORTGLAREINDEX) {
        //DaylightingControl
        //  boost::optional<double> maximumAllowableDiscomfortGlareIndex() const;
        //bool setMaximumAllowableDiscomfortGlareIndex(double maximumAllowableDiscomfortGlareIndex);
        //void resetMaximumAllowableDiscomfortGlareIndex();

      }

      else if (field == SPACENAME) {

      }

      else if (field == NAME) {

      }

      else if (field == ORIGINXCOORDINATE) {

      }

      else if (field == ORIGINYCOORDINATE) {

      }

      else if (field == ORIGINZCOORDINATE) {

      }

      else if (field == ROTATIONAROUNDXAXIS) {

      }

      else if (field == ROTATIONAROUNDYAXIS) {

      }

      else if (field == ROTATIONAROUNDZAXIS) {

      }

      else if (field == XLENGTH) {

      }

      else if (field == NUMBEROFXGRIDPOINTS) {

      }

      else if (field == YLENGTH) {

      }

      else if (field == NUMBEROFYGRIDPOINTS) {

      }

      else if (field == SPACENAME) {

      }

      else if (field == NAME) {

      }

      else if (field == POSITIONXCOORDINATE) {

      }

      else if (field == POSITIONYCOORDINATE) {

      }

      else if (field == POSITIONZCOORDINATE) {

      }

      else if (field == ROTATIONAROUNDXAXIS) {

      }

      else if (field == ROTATIONAROUNDYAXIS) {

      }

      else if (field == ROTATIONAROUNDZAXIS) {

      }

      else if (field == NUMBEROFGLAREVIEWVECTORS) {
        //GlareSensor
        //int numberofGlareViewVectors() const;
        //bool setNumberofGlareViewVectors(int numberofGlareViewVectors);
        //void resetNumberofGlareViewVectors();

      }

      else if (field == MAXIMUMALLOWABLEDAYLIGHTGLAREPROBABILITY) {
        //GlareSensor
        //boost::optional<double> maximumAllowableDaylightGlareProbability() const;
        //bool setMaximumAllowableDaylightGlareProbability(double maximumAllowableDaylightGlareProbability);
        //void resetMaximumAllowableDaylightGlareProbability();

      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString SpacesDaylightingGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void SpacesDaylightingGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void SpacesDaylightingGridController::onItemDropped(const OSItemId& itemId) {}

void SpacesDaylightingGridController::refreshModelObjects() {
  auto spaces = model().getConcreteModelObjects<model::Space>();
  std::sort(spaces.begin(), spaces.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(spaces));
}

}  // namespace openstudio
