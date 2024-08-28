/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "HVACSystemsView.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "OSItem.hpp"
#include "OSDropZone.hpp"
#include "../shared_gui_components/OSSwitch.hpp"
#include "ModelObjectItem.hpp"
#include "../shared_gui_components/OSViewSwitcher.hpp"
#include "../shared_gui_components/Buttons.hpp"
#include <openstudio/model/SetpointManagerOutdoorAirReset.hpp>
#include <openstudio/model/SetpointManagerFollowOutdoorAirTemperature.hpp>
#include <openstudio/model/SetpointManagerFollowGroundTemperature.hpp>
#include <openstudio/model/SetpointManagerSystemNodeResetTemperature.hpp>
#include <openstudio/model/SetpointManagerWarmest.hpp>
#include <openstudio/model/SetpointManagerWarmestTemperatureFlow.hpp>
#include <openstudio/model/SetpointManagerColdest.hpp>
#include <openstudio/model/Node.hpp>

#include <QApplication>
#include <QStackedWidget>
#include <QObjectList>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractItemView>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QButtonGroup>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <qnamespace.h>

namespace openstudio {

HVACToolbarView::HVACToolbarView() {
  // There are two modes.  One that shows just the label and the systemComboBox,
  // the other shows all of the controls but no label

  QString style;

  style.append("QWidget#NavigatorWidget { ");
  style.append("background: #808080; ");
  style.append("border-bottom: 1px solid black; ");
  style.append("}");

  setObjectName("NavigatorWidget");
  setStyleSheet(style);

  auto* mainHlayout = new QHBoxLayout();
  mainHlayout->setContentsMargins(5, 5, 5, 5);
  mainHlayout->setSpacing(5);
  setLayout(mainHlayout);

  m_viewStack = new QStackedWidget();
  m_viewStack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  mainHlayout->addWidget(m_viewStack);

  // Label Widget

  auto* labelWidget = new QWidget();
  auto* labelLayout = new QHBoxLayout();
  labelLayout->setContentsMargins(0, 0, 0, 0);
  labelLayout->setSpacing(5);
  labelLayout->setAlignment(Qt::AlignLeft);
  labelWidget->setLayout(labelLayout);
  m_viewStack->addWidget(labelWidget);

  label = new QLabel();
  label->setObjectName("H2");
  labelLayout->addWidget(label);

  // Controls Widget

  auto* controlWidget = new QWidget();
  auto* controlLayout = new QHBoxLayout();
  controlLayout->setContentsMargins(0, 0, 0, 0);
  controlLayout->setSpacing(5);
  controlWidget->setLayout(controlLayout);
  m_viewStack->addWidget(controlWidget);

  addButton = new QPushButton();
  addButton->setObjectName("AddButton");
  addButton->setFixedSize(24, 24);
  addButton->setFlat(true);
  controlLayout->addWidget(addButton);

  copyButton = new QPushButton();
  copyButton->setObjectName("CopyButton");
  copyButton->setFixedSize(24, 24);
  copyButton->setFlat(true);
  controlLayout->addWidget(copyButton);

  deleteButton = new QPushButton(this);
  deleteButton->setObjectName("DeleteButton");
  deleteButton->setFixedSize(24, 24);
  deleteButton->setFlat(true);
  controlLayout->addWidget(deleteButton);

  controlLayout->addStretch();

  auto* zoomButtonGroup = new QButtonGroup(this);
  zoomButtonGroup->setExclusive(true);

  topologyViewButton = new GrayButton();
  topologyViewButton->setCheckable(true);
  topologyViewButton->setText("Layout");
  zoomButtonGroup->addButton(topologyViewButton);
  controlLayout->addWidget(topologyViewButton);

  controlsViewButton = new GrayButton();
  controlsViewButton->setCheckable(true);
  controlsViewButton->setText("Control");
  zoomButtonGroup->addButton(controlsViewButton);
  controlLayout->addWidget(controlsViewButton);

  gridViewButton = new GrayButton();
  gridViewButton->setCheckable(true);
  gridViewButton->setText("Grid");
  zoomButtonGroup->addButton(gridViewButton);
  controlLayout->addWidget(gridViewButton);

  topologyViewButton->setChecked(true);

  controlLayout->addStretch();

  zoomInButton = new QPushButton();
  zoomInButton->setFlat(true);
  zoomInButton->setObjectName("ZoomInButton");
  zoomInButton->setFixedSize(20, 20);
  controlLayout->addWidget(zoomInButton);

  zoomOutButton = new QPushButton();
  zoomOutButton->setFlat(true);
  zoomOutButton->setObjectName("ZoomOutButton");
  zoomOutButton->setFixedSize(20, 20);
  controlLayout->addWidget(zoomOutButton);

  controlLayout->addStretch();

  systemComboBox = new OSComboBox2();
  systemComboBox->setMinimumContentsLength(40);
  mainHlayout->addWidget(systemComboBox);
  systemComboBox->setEnabled(true);
}

void HVACToolbarView::showControls(bool show) {
  if (show) {
    m_viewStack->setCurrentIndex(1);
  } else {
    m_viewStack->setCurrentIndex(0);
  }
}

void HVACToolbarView::hideAddCopyDeleteButtons() {
  addButton->hide();
  copyButton->hide();
  deleteButton->hide();
}

void HVACToolbarView::hideZoomButtons() {
  zoomInButton->hide();
  zoomOutButton->hide();
}

void HVACToolbarView::resetAllIndividualControlButtons() {
  addButton->show();
  copyButton->show();
  deleteButton->show();

  zoomInButton->show();
  zoomOutButton->show();

  topologyViewButton->show();
  controlsViewButton->show();
  gridViewButton->show();

  addButton->setEnabled(true);
  copyButton->setEnabled(true);
  deleteButton->setEnabled(true);

  zoomInButton->setEnabled(true);
  zoomOutButton->setEnabled(true);

  topologyViewButton->setEnabled(true);
  controlsViewButton->setEnabled(true);
  gridViewButton->setEnabled(true);
}

void HVACToolbarView::paintEvent([[maybe_unused]] QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

HVACSystemsView::HVACSystemsView() {
  setObjectName("GrayWidgetWithLeftTopBorders");

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(1, 1, 0, 0);
  mainVLayout->setSpacing(0);
  mainVLayout->setAlignment(Qt::AlignTop);
  setLayout(mainVLayout);

  hvacToolbarView = new HVACToolbarView();
  mainVLayout->addWidget(hvacToolbarView);

  mainViewSwitcher = new OSViewSwitcher();
  mainVLayout->addWidget(mainViewSwitcher);
}

//void HVACSystemsView::enableDeleteButton(bool enable)
//{
//  m_deleteButton->setEnabled(enable);
//}

//void HVACSystemsView::configureHeaderForNavigating()
//{
//  m_navigatorStack->setCurrentIndex(0);
//}

//void HVACSystemsView::configureHeaderWithText(const QString & text)
//{
//  m_text->setText(text);
//
//  m_navigatorStack->setCurrentIndex(1);
//}

QSize HVACSystemsView::sizeHint() const {
  return QSize(800, 550);
}

//model::OptionalModel HVACSystemsView::model() const
//{
//  return m_model;
//}

void HVACSystemsView::paintEvent([[maybe_unused]] QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//OSComboBox2 * HVACSystemsView::chooser() const
//{
//  return m_chooser;
//}

HVACGraphicsView::HVACGraphicsView(QWidget* parent) : QGraphicsView(parent) {
  setAcceptDrops(true);
  setAttribute(Qt::WA_AcceptTouchEvents, false);

  resetZoom();

  setObjectName("GrayWidget");
}

void HVACGraphicsView::wheelEvent(QWheelEvent* event) {
  if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
    // angleDelta: Returns the relative amount that the wheel was rotated, in eighths of a degree.
    // A positive value indicates that the wheel was rotated forwards away from the user;
    // a negative value indicates that the wheel was rotated backwards toward the user.
    // angleDelta().y() provides the angle through which the common vertical mouse wheel was rotated since the previous event
    const double verticalAngle = event->angleDelta().y();
    if (verticalAngle != 0) {
      constexpr double zoom_factor_base = 1.0015;
      const double numDegrees = event->angleDelta().y() / 8.0;
      const double factor = std::pow(zoom_factor_base, 3.0 * numDegrees);  // Abritrary factor here..
      this->scale(factor, factor);
      return;
    }
  }
  event->ignore();
}

void HVACGraphicsView::keyReleaseEvent(QKeyEvent* event) {
  if (QApplication::keyboardModifiers() & Qt::ControlModifier) {

    if (event->key() == Qt::Key_Plus) {
      zoomIn();
      return;
    }

    if (event->key() == Qt::Key_Minus) {
      zoomOut();
      return;
    }
  }
  event->ignore();
}

void HVACGraphicsView::zoomIn() {
  this->scale(1.25, 1.25);
}

void HVACGraphicsView::zoomOut() {
  this->scale(0.8, 0.8);
}

void HVACGraphicsView::resetZoom() {
  this->resetTransform();

  this->scale(0.65, 0.65);
}

/* Controls tab for an AirLoopHVAC */
HVACAirLoopControlsView::HVACAirLoopControlsView() {
  auto* widget = new QWidget();

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(5, 5, 5, 5);
  mainVLayout->setSpacing(10);
  mainVLayout->setAlignment(Qt::AlignTop);
  widget->setLayout(mainVLayout);

  setWidget(widget);
  setWidgetResizable(true);
  setFrameShape(QFrame::NoFrame);

  systemNameLabel = new QLabel("HVAC System");
  systemNameLabel->setObjectName("H1");
  mainVLayout->addWidget(systemNameLabel);

  auto* hClassificationLayout = new QHBoxLayout();
  hClassificationLayout->setContentsMargins(0, 0, 0, 0);
  hClassificationLayout->setSpacing(5);
  mainVLayout->addLayout(hClassificationLayout);

  auto* coolingTypeTitle = new QLabel("Cooling Type: ");
  coolingTypeTitle->setObjectName("H2");
  hClassificationLayout->addWidget(coolingTypeTitle);

  coolingTypeLabel = new QLabel();
  hClassificationLayout->addWidget(coolingTypeLabel);
  hClassificationLayout->addStretch();

  auto* heatingTypeTitle = new QLabel("Heating Type: ");
  heatingTypeTitle->setObjectName("H2");
  hClassificationLayout->addWidget(heatingTypeTitle);

  heatingTypeLabel = new QLabel();
  hClassificationLayout->addWidget(heatingTypeLabel);
  hClassificationLayout->addStretch();

  auto* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  auto* timeTitle = new QLabel("Time of Operation");
  timeTitle->setObjectName("H1");
  mainVLayout->addWidget(timeTitle);

  auto* operationScheduleTitle = new QLabel("HVAC Operation Schedule");
  operationScheduleTitle->setObjectName("H2");
  mainVLayout->addWidget(operationScheduleTitle);

  hvacOperationViewSwitcher = new OSViewSwitcher();
  hvacOperationViewSwitcher->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  mainVLayout->addWidget(hvacOperationViewSwitcher);

  auto* nightCycleTitle = new QLabel("Use Night Cycle");
  nightCycleTitle->setObjectName("H2");
  mainVLayout->addWidget(nightCycleTitle);

  auto* nightCycleHLayout = new QHBoxLayout();
  nightCycleHLayout->setContentsMargins(0, 0, 0, 0);
  nightCycleHLayout->setSpacing(5);
  nightCycleComboBox = new OSComboBox2();
  nightCycleComboBox->addItem("Follow the HVAC Operation Schedule", "StayOff");
  nightCycleComboBox->addItem("Cycle on Full System if Heating or Cooling Required", "CycleOnAny");
  nightCycleComboBox->addItem("Cycle on Zone Terminal Units if Heating or Cooling Required", "CycleOnAnyZoneFansOnly");
  nightCycleHLayout->addWidget(nightCycleComboBox);

  nightCycleHLayout->addStretch();

  mainVLayout->addLayout(nightCycleHLayout);

  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  auto* supplyAirTemperatureTitle = new QLabel("Supply Air Temperature");
  supplyAirTemperatureTitle->setObjectName("H1");
  mainVLayout->addWidget(supplyAirTemperatureTitle);

  supplyAirTemperatureViewSwitcher = new OSViewSwitcher();
  supplyAirTemperatureViewSwitcher->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  mainVLayout->addWidget(supplyAirTemperatureViewSwitcher);

  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  auto* outdoorAirTitle = new QLabel("Mechanical Ventilation");
  outdoorAirTitle->setObjectName("H1");
  mainVLayout->addWidget(outdoorAirTitle);

  ventilationViewSwitcher = new OSViewSwitcher();
  ventilationViewSwitcher->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  mainVLayout->addWidget(ventilationViewSwitcher);

  // Add a line
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  // AvailabilityManagers
  auto* avmTitle = new QLabel("Availability Managers");
  avmTitle->setObjectName("H1");
  mainVLayout->addWidget(avmTitle);

  auto* avmListTitle = new QLabel("Availability Managers from highest precedence to lowest");
  avmListTitle->setObjectName("H2");
  mainVLayout->addWidget(avmListTitle);

  availabilityManagerViewSwitcher = new OSViewSwitcher();
  availabilityManagerViewSwitcher->setSizePolicy(QSizePolicy::Preferred,
                                                 QSizePolicy::Fixed);  // QSizePolicy::MinimumExpanding, QSizePolicy::Expanding
  mainVLayout->addWidget(availabilityManagerViewSwitcher);
}

/* Controls tab for an AirLoopHVAC */
HVACPlantLoopControlsView::HVACPlantLoopControlsView() {
  auto* widget = new QWidget();

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(5, 5, 5, 5);
  mainVLayout->setSpacing(10);
  mainVLayout->setAlignment(Qt::AlignTop);
  widget->setLayout(mainVLayout);

  setWidget(widget);
  setWidgetResizable(true);
  setFrameShape(QFrame::NoFrame);

  systemNameLabel = new QLabel("HVAC System");
  systemNameLabel->setObjectName("H1");
  mainVLayout->addWidget(systemNameLabel);

  // Plant Loop Type
  auto* hClassificationLayout = new QHBoxLayout();
  hClassificationLayout->setContentsMargins(0, 0, 0, 0);
  hClassificationLayout->setSpacing(5);
  mainVLayout->addLayout(hClassificationLayout);

  auto* plantLoopTypeTitle = new QLabel("Plant Loop Type: ");
  plantLoopTypeTitle->setObjectName("H2");
  hClassificationLayout->addWidget(plantLoopTypeTitle);

  plantLoopTypeLabel = new QLabel();
  hClassificationLayout->addWidget(plantLoopTypeLabel);
  hClassificationLayout->addStretch();

  // Add a separation line
  auto* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  /***********************************************************************************************************************
   *                  P L A N T    E Q U I P M E N T    O P E R A T I O N    S C H E M E S
   ***********************************************************************************************************************/

  auto* spmTitle = new QLabel("Plant Equipment Operation Schemes");
  spmTitle->setObjectName("H1");
  mainVLayout->addWidget(spmTitle);

  // We are going to place 4 QVBoxLayout inside a QHBoxLayout
  hClassificationLayout = new QHBoxLayout();
  hClassificationLayout->setContentsMargins(0, 0, 0, 0);
  hClassificationLayout->setSpacing(5);
  mainVLayout->addLayout(hClassificationLayout);

  // Heating Components
  auto* vClassificationLayout = new QVBoxLayout();
  hClassificationLayout->addLayout(vClassificationLayout);

  auto* heatingComponentsTitle = new QLabel("Heating Components:");
  heatingComponentsTitle->setObjectName("H2");
  vClassificationLayout->addWidget(heatingComponentsTitle);

  heatingComponentsLabel = new QLabel();
  vClassificationLayout->addWidget(heatingComponentsLabel);
  hClassificationLayout->addStretch();

  // Cooling Components
  vClassificationLayout = new QVBoxLayout();
  hClassificationLayout->addLayout(vClassificationLayout);

  auto* coolingComponentsTitle = new QLabel("Cooling Components:");
  coolingComponentsTitle->setObjectName("H2");
  vClassificationLayout->addWidget(coolingComponentsTitle);

  coolingComponentsLabel = new QLabel();
  vClassificationLayout->addWidget(coolingComponentsLabel);
  hClassificationLayout->addStretch();

  // Setpoint Components
  vClassificationLayout = new QVBoxLayout();
  hClassificationLayout->addLayout(vClassificationLayout);

  auto* setpointComponentsTitle = new QLabel("Setpoint Components:");
  setpointComponentsTitle->setObjectName("H2");
  vClassificationLayout->addWidget(setpointComponentsTitle);

  setpointComponentsLabel = new QLabel();
  vClassificationLayout->addWidget(setpointComponentsLabel);
  hClassificationLayout->addStretch();

  // Uncontrolled Components
  vClassificationLayout = new QVBoxLayout();
  hClassificationLayout->addLayout(vClassificationLayout);

  auto* uncontrolledComponentsTitle = new QLabel("Uncontrolled Components:");
  uncontrolledComponentsTitle->setObjectName("H2");
  vClassificationLayout->addWidget(uncontrolledComponentsTitle);

  uncontrolledComponentsLabel = new QLabel();
  vClassificationLayout->addWidget(uncontrolledComponentsLabel);
  hClassificationLayout->addStretch();

  // Add a separation Line
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  auto* supplyTemperatureTitle = new QLabel("Supply Water Temperature");
  supplyTemperatureTitle->setObjectName("H1");
  mainVLayout->addWidget(supplyTemperatureTitle);

  supplyTemperatureViewSwitcher = new OSViewSwitcher();
  supplyTemperatureViewSwitcher->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  mainVLayout->addWidget(supplyTemperatureViewSwitcher);

  // Add a separation line
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(line);

  // AvailabilityManagers
  auto* avmTitle = new QLabel("Availability Managers");
  avmTitle->setObjectName("H1");
  mainVLayout->addWidget(avmTitle);

  auto* avmListTitle = new QLabel("Availability Managers from highest precedence to lowest");
  avmListTitle->setObjectName("H2");
  mainVLayout->addWidget(avmListTitle);

  availabilityManagerViewSwitcher = new OSViewSwitcher();
  availabilityManagerViewSwitcher->setSizePolicy(QSizePolicy::Preferred,
                                                 QSizePolicy::Fixed);  // QSizePolicy::MinimumExpanding, QSizePolicy::Expanding
  mainVLayout->addWidget(availabilityManagerViewSwitcher);
}

MechanicalVentilationView::MechanicalVentilationView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setAlignment(Qt::AlignTop);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* economizerHBoxLayout = new QHBoxLayout();
  economizerHBoxLayout->setSpacing(10);

  auto* economizerTitle = new QLabel("Economizer");
  economizerTitle->setObjectName("H2");
  economizerHBoxLayout->addWidget(economizerTitle);

  economizerComboBox = new OSComboBox2();
  economizerComboBox->setEnabled(true);
  economizerComboBox->addItem("Fixed Dry Bulb", "FixedDryBulb");
  economizerComboBox->addItem("Fixed Enthalpy", "FixedEnthalpy");
  economizerComboBox->addItem("Differential Dry Bulb", "DifferentialDryBulb");
  economizerComboBox->addItem("Differential Enthalpy", "DifferentialEnthalpy");
  economizerComboBox->addItem("Fixed Dewpoint and Dry Bulb", "FixedDewPointAndDryBulb");
  economizerComboBox->addItem("ElectronicEnthalpy", "ElectronicEnthalpy");
  economizerComboBox->addItem("Differential Dry Bulb and Enthalpy", "DifferentialDryBulbAndEnthalpy");
  economizerComboBox->addItem("No Economizer", "NoEconomizer");
  economizerHBoxLayout->addWidget(economizerComboBox);

  economizerHBoxLayout->addStretch();
  mainVLayout->addLayout(economizerHBoxLayout);

  auto* methodHBoxLayout = new QHBoxLayout();
  methodHBoxLayout->setSpacing(10);

  auto* methodTitle = new QLabel("Ventilation Calculation Method");
  methodTitle->setObjectName("H2");
  methodHBoxLayout->addWidget(methodTitle);
  methodTitle->setVisible(false);

  ventilationCalcMethodComboBox = new OSComboBox2();
  ventilationCalcMethodComboBox->setEnabled(false);
  ventilationCalcMethodComboBox->addItem("Zone Sum", "ZoneSum");
  ventilationCalcMethodComboBox->addItem("Standard 62.1 Ventilation Rate Procedure", "Standard62.1VentilationRateProcedure");
  ventilationCalcMethodComboBox->addItem("Standard 62.1 Ventilation Rate Procedure With Limit", "Standard62.1VentilationRateProcedureWithLimit");
  ventilationCalcMethodComboBox->addItem("Indoor Air Quality Procedure", "IndoorAirQualityProcedure");
  // TODO: OS 3.7.0 is missing some choices that E+ 23.2.0 has, pending https://github.com/NREL/OpenStudio/issues/5060
  // ventilationCalcMethodComboBox->addItem("Indoor Air Quality Procedure Generic Contaminant", "IndoorAirQualityProcedureGenericContaminant");
  // ventilationCalcMethodComboBox->addItem("Indoor Air Quality Procedure Combined", "IndoorAirQualityProcedureCombined");
  ventilationCalcMethodComboBox->addItem("Proportional Control Bseed On Design Occupancy", "ProportionalControlBasedOnDesignOccupancy");
  // ventilationCalcMethodComboBox->addItem("Proportional Control Bseed On Design OA Rate", "ProportionalControlBasedOnDesignOARate");
  ventilationCalcMethodComboBox->addItem("Proportional Control Bseed On Occupancy Schedule", "ProportionalControlBasedOnOccupancySchedule");
  ventilationCalcMethodComboBox->setVisible(false);

  methodHBoxLayout->addWidget(ventilationCalcMethodComboBox);
  methodHBoxLayout->addStretch();
  mainVLayout->addLayout(methodHBoxLayout);

  auto* dcvHBoxLayout = new QHBoxLayout();
  dcvHBoxLayout->setSpacing(10);
  mainVLayout->addLayout(dcvHBoxLayout);

  auto* dcvTitle = new QLabel("Demand Controlled Ventilation");
  dcvTitle->setObjectName("H2");
  dcvHBoxLayout->addWidget(dcvTitle);

  dcvButton = new OSSwitch2();
  dcvHBoxLayout->addWidget(dcvButton);

  dcvHBoxLayout->addStretch();
}

NoMechanicalVentilationView::NoMechanicalVentilationView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(5);
  setLayout(mainVLayout);

