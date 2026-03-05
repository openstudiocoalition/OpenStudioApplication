/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GROUNDTEMPERATUREMONTHLYINSPECTORVIEW_HPP
#define OPENSTUDIO_GROUNDTEMPERATUREMONTHLYINSPECTORVIEW_HPP

#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow.hpp>
#include <openstudio/model/SiteGroundTemperatureDeep.hpp>

#include <QWidget>

#include <array>

class QDoubleSpinBox;
class QLabel;
class QPushButton;

namespace openstudio {

class OSQuantityEdit2;

/** Abstract base for inspector widgets that show 12 monthly temperature fields. */
class SiteGroundTemperatureMonthlyWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureMonthlyWidget(bool isIP, QWidget* parent = nullptr);
  virtual ~SiteGroundTemperatureMonthlyWidget() = default;

  virtual void attach(const model::ModelObject& obj) = 0;
  void detach();

  virtual void applyConstantValue(double celsius) = 0;

 signals:
  void toggleUnitsClicked(bool displayIP);

 protected:
  bool m_isIP;
  QLabel* m_titleLabel = nullptr;
  QDoubleSpinBox* m_constantValueEdit = nullptr;
  QPushButton* m_applyConstantButton = nullptr;
  std::array<OSQuantityEdit2*, 12> m_edits{};
};

/** Inspector for Site:GroundTemperature:BuildingSurface — 12 monthly fields. */
class SiteGroundTemperatureBuildingSurfaceWidget : public SiteGroundTemperatureMonthlyWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureBuildingSurfaceWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj) override;
  void applyConstantValue(double celsius) override;

 private:
  boost::optional<model::SiteGroundTemperatureBuildingSurface> m_obj;
};

/** Inspector for Site:GroundTemperature:Shallow — 12 monthly fields. */
class SiteGroundTemperatureShallowWidget : public SiteGroundTemperatureMonthlyWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureShallowWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj) override;
  void applyConstantValue(double celsius) override;

 private:
  boost::optional<model::SiteGroundTemperatureShallow> m_obj;
};

/** Inspector for Site:GroundTemperature:Deep — 12 monthly fields. */
class SiteGroundTemperatureDeepWidget : public SiteGroundTemperatureMonthlyWidget
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureDeepWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj) override;
  void applyConstantValue(double celsius) override;

 private:
  boost::optional<model::SiteGroundTemperatureDeep> m_obj;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GROUNDTEMPERATUREMONTHLYINSPECTORVIEW_HPP
