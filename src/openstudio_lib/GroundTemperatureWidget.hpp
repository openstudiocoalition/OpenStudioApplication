/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GROUNDTEMPERATUREWIDGET_HPP
#define OPENSTUDIO_GROUNDTEMPERATUREWIDGET_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow.hpp>

#include <QWidget>

#include <array>

class QLabel;
class QPushButton;
class QStackedWidget;

namespace openstudio {

class OSQuantityEdit2;

enum class GroundTempType
{
  BuildingSurface,
  Shallow
};

/** A single clickable entry in the left-hand list (ScheduleTabDefault style). */
class SiteGroundTemperatureEntry : public QWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureEntry(const QString& label, QWidget* parent = nullptr);

  void setSelected(bool selected);

 signals:
  void clicked();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;

 private:
  bool m_mouseDown = false;
  bool m_hovering = false;
  bool m_selected = false;
  QLabel* m_label = nullptr;
};

/** Left-hand list: two SiteGroundTemperatureEntry items. */
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

 private:
  SiteGroundTemperatureEntry* m_bsEntry = nullptr;
  SiteGroundTemperatureEntry* m_shEntry = nullptr;
};

/** "Not present" right pane: message label + Add button. */
class GroundTemperatureNotPresentView : public QWidget
{
  Q_OBJECT

 public:
  explicit GroundTemperatureNotPresentView(QWidget* parent = nullptr);

  void setType(GroundTempType type, const QString& typeName, model::Model model);

 signals:
  void addClicked(openstudio::GroundTempType type);

 private:
  QLabel* m_label = nullptr;
  QPushButton* m_addButton = nullptr;
  GroundTempType m_type = GroundTempType::BuildingSurface;
  model::Model m_model;
};

/** Abstract base for widgets showing 12 monthly temperature fields. */
class SiteGroundTemperatureMonthlyWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureMonthlyWidget(bool isIP, QWidget* parent = nullptr);
  virtual ~SiteGroundTemperatureMonthlyWidget() = default;

  virtual void attach(const model::ModelObject& obj) = 0;
  void detach();

 signals:
  void toggleUnitsClicked(bool displayIP);

 protected:
  bool m_isIP;
  QLabel* m_titleLabel = nullptr;
  std::array<OSQuantityEdit2*, 12> m_edits{};
};

/** Concrete: Site:GroundTemperature:BuildingSurface */
class SiteGroundTemperatureBuildingSurfaceWidget : public SiteGroundTemperatureMonthlyWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureBuildingSurfaceWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj) override;

 private:
  boost::optional<model::SiteGroundTemperatureBuildingSurface> m_obj;
};

/** Concrete: Site:GroundTemperature:Shallow */
class SiteGroundTemperatureShallowWidget : public SiteGroundTemperatureMonthlyWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureShallowWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj) override;

 private:
  boost::optional<model::SiteGroundTemperatureShallow> m_obj;
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

 private:
  model::Model m_model;
  bool m_isIP;
  GroundTemperatureListView* m_listView = nullptr;
  GroundTemperatureNotPresentView* m_notPresentView = nullptr;
  SiteGroundTemperatureBuildingSurfaceWidget* m_bsView = nullptr;
  SiteGroundTemperatureShallowWidget* m_shView = nullptr;
  QStackedWidget* m_rightStack = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GROUNDTEMPERATUREWIDGET_HPP