  auto* noVentilationTitle = new QLabel("This system configuration does not provide mechanical ventilation");
  mainVLayout->addWidget(noVentilationTitle);
}

SingleZoneSPMView::SingleZoneSPMView(const QString& spmType) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setAlignment(Qt::AlignTop);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* singleZoneResetSPTitle = new QLabel(QString("Supply temperature is controlled by a <strong>%1</strong> setpoint manager.").arg(spmType));
  mainVLayout->addWidget(singleZoneResetSPTitle);

  auto* zoneSelectorHBoxLayout = new QHBoxLayout();
  zoneSelectorHBoxLayout->setSpacing(10);
  zoneSelectorHBoxLayout->setContentsMargins(0, 0, 0, 0);

  auto* controlZoneTitle = new QLabel("Control Zone");
  controlZoneTitle->setObjectName("H2");
  zoneSelectorHBoxLayout->addWidget(controlZoneTitle);

  controlZoneComboBox = new OSComboBox2();
  controlZoneComboBox->setEnabled(true);
  zoneSelectorHBoxLayout->addWidget(controlZoneComboBox);
  zoneSelectorHBoxLayout->addStretch();

  mainVLayout->addLayout(zoneSelectorHBoxLayout);
}

OAResetSPMView::OAResetSPMView(const model::SetpointManagerOutdoorAirReset& spm) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setAlignment(Qt::AlignTop);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  QString text;
  text.append("Supply temperature is controlled by an outdoor air reset setpoint manager.");
  auto* title = new QLabel(text);
  mainVLayout->addWidget(title);

  auto* series = new QLineSeries;

  const auto lowOATemp = spm.outdoorLowTemperature();           // 5
  const auto highOATemp = spm.outdoorHighTemperature();         // 30
  const auto lowVal = spm.setpointatOutdoorLowTemperature();    // 25
  const auto highVal = spm.setpointatOutdoorHighTemperature();  // 18
  series->append(lowOATemp - 5, lowVal);
  series->append(lowOATemp, lowVal);
  series->append(highOATemp, highVal);
  series->append(highOATemp + 5, highVal);

  auto* chart = new QChart;
  chart->legend()->hide();
  chart->addSeries(series);
  chart->createDefaultAxes();
  auto* xAxis = chart->axes(Qt::Horizontal)[0];
  xAxis->setTitleText("Outdoor Air Temperature [C]");
  auto* yAxis = chart->axes(Qt::Vertical)[0];
  yAxis->setTitleText("Setpoint Temperature [C]");
  yAxis->setRange(std::min(lowVal, highVal) - 2, std::max(lowVal, highVal) + 2);

  chart->setTitle("Setpoint Outdoor Air Temp Reset");
  chart->setAnimationOptions(QChart::SeriesAnimations);

  auto* m_defaultChartView = new QChartView(chart, this);
  m_defaultChartView->setRenderHint(QPainter::Antialiasing);
  m_defaultChartView->setMinimumSize(400, 400);
  mainVLayout->addWidget(m_defaultChartView);
  mainVLayout->addStretch();
}

