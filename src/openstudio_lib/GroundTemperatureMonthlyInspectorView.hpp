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
#include <functional>

class QBarSet;
class QDoubleSpinBox;
class QLabel;
class QLineSeries;
class QPushButton;
class QValueAxis;

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
  void setChartValues(const std::array<double, 12>& celsiusValues);
  void refreshChartFromModel();

  bool m_isIP;
  std::function<std::array<double, 12>()> m_valuesGetter;
  QLabel* m_titleLabel = nullptr;
  QDoubleSpinBox* m_constantValueEdit = nullptr;
  QPushButton* m_applyConstantButton = nullptr;
  std::array<OSQuantityEdit2*, 12> m_edits{};

 private:
  void refreshChartDisplay();

  QBarSet* m_chartBarSet = nullptr;
  QLineSeries* m_zeroLine = nullptr;
  QValueAxis* m_chartYAxis = nullptr;
  std::array<double, 12> m_cachedCelsius{};
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
  using MOType = model::SiteGroundTemperatureBuildingSurface;
  struct MonthBinding
  {
    double (MOType::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (MOType::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (MOType::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (MOType::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &MOType::januaryGroundTemperature,
      &MOType::setJanuaryGroundTemperature,
      &MOType::resetJanuaryGroundTemperature,
      &MOType::isJanuaryGroundTemperatureDefaulted,
    },
    {
      &MOType::februaryGroundTemperature,
      &MOType::setFebruaryGroundTemperature,
      &MOType::resetFebruaryGroundTemperature,
      &MOType::isFebruaryGroundTemperatureDefaulted,
    },
    {
      &MOType::marchGroundTemperature,
      &MOType::setMarchGroundTemperature,
      &MOType::resetMarchGroundTemperature,
      &MOType::isMarchGroundTemperatureDefaulted,
    },
    {
      &MOType::aprilGroundTemperature,
      &MOType::setAprilGroundTemperature,
      &MOType::resetAprilGroundTemperature,
      &MOType::isAprilGroundTemperatureDefaulted,
    },
    {
      &MOType::mayGroundTemperature,
      &MOType::setMayGroundTemperature,
      &MOType::resetMayGroundTemperature,
      &MOType::isMayGroundTemperatureDefaulted,
    },
    {
      &MOType::juneGroundTemperature,
      &MOType::setJuneGroundTemperature,
      &MOType::resetJuneGroundTemperature,
      &MOType::isJuneGroundTemperatureDefaulted,
    },
    {
      &MOType::julyGroundTemperature,
      &MOType::setJulyGroundTemperature,
      &MOType::resetJulyGroundTemperature,
      &MOType::isJulyGroundTemperatureDefaulted,
    },
    {
      &MOType::augustGroundTemperature,
      &MOType::setAugustGroundTemperature,
      &MOType::resetAugustGroundTemperature,
      &MOType::isAugustGroundTemperatureDefaulted,
    },
    {
      &MOType::septemberGroundTemperature,
      &MOType::setSeptemberGroundTemperature,
      &MOType::resetSeptemberGroundTemperature,
      &MOType::isSeptemberGroundTemperatureDefaulted,
    },
    {
      &MOType::octoberGroundTemperature,
      &MOType::setOctoberGroundTemperature,
      &MOType::resetOctoberGroundTemperature,
      &MOType::isOctoberGroundTemperatureDefaulted,
    },
    {
      &MOType::novemberGroundTemperature,
      &MOType::setNovemberGroundTemperature,
      &MOType::resetNovemberGroundTemperature,
      &MOType::isNovemberGroundTemperatureDefaulted,
    },
    {
      &MOType::decemberGroundTemperature,
      &MOType::setDecemberGroundTemperature,
      &MOType::resetDecemberGroundTemperature,
      &MOType::isDecemberGroundTemperatureDefaulted,
    },
  }};
  boost::optional<MOType> m_obj;
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
  using MOType = model::SiteGroundTemperatureShallow;
  struct MonthBinding
  {
    double (MOType::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (MOType::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (MOType::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (MOType::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &MOType::januarySurfaceGroundTemperature,
      &MOType::setJanuarySurfaceGroundTemperature,
      &MOType::resetJanuarySurfaceGroundTemperature,
      &MOType::isJanuarySurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::februarySurfaceGroundTemperature,
      &MOType::setFebruarySurfaceGroundTemperature,
      &MOType::resetFebruarySurfaceGroundTemperature,
      &MOType::isFebruarySurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::marchSurfaceGroundTemperature,
      &MOType::setMarchSurfaceGroundTemperature,
      &MOType::resetMarchSurfaceGroundTemperature,
      &MOType::isMarchSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::aprilSurfaceGroundTemperature,
      &MOType::setAprilSurfaceGroundTemperature,
      &MOType::resetAprilSurfaceGroundTemperature,
      &MOType::isAprilSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::maySurfaceGroundTemperature,
      &MOType::setMaySurfaceGroundTemperature,
      &MOType::resetMaySurfaceGroundTemperature,
      &MOType::isMaySurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::juneSurfaceGroundTemperature,
      &MOType::setJuneSurfaceGroundTemperature,
      &MOType::resetJuneSurfaceGroundTemperature,
      &MOType::isJuneSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::julySurfaceGroundTemperature,
      &MOType::setJulySurfaceGroundTemperature,
      &MOType::resetJulySurfaceGroundTemperature,
      &MOType::isJulySurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::augustSurfaceGroundTemperature,
      &MOType::setAugustSurfaceGroundTemperature,
      &MOType::resetAugustSurfaceGroundTemperature,
      &MOType::isAugustSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::septemberSurfaceGroundTemperature,
      &MOType::setSeptemberSurfaceGroundTemperature,
      &MOType::resetSeptemberSurfaceGroundTemperature,
      &MOType::isSeptemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::octoberSurfaceGroundTemperature,
      &MOType::setOctoberSurfaceGroundTemperature,
      &MOType::resetOctoberSurfaceGroundTemperature,
      &MOType::isOctoberSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::novemberSurfaceGroundTemperature,
      &MOType::setNovemberSurfaceGroundTemperature,
      &MOType::resetNovemberSurfaceGroundTemperature,
      &MOType::isNovemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &MOType::decemberSurfaceGroundTemperature,
      &MOType::setDecemberSurfaceGroundTemperature,
      &MOType::resetDecemberSurfaceGroundTemperature,
      &MOType::isDecemberSurfaceGroundTemperatureDefaulted,
    },
  }};
  boost::optional<MOType> m_obj;
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
  using MOType = model::SiteGroundTemperatureDeep;
  struct MonthBinding
  {
    double (MOType::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (MOType::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (MOType::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (MOType::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &MOType::januaryDeepGroundTemperature,
      &MOType::setJanuaryDeepGroundTemperature,
      &MOType::resetJanuaryDeepGroundTemperature,
      &MOType::isJanuaryDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::februaryDeepGroundTemperature,
      &MOType::setFebruaryDeepGroundTemperature,
      &MOType::resetFebruaryDeepGroundTemperature,
      &MOType::isFebruaryDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::marchDeepGroundTemperature,
      &MOType::setMarchDeepGroundTemperature,
      &MOType::resetMarchDeepGroundTemperature,
      &MOType::isMarchDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::aprilDeepGroundTemperature,
      &MOType::setAprilDeepGroundTemperature,
      &MOType::resetAprilDeepGroundTemperature,
      &MOType::isAprilDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::mayDeepGroundTemperature,
      &MOType::setMayDeepGroundTemperature,
      &MOType::resetMayDeepGroundTemperature,
      &MOType::isMayDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::juneDeepGroundTemperature,
      &MOType::setJuneDeepGroundTemperature,
      &MOType::resetJuneDeepGroundTemperature,
      &MOType::isJuneDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::julyDeepGroundTemperature,
      &MOType::setJulyDeepGroundTemperature,
      &MOType::resetJulyDeepGroundTemperature,
      &MOType::isJulyDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::augustDeepGroundTemperature,
      &MOType::setAugustDeepGroundTemperature,
      &MOType::resetAugustDeepGroundTemperature,
      &MOType::isAugustDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::septemberDeepGroundTemperature,
      &MOType::setSeptemberDeepGroundTemperature,
      &MOType::resetSeptemberDeepGroundTemperature,
      &MOType::isSeptemberDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::octoberDeepGroundTemperature,
      &MOType::setOctoberDeepGroundTemperature,
      &MOType::resetOctoberDeepGroundTemperature,
      &MOType::isOctoberDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::novemberDeepGroundTemperature,
      &MOType::setNovemberDeepGroundTemperature,
      &MOType::resetNovemberDeepGroundTemperature,
      &MOType::isNovemberDeepGroundTemperatureDefaulted,
    },
    {
      &MOType::decemberDeepGroundTemperature,
      &MOType::setDecemberDeepGroundTemperature,
      &MOType::resetDecemberDeepGroundTemperature,
      &MOType::isDecemberDeepGroundTemperatureDefaulted,
    },
  }};
  boost::optional<MOType> m_obj;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GROUNDTEMPERATUREMONTHLYINSPECTORVIEW_HPP
