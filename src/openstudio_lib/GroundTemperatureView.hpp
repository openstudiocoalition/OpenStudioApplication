/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GROUNDTEMPERATUREVIEW_HPP
#define OPENSTUDIO_GROUNDTEMPERATUREVIEW_HPP

#include "GroundTemperatureMonthlyInspectorView.hpp"
#include "SiteWaterMainsTemperatureWidget.hpp"

#include <openstudio/model/Model.hpp>

#include <QWidget>

class QLabel;
class QPushButton;
class QStackedWidget;

namespace openstudio {

class OSItemSelectorButtons;

enum class GroundTempType
{
  BuildingSurface,
  Shallow,
  Deep,
  WaterMains
};

/** A single clickable entry in the left-hand list (ScheduleTabDefault style). */
class GroundTemperatureEntry : public QWidget
{
  Q_OBJECT

 public:
  explicit GroundTemperatureEntry(const QString& label, QWidget* parent = nullptr);

  void setSelected(bool selected);

 signals:
  void clicked();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  bool m_mouseDown = false;
  QLabel* m_label = nullptr;
};

/** Left-hand list: one entry per ground-temperature type. */
class GroundTemperatureListView : public QWidget
{
  Q_OBJECT

 public:
  explicit GroundTemperatureListView(QWidget* parent = nullptr);

  void selectFirst();

 signals:
  void typeSelected(openstudio::GroundTempType type);

 private slots:
  void onBuildingSurfaceClicked();
  void onShallowClicked();
  void onDeepClicked();
  void onWaterMainsClicked();

 private:
  GroundTemperatureEntry* m_bsEntry = nullptr;
  GroundTemperatureEntry* m_shEntry = nullptr;
  GroundTemperatureEntry* m_deepEntry = nullptr;
  GroundTemperatureEntry* m_waterMainsEntry = nullptr;
};

/** Right pane shown when the selected object is not yet in the model. */
class GroundTemperatureNotPresentView : public QWidget
{
  Q_OBJECT

 public:
  explicit GroundTemperatureNotPresentView(QWidget* parent = nullptr);

  void setType(GroundTempType type, model::Model model);

 signals:
  void addClicked(openstudio::GroundTempType type);

 private:
  QLabel* m_label = nullptr;
  QLabel* m_epwInfoLabel = nullptr;
  QPushButton* m_addButton = nullptr;
  QPushButton* m_importFromEPWButton = nullptr;
  GroundTempType m_type = GroundTempType::BuildingSurface;
  model::Model m_model;
};

/** Top-level widget for the Ground Temperatures sub-tab. */
class GroundTemperatureView : public QWidget
{
  Q_OBJECT

 public:
  explicit GroundTemperatureView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

 signals:
  void toggleUnitsClicked(bool displayIP);

 private slots:
  void onTypeSelected(openstudio::GroundTempType type);
  void onObjectCreated(openstudio::GroundTempType type);
  void onRemoveClicked();

 private:
  model::Model m_model;
  bool m_isIP;
  GroundTempType m_currentType = GroundTempType::BuildingSurface;
  GroundTemperatureListView* m_listView = nullptr;
  OSItemSelectorButtons* m_selectorButtons = nullptr;
  GroundTemperatureNotPresentView* m_notPresentView = nullptr;
  SiteGroundTemperatureBuildingSurfaceWidget* m_bsView = nullptr;
  SiteGroundTemperatureShallowWidget* m_shView = nullptr;
  SiteGroundTemperatureDeepWidget* m_deepView = nullptr;
  SiteWaterMainsTemperatureWidget* m_waterMainsView = nullptr;
  QStackedWidget* m_rightStack = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GROUNDTEMPERATUREVIEW_HPP