SystemNodeResetSPMView::SystemNodeResetSPMView(const model::SetpointManagerSystemNodeResetTemperature& spm) {

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setAlignment(Qt::AlignTop);
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  QString nodeName = "which is <strong style=\"color:red\">missing its Reference Node</strong>";
  if (auto node_ = spm.referenceNode()) {
    nodeName = QString("with Reference Node '%1'").arg(QString::fromStdString(node_->nameString()));
  }

  QString text = QString("Supply temperature is controlled by a System Node Reset Temperature setpoint manager %1.").arg(nodeName);
  auto* title = new QLabel(text);
  mainVLayout->addWidget(title);

  auto* series = new QLineSeries;

  const auto lowOATemp = spm.lowReferenceTemperature();           // 5
  const auto highOATemp = spm.highReferenceTemperature();         // 30
  const auto lowVal = spm.setpointatLowReferenceTemperature();    // 25
  const auto highVal = spm.setpointatHighReferenceTemperature();  // 18
  series->append(lowOATemp - 5, lowVal);
  series->append(lowOATemp, lowVal);
  series->append(highOATemp, highVal);
  series->append(highOATemp + 5, highVal);

  auto* chart = new QChart;
  chart->legend()->hide();
  chart->addSeries(series);
  chart->createDefaultAxes();
  auto* xAxis = chart->axes(Qt::Horizontal)[0];
  xAxis->setTitleText("System Node Temperature [C]");
  auto* yAxis = chart->axes(Qt::Vertical)[0];
  yAxis->setTitleText("Setpoint Temperature [C]");
  yAxis->setRange(std::min(lowVal, highVal) - 2, std::max(lowVal, highVal) + 2);

  chart->setTitle("Setpoint System Node Reset Temperature");
  chart->setAnimationOptions(QChart::SeriesAnimations);

  auto* m_defaultChartView = new QChartView(chart, this);
  m_defaultChartView->setRenderHint(QPainter::Antialiasing);
  m_defaultChartView->setMinimumSize(400, 400);
  mainVLayout->addWidget(m_defaultChartView);
  mainVLayout->addStretch();
}

