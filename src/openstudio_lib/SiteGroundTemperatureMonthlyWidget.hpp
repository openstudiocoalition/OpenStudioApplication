/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SITEGROUNDTEMPERATUREMONTHLYWIDGET_HPP
#define OPENSTUDIO_SITEGROUNDTEMPERATUREMONTHLYWIDGET_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"
#include "OSItem.hpp"
#include "OSItemSelector.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow.hpp>

#include <QWidget>

#include <array>

namespace openstudio {

class OSQuantityEdit2;

/** A single entry in the left-hand list for one Site:GroundTemperature:* unique object.
 *  Uses a caller-supplied display name instead of name().get() (which would crash for
 *  objects that have no Name IDD field). */
class SiteGroundTemperatureMonthlyItem : public OSItem
{
  Q_OBJECT

 public:
  SiteGroundTemperatureMonthlyItem(const model::ModelObject& obj, const QString& displayName, QWidget* parent = nullptr);
  virtual ~SiteGroundTemperatureMonthlyItem() = default;

  model::ModelObject modelObject() const;

  bool equal(const OSItem* other) const override;

 private:
  model::ModelObject m_modelObject;
};

/** Left-hand picker list for Site:GroundTemperature objects that carry 12 monthly values.
 *  Directly creates SiteGroundTemperatureMonthlyItem entries — bypasses ModelObjectItem /
 *  makeItem so that objects without a Name IDD field do not crash. */
class SiteGroundTemperatureMonthlyListView : public OSItemSelector
{
  Q_OBJECT

 public:
  SiteGroundTemperatureMonthlyListView(const model::Model& model, QWidget* parent = nullptr);
  virtual ~SiteGroundTemperatureMonthlyListView() = default;

  OSItem* selectedItem() const override;

 private slots:
  void onItemClicked(OSItem* item);
  void selectFirst();

 private:
  OSItem* m_selectedItem = nullptr;
  SiteGroundTemperatureMonthlyItem* m_bsItem = nullptr;
};

class SiteGroundTemperatureMonthlyView : public ModelSubTabView
{
  Q_OBJECT

 public:
  SiteGroundTemperatureMonthlyView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SiteGroundTemperatureMonthlyView() = default;
};

class SiteGroundTemperatureMonthlyInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  SiteGroundTemperatureMonthlyInspectorView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SiteGroundTemperatureMonthlyInspectorView();

 protected:
  virtual void onSelectItem(OSItem* item) override;
  virtual void onClearSelection() override;
  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onUpdate() override;

 private:
  void createWidgets();
  void attachBuildingSurface(const model::SiteGroundTemperatureBuildingSurface& obj);
  void attachShallow(const model::SiteGroundTemperatureShallow& obj);
  void detach();

  bool m_isIP;

  boost::optional<model::SiteGroundTemperatureBuildingSurface> m_buildingSurface;
  boost::optional<model::SiteGroundTemperatureShallow> m_shallow;

  int m_hiddenWidgetIndex;
  int m_monthlyWidgetIndex;

  QLabel* m_titleLabel = nullptr;
  QLabel* m_temperatureHeader = nullptr;

  std::array<OSQuantityEdit2*, 12> m_edits{};
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SITEGROUNDTEMPERATUREMONTHLYWIDGET_HPP
