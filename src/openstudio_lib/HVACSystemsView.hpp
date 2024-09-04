/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_HVACSYSTEMSVIEW_HPP
#define OPENSTUDIO_HVACSYSTEMSVIEW_HPP

#include <QWidget>
#include <QScrollArea>
#include <QGraphicsView>
#include "../shared_gui_components/OSComboBox.hpp"

class QStackedWidget;
class QPushButton;
class QLabel;
class QVBoxLayout;

namespace openstudio {

class HVACGraphicsView;
class HVACAirLoopControlsView;
class HVACPlantLoopControlsView;
class HVACToolbarView;
class VentilationControlsView;
class SingleZoneResetSPView;
class OSViewSwitcher;
class OSSwitch2;

namespace model {
class SetpointManager;
class SetpointManagerOutdoorAirReset;
class SetpointManagerFollowOutdoorAirTemperature;
class SetpointManagerFollowGroundTemperature;
class SetpointManagerSystemNodeResetTemperature;

class SetpointManagerWarmest;
class SetpointManagerWarmestTemperatureFlow;
class SetpointManagerColdest;
}  // namespace model

class HVACSystemsView : public QWidget
{
  Q_OBJECT

 public:
  HVACSystemsView();

  virtual ~HVACSystemsView() {}

  OSViewSwitcher* mainViewSwitcher;

  HVACToolbarView* hvacToolbarView;

  QSize sizeHint() const override;

 protected:
  void paintEvent(QPaintEvent* event) override;
};

class HVACToolbarView : public QWidget
{
  Q_OBJECT

 public:
  HVACToolbarView();

  virtual ~HVACToolbarView() = default;

  QLabel* label;

  QPushButton* addButton;

  QPushButton* copyButton;

  QPushButton* deleteButton;

  QPushButton* topologyViewButton;

  QPushButton* controlsViewButton;

  QPushButton* gridViewButton;

  QPushButton* zoomInButton;

  QPushButton* zoomOutButton;

  OSComboBox2* systemComboBox;

  // Hide or show add, delete, zoom in, and zoom out buttons.
  // If control buttons are hidden only the systemComboBox and label are shown.
  void showControls(bool show);

  // calls show() on all individual buttons and make them enabled
  void resetAllIndividualControlButtons();

  void hideAddCopyDeleteButtons();

  void hideZoomButtons();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QStackedWidget* m_viewStack;
};

class HVACGraphicsView : public QGraphicsView
{
  Q_OBJECT

 public:
  explicit HVACGraphicsView(QWidget* parent = nullptr);

  virtual ~HVACGraphicsView() = default;

 protected:
  // Override to allow zooming with CTRL + Wheel Up/Down
  virtual void wheelEvent(QWheelEvent* event) override;
  // Override to allow zooming with CTRL + '+' / '-'
  virtual void keyReleaseEvent(QKeyEvent* event) override;

 public slots:
  // Display adjustments
  void zoomIn();
  void zoomOut();
  void resetZoom();

 private:
  float m_zoomX = 0.0;

  float m_zoomY = 0.0;
};

/* Controls tab for an AirLoopHVAC */
class HVACAirLoopControlsView : public QScrollArea
{
  Q_OBJECT

 public:
  HVACAirLoopControlsView();

  virtual ~HVACAirLoopControlsView() {}

  QLabel* coolingTypeLabel;

  QLabel* heatingTypeLabel;

  QLabel* systemNameLabel;

  OSViewSwitcher* supplyAirTemperatureViewSwitcher;

  OSViewSwitcher* ventilationViewSwitcher;

  OSViewSwitcher* hvacOperationViewSwitcher;

  OSComboBox2* nightCycleComboBox;

  OSViewSwitcher* availabilityManagerViewSwitcher;
};

class HVACPlantLoopControlsView : public QScrollArea
{
  Q_OBJECT

 public:
  HVACPlantLoopControlsView();

  virtual ~HVACPlantLoopControlsView() {}

  QLabel* systemNameLabel;
  QLabel* plantLoopTypeLabel;
  QLabel* heatingComponentsLabel;
  QLabel* coolingComponentsLabel;
  QLabel* setpointComponentsLabel;
  QLabel* uncontrolledComponentsLabel;

  OSViewSwitcher* supplyTemperatureViewSwitcher;

  OSViewSwitcher* availabilityManagerViewSwitcher;
};

class MechanicalVentilationView : public QWidget
{
  Q_OBJECT;

 public:
  MechanicalVentilationView();

  virtual ~MechanicalVentilationView() = default;

  QComboBox* economizerComboBox;

  QComboBox* ventilationCalcMethodComboBox;

  OSSwitch2* dcvButton;
};

class NoMechanicalVentilationView : public QWidget
{
  Q_OBJECT;

 public:
  NoMechanicalVentilationView();

  virtual ~NoMechanicalVentilationView() = default;
};

class SingleZoneSPMView : public QWidget
{
 public:
  explicit SingleZoneSPMView(const QString& spmType = "SingleZoneReheat");

  virtual ~SingleZoneSPMView() = default;

  OSComboBox2* controlZoneComboBox;
};

class ScheduledSPMView : public QWidget
{
 public:
  ScheduledSPMView();

  virtual ~ScheduledSPMView() = default;

  OSViewSwitcher* supplyAirTemperatureViewSwitcher;
};

class FollowOATempSPMView : public QWidget
{
 public:
  explicit FollowOATempSPMView(const model::SetpointManagerFollowOutdoorAirTemperature& spm);

  virtual ~FollowOATempSPMView() = default;
};

class FollowGroundTempSPMView : public QWidget
{
 public:
  explicit FollowGroundTempSPMView(const model::SetpointManagerFollowGroundTemperature& spm);

  virtual ~FollowGroundTempSPMView() = default;
};

class OAResetSPMView : public QWidget
{
 public:
  explicit OAResetSPMView(const model::SetpointManagerOutdoorAirReset& spm);
  virtual ~OAResetSPMView() = default;
};

class SystemNodeResetSPMView : public QWidget
{
 public:
  explicit SystemNodeResetSPMView(const model::SetpointManagerSystemNodeResetTemperature& spm);
  virtual ~SystemNodeResetSPMView() = default;
};

class WarmestColdestSPMView : public QWidget
{
 public:
  explicit WarmestColdestSPMView(const model::SetpointManagerWarmest& spm);
  explicit WarmestColdestSPMView(const model::SetpointManagerWarmestTemperatureFlow& spm);
  explicit WarmestColdestSPMView(const model::SetpointManagerColdest& spm);
  virtual ~WarmestColdestSPMView() = default;

 private:
  void commonSetup(double minimumSetpointTemperature, double maximumSetpointTemperature, const std::string& strategy, const QString& spmType);
};

class AirLoopHVACUnitaryHeatPumpAirToAirControlView : public QWidget
{
 public:
  AirLoopHVACUnitaryHeatPumpAirToAirControlView();

  virtual ~AirLoopHVACUnitaryHeatPumpAirToAirControlView() = default;

  OSComboBox2* controlZoneComboBox;
};

class NoSupplyAirTempControlView : public QWidget
{
 public:
  NoSupplyAirTempControlView();

  explicit NoSupplyAirTempControlView(const model::SetpointManager& spm);

  virtual ~NoSupplyAirTempControlView() = default;
};

class NoControlsView : public QWidget
{
 public:
  NoControlsView();

  virtual ~NoControlsView() = default;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_HVACSYSTEMSVIEW_HPP
