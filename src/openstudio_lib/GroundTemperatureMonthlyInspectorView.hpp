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
  using BS = model::SiteGroundTemperatureBuildingSurface;
  struct MonthBinding
  {
    double (BS::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (BS::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (BS::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (BS::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &BS::januaryGroundTemperature,
      &BS::setJanuaryGroundTemperature,
      &BS::resetJanuaryGroundTemperature,
      &BS::isJanuaryGroundTemperatureDefaulted,
    },
    {
      &BS::februaryGroundTemperature,
      &BS::setFebruaryGroundTemperature,
      &BS::resetFebruaryGroundTemperature,
      &BS::isFebruaryGroundTemperatureDefaulted,
    },
    {
      &BS::marchGroundTemperature,
      &BS::setMarchGroundTemperature,
      &BS::resetMarchGroundTemperature,
      &BS::isMarchGroundTemperatureDefaulted,
    },
    {
      &BS::aprilGroundTemperature,
      &BS::setAprilGroundTemperature,
      &BS::resetAprilGroundTemperature,
      &BS::isAprilGroundTemperatureDefaulted,
    },
    {
      &BS::mayGroundTemperature,
      &BS::setMayGroundTemperature,
      &BS::resetMayGroundTemperature,
      &BS::isMayGroundTemperatureDefaulted,
    },
    {
      &BS::juneGroundTemperature,
      &BS::setJuneGroundTemperature,
      &BS::resetJuneGroundTemperature,
      &BS::isJuneGroundTemperatureDefaulted,
    },
    {
      &BS::julyGroundTemperature,
      &BS::setJulyGroundTemperature,
      &BS::resetJulyGroundTemperature,
      &BS::isJulyGroundTemperatureDefaulted,
    },
    {
      &BS::augustGroundTemperature,
      &BS::setAugustGroundTemperature,
      &BS::resetAugustGroundTemperature,
      &BS::isAugustGroundTemperatureDefaulted,
    },
    {
      &BS::septemberGroundTemperature,
      &BS::setSeptemberGroundTemperature,
      &BS::resetSeptemberGroundTemperature,
      &BS::isSeptemberGroundTemperatureDefaulted,
    },
    {
      &BS::octoberGroundTemperature,
      &BS::setOctoberGroundTemperature,
      &BS::resetOctoberGroundTemperature,
      &BS::isOctoberGroundTemperatureDefaulted,
    },
    {
      &BS::novemberGroundTemperature,
      &BS::setNovemberGroundTemperature,
      &BS::resetNovemberGroundTemperature,
      &BS::isNovemberGroundTemperatureDefaulted,
    },
    {
      &BS::decemberGroundTemperature,
      &BS::setDecemberGroundTemperature,
      &BS::resetDecemberGroundTemperature,
      &BS::isDecemberGroundTemperatureDefaulted,
    },
  }};
  boost::optional<BS> m_obj;
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
  using SH = model::SiteGroundTemperatureShallow;
  struct MonthBinding
  {
    double (SH::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (SH::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (SH::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (SH::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &SH::januarySurfaceGroundTemperature,
      &SH::setJanuarySurfaceGroundTemperature,
      &SH::resetJanuarySurfaceGroundTemperature,
      &SH::isJanuarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::februarySurfaceGroundTemperature,
      &SH::setFebruarySurfaceGroundTemperature,
      &SH::resetFebruarySurfaceGroundTemperature,
      &SH::isFebruarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::marchSurfaceGroundTemperature,
      &SH::setMarchSurfaceGroundTemperature,
      &SH::resetMarchSurfaceGroundTemperature,
      &SH::isMarchSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::aprilSurfaceGroundTemperature,
      &SH::setAprilSurfaceGroundTemperature,
      &SH::resetAprilSurfaceGroundTemperature,
      &SH::isAprilSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::maySurfaceGroundTemperature,
      &SH::setMaySurfaceGroundTemperature,
      &SH::resetMaySurfaceGroundTemperature,
      &SH::isMaySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::juneSurfaceGroundTemperature,
      &SH::setJuneSurfaceGroundTemperature,
      &SH::resetJuneSurfaceGroundTemperature,
      &SH::isJuneSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::julySurfaceGroundTemperature,
      &SH::setJulySurfaceGroundTemperature,
      &SH::resetJulySurfaceGroundTemperature,
      &SH::isJulySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::augustSurfaceGroundTemperature,
      &SH::setAugustSurfaceGroundTemperature,
      &SH::resetAugustSurfaceGroundTemperature,
      &SH::isAugustSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::septemberSurfaceGroundTemperature,
      &SH::setSeptemberSurfaceGroundTemperature,
      &SH::resetSeptemberSurfaceGroundTemperature,
      &SH::isSeptemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::octoberSurfaceGroundTemperature,
      &SH::setOctoberSurfaceGroundTemperature,
      &SH::resetOctoberSurfaceGroundTemperature,
      &SH::isOctoberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::novemberSurfaceGroundTemperature,
      &SH::setNovemberSurfaceGroundTemperature,
      &SH::resetNovemberSurfaceGroundTemperature,
      &SH::isNovemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::decemberSurfaceGroundTemperature,
      &SH::setDecemberSurfaceGroundTemperature,
      &SH::resetDecemberSurfaceGroundTemperature,
      &SH::isDecemberSurfaceGroundTemperatureDefaulted,
    },
  }};
  boost::optional<SH> m_obj;
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
  using DP = model::SiteGroundTemperatureDeep;
  struct MonthBinding
  {
    double (DP::*getter)() const;   // cppcheck-suppress unusedStructMember
    bool (DP::*setter)(double);     // cppcheck-suppress unusedStructMember
    void (DP::*resetter)();         // cppcheck-suppress unusedStructMember
    bool (DP::*defaulted)() const;  // cppcheck-suppress unusedStructMember
  };
  inline static const std::array<MonthBinding, 12> s_monthBinders{{
    {
      &DP::januaryDeepGroundTemperature,
      &DP::setJanuaryDeepGroundTemperature,
      &DP::resetJanuaryDeepGroundTemperature,
      &DP::isJanuaryDeepGroundTemperatureDefaulted,
    },
    {
      &DP::februaryDeepGroundTemperature,
      &DP::setFebruaryDeepGroundTemperature,
      &DP::resetFebruaryDeepGroundTemperature,
      &DP::isFebruaryDeepGroundTemperatureDefaulted,
    },
    {
      &DP::marchDeepGroundTemperature,
      &DP::setMarchDeepGroundTemperature,
      &DP::resetMarchDeepGroundTemperature,
      &DP::isMarchDeepGroundTemperatureDefaulted,
    },
    {
      &DP::aprilDeepGroundTemperature,
      &DP::setAprilDeepGroundTemperature,
      &DP::resetAprilDeepGroundTemperature,
      &DP::isAprilDeepGroundTemperatureDefaulted,
    },
    {
      &DP::mayDeepGroundTemperature,
      &DP::setMayDeepGroundTemperature,
      &DP::resetMayDeepGroundTemperature,
      &DP::isMayDeepGroundTemperatureDefaulted,
    },
    {
      &DP::juneDeepGroundTemperature,
      &DP::setJuneDeepGroundTemperature,
      &DP::resetJuneDeepGroundTemperature,
      &DP::isJuneDeepGroundTemperatureDefaulted,
    },
    {
      &DP::julyDeepGroundTemperature,
      &DP::setJulyDeepGroundTemperature,
      &DP::resetJulyDeepGroundTemperature,
      &DP::isJulyDeepGroundTemperatureDefaulted,
    },
    {
      &DP::augustDeepGroundTemperature,
      &DP::setAugustDeepGroundTemperature,
      &DP::resetAugustDeepGroundTemperature,
      &DP::isAugustDeepGroundTemperatureDefaulted,
    },
    {
      &DP::septemberDeepGroundTemperature,
      &DP::setSeptemberDeepGroundTemperature,
      &DP::resetSeptemberDeepGroundTemperature,
      &DP::isSeptemberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::octoberDeepGroundTemperature,
      &DP::setOctoberDeepGroundTemperature,
      &DP::resetOctoberDeepGroundTemperature,
      &DP::isOctoberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::novemberDeepGroundTemperature,
      &DP::setNovemberDeepGroundTemperature,
      &DP::resetNovemberDeepGroundTemperature,
      &DP::isNovemberDeepGroundTemperatureDefaulted,
    },
    {
      &DP::decemberDeepGroundTemperature,
      &DP::setDecemberDeepGroundTemperature,
      &DP::resetDecemberDeepGroundTemperature,
      &DP::isDecemberDeepGroundTemperatureDefaulted,
    },
  }};
  boost::optional<DP> m_obj;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GROUNDTEMPERATUREMONTHLYINSPECTORVIEW_HPP
