/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SITEGROUNDTEMPERATUREWIDGET_HPP
#define OPENSTUDIO_SITEGROUNDTEMPERATUREWIDGET_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"
#include "OSCollapsibleItemList.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface.hpp>

#include <QWidget>

namespace openstudio {

class OSDoubleEdit2;

class SiteGroundTemperatureListView : public OSCollapsibleItemList
{
  Q_OBJECT

 public:
  SiteGroundTemperatureListView(const model::Model& model, bool addScrollArea, OSItemType headerType, QWidget* parent = nullptr);

  virtual ~SiteGroundTemperatureListView() = default;

 private:
  model::Model m_model;
};

class SiteGroundTemperatureView : public ModelSubTabView
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~SiteGroundTemperatureView() = default;
};

class SiteGroundTemperatureInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit SiteGroundTemperatureInspectorView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~SiteGroundTemperatureInspectorView();

 protected:
  virtual void onSelectItem(OSItem* item) override;
  virtual void onClearSelection() override;
  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onUpdate() override;

 private:
  void createWidgets();
  void attachBuildingSurface(const model::SiteGroundTemperatureBuildingSurface& obj);
  void detach();

  boost::optional<model::SiteGroundTemperatureBuildingSurface> m_buildingSurface;

  int m_hiddenWidgetIndex;
  int m_buildingSurfaceWidgetIndex;

  OSDoubleEdit2* m_januaryEdit = nullptr;
  OSDoubleEdit2* m_februaryEdit = nullptr;
  OSDoubleEdit2* m_marchEdit = nullptr;
  OSDoubleEdit2* m_aprilEdit = nullptr;
  OSDoubleEdit2* m_mayEdit = nullptr;
  OSDoubleEdit2* m_juneEdit = nullptr;
  OSDoubleEdit2* m_julyEdit = nullptr;
  OSDoubleEdit2* m_augustEdit = nullptr;
  OSDoubleEdit2* m_septemberEdit = nullptr;
  OSDoubleEdit2* m_octoberEdit = nullptr;
  OSDoubleEdit2* m_novemberEdit = nullptr;
  OSDoubleEdit2* m_decemberEdit = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SITEGROUNDTEMPERATUREWIDGET_HPP