ScheduledSPMView::ScheduledSPMView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* scheduledSPMlabel = new QLabel("Supply temperature is controlled by a scheduled setpoint manager.");
  mainVLayout->addWidget(scheduledSPMlabel);

  auto* supplyAirTempScheduleTitle = new QLabel("Supply Temperature Schedule");
  supplyAirTempScheduleTitle->setObjectName("H2");
  mainVLayout->addWidget(supplyAirTempScheduleTitle);

  supplyAirTemperatureViewSwitcher = new OSViewSwitcher();
  supplyAirTemperatureViewSwitcher->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  mainVLayout->addWidget(supplyAirTemperatureViewSwitcher);
}

FollowOATempSPMView::FollowOATempSPMView(const model::SetpointManagerFollowOutdoorAirTemperature& spm) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto const refTempType = QString::fromStdString(spm.referenceTemperatureType());
  const auto offset = spm.offsetTemperatureDifference();
  const auto minVal = spm.minimumSetpointTemperature();
  const auto maxVal = spm.maximumSetpointTemperature();

  auto* followOATempSPMlabel =
    new QLabel(QString("Supply temperature follows the %1 temperature with an offset of %2 C.").arg(refTempType, QString::number(offset)));
  mainVLayout->addWidget(followOATempSPMlabel);

  auto* series = new QLineSeries;
  series->setName("Setpoint Temperature");
  auto* seriesOA = new QLineSeries;
  seriesOA->setName(refTempType);
  QRgb color1 = qRgb(255, 0, 0);
  QPen pen1(color1);
  pen1.setStyle(Qt::DotLine);
  pen1.setWidth(1);
  seriesOA->setPen(pen1);

  std::vector<double> xVals{-20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40};
  for (const auto& x : xVals) {
    double y = std::clamp(x + offset, minVal, maxVal);
    series->append(x, y);
    seriesOA->append(x, x);
  }

  auto* chart = new QChart;
  chart->legend()->hide();
  chart->addSeries(series);
  chart->addSeries(seriesOA);
  chart->createDefaultAxes();
  auto* xAxis = chart->axes(Qt::Horizontal)[0];
  xAxis->setTitleText(QString("%1 Temperature [C]").arg(refTempType));
  auto* yAxis = chart->axes(Qt::Vertical)[0];
  yAxis->setTitleText("Setpoint Temperature [C]");

  chart->setTitle("Setpoint Follow Outdoor Air Temperature");
  chart->setAnimationOptions(QChart::SeriesAnimations);

  auto* m_defaultChartView = new QChartView(chart, this);
  m_defaultChartView->setRenderHint(QPainter::Antialiasing);
  m_defaultChartView->setMinimumSize(400, 400);
  mainVLayout->addWidget(m_defaultChartView);
  mainVLayout->addStretch();
}

FollowGroundTempSPMView::FollowGroundTempSPMView(const model::SetpointManagerFollowGroundTemperature& spm) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  const auto offset = spm.offsetTemperatureDifference();
  const auto minVal = spm.minimumSetpointTemperature();
  const auto maxVal = spm.maximumSetpointTemperature();

  auto* followTempSPMlabel =
    new QLabel(QString("Supply temperature follows the Ground Temperature with an offset of %2 C.").arg(QString::number(offset)));
  mainVLayout->addWidget(followTempSPMlabel);

  auto* series = new QLineSeries;
  series->setName("Setpoint Temperature");
  auto* seriesOA = new QLineSeries;
  seriesOA->setName("Ground Temperature");
  QRgb color1 = qRgb(255, 0, 0);
  QPen pen1(color1);
  pen1.setStyle(Qt::DotLine);
  pen1.setWidth(1);
  seriesOA->setPen(pen1);

  std::vector<double> xVals{-20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40};
  for (const auto& x : xVals) {
    double y = std::clamp(x + offset, minVal, maxVal);
    series->append(x, y);
    seriesOA->append(x, x);
  }

  auto* chart = new QChart;
  chart->legend()->hide();
  chart->addSeries(series);
  chart->addSeries(seriesOA);
  chart->createDefaultAxes();
  auto* xAxis = chart->axes(Qt::Horizontal)[0];
  xAxis->setTitleText("Ground Temperature [C]");
  auto* yAxis = chart->axes(Qt::Vertical)[0];
  yAxis->setTitleText("Setpoint Temperature [C]");

  chart->setTitle("Setpoint Follow Ground Temperature");
  chart->setAnimationOptions(QChart::SeriesAnimations);

  auto* m_defaultChartView = new QChartView(chart, this);
  m_defaultChartView->setRenderHint(QPainter::Antialiasing);
  m_defaultChartView->setMinimumSize(400, 400);
  mainVLayout->addWidget(m_defaultChartView);
  mainVLayout->addStretch();
}

WarmestColdestSPMView::WarmestColdestSPMView(const model::SetpointManagerWarmest& spm) {

  commonSetup(spm.minimumSetpointTemperature(), spm.maximumSetpointTemperature(), spm.strategy(), "Warmest");
}
WarmestColdestSPMView::WarmestColdestSPMView(const model::SetpointManagerWarmestTemperatureFlow& spm) {
  commonSetup(spm.minimumSetpointTemperature(), spm.maximumSetpointTemperature(), spm.strategy(), "Warmest Temperature And Flow");
}

WarmestColdestSPMView::WarmestColdestSPMView(const model::SetpointManagerColdest& spm) {
  commonSetup(spm.minimumSetpointTemperature(), spm.maximumSetpointTemperature(), spm.strategy(), "Coldest");
}

void WarmestColdestSPMView::commonSetup(double minimumSetpointTemperature, double maximumSetpointTemperature, const std::string& strategy,
                                        const QString& spmType) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* label = new QLabel(
    QString("Supply Air Temperature is managed by a Setpoint Manager %1 with a minimum temperature of %2 C and a maximum temperature of %3 C, with a "
            "strategy of %4")
      .arg(spmType, QString::number(minimumSetpointTemperature), QString::number(maximumSetpointTemperature), QString::fromStdString(strategy)));

  label->setWordWrap(true);
  mainVLayout->addWidget(label);
}

AirLoopHVACUnitaryHeatPumpAirToAirControlView::AirLoopHVACUnitaryHeatPumpAirToAirControlView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* heatPumplabel = new QLabel("Supply air temperature is managed by the \"AirLoopHVACUnitaryHeatPumpAirToAir\" component.");
  mainVLayout->addWidget(heatPumplabel);

  auto* zoneSelectorHBoxLayout = new QHBoxLayout();
  zoneSelectorHBoxLayout->setSpacing(10);
  zoneSelectorHBoxLayout->setContentsMargins(0, 0, 0, 0);

  auto* controlZoneTitle = new QLabel("Control Zone");
  controlZoneTitle->setObjectName("H2");
  zoneSelectorHBoxLayout->addWidget(controlZoneTitle);

  controlZoneComboBox = new OSComboBox2();
  controlZoneComboBox->setEnabled(true);
  zoneSelectorHBoxLayout->addWidget(controlZoneComboBox);
  zoneSelectorHBoxLayout->addStretch();

  mainVLayout->addLayout(zoneSelectorHBoxLayout);
}

NoSupplyAirTempControlView::NoSupplyAirTempControlView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* label = new QLabel("<strong style=\"color:red\">Missing supply temperature control</strong>. "
                           "Try adding a setpoint manager to the supply outlet node of your system.");
  label->setWordWrap(true);
  mainVLayout->addWidget(label);
}

NoSupplyAirTempControlView::NoSupplyAirTempControlView(const model::SetpointManager& spm) {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(10);
  setLayout(mainVLayout);

  auto* label = new QLabel(QString("Supply temperature control of type '%1'.").arg(QString::fromStdString(spm.iddObjectType().valueDescription())));
  label->setWordWrap(true);
  mainVLayout->addWidget(label);
}

NoControlsView::NoControlsView() {
  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(5, 5, 5, 5);
  mainVLayout->setSpacing(10);
  mainVLayout->setAlignment(Qt::AlignTop);
  setLayout(mainVLayout);

  auto* label = new QLabel("Control options are currently only available for air systems.");
  label->setWordWrap(true);
  label->setObjectName("H1");
  mainVLayout->addWidget(label);
}

}  // namespace openstudio
